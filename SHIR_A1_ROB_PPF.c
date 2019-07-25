/*
 * File:          SHIR_A1_ROB_PPF.c
 * Date:          03/04/2018
 * Description:   This controller tells each of the robots how to behave in order
 *                to achieve complete area coverage of the ship hull during inspection.
 * Author:        Matthew Haire
 */

/* Webot specific libraries */
#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/supervisor.h>
#include <webots/gps.h>
#include <webots/inertial_unit.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/distance_sensor.h>

/* Standard C libraries */
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Macros used throughout program */
#define TIME_STEP 16.0
#define WATERLINE 19.0
#define MAXDIST 2.0
#define SPACE 2.0
#define ASSEMBLY_TIME 10.0

/* PID Controller Structure */
struct SPID
{
  double pGain, iGain, dGain; // proportional, integral, and differential gains
  double iState; // integrator state
  double dState; // last position input
} SPID_X, SPID_Y, SPID_Z;

/* Update Robot position function */
double UpdatePos(struct SPID pid, double current_pos, double desired_pos);

/* Main program */
int main(int argc, char **argv)
{
  wb_robot_init();
  /* Distance sensors */
  const WbDeviceTag ds_ft = wb_robot_get_device("ds_ft");
  wb_distance_sensor_enable(ds_ft, TIME_STEP);
  const WbDeviceTag ds_fb = wb_robot_get_device("ds_fb");
  wb_distance_sensor_enable(ds_fb, TIME_STEP);
  const WbDeviceTag ds_fl = wb_robot_get_device("ds_fl");
  wb_distance_sensor_enable(ds_fl, TIME_STEP);
  const WbDeviceTag ds_fr = wb_robot_get_device("ds_fr");
  wb_distance_sensor_enable(ds_fr, TIME_STEP);
  const WbDeviceTag ps_t = wb_robot_get_device("ps_t");
  wb_distance_sensor_enable(ps_t, TIME_STEP);
  const WbDeviceTag ps_b = wb_robot_get_device("ps_b");
  wb_distance_sensor_enable(ps_b, TIME_STEP);
  const WbDeviceTag ps_l = wb_robot_get_device("ps_l");
  wb_distance_sensor_enable(ps_l, TIME_STEP);
  const WbDeviceTag ps_r = wb_robot_get_device("ps_r");
  wb_distance_sensor_enable(ps_r, TIME_STEP);
  /* GPS */
  const WbDeviceTag gps  = wb_robot_get_device("gps");
  wb_gps_enable(gps, TIME_STEP);
  /* Motors */
  const WbDeviceTag rmxft = wb_robot_get_device("rmxft");
  wb_motor_set_position(rmxft, INFINITY);
  wb_motor_set_velocity(rmxft, 0);
  const WbDeviceTag rmxfb = wb_robot_get_device("rmxfb");
  wb_motor_set_position(rmxfb, INFINITY);
  wb_motor_set_velocity(rmxfb, 0);
  const WbDeviceTag rmxfl = wb_robot_get_device("rmxfl");
  wb_motor_set_position(rmxfl, INFINITY);
  wb_motor_set_velocity(rmxfl, 0);
  const WbDeviceTag rmxfr = wb_robot_get_device("rmxfr");
  wb_motor_set_position(rmxfr, INFINITY);
  wb_motor_set_velocity(rmxfr, 0);
  const WbDeviceTag rmy = wb_robot_get_device("rmy");
  wb_motor_set_position(rmy, INFINITY);
  wb_motor_set_velocity(rmy, 0);
  const WbDeviceTag rmz = wb_robot_get_device("rmz");
  wb_motor_set_position(rmz, INFINITY);
  wb_motor_set_velocity(rmz, 0);

  /* PID initial Controller variables */
  SPID_X.pGain = 1;     // increase speed of responce
  SPID_X.iGain = 100;   // reduce error between actual and desired sensor values
  SPID_X.dGain = -85;   // Remove oscillation, but increase cumulative error (threatens stability of system)
  SPID_X.iState = 0;
  SPID_X.dState = 0.001;
  SPID_Y = SPID_X;
  SPID_Z = SPID_X;

  /* Local variables */
  double x_pos = 0.0, y_pos = 0.0, z_pos = 0.0;
  double goal_x = 24.0, goal_y = 18.25, end_goal = -20.0;
  //double lost_x = 0.0, lost_y = 0.0, lost_z = 0.0;
  double ps_error = 0.0;
  bool ASSEMBLY_COMPLETE = 0, SEARCH_COMPLETE = 0, WAIT = 0;
  // set random fail time
  srand(time(NULL));
  int fail_time = (rand() % 10) +1;

  /* Main loop */
  while (wb_robot_step(TIME_STEP) != -1)
  {
    x_pos = wb_gps_get_values(gps)[0];
    y_pos = wb_gps_get_values(gps)[1];
    z_pos = wb_gps_get_values(gps)[2];

      ASSEMBLY_COMPLETE = 1; // SKIP ASSEMBLY STAGE

      if(!ASSEMBLY_COMPLETE)
      {
        /* ASSEMBLY STAGE */
        // Set motor velocity of lost agents to 0.0
        if(wb_distance_sensor_get_value(ds_ft) >= 3.0 && wb_distance_sensor_get_value(ds_fb) >= 3.0 && wb_distance_sensor_get_value(ds_fl) >= 3.0 && wb_distance_sensor_get_value(ds_fr) >= 3.0)
        {
        
        // change this section so that if the robot loses it tethers with other robots it defaults to the position where it lost contact and waits.
          wb_motor_set_velocity(rmxft, 0.0);
          wb_motor_set_velocity(rmxfb, 0.0);
          wb_motor_set_velocity(rmxfl, 0.0);
          wb_motor_set_velocity(rmxfr, 0.0);
          wb_motor_set_velocity(rmy, 0.0);
          wb_motor_set_velocity(rmz, 0.0);
        }
            // Stay centred
            wb_motor_set_velocity(rmz, -(UpdatePos(SPID_Z, x_pos, goal_x)));
            //get difference in distance between agents using sensors
            ps_error = wb_distance_sensor_get_value(ps_t) - wb_distance_sensor_get_value(ps_b);
            
              // Maintain distance of 2.0m from ship hull
              wb_motor_set_velocity(rmxft, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_ft), MAXDIST))/2);
              wb_motor_set_velocity(rmxfb, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fb), MAXDIST))/2);
              wb_motor_set_velocity(rmxfl, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fl), MAXDIST))/2);
              wb_motor_set_velocity(rmxfr, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fr), MAXDIST))/2);
            
            //set desired positions
            if(wb_distance_sensor_get_value(ps_t) >= 3.0 && y_pos >= WATERLINE && z_pos > 0.0)
            {
              wb_motor_set_velocity(rmy, (UpdatePos(SPID_Y, y_pos, WATERLINE + 0.5))*2); // Stay close to the 
            }
            else if(wb_distance_sensor_get_value(ps_b) >= 3.0 && y_pos >= WATERLINE && z_pos < 0.0)
            {
              wb_motor_set_velocity(rmy, -(UpdatePos(SPID_Y, y_pos, WATERLINE + 0.5))*2); // Stay close to the Waterline
            }
            else
            {
              wb_motor_set_velocity(rmy, -(UpdatePos(SPID_Y, ps_error, 0.0)));
            }
        //timer for assembly complete
        if(wb_robot_get_time() >= ASSEMBLY_TIME) ASSEMBLY_COMPLETE = 1;
    }
    else
    {
      /* SEARCH STAGE */
      // search complete
      if(x_pos <= end_goal || wb_robot_get_time() >= fail_time) SEARCH_COMPLETE = 1;

      if(!SEARCH_COMPLETE)
      {
        // Move at a steady speed towards the end position
        wb_motor_set_velocity(rmz, 5.0);
        //get difference in distance between agents using sensors
        ps_error = wb_distance_sensor_get_value(ps_t) - wb_distance_sensor_get_value(ps_b);
        
              // Maintain distance of 2.0m from ship hull
              wb_motor_set_velocity(rmxft, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_ft), MAXDIST))/2);
              wb_motor_set_velocity(rmxfb, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fb), MAXDIST))/2);
              wb_motor_set_velocity(rmxfl, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fl), MAXDIST))/2);
              wb_motor_set_velocity(rmxfr, (UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fr), MAXDIST))/2);
        
        // set desired positions
        if(wb_distance_sensor_get_value(ps_t) >= 3.0 && y_pos >= WATERLINE && z_pos > 0.0)
        {
          wb_motor_set_velocity(rmy, (UpdatePos(SPID_Y, y_pos, WATERLINE + 0.5))*2); // Stay close to the 
        }
        else if(wb_distance_sensor_get_value(ps_b) >= 3.0 && y_pos >= WATERLINE && z_pos < 0.0)
        {
          wb_motor_set_velocity(rmy, -(UpdatePos(SPID_Y, y_pos, WATERLINE + 0.5))*2); // Stay close to the Waterline
        }
        else
        {
          wb_motor_set_velocity(rmy, -(UpdatePos(SPID_Y, ps_error, 0.0)));
        }
      }
      else
      {
        if(!WAIT)
        {
          /* EXIT STAGE */
          goal_x = x_pos - 0.5;
          goal_y = y_pos;
          
          /* print fail_time to file */
          FILE * fp;
          fp = fopen("SHIR_A1_PPF_10_FT.txt", "a");
          fprintf(fp, "\nFail Time = %3d", fail_time);
          fclose(fp);
          
          WAIT = 1;
        }
        else
        {
          // Maintain positions until end of simulation
          wb_motor_set_velocity(rmz, -UpdatePos(SPID_Z, x_pos, goal_x));
          wb_motor_set_velocity(rmy, UpdatePos(SPID_Y, y_pos, goal_y));
          wb_motor_set_velocity(rmxft, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_ft), MAXDIST));
          wb_motor_set_velocity(rmxfb, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fb), MAXDIST));
          wb_motor_set_velocity(rmxfl, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fl), MAXDIST));
          wb_motor_set_velocity(rmxfr, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fr), MAXDIST));
        }
      }
    }
  }
  wb_robot_cleanup();
  return(0);
}

double UpdatePos(struct SPID pid, double current_pos, double desired_pos)
{
  double error = desired_pos - current_pos;
  double pTerm, iTerm, dTerm;
  pTerm = pid.pGain * error;
  pid.iState += error;
  iTerm = pid.iGain * pid.iState;
  dTerm = pid.dGain  * (error - pid.dState);
  pid.dState = error;
  double result = pTerm + iTerm + dTerm;
  if(result >= 10.0) return(10.0);
  else if(result <= -10.0) return(-10.0);
  else return(result);
}