/*
 * File:          SHIR_A1_SUP_PPF_10.c
 * Date:          03/04/2019
 * Description:   Master supervisor controller for ship hull inspection robots
 *                simulation. Monitors robots and records the results of all
 *                scenarios - gaps between and overlap of robot fields of view.
 *                It is also the controller responsible for running multiple
 *                expeirments.
 * Author:        Matthew Haire
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <webots/robot.h>
#include <webots/supervisor.h>

#define ROBOTS 20
#define TIME_STEP 16
#define INIT_X_POS 24.0

int main(int argc, char **argv)
{
  const char *robot_num[ROBOTS] = { "ROB1", "ROB2", "ROB3", "ROB4", "ROB5", "ROB6", "ROB7", "ROB8", "ROB9", "ROB10", "ROB20", "ROB19", "ROB18", "ROB17", "ROB16", "ROB15", "ROB14", "ROB13", "ROB12", "ROB11" };

  WbNodeRef node;
  WbFieldRef robot_translation_field[ROBOTS], robot_rotation_field[ROBOTS];

  int i = 0, j = 0, measure = 0, count = 0;
  double overlap = 0.0, total_overlap = 0.0, gap = 0.0, total_gap = 0.0, measurement_zone = (INIT_X_POS - 0.5);
  double temp[3] = { 0.0, 0.0, 0.0 };
  double diff_dist[ROBOTS];
  // Initial translations of robots are set to assembled stage from beginning of experiment.
  double robot_initial_translation[ROBOTS][3] = {
      {INIT_X_POS, 19.4, 7.59},
      {INIT_X_POS, 18.4, 7.52},
      {INIT_X_POS, 17.5, 7.45},
      {INIT_X_POS, 16.7, 7.38},
      {INIT_X_POS, 15.8, 7.31},
      {INIT_X_POS, 14.9, 7.08},
      {INIT_X_POS, 14.2, 6.35},
      {INIT_X_POS, 14.0, 4.73},
      {INIT_X_POS, 14.0, 3.1},
      {INIT_X_POS, 14.0, 1.37},
      {INIT_X_POS, 19.4, -5.68},
      {INIT_X_POS, 18.4, -5.6},
      {INIT_X_POS, 17.5, -5.53},
      {INIT_X_POS, 16.6, -5.46},
      {INIT_X_POS, 15.6, -5.38},
      {INIT_X_POS, 14.7, -5.04},
      {INIT_X_POS, 14.0, -4.1},
      {INIT_X_POS, 14.0, -2.81},
      {INIT_X_POS, 14.0, -1.42},
      {INIT_X_POS, 14.0, -0.01},
      };
  double robot_initial_rotation[ROBOTS][4] = {
      {-0.0392, 0.998, 0.0397, -1.57},
      {-0.0398, 0.998, 0.0396, -1.57},
      {-0.039, 0.998, 0.0393, -1.57},
      {-0.0401, 0.998, 0.0405, -1.57},
      {-0.042, 0.998, 0.0424, -1.57},
      {-0.238, 0.942, 0.238, -1.64},
      {-0.477, 0.738, 0.477, -1.88},
      {-0.578, 0.581, 0.574, -2.1},
      {-0.577, 0.581, 0.574, -2.1},
      {-0.58, 0.583, 0.568, -2.11},
      {-0.711, 0.0285, 0.703, -3.09},
      {-0.709, 0.0282, 0.705, -3.09},
      {-0.707, 0.0283, 0.706, -3.09},
      {-0.706, 0.0294, 0.707, -3.09},
      {-0.706, 0.0335, 0.707, -3.08},
      {-0.685, 0.233, 0.691, -2.69},
      {-0.611, 0.492, 0.62, -2.22},
      {-0.574, 0.576, 0.582, -2.09},
      {-0.574, 0.576, 0.582, -2.09},
      {-0.571, 0.574, 0.587, -2.08}
      };

  double robot_ordered[ROBOTS][3];

  const double *robot_translation[ROBOTS];

  int length = 0, batch_num = 0;
  // Read last digit of specified text file to determine batch number
  FILE *fp2;
  fp2 = fopen("SHIR_A1_PPF_10.txt", "r");
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

  while(wb_robot_step(TIME_STEP)!=-1)
  {
    for (i = 0; i < ROBOTS; i++)
    {
      robot_translation[i]=wb_supervisor_field_get_sf_vec3f(robot_translation_field[i]);
      robot_ordered[i][0] = robot_translation[i][0];
      robot_ordered[i][1] = robot_translation[i][1];
      robot_ordered[i][2] = robot_translation[i][2];
      if(robot_ordered[i][0] <= measurement_zone) measure += 1; // take measurment once every 0.5 meters of ship hull inspected
      // Exclude out of bounds robots
      if(robot_ordered[i][2] >= 9 || robot_ordered[i][2] <= -7)
      {
        robot_ordered[i][0] = -1000;
        robot_ordered[i][1] = -1000;
        robot_ordered[i][2] = -1000;
      }
    }
    for (i = 0; i < (ROBOTS - 1); i++)
    {
      // Exclude failed or lagging robots
      if(robot_ordered[i+1][0] >= (robot_ordered[i][0] + 2.0) || robot_ordered[i][2] >= 9 || robot_ordered[i][2] <= -7)
      {
        robot_ordered[i][0] = -1000;
        robot_ordered[i][1] = -1000;
        robot_ordered[i][2] = -1000;
      }
    }
    // Sort all the robots from largest to smallest values
    for(i = 0; i < ROBOTS; i++)
    {
      for(j = 0; j < (ROBOTS - 1); j++)
      {
        if(robot_ordered[j][2] < robot_ordered[j+1][2])
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

    if(measure >= 10) // if more than half of the functioning agents pass the measurment zone, take measurement
    {
      for (i = 0; i < (ROBOTS - 1); i++)
      {
        // Assumed that robots successfully maintain a distance of 2m from ship hull.
        if(robot_ordered[i + 1][2] > - 500)
        {
          // reuse of temp variables - shouldn't interfere with sorting algorithm
          temp[1] = robot_ordered[i][1] - robot_ordered[i + 1][1];
          temp[2] = robot_ordered[i][2] - robot_ordered[i + 1][2];
          diff_dist[i] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
          diff_dist[i] = sqrt(diff_dist[i]);
          if ((4.0 - diff_dist[i]) > 0.0) overlap += (4.0 - diff_dist[i]) * 4.0; // FOV overlap calculation adjusted
          else gap += (4.0 - diff_dist[i]) * 4.0; // FOV gap calculation adjusted
        }
      }
      count++;
      total_overlap += overlap;
      total_gap += gap;
      measurement_zone = measurement_zone - 0.5;
      measure = 0;
      overlap = 0.0;
      gap = 0.0;
    }
    else
    {
      measure = 0;
    }
    	// Potentially change frame of future simulations so all calculations take place in positive zones (x, y and z axis)
    if (measurement_zone <= -19.5)
    {
      total_overlap = total_overlap / 8.0;  // Divide cumulitive total to adjust for sampling rate (2 samples per meter traversed)
      total_gap = total_gap / 8.0; // Divide cumulitive total to adjust for sampling rate (2 samples per meter traversed)
      // Save results of experiment to same text file specified at top of program
      FILE * fp;
      fp = fopen("SHIR_A1_PPF_10.txt", "a");
      fprintf(fp, "\nOverlap: %5.2f   Gaps: %5.2f   Time: %5.2f   Number: %5d", total_overlap, total_gap, wb_robot_get_time(), batch_num);
      fclose(fp);

      // Following section needs delagated to a bash script outside of Webots
      if(batch_num <= 50) wb_supervisor_world_reload();// reset the experiment from the begining
      else wb_supervisor_simulation_quit(EXIT_SUCCESS); // quit simulation once batch complete
    }
  }
  wb_robot_cleanup();
  return 0;
}
