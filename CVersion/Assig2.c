#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

void setArrayToZero(int* arr, int size);
void initialiseArray(int*** arr, int size);
void loadFile(int** nodeArray, int nodes, FILE* f);
void printArray(int** arr, int len);
int processArray(int nodes, int startNode, int current, int* doneList, int** nodeArray, int** dataArray);
int getLowestThatIsntDone(int startNode,int** nodeArray, int* doneArray, int nodes);
void printArrayToFile(char* fileName,int nodes, int** arr);

int main(int argc, char** argv){
	
	int nodes=0;
	int* doneList;
	int** nodeArray;
	int** dataArray;	
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
	doneList = malloc(sizeof(int)*nodes);
	setArrayToZero(doneList,nodes);
	initialiseArray(&nodeArray,nodes);
	initialiseArray(&dataArray,nodes);
	
	
	loadFile(dataArray,nodes,f);
	printf("Calculating...\n");
	clock_t begin = clock();
	int next = 0;
	for(int i=0;i<nodes;i++){	
		next = i;
		while(next != -1){
			next= processArray(nodes,i,next,doneList,nodeArray,dataArray);
		}
		setArrayToZero(doneList,nodes);
	}
	clock_t end = clock();
	double procTimeTaken = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("Completed in: %f\n",procTimeTaken);
	printArrayToFile(argv[1],nodes,nodeArray);
	fclose(f);
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
	int** arr = malloc(sizeof(int*)*size);
	for(int i=0; i<size; i++){
		arr[i] = (int*)malloc(sizeof(int)*size);
		for(int j=0;j<size;j++){
			arr[i][j] = 0;
		}
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