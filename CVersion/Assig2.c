//CITS3402 - Assignment 2 
//Name:  Joshua Radich
//StudentNo: 22744833

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "mpi.h"

void setArrayToZero(int* arr, int size);
void initialiseArray(int*** arr, int size);
void loadFile(int** dataArray, int vertices, FILE* f);
void printArray(int** arr, int len);
int processArray(int vertices, int startVert, int current, int* doneList, int** resultArray, int** dataArray);
int getLowestThatIsntDone(int startVert,int** resultArray, int* doneArray, int vertices);
void printArrayToFile(char* fileName,int vertices, int** arr);
void getMinAndMax(int vertices,int procs, int rank, int* min, int* max);

int main(int argc, char** argv){
	
	MPI_Init(&argc,&argv); //begin

	int vertices=0, min=0, max=0;
	int* doneList;
	int** resultArray;
	int** dataArray;
	int** finalArray;
	clock_t begin, end;	
	
	int rank,total;
	MPI_Request request;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank); //get rank of current node
	MPI_Comm_size(MPI_COMM_WORLD, &total); // get total number of nodes

	if(rank==0){  //head node
		FILE* f;
		if(argc < 2){ // check if there's a file name
			printf("File name needs to be supplied");
			exit(0);
		}	
	
		f= fopen(argv[1],"rb");
		if(f==NULL){  // try to open supplied file 
			printf("Unable to open file\n");
			exit(0);
		}

		fread(&vertices,sizeof(int),1,f); //get number of vertices from file  
		MPI_Bcast(&vertices,1,MPI_INT,0,MPI_COMM_WORLD);  //send number of vertices to all nodes
		initialiseArray(&dataArray,vertices); //initialise data array
		initialiseArray(&finalArray,vertices); //initialise final results array
		loadFile(dataArray,vertices,f);  // load data into array from file 
		fclose(f);  //close file 
		begin = clock();  // begin timing 
		printf("Calculating...\n");
		MPI_Bcast(&(dataArray[0][0]),vertices*vertices,MPI_INT,0,MPI_COMM_WORLD); //send data array to all nodes
		
	}
	else{  //for all other available nodes
		MPI_Bcast(&vertices,1,MPI_INT,0,MPI_COMM_WORLD); //receive number of vertices from head node 
		initialiseArray(&dataArray,vertices); //all other nodes initialise data array 
		MPI_Bcast(&(dataArray[0][0]),vertices*vertices,MPI_INT,0,MPI_COMM_WORLD); //receive data array
	}
	
	// all nodes including head node 
	doneList = malloc(sizeof(int)*vertices);  //initialise done list array
	setArrayToZero(doneList,vertices);        //set it to zero
	initialiseArray(&resultArray,vertices);     //initialise temp results array
	getMinAndMax(vertices,total,rank,&min,&max); // get verticies that node is going to process depending on rank 

	if(max>=min){  // if max is less than min, that node has not been used as there were more nodes than vertices
		int next = 0;
		for(int i=min;i<=max;i++){ //each node processes their assigned rows 
			next = i;
			while(next != -1){  //process path for each point 
				next = processArray(vertices,i,next,doneList,resultArray,dataArray);
			}
			setArrayToZero(doneList,vertices); //reset done list for next point 
		}

		int len = (max - min + 1)*vertices;  //stitch final result array together at head node. 
		MPI_Gather(&(resultArray[min][0]),len,MPI_INT,&(finalArray[min][0]),len,MPI_INT,0,MPI_COMM_WORLD);
	}

	if(rank==0){  //head node only 
		end = clock();
		double procTimeTaken = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("Completed in: %f\n",procTimeTaken);  //calculate time taken
		printArrayToFile(argv[1],vertices,finalArray); //print array to file 
	}
	
	MPI_Finalize();  //finish 
}

//based on the rank, the number of nodes and the number of vertices, returns 
//what vertices the node will be processing. 
//tries to divide vertices out amoungst nodes as evenly as possible. 
void getMinAndMax(int vertices,int procs, int rank, int* min, int* max){
	int toAssign = vertices;
	int assigned = 0;	
	assigned = toAssign / procs;  //divide verticies amoungst nodes
	toAssign = toAssign % procs;  // get remainder 
	if(rank < toAssign){  // divide remainder out evenly 
		assigned++;
		*min = rank * assigned;
		*max = *min + assigned -1;
	}
	else{
		*min = rank * assigned + toAssign;
		*max = *min + assigned - 1;
	}
}

