/*

*Created by Nirmal Mudiyanselage 1811342
*Use -pthread and -lm when compiling

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

//ptread data
typedef struct 
{
	int rank; // rank of the thread
	long long start; // starting bit of numbers array for the thread
	long long end; // end bit of numbers array for the thread
	long prime_nums; // how many primes found by given thred
	long *nums; // array of prime numbers for the thread
	
}tread_info;

// Initializing globel variables

tread_info* ti; // thread info array
long* nums; // numbers array of the text file


// find if a number is prime 
/*
* funtion to calculate prime numbers
* returns 1 if number is prime, 0 if a number is 1
*/
int isprime(int num){

	if (num <= 1) // number is not prime if the number is les than 1
    {
        return 0;
    }

    for (int i = 2; i <= sqrt(num); ++i) // use the squrt method to find if a number is prime 
    {
        if (num % i == 0)
        {
            return 0;
            break;
        }
    }
    return 1;
}

// thread runner method
/*
* thread funtion
*/
void* thread_runner(void* rank){
	int thread_rank = *(int*)rank;
	int j=0;
	long start = ti[thread_rank].start;
	long end = ti[thread_rank].end ;
	int prime;


	for (start; start <= end; ++start)
	{
		prime = isprime(nums[start]);

		if (prime == 1){
			ti[thread_rank].nums[j] = nums[start];
			j++;
			ti[thread_rank].prime_nums ++;
			//printf("%ld\n",nums[start] );
		}
	}
}

// main
void main(int argc, char const *argv[])
{	
	// Initializing variables
	int numthreads; // number of threads arg[1]
	long chunksize; // chunck size
	char* outputfilename = "Outputfile.txt"; // output file name 
	numthreads = atoll(argv[1]);
	long filelength=0,i,j;
	char *filename = malloc (sizeof(char)*100);
	char ch;
	
	//asking for the filename
	printf("Enter File name to import: ");
	scanf("%s",filename);
	FILE* numfile = fopen(filename,"r"); //opening the file
	
	//read error check
	if (numfile==NULL){
	printf("Error importing File. Try Again... \n");
	exit(0);
	}

	// finding number of numbers in the file
	while((ch=fgetc(numfile))!=EOF){
		if (ch == '\n'){
			filelength++;
		}
	} 
	rewind(numfile);

	// create array of tread infor and numbers
	ti = malloc(numthreads*sizeof(tread_info));
	nums = malloc(filelength * sizeof(long));
	pthread_t thread_ids[numthreads];

	//	//aranging the numbers to arrays
		for (i = 0; i < filelength; i++)
	{
		fscanf(numfile,"%ld",&nums[i]);
		
	}

	// close file 
	fclose(numfile);

    //spliting the workload according to number of threads
    chunksize = filelength/numthreads;

    for (i = 0; i <numthreads; ++i)
    {
    	if (i==0){
    		ti[i].start = i;
    		ti[i].end = i+chunksize-1;
    		continue;
    	}
    	if (i == (numthreads-1))
    	{
    		ti[i].start = ti[i-1].end+1;
    		ti[i].end = (filelength-1);
    		break;
    	}
    	ti[i].start = ti[i-1].end+1;
    	ti[i].end = ti[i].start+chunksize;
    }


    // creating treads
    for (int local_rank = 0; local_rank < numthreads; ++local_rank)
	{
		ti[local_rank].rank = local_rank;
		ti[local_rank].nums = malloc((ti[i].end-ti[i].start)*(sizeof(long)+1));
		int return_value = pthread_create(&thread_ids[local_rank],NULL,thread_runner,
			(void*)&ti[local_rank].rank);
		
		if (return_value != 0)
		{
			printf("Error occured while creating threads. value=%d\n",return_value );
		}
	}

	// join treads
	for (int local_rank = 0; local_rank < numthreads; ++local_rank)
	{
		pthread_join(thread_ids[local_rank], NULL);
	}

	// dispaying results in the CMD
	printf("number of total numbers = %ld\n",filelength );
	int totalprimes = 0;
	for (int i = 0; i < numthreads; ++i)
	{
		printf("\nThread ID =%d\n",i );
		printf("starting index= %lld ending index= %lld\n",ti[i].start,ti[i].end);
		printf("Number of primes found by the thread=%ld\n",ti[i].prime_nums );
		totalprimes = totalprimes+ti[i].prime_nums;
		//printf("%d\n",totalprimes );
	}

	// writing numbers to output file
	FILE* fp = fopen (outputfilename,"w");

	//  writing the total number of numbers to the file
	fprintf (fp, "There are %d Prime numbers\n",totalprimes);

	// writing all the numbers to a text file
	for (int i = 0; i < numthreads; ++i)
	{
		for (int z = 0; z < ti[i].prime_nums; ++z)
		{
			fprintf(fp, "%ld\n",ti[i].nums[z] );
		}
	}

	// output to the file
	printf("\nResults exported to outputfile.txt\n");

	fclose(fp);
	
	// free memory
	free (filename);
	free (ti);
	free (nums);
	
	for (int i = 0; i < numthreads; ++i)
	{
		free(ti[i].nums);
	}
	
}
