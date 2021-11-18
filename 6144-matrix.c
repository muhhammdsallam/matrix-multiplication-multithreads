#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 1000 

 
int r1,c1,r2,c2;
int matrix1[MAX][MAX];
int matrix2[MAX][MAX];
int matrix3Elements[MAX][MAX];
int matrix3Rows[MAX][MAX];
clock_t start1, end1;
double time1;
clock_t start2, end2;
double time2;


void *elementByElement(void *arg){
  
   int *arr = (int *)arg;
   int i = arr[0];
   int j = arr[1];
   for(int k=0;k<c1;k++){
         matrix3Elements[i][j] += matrix1[i][k] * matrix2[k][j];
   }
   
}

void *mulRows(void *arg){
   
   int i = *((int *)arg);
   for(int j=0;j<c2;j++){
       for(int k=0;k<c1;k++){
          matrix3Rows[i][j] += matrix1[i][k] * matrix2[k][j];
       }
   }
 
}

void executeThreadsForElements(){

  //we need r1*c2 threads
  //Multiplication by thread for every element
  pthread_t thread[r1][c2];
  start1 = clock();
  for(int i=0;i<r1;i++){
       for(int j=0;j<c2;j++){
           int *arr = malloc(2*sizeof(int));
           arr[0] = i;
           arr[1] = j;
           pthread_create(&thread[i][j],NULL,elementByElement,arr); 
       }
  }
  for (int i = 0; i < r1; i++){
      for(int j=0;j<c2;j++){
            pthread_join(thread[i][j], NULL);
        }
    }
   end1 = clock();
   time1 = ((double) (end1 - start1)) / CLOCKS_PER_SEC; 
}

void executeThreadsForRows(){
   
   //we need r1 threads
   //Multiplication by thread for every row
  pthread_t th[r1];
  start2 = clock();     
  for(int i=0;i<r1;i++){
    int *row;
    row = malloc(sizeof(*row));
    *row = i;
    pthread_create(&th[i],NULL,mulRows,(void *)row);
  }
  for(int i=0;i<r1;i++){
            pthread_join(th[i], NULL);
        }
  end2 = clock();
  time2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
}

void writeOutput(){

  FILE *outputFile;
  outputFile = fopen("output.txt", "w");
  if (outputFile == NULL){
        printf("Output File Not Found\n");
        exit(EXIT_FAILURE);
        }
  else {
    
    fprintf(outputFile,"By Element\n");
    for(int i=0;i<r1;i++){
      for(int j=0;j<c2;j++){
         fprintf(outputFile,"%d\t",matrix3Elements[i][j]);
      }
      fprintf(outputFile,"\n");
    }
   fprintf(outputFile,"Time:   %f s\n",time1);
   fprintf(outputFile,"By Row\n");
    for(int i=0;i<r1;i++){
      for(int j=0;j<c2;j++){
         fprintf(outputFile,"%d\t",matrix3Rows[i][j]);
      }
      fprintf(outputFile,"\n");
    }
    fprintf(outputFile,"Time:   %f s\n",time2);
    fclose(outputFile);

}          
}


int main(int argc, char *argv[]){

//read the matrices file

FILE *file;
file = fopen(argv[1], "r");
if (file == NULL){
        printf("File Not Found\n");
        exit(EXIT_FAILURE);
        }
else {

    fscanf(file, "%d", &r1);
    fscanf(file, "%d", &c1);
    
    for(int i=0;i<r1;i++){
      for(int j=0;j<c1;j++){
         fscanf(file,"%d",&matrix1[i][j]);
      }
    }
    
    fscanf(file, "%d", &r2);
    fscanf(file, "%d", &c2);
    
    for(int i=0;i<r2;i++){
      for(int j=0;j<c2;j++){
         fscanf(file,"%d",&matrix2[i][j]);
      }
    }
    
    fclose(file);

}      
if(c1!=r2){
  
  FILE *outputFile;
  outputFile = fopen("output.txt", "w");
  if (outputFile == NULL){
        printf("Output File Not Found\n");
        exit(EXIT_FAILURE);
        }
  else {
  fprintf(outputFile,"Multipication of Matrix is not Possible !!");
  fclose(outputFile);
  exit(EXIT_FAILURE);
  }

}
else{
  executeThreadsForElements(); 
  executeThreadsForRows();
  writeOutput();          
}
return 0;
}
