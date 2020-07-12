//
// Naive matrix multiplication
//
 
//
// Author:  P.J. Drongowski
// Date:    10 June 2013
//
// Copyright (c) 2013 Paul J. Drongowski
//
 
#include <stdlib.h>
#include <stdio.h>

void initialize_matrices(float **matrix_a,float **matrix_b,float **matrix_r,int msize)
{
  int i, j ;
 
  for (i = 0 ; i < msize ; i++) {
    #pragma GCC ivdep
    for (j = 0 ; j < msize ; j++) {
      matrix_a[i][j] = (float) rand() / RAND_MAX ;
      matrix_b[i][j] = (float) rand() / RAND_MAX ;
      matrix_r[i][j] = 0.0 ;
    }
  }
}
 
void multiply_matrices(float **matrix_a,float **matrix_b,float **matrix_r,int msize)
{
  int i, j, k ;
 
  for (i = 0 ; i < msize ; i++) {
    for (j = 0 ; j < msize ; j++) {
      float sum = 0.0 ;
      #pragma GCC ivdep
      for (k = 0 ; k < msize ; k++) {
        sum = sum + (matrix_a[i][k] * matrix_b[k][j]) ;
      }
      matrix_r[i][j] = sum ;
    }
  }
}
 
 
void multiply_matrices_loop_interchange(float **matrix_a,float **matrix_b,float **matrix_r,int msize)
{
  int i, j, k ;
 
  for (i = 0 ; i < msize ; i++) {
   for (k = 0 ; k < msize ; k++) {
      #pragma GCC ivdep
      for (j = 0 ; j < msize ; j++) {
         matrix_r[i][j] += (matrix_a[i][k] * matrix_b[k][j]) ;
      }
    }
  }
}

int main(int argc, char* argv[])
{
  if(argc < 3){
  	printf("./sort version(1 naive,2 loop nest interchange) size\n");
	exit(0);
  }  

  int version = atoi(argv[1]);
  int msize = atoi(argv[2]);

  float **matrix_a = (float**) malloc(msize * sizeof(float*));
  float **matrix_b = (float**) malloc(msize * sizeof(float*));
  float **matrix_r = (float**) malloc(msize * sizeof(float*));

  for(int i = 0; i < msize;i++){
  	matrix_a[i] = (float *) malloc(msize * sizeof(float));
	matrix_b[i] = (float *) malloc(msize * sizeof(float));
	matrix_r[i] = (float *) malloc(msize * sizeof(float));
  }
  

  initialize_matrices(matrix_a,matrix_b,matrix_r,msize) ;
  
  if(version == 1)
  	multiply_matrices(matrix_a,matrix_b,matrix_r,msize) ;
  else
  	multiply_matrices_loop_interchange(matrix_a,matrix_b,matrix_r,msize) ;
	  
  return( EXIT_SUCCESS ) ;
}
