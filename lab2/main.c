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
#define MxM 0
#define MxV 1
#define VxV 2
#define MATRIX_ROWS 1

double *create_array(int rows, int columns){
  double*t=malloc(rows*columns*sizeof(double));

  for(int i=0;i<rows*columns;i++){
    t[i]=rand()%10;
  }
  return t;
}

void printa(double *t,int rows,int columns){
  for(int r=0;r<rows;r++){
    for(int c=0;c<columns;c++){
      printf (" %g", t[r*columns+c]);
    }
    printf ("\n");
  }
  printf ("\n");
}

// --------------------------------------------------------measure time for vector o vector ---------------------------------------------------------------------------------------------------
long int measure_time_vv(int r){
 

  double *t1=create_array(r,1);
  double *t2=create_array(r,1);
  gsl_vector_view T1 = gsl_vector_view_array(t1, r);
  gsl_vector_view T2 = gsl_vector_view_array(t2, r);
  double scalar=-1;

  struct timeval start;
  gettimeofday(&start , NULL); 
  gsl_blas_ddot(&T1.vector,&T2.vector,&scalar);
  struct timeval end;
  gettimeofday(&end , NULL);
  long int micros = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  
  // printa(t1,r,1);
  // printa(t2,r,1);
  // printf("scalar=%f\n",scalar);

  free(t1);free(t2);
  // printf("real: %ld us\n", micros);
  return micros;
}

// --------------------------------------------------------measure time for matrix x vector ---------------------------------------------------------------------------------------------------
 

long int measure_time_mv(int r1, int c1){

  double *t1=create_array(r1,c1);
  double *t2=create_array(c1,1);
  double *t=create_array(r1,1);

  for(int i=0;i<r1;i++){
    t[i]=0;
  }

  gsl_matrix_view T1 = gsl_matrix_view_array(t1, r1, c1);
  gsl_vector_view T2 = gsl_vector_view_array(t2, c1);
  gsl_vector_view T = gsl_vector_view_array(t,r1);

  struct timeval start;
  gettimeofday(&start , NULL);  
  gsl_blas_dgemv(CblasNoTrans,1.0,&T1.matrix,&T2.vector,0.0,&T.vector);
  struct timeval end;
  gettimeofday(&end , NULL);
  long int micros = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  // printf("real: %ld us\n", micros);

  // printa(t1,r1,c1);
  // printa(t2,c1,1);
  // printa(t,r1,1);


  free(t);free(t1);free(t2);
  return micros;
}

// --------------------------------------------------------measure time for matrix x matrix ---------------------------------------------------------------------------------------------------

long int measure_time_mm(int r1,int c1,int c2){

  double *t1=create_array(r1,c1);
  double *t2=create_array(c1,c2);
  double *t=create_array(r1,c2);
  for(int i=0;i<r1*c2;i++){
    t[i]=0;
  }
   
  gsl_matrix_view T1 = gsl_matrix_view_array(t1, r1, c1);
  gsl_matrix_view T2 = gsl_matrix_view_array(t2, c1, c2);
  gsl_matrix_view T = gsl_matrix_view_array(t,r1,c2);

  struct timeval start;
  gettimeofday(&start , NULL);  
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,&T1.matrix,&T2.matrix,0.0,&T.matrix);
  struct timeval end;
  gettimeofday(&end , NULL);
  long int micros = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  // printf("real: %ld us\n", micros);

  // printa(t1,r1,c1);
  // printa(t2,c1,c2);
  // printa(t,r1,c2);

  free(t);free(t1);free(t2);
  return micros;
  
}

//---------------------------------------------------------------------------------------main-------------------------------------------------------------------------------------------

void measure_and_save(int rows){
  long int t1=measure_time_vv(rows);
  long int t2=measure_time_mv(MATRIX_ROWS,rows);
  printf("%d;%ld;%ld\n",rows,t1,t2);
}

int main (void)
{

  int fd;
  fd = open("../results.csv",O_WRONLY|O_CREAT|O_TRUNC,0644); //O_TRUNC removes whole content of file if it already exists
  if(fd==-1) FAILURE_EXIT("Failed opening or creating results.csv");

  int savestdout = dup2(fd,1);
  if(savestdout==-1) FAILURE_EXIT("Failed to set stdout to file");


  printf("vector_size;vector_vector;matrix_vector\n");

  for(int i=0;i<10;i++){measure_and_save(2);}
  for(int i=0;i<10;i++){measure_and_save(30);}
  for(int i=0;i<10;i++){measure_and_save(100);}
  for(int i=0;i<10;i++){measure_and_save(500);}
  for(int i=0;i<10;i++){measure_and_save(1000);}
  for(int i=0;i<10;i++){measure_and_save(10000);}
  for(int i=0;i<10;i++){measure_and_save(100000);}
  for(int i=0;i<10;i++){measure_and_save(500000);}
  for(int i=0;i<10;i++){measure_and_save(850000);}
  for(int i=0;i<10;i++){measure_and_save(1000000);}
  for(int i=0;i<10;i++){measure_and_save(3000000);}
  

  return 0;
}
