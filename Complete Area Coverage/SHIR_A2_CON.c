/*
* File:          SHIR_A2_CON.c
* Date:          25/03/2019
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Macros used throughout program */
#define TIME_STEP 16.0
#define WATERLINE 19.0
#define MAXDIST 2.00
#define ASSEMBLY_TIME 2.00

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
  /*
  const WbDeviceTag ps_t = wb_robot_get_device("ps_t");
  wb_distance_sensor_enable(ps_t, TIME_STEP);
  const WbDeviceTag ps_b = wb_robot_get_device("ps_b");
  wb_distance_sensor_enable(ps_b, TIME_STEP);
  const WbDeviceTag ps_l = wb_robot_get_device("ps_l");
  wb_distance_sensor_enable(ps_l, TIME_STEP);
  const WbDeviceTag ps_r = wb_robot_get_device("ps_r");
  wb_distance_sensor_enable(ps_r, TIME_STEP);
  */
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
  SPID_X.pGain = 1;   // increase speed of responce
  SPID_X.iGain = 100; // reduce the error caused by gravity pulling object down 
  SPID_X.dGain = -85;   // Remove oscillation
  SPID_X.iState = 0;
  SPID_X.dState = 0.001;
  SPID_Y = SPID_X;
  SPID_Y.pGain = 2;  
  SPID_Z = SPID_Y;
  
  /* Local variables */
  double x_pos = 0.0, y_pos = 0.0, z_pos = 0.0;
  double goal_x = 0.0, goal_y = 0.0;
  bool SEARCH_COMPLETE = 0, DOWN = 1, UP = 0;
  
  /* Main loop */
  while (wb_robot_step(TIME_STEP) != -1)
  {
    x_pos = wb_gps_get_values(gps)[0];
    y_pos = wb_gps_get_values(gps)[1];
    z_pos = wb_gps_get_values(gps)[2];

    // This approach does not require an assembly protocol
    if(wb_robot_get_time() <= TIME_STEP)
    {
      goal_x = x_pos;
    }
    
    if(!SEARCH_COMPLETE)
    {
      // If agent goes out of bounds, deactivate.
      if(x_pos <= -21.0 || x_pos >= 26.0 || y_pos <= 12.0 || y_pos >= 21.0 || z_pos >= 9.0 || z_pos <= -7.0)
      {
        wb_motor_set_velocity(rmxft, 0.0);
        wb_motor_set_velocity(rmxfb, 0.0);
        wb_motor_set_velocity(rmxfl, 0.0);
        wb_motor_set_velocity(rmxfr, 0.0);
        wb_motor_set_velocity(rmy, 0.0);
        wb_motor_set_velocity(rmz, 0.0);
        break;
      }
      else
      {      
        //printf("DOWN: %d UP: %d\n", DOWN, UP);
        // Maintain distance of 2.0m from ship hull
        wb_motor_set_velocity(rmxft, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_ft), MAXDIST)/2);
        wb_motor_set_velocity(rmxfb, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fb), MAXDIST)/2);
        wb_motor_set_velocity(rmxfl, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fl), MAXDIST)/2);
        wb_motor_set_velocity(rmxfr, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fr), MAXDIST)/2);
        // MOVE DOWN      
        if(DOWN && !UP)
        {
          wb_motor_set_velocity(rmy, -5.0);
          wb_motor_set_velocity(rmz, -UpdatePos(SPID_Z, x_pos, goal_x));
          if(z_pos < 0.0 && y_pos >= WATERLINE)
          {
            UP = 1;
            goal_x = (x_pos + 1.0);
          }
        }
        // MOVE FORWARD
        if(DOWN && UP)
        {
          wb_motor_set_velocity(rmy, -UpdatePos(SPID_Y, y_pos, WATERLINE));
          wb_motor_set_velocity(rmz, -5.0);
          if(x_pos >= goal_x)
          {
            DOWN = 0;
            goal_x = x_pos;
          }
        }
        // MOVE UP
        if(!DOWN && UP)
        {
          wb_motor_set_velocity(rmy, 5.0);
          wb_motor_set_velocity(rmz, -UpdatePos(SPID_Z, x_pos, goal_x));
          if(z_pos > 0.0 && y_pos >= WATERLINE)
          {
            UP = 0;
            DOWN = 0;
            goal_x = (x_pos + 1.0);
          }
        }
        // MOVE FORWARD
        if(!DOWN && !UP)
        {
          wb_motor_set_velocity(rmy, UpdatePos(SPID_Y, y_pos, WATERLINE));
          wb_motor_set_velocity(rmz, -5.0);
          if(x_pos >= goal_x)
          {
            DOWN = 1;
            goal_x = x_pos;
          }
        }
        // SEARCH COMPLETE
        //This section requires editing for PPF experiments
        if(x_pos >= 27.0)
        {
          SEARCH_COMPLETE = 1;
          goal_x = x_pos + 1.0;
          goal_y = WATERLINE;
        }
      }
    }
    else
    {
      break;
      // ASSEMBLE AT THE FRONT OF THE SHIP
      //wb_motor_set_velocity(rmxft, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_ft), MAXDIST));
      //wb_motor_set_velocity(rmxfb, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fb), MAXDIST));
      //wb_motor_set_velocity(rmxfl, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fl), MAXDIST));
      //wb_motor_set_velocity(rmxfr, UpdatePos(SPID_X, wb_distance_sensor_get_value(ds_fr), MAXDIST));
      //wb_motor_set_velocity(rmy, UpdatePos(SPID_Y, y_pos, goal_y));
      //wb_motor_set_velocity(rmz, -UpdatePos(SPID_Z, x_pos, goal_x));
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
  return(result);
}
