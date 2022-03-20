/**
 * @file	scheduler.c
 * @author  Eriks Zaharans and Massimiiliano Raciti
 * @date    1 Jul 2013
 *
 * @section DESCRIPTION
 *
 * Cyclic executive scheduler library.
 */

/* -- Includes -- */
/* system libraries */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <assert.h>
/* project libraries */
#include "scheduler.h"
#include "task.h"
#include "timelib.h"
#include "def.h"
/* -- Defines -- */

#define     t_TASK_MISSION                  1
#define     t_TASK_NAVIGATE                 10
#define     t_TASK_CONTROL                  50
#define     t_TASK_REFINE                   49
#define     t_TASK_REPORT                   15
#define     t_TASK_COMMUNICATE              12
#define     t_TASK_AVOID                    50

static double deadline_overruns[8]  = {0};
int tasks_cnt[8] = {0};
struct victims_data_struct
{
	char *id; // Victim ID
	int x;	// Victim X position
	int y;	// Victim Y position
};

struct victims_data_struct victims_data[24] = {
												{"020058F5BD", 340, 340},
											   {"020053A537", 975, 1115},
											   {"020053E0BA", 1845, 925},
											   {"01004B835E", 2670, 355},
											   {"020053C80E", 3395, 870},
											   {"020058100D", 4645, 910},
											   {"0200580B96", 4800, 250},
											   {"02005345B6", 5395, 1060},
											   {"020058F121", 5830, 1895},
											   {"0200581B9E", 5110, 2390},
											   {"020058066F", 5770, 3790},
											   {"020058212D", 4500, 3190},
											   {"020058022D", 4315, 3200},
											   {"0200581542", 4150, 1810},
											   {"0200534E5C", 3720, 3710},
											   {"020053AB2C", 2580, 3770},
											   {"01004A11E8", 2970, 2805},
											   {"020053E282", 3030, 2070},
											   {"0200553505", 3120, 1965},
											   {"01004751A2", 2880, 1840},
											   {"02005097C0", 1890, 2580},
											   {"020053BF78", 985, 3020},
											   {"020056D0EF", 730, 3175},
											   {"01004BDF7B", 320, 1800}};
/* -- Functions -- */

/**
 * Initialize cyclic executive scheduler
 * @param minor Minor cycle in miliseconds (ms)
 * @return Pointer to scheduler structure
 */
scheduler_t *scheduler_init(void)
{
	// Allocate memory for Scheduler structure
	scheduler_t *ces = (scheduler_t *) malloc(sizeof(scheduler_t));

	return ces;
}

/**
 * Deinitialize cyclic executive scheduler
 * @param ces Pointer to scheduler structure
 * @return Void
 */
void scheduler_destroy(scheduler_t *ces)
{
	// Free memory
	free(ces);
}

/**
 * Start scheduler
 * @param ces Pointer to scheduler structure
 * @return Void
 */
void scheduler_start(scheduler_t *ces)
{
	// Set timers
	timelib_timer_set(&ces->tv_started);
	timelib_timer_set(&ces->tv_cycle);
}

int task_deadlines(int task_id)
{

    switch (task_id)
    {
        case 1 :// Mission
            return t_TASK_MISSION;
        case 2:// Navigate
            return t_TASK_NAVIGATE;
        case 3:// Control
            return t_TASK_CONTROL;
        case 4:// Refine
            return t_TASK_REFINE;
        case 5:// Report
            return t_TASK_REPORT;
        case 6:// Communicate
            return t_TASK_COMMUNICATE;
        case 7:// Avoid
            return t_TASK_AVOID;
        default:// Wrong
            return -1;
    }
}
/**
 * Wait (sleep) till end of minor cycle
 * @param ces Pointer to scheduler structure
 * @return Void
 */
void scheduler_wait_for_timer(scheduler_t *ces)
{
	int sleep_time; // Sleep time in microseconds

	// Calculate time till end of the minor cycle
	sleep_time = (ces->minor * 1000) - (int)(timelib_timer_get(ces->tv_cycle) * 1000);

	// Add minor cycle period to timer
	timelib_timer_add_ms(&ces->tv_cycle, ces->minor);

	// Check for overrun and execute sleep only if there is no
	if(sleep_time > 0)
	{
		// Go to sleep (multipy with 1000 to get miliseconds)
		usleep(sleep_time);
	}
}

/**
 * Execute task
 * @param ces Pointer to scheduler structure
 * @param task_id Task ID
 * @return Void
 */
void scheduler_exec_task(scheduler_t *ces, int task_id)
{
	switch(task_id)
	{
	// Mission
	case s_TASK_MISSION_ID :
		task_mission();
		break;
	// Navigate
	case s_TASK_NAVIGATE_ID :
		task_navigate();
		break;
	// Control
	case s_TASK_CONTROL_ID :
		task_control();
		break;
	// Refine
	case s_TASK_REFINE_ID :
		task_refine();
		break;
	// Report
	case s_TASK_REPORT_ID :
		task_report();
		break;
	// Communicate
	case s_TASK_COMMUNICATE_ID :
		task_communicate();
		break;
	// Collision detection
	case s_TASK_AVOID_ID :
		task_avoid();
		break;
	// Other
	default :
		// Do nothing
		break;
	}
}

