#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>


struct timespec tstart={0,0}, tend={0,0};

int main (int argc, char* argv[])
{
//TODO better error checking that argv contains a valid big core and a valid small core
	if( argc != 3){
		printf("%s requires 2 arguments\n",argv[0]);
		return 0;
	}
	int core1 = atoi(argv[1]);
	int core2 = atoi(argv[2]);
	
	cpu_set_t *cpusetp;
	cpusetp = CPU_ALLOC(4);
	CPU_ZERO(cpusetp);
	CPU_SET(2, cpusetp);
	if( -1 == sched_setaffinity( 0, sizeof(cpu_set_t), cpusetp) ){
		printf("error\n");
		return 1;
	}
	int i;
//START TIMING
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	for( i=0; i< 1000000; i++ )
	{
		if((i%2)==0){
			CPU_ZERO(cpusetp);
			CPU_SET(3, cpusetp);
			CPU_SET(1, cpusetp);
			if( -1 == sched_setaffinity(0, sizeof(cpu_set_t), cpusetp)){
				printf("error\n");
				return 2;
			}
		}else{
			CPU_ZERO(cpusetp);
			CPU_SET(2, cpusetp);
			CPU_SET(4, cpusetp);
			if( -1 == sched_setaffinity(0, sizeof(cpu_set_t), cpusetp)){
				printf("error\n");
				return 3;
			}
		}
	}
	
	clock_gettime(CLOCK_MONOTONIC, &tend);
//END TIMING

	printf("total time %9f\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
	return 0;
}
/*TIMING TOOL
	for( i=0; i<4; i++){
		if(CPU_ISSET( i, cpusetp)) printf("%i set\n",i);
	}
*/
