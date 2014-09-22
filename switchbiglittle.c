#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#define FILE_NAME "switch01.dat"


struct timespec tstart={0,0}, tend={0,0};

int main (int argc, char* argv[])
{
//TODO better error checking that argv contains a valid core numbers
	if( argc != 3){
		printf("%s requires 2 arguments\n",argv[0]);
		return 0;
	}
	int core1 = atoi(argv[1]);
	int core2 = atoi(argv[2]);
	
	//file to record timing data
	FILE *record = fopen(FILENAME,"w");
	
//set inital thread association
	cpu_set_t *cpusetp;
	cpusetp = CPU_ALLOC(8);
	if (cpusetp == NULL) {
		perror("CPU_ALLOC");
		exit(EXIT_FAILURE);
	}
	CPU_ZERO(cpusetp);
	CPU_SET(core1, cpusetp);

	if( -1 == sched_setaffinity( 0, sizeof(cpu_set_t), cpusetp) ){
		printf("error 1\n");
		return 1;
	}
		


	int i;
	int j = 0;
//START TIMING
	while( j < 10000){
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		for( i=0; i< 100; i++ )
		{
			//CHANGE THREAD ASSOCIATION
			if((i%2)==0){
				CPU_ZERO(cpusetp);
				CPU_SET(core2, cpusetp);
				if( -1 == sched_setaffinity(0, sizeof(cpu_set_t), cpusetp)){
					printf("error 2\n");
					return 2;
				}
			}else{
				CPU_ZERO(cpusetp);
				CPU_SET(core1, cpusetp);
				if( -1 == sched_setaffinity(0, sizeof(cpu_set_t), cpusetp)){
					printf("error 3\n");
					return 3;
				}
			}
		}
	
		clock_gettime(CLOCK_MONOTONIC, &tend);
//END TIMING

//RECORD DATA
		fprintf(record,"%9f\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
		   ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
		printf("total time %9f\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
		   ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
		j++;
	}
	fclose(record);
	return 0;
}

