// include hedders
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include "lodepng.h"

// struct thread with infor
typedef struct 
{
	int rank;
	long start;
	long end;
	unsigned char* blured_pixels;

}tread_info;

// image data arrays
tread_info* ti; //thread info array
unsigned char** image2D; //[height][width*4];

// variables	
unsigned int width, height; //image width and height

// blur image
/*
*This function apply box blur to every pixel
*/
int image_blur(int rank, int col, int row){
	
	// blur for the first row
	if(row == 0){
		if (col == 0 || col == 1|| col == 2){
			return (image2D[row][col]+ image2D[row][col+4] + image2D[row+1][col] + image2D[row+1][col+4])/4;
		}
		if (col == ((width*4)-2) || col == ((width*4)-3) || col == ((width*4)-4) ){
			return (image2D[row][col]+ image2D[row][col-4] + image2D[row+1][col] + image2D[row+1][col-4])/4;
		}
		return (image2D[row][col]+image2D[row][col-4]+image2D[row+1][col]+image2D[row+1][col-4]+image2D[row][col+4]+image2D[row+1][col+4])/4;
	}

	// blur for the last row
	if (row == height-1){
		if (col == 0 || col == 1|| col == 2){
			return (image2D[row][col]+ image2D[row][col+4] + image2D[row-1][col] + image2D[row-1][col+4])/4;
		}
		if (col == ((width*4)-2) || col == ((width*4)-3) || col == ((width*4)-4) ){
			return (image2D[row][col]+ image2D[row][col-4] + image2D[row-1][col] + image2D[row-1][col-4])/4;
		}
		return (image2D[row][col]+image2D[row][col-4]+image2D[row-1][col]+image2D[row-1][col-4]+image2D[row][col+4]+image2D[row-1][col+4])/4;
	}
	
	// blur for the end colums
	if (col == 0 || col == 1|| col == 2){
		return (image2D[row][col]+image2D[row-1][col]+image2D[row+1][col]+image2D[row][col+4]+image2D[row-1][col+4]+image2D[row+1][col+4])/6;
	}
	if (col == ((width*4)-2) || col == ((width*4)-3) || col == ((width*4)-4) ){
		return (image2D[row][col]+image2D[row-1][col]+image2D[row+1][col]+image2D[row][col-4]+image2D[row-1][col-4]+image2D[row+1][col-4])/6;
	}

	return (image2D[row][col]+image2D[row-1][col-4]+image2D[row-1][col]+image2D[row-1][col+4]+image2D[row][col-4]+
		image2D[row][col+4]+image2D[row+1][col-4]+image2D[row+1][col]+image2D[row+1][col+4])/9;
	
}

// thread runner
/*
*This function is passed to every tread
*/
void* thread_runner(void*rank){
	int thread_rank = *(int*)rank;
	int j = 0;
	long start = ti[thread_rank].start;
	long end = ti[thread_rank].end ;

	for (start; start <= end; ++start){
		for (int i = 0; i < width*4; ++i)
		{	
			// ignoring the alpah value of the image
			if((i+1)%4==0){
				ti[thread_rank].blured_pixels[j] = image2D[start][i];
				j++;
				continue;	
			}
			
			ti[thread_rank].blured_pixels[j] = image_blur(thread_rank,i,start);
			j++;
		}
	
	}
}

/*
*main function
*/
int main(int argc, char const *argv[])
{	
	//arg error check
	if (argc < 1){
		printf("Usage: %s required 2 args\n",argv[0]);
		exit(-1);
	}
	
	// Initializing variables
	unsigned char* image; // original image
	unsigned char* blur_image; // blured image
	char* filename =malloc(1000* sizeof(char));
	unsigned int error;
	int numthreads = atoll(argv[1]);
	char* outputPNG = "output.png";


	// file name input
	printf("Enter filename:");  
	scanf("%s",filename);

	//import png
	error = lodepng_decode32_file(&image, &width, &height, filename);

	//error cheack
	if (error)
	{
		printf("error %u: %s \n", error, lodepng_error_text(error));
	}

	//Initializing array sizes dynamicly
	ti = malloc(numthreads*sizeof(tread_info)); // thread info array
	
	//dynamicly allocating 2D image array
	image2D = malloc(sizeof(char*)*height);
	for (int i = 0; i < height; ++i)
	{
		image2D[i]= malloc(sizeof(char)*width*4);
	}
	
	// blured image array
	blur_image = malloc(height*width*4*sizeof(char)); 


	// convert image array to a 2d array
	for(int row=0; row<height; row++){
		for(int col=0; col<width*4; col=col+4){
			image2D[row][col]=image[(row*width*4)+col];
			image2D[row][col+1]=image[(row*width*4)+col+1];
			image2D[row][col+2]=image[(row*width*4)+col+2];
			image2D[row][col+3]=image[(row*width*4)+col+3];
		}
	}

	/*
	// print array
	for(int row1=0; row1<height; row1++){
		for(int col1=0; col1<width*4; col1++){
			printf("%d ", image2D[row1][col1]);
			if((col1+1)%4 == 0){
				printf("| ");
			}
		}
		printf("\n");
	}
	*/
	
	
	/* 
	*spliting the work dynamicly
	* for this tast we will split the image pixel rows between threads equally.
	*/
	int chunksize = height/numthreads;
	for (int i = 0; i < numthreads; ++i)
	{
		if (i == 0){
			ti[i].start = i* chunksize;
			ti[i].end = (i*chunksize)+chunksize;
			continue;
	}
		if (i == (numthreads-1)){
			ti[i].start = i* chunksize+1;
			ti[i].end = height-1;
			break;
		}
		ti[i].start = (i* chunksize)+1;
		ti[i].end = (i*chunksize)+chunksize;
	}


	// creating threads array
	pthread_t thread_ID [numthreads];

	// creating threads
	for (int i = 0; i < numthreads; ++i)
	{
		ti[i].rank = i;
		ti[i].blured_pixels = malloc(((ti[i].end-ti[i].start)+1)*4*width*(sizeof(char)+1));
		pthread_create(&thread_ID[i],NULL,thread_runner,&ti[i].rank);
	}

	//join threads
	for (int i = 0; i < numthreads; ++i)
	{
		pthread_join(thread_ID[i],NULL);
	}
	

	// Display tread info
	printf("Total Rows=%d\n",height );
	for (int i = 0; i < numthreads; ++i)
	{
		printf("Thread ID=%d | Starting row=%ld | End row=%ld | Number of Comp=%ld\n",ti[i].rank, ti[i].start, ti[i].end,(ti[i].end+1)-(ti[i].start)  );
	}

	// combine each tread pixel to one array
	blur_image = malloc((4*height*width*(sizeof(char))));
	int range = 0;
	int blur_image_rank=0;
	
	for (int i = 0; i < numthreads; ++i)
	{
		range = (ti[i].end-ti[i].start)+1;
		for (int j = 0; i < range; ++i)
		{
			blur_image[blur_image_rank]=ti[i].blured_pixels[range];
			blur_image_rank++;
		}
	}

	//encode image
	lodepng_encode32_file(outputPNG,blur_image, width, height);
	printf("New image created as output.png\n");

	//free memory
	free (image);
	free (image2D);
	free(ti);
	
	for (int i = 0; i < numthreads; ++i)
	{
		free(ti[i].blured_pixels);
	}
	
	free(blur_image);
	
	return 0;
}
