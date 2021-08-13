// include hedders
#include<stdio.h>
#include<stdlib.h>

void main(){
	
	//initializing variables
	int n, i=0;
	double X,Y;
	double sumX=0;
	double sumX2=0;
	double sumY=0;
	double sumXY=0;
	double a;
	double b;
	char *filename = malloc (sizeof(char)*100),ch;

	//asking for the file name
	printf("Enter File name to import: ");
	scanf("%s",filename);
	FILE* numfile = fopen(filename,"r"); //opening the file
	free(filename);

	//read error check
	if (numfile==NULL){
	printf("Error importing File. Try Again... \n");
	exit(0);
	}
	
	// finding number of lines in the fil
	while((ch=fgetc(numfile))!=EOF){
		if (ch == ','){
			n++;
		}
		
	}

	// get the pointer to the start of the file
	rewind(numfile);

	//aranging numbers to X and Y arrays
	double x [n];
	double y [n];

	//reading the file and aranging to 2 arrays	
	for (i = 0; i < n; i++)
	{
		fscanf(numfile,"%lf,%lf",&x[i],&y[i]);
		
	}
	fclose(numfile);
	
	//for (i=0;i<n;i++){
		//printf("x=%lf y=%lf\n",x[i],y[i]);
	//}
	

	//printf("%d\n",n );
	//Calculating sums
	for(i=0;i<n;i++){

  		sumX = sumX + x[i]; // calculating the sumX
  		sumX2 = sumX2 + x[i]*x[i]; // calculating sum ^ 2
  		sumY = sumY + y[i]; // calculating sumY
  		sumXY = sumXY + x[i]*y[i]; // calculating sumX*Y
 	}

	//Calculating a and b (using the linear formular)
	b = (n*sumXY-sumX*sumY)/(n*sumX2-sumX*sumX); //  calculating a 
 	a = (sumY - b*sumX)/n; // calculating b 

	//Displaing results
 	printf("Values : a=%lf and b = %lf",a,b);
 	printf("\nbest fit is: y = %lf + %lfx\n",a,b); 
	
	//using the value of x calculating y 
	printf("Enter X value to calculate Y: ");
	scanf("%lf",&X);
	Y = a+b*X;
	printf("Y value is %lf \n",Y);
	
}

