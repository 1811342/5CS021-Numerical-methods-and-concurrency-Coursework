/*
* include -pthread and -lm when compiling
*/
// includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

//ptread data
typedef struct $
{
	int rank; //thread rank
	long long itter_start; // starting number 
	long long itter_end; // end number 
	double sum; // sum of pi for each thread
	
}tread_info;

// Initializing globel variables
tread_info* ti; // threadinfo struct array


//calculate pi
/*
* calculate the value of pi using Leibniz formula  
*/
double cal_pi(int rank){
	double local_sum = 0;
	double num, den ;
	for (int i = ti[rank].itter_start; i <=ti[rank].itter_end; ++i)
	
	{
		num = pow(-1,i);
		den = (2*i)+1;
		local_sum = local_sum + (num/den);
	}
	return local_sum;	
}


//tread funtion 
/*
* thread funtion to
*/
void* thread_runner (void *rank) {

	int thread_rank = *(int*)rank; // casting the void pointer to an int 
	double incremented_val = 0;
	int sign;
	int temp = thread_rank;
	
	ti[thread_rank].sum = cal_pi(thread_rank);

}


//main
void main(int argc, char const *argv[])
{
	//arg check
	if (argc < 3){
		printf("Usage: %s required 2 args\n",argv[0]);
		exit(-1);
	}

	int numthreads; // number of thread arg[1]
	long long chunksize; // chunck size
	double pi; // value of pi

	// getting the tread number and number of itterations from the user
	long long itter= atoll(argv[1]);
	long long pitter =itter;
	numthreads = atoll(argv[2]); 
	chunksize = itter/numthreads;

	// create array of tread infor 
	ti = calloc(numthreads,sizeof(tread_info));
	pthread_t thread_ids[numthreads];

	// spliting the workload according to the number of threads
	for (int i = 0; i < numthreads; ++i)
	{
		if (i == 0){
			ti[i].itter_start = i* chunksize;
			ti[i].itter_end = (i*chunksize)+chunksize;
			continue;
	}
		if (i == (numthreads-1)){
			ti[i].itter_start = i* chunksize+1;
			ti[i].itter_end = itter;
			break;
		}
			ti[i].itter_start = (i* chunksize)+1;
			ti[i].itter_end = (i*chunksize)+chunksize;
	}

	// creating treads 
	for (int local_rank = 0; local_rank < numthreads; ++local_rank)
	{
		ti[local_rank].rank = local_rank;
		int return_value = pthread_create(&thread_ids[local_rank],NULL,thread_runner,
			(void*)&ti[local_rank].rank);
		if (return_value != 0)
		{
			printf("Error occured while creating threads. value=%d\n",return_value );
		}
	}

	// showing tread info in CMD
	for (int i = 0; i < numthreads; ++i)

	{
		printf("thread ID= %d\n itterations start= %lld itterations end= %lld\n\n",ti[i].rank+1,ti[i].itter_start,ti[i].itter_end);
	}
	
	// join treads
	for (int local_rank = 0; local_rank < numthreads; ++local_rank)
	{
		pthread_join(thread_ids[local_rank], NULL);
	}

	// adding the final sum of every thread to one var
	for (int local_rank = 0; local_rank < numthreads; ++local_rank)
	{
		pi += ti[local_rank].sum;
	}
	
	// printing results
	printf("value of pi is after %lld itterations using %d threads is %lf \n",pitter,numthreads,pi*4);
	
	// free mem
	free(ti);
}