float victim_distance_mea(char *id, int x, int y)
{
	for (int i = 0; i < 24; i++)
	{
		if (strcmp(id, victims_data[i].id) == 0)
		{
			return sqrt(pow(victims_data[i].x - abs(x), 2) +
						pow(victims_data[i].y - abs(y), 2));
		}
	}
}

void combined_task(int task_id, struct timeval *timer, scheduler_t *ces)
{
    int deadline;
    double exec_time;
		++tasks_cnt[task_id];
		timelib_timer_set(timer);
   // Execute the task
    scheduler_exec_task(ces,task_id);
    // Obtain execution time
    exec_time = timelib_timer_get(*timer);
    // Fetch deadline
    deadline = task_deadlines(task_id);
    // Check for deadline overrun: if detected, add up counter; if not,
    // just sleep until the deadline
/*if(task_id == 1){
//printf("%d\n", task_id);
printf("%f\n", exec_time);
}*/
		if (exec_time > deadline)
    {
        ++deadline_overruns[task_id];
				//printf("%d\n", task_id);
				//printf("%f\n", deadline_overruns[task_id]/tasks_cnt[task_id]);
    }
  	if (task_id == 6){
		//printf("victim sent : %d\n", bytes_vit_cnt);
		//printf("robot sent : %d\n", bytes_robot_cnt);
		//printf("pheromone sent :%d\n", bytes_pher_cnt);
		//printf("stream sent :%d\n", bytes_stream_cnt);


	}
}



/**
 * Run scheduler
 * @param ces Pointer to scheduler structure
 * @return Void
 */


void scheduler_run(scheduler_t *ces)
{
	/* --- Local variables (define variables here) --- */
//signal(SIGINT, stopping_signal);   // triggered by ctrl+c
double now ,deff_time;
int old_cnt = 0;
struct timeval task_exec_time;
	/* --- Set minor cycle period --- */
	ces->minor = 125;

	/* --- Write your code here --- */

	now = timelib_unix_timestamp()/1000;  //timelib_unix_timestamp() in ms
	deff_time = ceil(now) -now; //in seconds
	usleep(deff_time * 1e6);
	scheduler_start(ces);


//for (int u=1; u<=5; u++){
while(1){
	for (int i=1; i<9; i++){
		//timelib_timer_set(&task_exec_time);
		if (i == 2 || i ==6 ){

			combined_task(s_TASK_MISSION_ID ,&task_exec_time,ces);
			combined_task(s_TASK_REFINE_ID ,&task_exec_time,ces);
			combined_task(s_TASK_NAVIGATE_ID ,&task_exec_time,ces);
			combined_task(s_TASK_REPORT_ID ,&task_exec_time,ces);
			combined_task(s_TASK_AVOID_ID ,&task_exec_time,ces);
			scheduler_wait_for_timer(ces);
			}
			else if(i ==3 || i ==7 ){

				combined_task(s_TASK_REFINE_ID ,&task_exec_time,ces);
				combined_task(s_TASK_REPORT_ID ,&task_exec_time,ces);
				combined_task(s_TASK_CONTROL_ID ,&task_exec_time,ces);
				combined_task(s_TASK_AVOID_ID ,&task_exec_time,ces);
				scheduler_wait_for_timer(ces);
				}
			else if (i == g_config.robot_id){

			combined_task(s_TASK_COMMUNICATE_ID ,&task_exec_time,ces);
			combined_task(s_TASK_AVOID_ID ,&task_exec_time,ces);
			scheduler_wait_for_timer(ces);
			}

			else {

				combined_task(s_TASK_REFINE_ID ,&task_exec_time,ces);
				combined_task(s_TASK_REPORT_ID ,&task_exec_time,ces);
				combined_task(s_TASK_AVOID_ID ,&task_exec_time,ces);
				scheduler_wait_for_timer(ces);
			}
		}

		/*if (g_task_mission_data.victim_count > old_cnt)
		{
			old_cnt++;
			float total_distance = 0;
			for (int j = 0; j < g_task_mission_data.victim_count; j++)
			{
				float distance = victim_distance_mea(g_task_mission_data.victims[j].id,
												 g_task_mission_data.victims[j].x,
												 g_task_mission_data.victims[j].y);
				total_distance += distance;
				printf("Victim id : %s x : %d y : %d distance : %f\n",
						 g_task_mission_data.victims[j].id,
						 g_task_mission_data.victims[j].x,
						 g_task_mission_data.victims[j].y, distance);
			}
			printf("Average distance of  victims = %f\n", total_distance /
																g_task_mission_data.victim_count);
		}*/
	} //end of while loop
}
