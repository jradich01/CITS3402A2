#include<stdio.h>
#include<stdlib.h>



void initialiseArray(int*** arr, int size);
void loadFile(int** nodeArray, int nodes);

int main(int argc, char** argv){
	
	int nodes =4;
	int doneList[nodes];
	int** nodeArray;
	int** dataArray;
	
	initialiseArray(&nodeArray,nodes);
	initialiseArray(&dataArray,nodes);
	
	loadFile(nodeArray,nodes);
	printf("Worx!! %d \n",nodeArray[0][0]);
	
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

void loadFile(int** nodeArray, int nodes){
	int sumNum = 0;
	FILE* f = fopen("4.in","rb");
	fread(&sumNum,sizeof(int),1,f);
	fread(&sumNum,sizeof(int),1,f);
	fclose(f);
	printf("Num: %d",sumNum);
	
}