//processes path length between the 'current' vertex and all other vertices.  The vertex length that is the 
//shortest will then be selected and used until all vertices are exhausted. 
int processArray(int vertices, int startVert, int current, int* doneList, int** resultArray, int** dataArray){
	int val=0;
	for(int i=0;i<vertices;i++){
		if(i!=startVert){ //dont caclulate between start vertex and vertex being processed 
			if(doneList[i]!=1){  // if vertex hasn't been visited yet 
				if(dataArray[current][i] != 0){  // if the distance from current to i isn't 0  (no path) 
					val = resultArray[startVert][current] + dataArray[current][i];  //make value what's currently recorded from start  to current + current to i 
					if(resultArray[startVert][i] == 0 || val < resultArray[startVert][i]){  //is this is less than what's recorded for start to i then replace 
						resultArray[startVert][i] = val;
					}
				}
			}
		}
	}
	
	doneList[current] =1; // get current node as done 
	int next = getLowestThatIsntDone(startVert,resultArray,doneList,vertices); //get path with the lowest value or -1 if none are available 
	return next;
}

//gets the shortest direct path from the available vertices from the start nodes to another.  
//returns -1 if all vertices have been exhausted. 
int getLowestThatIsntDone(int startVert,int** resultArray, int* doneArray, int vertices){
	int lowest = -1;
	int node = -1;
	for(int i=0; i<vertices; i++){
		if(doneArray[i] != 1){ //check if vertex has been processed 
			if(resultArray[startVert][i] !=0){ // check if path is valid 
				if(lowest==-1||resultArray[startVert][i]<lowest){
					lowest = resultArray[startVert][i];  //save as lowest 
					node = i;
				}
			}
		}
	}
	return node;
}

//initialises a 2d array but using a 1d grid so that parts of the 2d array can easily be sent using MPI 
void initialiseArray(int*** incArr, int size){
	int fullSize = size * size;
	int** arr = malloc(sizeof(int*)*size);  //create 2d array of pointers 
	int* data = (int*)malloc(sizeof(int)*fullSize);  // create 1d array thats contiguous 
	
	for(int i=0;i<fullSize;i++){ // set values in 1d array to 0
		data[i]=0;
	}

	for(int i=0; i<size; i++){  //for each row cell in 2d array, assign it the address of the 1d array that correlates to the start of the row 
		arr[i] = &(data[size*i]);
	}
	*incArr = arr;	// assign 2d array to array sent to function 
}

//sets 1d array values to 0 
void setArrayToZero(int* arr, int size){
	for(int i=0;i<size;i++){
		arr[i]=0;
	}
}

//assigns numbers in file to a 2d array 
void loadFile(int** resultArray, int vertices, FILE* f){
	
	int val;
	for(int i=0;i<vertices;i++){
		for(int j=0;j<vertices;j++){
			if(fread(&val,sizeof(int),1,f)!=1){  
				printf("Not enough values in file\n");
				exit(0);
			}
			resultArray[i][j] = val;
		}
	}
}

//prints 2d array to screen.  Was used for testing 
void printArray(int** arr, int len){
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d ",arr[i][j]);
		}
		printf("\n");
	}
}

//opens a file and prints array to that file 
void printArrayToFile(char* fileName,int vertices, int** arr){
	char outName[100];
	int i=0;
	
	while(fileName[i] != '.'){ //find dot(.) in incoming file name first to make the same named .out file. 
		outName[i] = fileName[i];
		i++;
	}
	outName[i] = '\0';
	strcat(outName,".out"); //add .out to file name. 
	
	FILE* f= fopen(outName,"w"); //open file. 
	if(f==NULL){
		printf("Unable to create output file\n");
		exit(0);
	}
	
	fprintf(f,"%d\n",vertices);  //print vertices to file first 
	for(int i=0;i<vertices;i++){
		for(int j=0;j<vertices;j++){  // print array to file. 
			fprintf(f,"%d ",arr[i][j]);
		}
		fprintf(f,"\n");
	}
	fclose(f);	
}
