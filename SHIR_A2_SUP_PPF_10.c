/*
 * File:          SHIR_A2_SUP_PPF_10.c
 * Date:          03/04/2019
 * Description:   Supervisor for SHIR Approach 2 simulation monitoring and control
 * Author:        Matthew Haire
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <webots/robot.h>
#include <webots/supervisor.h>

// ROBOTS redefined from 20 to 18
#define ROBOTS 18
#define TIME_STEP 16
#define INIT_Y_POS 19.5

int main(int argc, char **argv)
{
  //ROB4 and ROB12 removed
  const char *robot_num[ROBOTS] = { "ROB1", "ROB2", "ROB3", "ROB5", "ROB6", "ROB7", "ROB8", "ROB9", "ROB10", "ROB11", "ROB13", "ROB14", "ROB15", "ROB16", "ROB17", "ROB18", "ROB19", "ROB20" };

  WbNodeRef node;
  WbFieldRef robot_translation_field[ROBOTS], robot_rotation_field[ROBOTS];

  int i = 0, j = 0, measure = 0, count = 0;
  double overlap = 0.0, total_overlap = 0.0, gap = 0.0, total_gap = 0.0;
  double temp[3] = { 0.0, 0.0, 0.0 };
  double diff_dist[ROBOTS];
  double robot_initial_translation[ROBOTS][3] = {
      {-20.0, INIT_Y_POS, 8.0},
      {-17.7, INIT_Y_POS, 8.0},
      {-15.4, INIT_Y_POS, 8.0},
      //{-13.1, INIT_Y_POS, 8.0},
      {-10.8, INIT_Y_POS, 8.0},
      {-8.5, INIT_Y_POS, 8.0},
      {-6.2, INIT_Y_POS, 8.0},
      {-3.9, INIT_Y_POS, 8.0},
      {-1.6, INIT_Y_POS, 8.0},
      {0.7, INIT_Y_POS, 8.0},
      {3.0, INIT_Y_POS, 8.0},
      //{5.3, INIT_Y_POS, 8.0},
      {7.6, INIT_Y_POS, 8.0},
      {9.9, INIT_Y_POS, 8.0},
      {12.2, INIT_Y_POS, 8.0},
      {14.5, INIT_Y_POS, 8.0},
      {16.8, INIT_Y_POS, 8.0},
      {19.1, INIT_Y_POS, 8.0},
      {21.4, INIT_Y_POS, 8.0},
      {23.7, INIT_Y_POS, 8.0},
      };
  double robot_initial_rotation[ROBOTS][4] = {
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      //{-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      //{-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      {-0.0767, 0.9941, 0.0758, 4.69809},
      };

  double robot_ordered[ROBOTS][3];
  double robot_previous[ROBOTS][3];

  const double *robot_translation[ROBOTS];

  int length = 0, batch_num = 0;
    // Read last digit of specified text file to determine batch number
    FILE *fp2;
    fp2 = fopen("SHIR_A2_PPF_10.txt", "r");
    if (fp2 == NULL)
    {
      batch_num = 1;
    }
    else
    {
      fseek(fp2, 0, SEEK_END);
      length = ftell(fp2);
      fseek(fp2, (length - 2), SEEK_SET);
      fscanf(fp2, "%2d", &batch_num);
      fclose(fp2);
      batch_num += 1;
    }

  wb_robot_init();

  // Set initial translation and rotation of robots
  for (i = 0; i < ROBOTS; i++)
  {
    node = wb_supervisor_node_get_from_def(robot_num[i]);
    robot_translation_field[i] = wb_supervisor_node_get_field(node,"translation");
    wb_supervisor_field_set_sf_vec3f(robot_translation_field[i], robot_initial_translation[i]);
    robot_rotation_field[i] = wb_supervisor_node_get_field(node,"rotation");
    wb_supervisor_field_set_sf_rotation(robot_rotation_field[i], robot_initial_rotation[i]);
  }

  for(i=0; i <ROBOTS; i++)
  {
    robot_previous[i][0] = robot_initial_translation[i][0];
    robot_previous[i][1] = robot_initial_translation[i][1];
    robot_previous[i][2] = robot_initial_translation[i][2];
  }

  while(wb_robot_step(TIME_STEP)!=-1)
  {
    for (i = 0; i < ROBOTS; i++)
    {
      robot_translation[i]=wb_supervisor_field_get_sf_vec3f(robot_translation_field[i]);
      if(robot_translation[i][2] >= 9 || robot_translation[i][2] <= -7 || robot_translation[i][1] >= 22 || robot_translation[i][1] <= 12)
      {
        robot_ordered[i][0] = -1000;
        robot_ordered[i][1] = -1000;
        robot_ordered[i][2] = -1000;
      }
      else
      {
        robot_ordered[i][0] = robot_translation[i][0];
        robot_ordered[i][1] = robot_translation[i][1];
        robot_ordered[i][2] = robot_translation[i][2];
      }
    }

    // sort all the robots from largest to smallest value position here
    for(i = 0; i < ROBOTS; i++)
    {
      for(j = 0; j < (ROBOTS - 1); j++)
      {
        if(robot_ordered[j][0] < robot_ordered[j+1][0])
        {
          temp[0] = robot_ordered[j][0];
          temp[1] = robot_ordered[j][1];
          temp[2] = robot_ordered[j][2];
          robot_ordered[j][0] = robot_ordered[j+1][0];
          robot_ordered[j][1] = robot_ordered[j+1][1];
          robot_ordered[j][2] = robot_ordered[j+1][2];
          robot_ordered[j+1][0] = temp[0];
          robot_ordered[j+1][1] = temp[1];
          robot_ordered[j+1][2] = temp[2];
        }
      }
    }

    //start collecting measurment values
    for(i = 0; i < ROBOTS; i++)
    {
      temp[0] = robot_ordered[i][0];
      temp[1] = robot_ordered[i][1];
      temp[2] = robot_ordered[i][2];
      if(temp[2] > -1000)
      {
        if(sqrt(pow((robot_previous[i][1] - temp[1]), 2.0)+pow((robot_previous[i][2] - temp[2]), 2.0)) >= 0.5) measure++;
      }
    }

    if(measure >= 10) // if more than half the agents have moved more than 0.5 meters, take measurement
    {
      for (i = 0; i < (ROBOTS - 1); i++)
      {
        // Assumed that robots successfully maintain a distance of 2m from ship hull.
        if(robot_ordered[i+1][0] > -500 && robot_ordered[i+1][1] > -500 && robot_ordered[i+1][2] > -500)
        {
          diff_dist[i] = pow((robot_ordered[i][0] - robot_ordered[i + 1][0]), 2.0);
          diff_dist[i] = sqrt(diff_dist[i]);
          if ((4.0 - diff_dist[i]) > 0.0) overlap += (diff_dist[i] * 4.0);
          else
          {
            gap -= (diff_dist[i] * 4.0); // don't record where the gap occurs yet
          }
        }
      }
      count++;
      total_overlap += overlap;
      total_gap += gap;
      measure = 0;
      overlap = 0;
      gap = 0;
      for(i=0; i <ROBOTS; i++)
      {
        robot_previous[i][0] = robot_ordered[i][0];
        robot_previous[i][1] = robot_ordered[i][1];
        robot_previous[i][2] = robot_ordered[i][2];
      }
    }
    else
    {
      measure = 0;
    }

    if(robot_translation[9][0] >= 7.0 && robot_translation[9][1] >= 18.5 && robot_translation[9][2] >= 7.5)  // dependant on ROB3 position
    {
      total_overlap = total_overlap / 8.0;  // Divide cumulitive total to adjust for sampling rate (2 samples per meter traversed)
      total_gap = total_gap / 8.0; // Divide cumulitive total to adjust for sampling rate (2 samples per meter traversed)
      // Save results of experiment to same text file specified at top of program
      FILE * fp;
      fp = fopen("SHIR_A2_PPF_10.txt", "a");
      fprintf(fp, "\nOverlap: %5.2f   Gaps: %5.2f   Time: %5.2f   Number: %5d", total_overlap, total_gap, wb_robot_get_time(), batch_num);
      fclose(fp);

      // Following section needs delagated to a bash script outside of Webots
      if(batch_num < 10) wb_supervisor_world_reload();// reset the experiment from the begining
      else wb_supervisor_simulation_quit(EXIT_SUCCESS); // quit simulation once batch complete
    }
  }
  wb_robot_cleanup();
  return 0;
}