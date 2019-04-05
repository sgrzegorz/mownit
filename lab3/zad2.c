#include <stdio.h>
#include <gsl/gsl_ieee_utils.h>
#include <gsl/gsl_blas.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h> // for usleep()
#include <sys/user.h>
#include <sys/resource.h>
#include <string.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function


#define FAILURE_EXIT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));exit(-1);}
#define NAIVE 0
#define BETTER 1
#define BLAS 2

// -------------------------------------------------------- structure to hold matrix and related functions ---------------------------------------------------------------------------------------------------


typedef struct{
  double* array;
  int rows;
  int columns;
}matrix;

matrix create_matrix(int rows, int columns){
  matrix m;
  m.array=malloc(rows*columns*sizeof(double));

  for(int i=0;i<rows*columns;i++){
    m.array[i]=rand()%10;
  }
  m.rows = rows;
  m.columns = columns;
  return m;
}

void printm(matrix m){
  for(int r=0;r<m.rows;r++){
    for(int c=0;c<m.columns;c++){
      printf (" %g", m.array[r*m.columns+c]);
    }
    printf ("\n");
  }
  printf ("\n");
}

double get(matrix m,int r,int c){
  r-=1;
  c-=1;
  if(r<0 || r>= m.rows || c<0 || c>=m.columns) FAILURE_EXIT("get received incorrect arguments r=%d,c=%d\n",r+1,c+1);

  return m.array[r*m.columns+c];
}

void set(matrix m,int r, int c,double val){
  r-=1;
  c-=1;
  if(r<0 || r>= m.rows || c<0 || c>=m.columns) FAILURE_EXIT("set received incorrect arguments r=%d,c=%d\n",r+1,c+1);
  m.array[r*m.columns+c]=val;
}

void zeros(matrix m){
  for(int i=0;i<m.rows*m.columns;i++){
    m.array[i]=0;
  }
}


// -------------------------------------------------------- 3 matrix x matrix multiplication functions ---------------------------------------------------------------------------------------------------


void naive_multiplication(matrix A,matrix B){
  matrix C = create_matrix(A.rows,B.columns);
  zeros(C);

  for(int i=1;i<=A.rows;i++){
    for(int j=1;j<=B.columns;j++){
      for(int k=1;k<=A.columns;k++){
        double val = get(C,i,j) + get(A,i,k)*get(B,k,j);
        set(C,i,j,val);
      }
    }
  }
  // printm(C);
}

void better_multiplication(matrix A,matrix B){
  matrix C = create_matrix(A.rows,B.columns);
  zeros(C);
  for(int j=1;j<=B.columns;j++){
    for(int k=1;k<=A.columns;k++){
      for(int i=1;i<=A.rows;i++){    
        double val = get(C,i,j) + get(A,i,k)*get(B,k,j);
        set(C,i,j,val);
      }
    }
  }
  // printm(C);
}


void blas_multiplication(matrix A,matrix B){
  if(A.columns!=B.rows) FAILURE_EXIT("A has different number of columns than B has rows\n");

  matrix C=create_matrix(A.rows,B.columns);
  zeros(C);
   
  gsl_matrix_view a = gsl_matrix_view_array(A.array, A.rows, A.columns);
  gsl_matrix_view b = gsl_matrix_view_array(B.array, B.rows, B.columns);
  gsl_matrix_view c = gsl_matrix_view_array(C.array,C.rows,C.columns);

   
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,&a.matrix,&b.matrix,0.0,&c.matrix);
  
  // printm(C);

  free(C.array);
  
}

// -------------------------------------------------------- function that measures time for selected multiplication method on selected matrix size  ---------------------------------------------------------------------------------------------------



double gettime(int operation,int size){
  matrix A=create_matrix(size,size);
  matrix B=create_matrix(size,size);

  struct timeval start;
  gettimeofday(&start,NULL);
  switch(operation){
    case NAIVE:
      naive_multiplication(A,B);
      break;
    case BETTER:
      better_multiplication(A,B);
      break;
    case BLAS:
      blas_multiplication(A,B);
      break;
    default:
      FAILURE_EXIT("Incorrect operation\n");
      break;
  }

  struct timeval end;
  gettimeofday(&end,NULL);
  double seconds = (end.tv_sec-start.tv_sec) + (double)(end.tv_usec -start.tv_usec)/1000000;
  free(A.array); free(B.array);

  // printf("real: %Lf us\n", seconds);
  return seconds;

}


//---------------------------------------------------------------------------------------main-------------------------------------------------------------------------------------------

int main (void)
{

  // matrix A = create_matrix(1,4);  

  // matrix B = create_matrix(4,10); 
  // printm(A);
  // printm(B);
  




  int fd;
  fd = open("../notoptimalised.csv",O_WRONLY|O_CREAT|O_TRUNC,0644); //O_TRUNC removes whole content of file if it already exists
  if(fd==-1) FAILURE_EXIT("Failed opening or creating results.csv");

  int savestdout = dup2(fd,1);
  if(savestdout==-1) FAILURE_EXIT("Failed to set stdout to file");


  printf("sizes;naive;better;blas\n");


  int sizes[] = {2,10,50,100,150,200,300};
  for(int i=0;i<sizeof(sizes)/sizeof(sizes[0]);i++){
    for(int j=0;j<10;j++){
      double time1=gettime(NAIVE,sizes[i]);
      double time2=gettime(BETTER,sizes[i]);
      double time3=gettime(BLAS,sizes[i]);
      printf("%d;%f;%f;%f\n",sizes[i],time1,time2,time3);
    }
  }
  
  

  return 0;
}
