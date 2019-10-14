#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "mpi.h"

void setArrayToZero(int* arr, int size);
void initialiseArray(int*** arr, int size);
void loadFile(int** nodeArray, int nodes, FILE* f);
void printArray(int** arr, int len);
int processArray(int nodes, int startNode, int current, int* doneList, int** nodeArray, int** dataArray);
int getLowestThatIsntDone(int startNode,int** nodeArray, int* doneArray, int nodes);
void printArrayToFile(char* fileName,int nodes, int** arr);
void getMinAndMax(int nodes,int procs, int rank, int* min, int* max);

int main(int argc, char** argv){
	
	MPI_Init(&argc,&argv);

	int nodes=0, min=0, max=0;
	int* doneList;
	int** nodeArray;
	int** dataArray;
	clock_t begin, end;	
	
	int rank,total;
	MPI_Request request;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD, &total);

	

	if(rank==0){
		FILE* f;
		if(argc < 2){
			printf("File name need to be supplied");
			exit(0);
		}	
	
		f= fopen(argv[1],"rb");
		if(f==NULL){
			printf("Unable to open file\n");
			exit(0);
		}

		fread(&nodes,sizeof(int),1,f);
		MPI_Bcast(&nodes,1,MPI_INT,0,MPI_COMM_WORLD);  //send number of nodes to everyone
		initialiseArray(&dataArray,nodes);
		loadFile(dataArray,nodes,f);
		fclose(f);
		MPI_Bcast(&(dataArray[0][0]),nodes*nodes,MPI_INT,0,MPI_COMM_WORLD); //send data array
		printf("Calculating...\n");
		begin = clock();
	}
	else{
		MPI_Bcast(&nodes,1,MPI_INT,0,MPI_COMM_WORLD); //receive number of nodes
		initialiseArray(&dataArray,nodes);
		MPI_Bcast(&(dataArray[0][0]),nodes*nodes,MPI_INT,0,MPI_COMM_WORLD); //receive array
	}
	
	printf("Rank: %d working\n",rank);
	doneList = malloc(sizeof(int)*nodes);  //all ranks do this
	setArrayToZero(doneList,nodes);        //
	initialiseArray(&nodeArray,nodes);     //
	getMinAndMax(nodes,total,rank,&min,&max);

	printf("I'm rank: %d and I'm doing from %d to %d\n",rank,min,max);

	if(max>=min){  // if max is less than min, theres more procs than nodes.
		int next = 0;
		for(int i=min;i<=max;i++){
			next = i;
			while(next != -1){
				next = processArray(nodes,i,next,doneList,nodeArray,dataArray);
			}
			setArrayToZero(doneList,nodes);
		}
		printf("Rank %d worked\n",rank);
	}

	
	/*
	int next = 0;
	for(int i=0;i<nodes;i++){	
		next = i;
		while(next != -1){
			next= processArray(nodes,i,next,doneList,nodeArray,dataArray);
		}
		setArrayToZero(doneList,nodes);
	} 


	if(rank==0){
		end = clock();
		double procTimeTaken = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("Completed in: %f\n",procTimeTaken);
		printArrayToFile(argv[1],nodes,nodeArray);
	}
	*/
	MPI_Finalize();
}

void getMinAndMax(int nodes,int procs, int rank, int* min, int* max){
	int toAssign = nodes;
	int assigned = 0;
	
	
	assigned = toAssign / procs;
	toAssign = toAssign % procs;
	if(rank < toAssign){
		assigned++;
		*min = rank * assigned;
		*max = *min + assigned -1;
	}
	else{
		*min = rank * assigned + toAssign;
		*max = *min + assigned - 1;
	}
}

int processArray(int nodes, int startNode, int current, int* doneList, int** nodeArray, int** dataArray){
	int val=0;
	for(int i=0;i<nodes;i++){
		if(i!=startNode){
			if(doneList[i]!=1){
				if(dataArray[current][i] != 0){
					val = nodeArray[startNode][current] + dataArray[current][i];
					if(nodeArray[startNode][i] == 0 || val < nodeArray[startNode][i]){
						nodeArray[startNode][i] = val;
					}
				}
			}
		}
	}
	
	doneList[current] =1;
	int next = getLowestThatIsntDone(startNode,nodeArray,doneList,nodes);
	return next;
}

int getLowestThatIsntDone(int startNode,int** nodeArray, int* doneArray, int nodes){
	int lowest = -1;
	int node = -1;
	for(int i=0; i<nodes; i++){
		if(doneArray[i] != 1){
			if(nodeArray[startNode][i] !=0){
				if(lowest==-1||nodeArray[startNode][i]<lowest){
					lowest = nodeArray[startNode][i];
					node = i;
				}
			}
		}
	}
	return node;
}

void initialiseArray(int*** incArr, int size){
	int fullSize = size * size;
	int** arr = malloc(sizeof(int*)*size);
	int* data = (int*)malloc(sizeof(int)*fullSize);
	
	for(int i=0;i<fullSize;i++){
		data[i]=0;
	}

	for(int i=0; i<size; i++){
		arr[i] = &(data[size*i]);
	}
	*incArr = arr;	
}

void setArrayToZero(int* arr, int size){
	for(int i=0;i<size;i++){
		arr[i]=0;
	}
}

void loadFile(int** nodeArray, int nodes, FILE* f){
	
	int tot = nodes * nodes;
	int val;
	for(int i=0;i<nodes;i++){
		for(int j=0;j<nodes;j++){
			if(fread(&val,sizeof(int),1,f)!=1){
				printf("Not enough values in file\n");
				exit(0);
			}
			nodeArray[i][j] = val;
		}
	}
}

void printArray(int** arr, int len){
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d ",arr[i][j]);
		}
		printf("\n");
	}
}

void printArrayToFile(char* fileName,int nodes, int** arr){
	char outName[100];
	int i=0;
	while(fileName[i] != '.'){
		outName[i] = fileName[i];
		i++;
	}
	outName[i] = '\0';
	strcat(outName,".out");
	
	FILE* f= fopen(outName,"w");
	if(f==NULL){
		printf("Unable to create output file\n");
		exit(0);
	}
	
	fprintf(f,"%d\n",nodes);
	for(int i=0;i<nodes;i++){
		for(int j=0;j<nodes;j++){
			fprintf(f,"%d ",arr[i][j]);
		}
		fprintf(f,"\n");
	}
	fclose(f);	
}
