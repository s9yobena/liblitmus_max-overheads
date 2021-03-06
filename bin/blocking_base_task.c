/* based_task.c -- A basic real-time task skeleton. 
 *
 * This (by itself useless) task demos how to setup a 
 * single-threaded LITMUS^RT real-time task.
 */

/* First, we include standard headers.
 * Generally speaking, a LITMUS^RT real-time task can perform any
 * system call, etc., but no real-time guarantees can be made if a
 * system call blocks. To be on the safe side, only use I/O for debugging
 * purposes and from non-real-time sections.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Second, we include the LITMUS^RT user space library header.
 * This header, part of liblitmus, provides the user space API of
 * LITMUS^RT.
 */
#include "litmus.h"

/* Next, we define period and execution cost to be constant. 
 * These are only constants for convenience in this example, they can be
 * determined at run time, e.g., from command line parameters.
 */
/* #define PERIOD		 2000000000 */
/* #define EXEC_COST	 0.0000001 */

/* Catch errors.
 */
#define CALL( exp ) do { \
		int ret; \
		ret = exp; \
		if (ret != 0) \
			fprintf(stderr, "%s failed: %m\n", #exp);\
		else \
			fprintf(stderr, "%s ok.\n", #exp); \
	} while (0)


/* Declare the periodically invoked job. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int this_rt_id;
int job(int*,int*,int);

/* typically, main() does a couple of things: 
 * 	1) parse command line parameters, etc.
 *	2) Setup work environment.
 *	3) Setup real-time parameters.
 *	4) Transition to real-time mode.
 *	5) Invoke periodic or sporadic jobs.
 *	6) Transition to background mode.
 *	7) Clean up and exit.
 *
 * The following main() function provides the basic skeleton of a single-threaded
 * LITMUS^RT real-time task. In a real program, all the return values should be 
 * checked for errors.
 */
int main(int argc, char** argv)
{
	int do_exit;
	int PERIOD, EXEC_COST;
	int sCounter;
	int counter;
	int pCounter;
	int ret;

	/* The task is in background mode upon startup. */		


	/*****
	 * 1) Command line paramter parsing would be done here.
	 */
	if (argc !=4){
	  fprintf(stderr,
		  "Usage: base_task EXEC_COST PERIOD COUNTER \n COUNTER: print info every COUNTER times called \n");
	  exit(1);
	}
	EXEC_COST = atoi(argv[1]);
	PERIOD = atoi(argv[2]);
	sCounter = atoi(argv[3]);

       
	/*****
	 * 2) Work environment (e.g., global data structures, file data, etc.) would
	 *    be setup here.
	 */
	counter = 0;
	pCounter = 0;

	

	/*****
	 * 3) Setup real-time parameters. 
	 *    In this example, we create a sporadic task that does not specify a 
	 *    target partition (and thus is intended to run under global scheduling). 
	 *    If this were to execute under a partitioned scheduler, it would be assigned
	 *    to the first partition (since partitioning is performed offline).
	 */
	CALL( init_litmus() );
	CALL( sporadic_global(EXEC_COST, PERIOD) );

	/* To specify a partition, use sporadic_partitioned().
	 * Example:
	 *
	 *		sporadic_partitioned(EXEC_COST, PERIOD, CPU);
	 *
	 * where CPU ranges from 0 to "Number of CPUs" - 1.
	 */



	/*****
	 * 4) Transition to real-time mode.
	 */
	CALL( task_mode(LITMUS_RT_TASK) );

	/* The task is now executing as a real-time task if the call didn't fail. 
	 */

	this_rt_id = gettid();
	ret = wait_for_ts_release();

	/*****
	 * 5) Invoke real-time jobs.
	 */
	do {
		/* Wait until the next job is released. */
		sleep_next_period();
		/* Invoke job. */
		do_exit = job(&counter, &pCounter, sCounter );		
	} while (!do_exit);


	
	/*****
	 * 6) Transition to background mode.
	 */
	CALL( task_mode(BACKGROUND_TASK) );



	/***** 
	 * 7) Clean up, maybe print results and stats, and exit.
	 */
	return 0;
}



/* the job basically increments pCounter every time it is invoked.
 * every sCounter times, the job prints a message  */
int job(int *counter, int *pCounter, int sCounter) 
{
	/* Do real-time calculation. */
  /* FILE *pf; */
  /* pf = fopen("task-logf.tsk","a"); */
  (*pCounter)++;
  if (*pCounter == sCounter) {
    *pCounter = 0;
    (*counter)++;
    printf("rt_id: %d \t nbr. times called: %d \n",this_rt_id, (*counter)*sCounter );
    /* fprintf(pf, "rt_id: %d \t nbr. times called: %d \n",this_rt_id, (*counter)*sCounter ); */
  }
  usleep(10);

	/* Don't exit. */
	return 0;
}
