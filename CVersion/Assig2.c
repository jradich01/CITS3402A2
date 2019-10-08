#include<stdio.h>
#include<stdlib.h>


void setArrayToZero(int* arr, int size);
void initialiseArray(int*** arr, int size);
void loadFile(int** nodeArray, int nodes, FILE* f);
void printArray(int** arr, int len);
void processArray(int nodes, int startNode, int current, int* doneList, int** nodeArray, int** dataArray);
int getLowestThatIsntDone(int startNode,int** nodeArray, int* doneArray, int nodes);

int main(int argc, char** argv){
	
	int nodes=0;
	int doneList[nodes];
	int** nodeArray;
	int** dataArray;
	
	FILE* f = fopen("4.in","rb");
	fread(&nodes,sizeof(int),1,f);
	initialiseArray(&nodeArray,nodes);
	initialiseArray(&dataArray,nodes);
	loadFile(dataArray,nodes,f);
	
	for(int i=0;i<nodes;i++){
		
		processArray(nodes,i,i,doneList,nodeArray,dataArray);
		setArrayToZero(doneList,nodes);
	}
	
	printArray(nodeArray,nodes);
	printf("Worx!! %d \n",nodeArray[0][0]);
	printf("Nodes: %d\n",nodes);
	fclose(f);
}

void processArray(int nodes, int startNode, int current, int* doneList, int** nodeArray, int** dataArray){
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
	
	if(next!=-1){
		processArray(nodes,startNode,next,doneList,nodeArray,dataArray);
	}
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
		arr[i] = (int*)malloc(sizeof(int)*4);
		for(int j=0;j<size;j++){
			arr[i][j] = 0;
		}
	}
	printf("%d\n",arr[0][0]);
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