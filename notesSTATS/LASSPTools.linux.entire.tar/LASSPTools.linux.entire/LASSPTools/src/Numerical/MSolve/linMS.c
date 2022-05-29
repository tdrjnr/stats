#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "linMS.h"

#define TRUE 1
#define FALSE 0

/*========================================================================*/

#ifndef _NO_PROTO
void d_gen_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_gen_ms_factor(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int JOB[1];
  int LDA[1];
  int N[1];
  int OB[1];
  int incy[1];
  int i;
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows;
  *N = A->cols; /* A->cols == B->rows or error results after get_matrix */
  *OB = B->cols; 
  *incy = 1;
  *JOB=1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dgefa(A->data, LDA, N, Z->ipvt, INFO);
#else
  dgefa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
    
    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dgesl(A->data, LDA, N, Z->ipvt, Z->work, JOB);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dgesl_(A->data, LDA, N, Z->ipvt, Z->work, JOB);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/

#ifndef _NO_PROTO
void d_gen_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_gen_ms_condition(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int JOB[1];
  int i;
  int incy[1];
  int OB[1];
  double *dx;
  double RCOND[1];

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  *JOB=1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dgeco(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  dgeco_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dgesl(A->data, LDA, N, Z->ipvt, Z->work, JOB);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dgesl_(A->data, LDA, N, Z->ipvt, Z->work, JOB);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve:Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_sym_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_sym_ms_factor(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int incy[1];
  int i;
  int OB[1];
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols; 
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dsifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  dsifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dsisl(A->data, LDA, N, Z->ipvt, Z->work);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dsisl_(A->data, LDA, N, Z->ipvt, Z->work);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_sym_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_sym_ms_condition(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int incy[1];
  int i;
  int OB[1];
  double RCOND[1];
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dsico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  dsico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dsisl(A->data, LDA, N, Z->ipvt, Z->work);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dsisl_(A->data, LDA, N, Z->ipvt, Z->work);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_upper_tri_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_upper_tri_ms_factor(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  int JOB[1];
  int incy[1];
  int i;
  int OB[1];
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  JOB[0] = 10; /* 10 tells dtrsl to use the lower triangle      *
		* this routine calls the linpack lower triangle *
		* routine be cause the matrix gets transposed   *
		* when it is passed from c to frotran           */

  for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
    dcopy(N,dx,OB,Z->work,incy);
    dtrsl(A->data, LDA, N, Z->work, JOB, INFO);
    dcopy(N,Z->work,incy,dx,OB);
#else
    dcopy_(N,dx,OB,Z->work,incy);
    dtrsl_(A->data, LDA, N, Z->work, JOB, INFO);
    dcopy_(N,Z->work,incy,dx,OB);
#endif
  }
  if(*INFO != 0)
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_upper_tri_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_upper_tri_ms_condition(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  double RCOND[1];
  int LDA[1];
  int N[1];
  int JOB[1];
  int incy[1];
  int i;
  int OB[1];
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  INFO[0] = 0;
  /* 0 tells dtrco to use the lower matrix. the matrix gets transposed
   *  when it is passed to linpack because fortran is column oriented
   * but c is row oriented. ie in c first comes row 1 element 1 thru
   * row 1 element n and then comes row 2 element 1. In fortran the
   * order is row 1 element 1 row 2 element 1 thru row n element 1 and 
   * then comes row 1 element 2.
   */
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }

#ifndef UNDERSCOREFORTRAN
  dtrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  dtrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);
    
    JOB[0] = 10; /* 10 tells dtrsl to use the lower triangle      *
		  * this routine calls the linpack lower triangle *
		  * routine be cause the matrix gets transposed   *
		  * when it is passed from c to frotran           */
    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dtrsl(A->data, LDA, N, Z->work, JOB, INFO);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dtrsl_(A->data, LDA, N, Z->work, JOB, INFO);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_lower_tri_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_lower_tri_ms_factor(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int JOB[1];
  int incy[1];
  int i;
  int OB[1];
  double *dx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  JOB[0] =11; /* 11 tells dtrsl to use the upper
	       * triangular matrix because the matrix gets transposed
	       * when it is passed to linpack */

  for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
    dcopy(N,dx,OB,Z->work,incy);
    dtrsl(A->data, LDA, N, Z->work, JOB, INFO);
    dcopy(N,Z->work,incy,dx,OB);
#else
    dcopy_(N,dx,OB,Z->work,incy);
    dtrsl_(A->data, LDA, N, Z->work, JOB, INFO);
    dcopy_(N,Z->work,incy,dx,OB);
#endif
  }
  if(*INFO != 0)
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_lower_tri_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out)
#else
void d_lower_tri_ms_condition(A, B, Z, cond_out)
Matrix *A;
Matrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  int JOB[1];
  int incy[1];
  int i;
  int OB[1];
  double *dx;
  double RCOND[1];

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  INFO[0] = 1;
  /* 1 tells dtrco to use the upper matrix. the matrix gets transposed
   *  when it is passed to linpack because fortran is column oriented
   * but c is row oriented. ie in c first comes row 1 element 1 thru
   * row 1 element n and then comes row 2 element 1. In fortran the
   * order is row 1 element 1 row 2 element 1 thru row n element 1 and 
   * then comes row 1 element 2.
   */
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
  }

#ifndef UNDERSCOREFORTRAN
  dtrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  dtrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    JOB[0] =11; /* 11 tells dtrsl to use the upper
		 * triangular matrix because the matrix gets transposed
		 * when it is passed to linpack */
    
    for(dx = B->data; dx < B->data + B->cols; dx++) {
#ifndef UNDERSCOREFORTRAN
      dcopy(N,dx,OB,Z->work,incy);
      dtrsl(A->data, LDA, N, Z->work, JOB, INFO);
      dcopy(N,Z->work,incy,dx,OB);
#else
      dcopy_(N,dx,OB,Z->work,incy);
      dtrsl_(A->data, LDA, N, Z->work, JOB, INFO);
      dcopy_(N,Z->work,incy,dx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/

/* the definitions for the complex fucntions */

/*========================================================================*/

#ifndef _NO_PROTO
void z_gen_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
		     WorkSpace *Z, FILE *cond_out)
#else
void z_gen_ms_factor(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  
  for(zx = A->data; zx < A->data + *N; zx++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(LDA,zx,N,Tdata,incy);
#else
    zcopy_(LDA,zx,N,Tdata,incy);
#endif
    Tdata += *LDA;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols;
  *JOB=0;
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zgefa(Tdata, LDA, N, Z->ipvt, INFO);
#else
  zgefa_(Tdata, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
    
    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zgesl(Tdata, LDA, N, Z->ipvt, Z->work, JOB);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zgesl_(Tdata, LDA, N, Z->ipvt, Z->work, JOB);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_gen_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			WorkSpace *Z, FILE *cond_out)
#else
void z_gen_ms_condition(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  
  for(zx = A->data; zx < A->data + *N; zx++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(LDA,zx,N,Tdata,incy);
#else
    zcopy_(LDA,zx,N,Tdata,incy);
#endif
    Tdata += *LDA;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols; 
  *JOB=0;
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zgeco(Tdata, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zgeco_(Tdata, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zgesl(Tdata, LDA, N, Z->ipvt, Z->work, JOB);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zgesl_(Tdata, LDA, N, Z->ipvt, Z->work, JOB);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_sym_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
		     WorkSpace *Z, FILE *cond_out)
#else
void z_sym_ms_factor(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  int incy[1];
  int OB[1];
  int i;
  ComplexDouble *zx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zsifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  zsifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zsisl(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zsisl_(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_sym_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			WorkSpace *Z, FILE *cond_out)
#else
void z_sym_ms_condition(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int incy[1];
  int OB[1];
  int i;
  double RCOND[1];
  ComplexDouble *zx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = A->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zsico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zsico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zsisl(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zsisl_(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_hermitian_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out)
#else
void z_hermitian_ms_factor(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  int incy[1];
  int OB[1];
  int i;
  ComplexDouble *zx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  for(zx = A->data; zx < A->data + (A->rows * A->cols); zx++)
    (*zx).imaginary = -(*zx).imaginary;

  *LDA = A->rows;
  *N = A->cols;
  *OB = B->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zhifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  zhifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zhisl(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zhisl_(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_hermitian_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out)
#else
void z_hermitian_ms_condition(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int incy[1];
  int OB[1];
  int i;
  double RCOND[1];
  ComplexDouble *zx;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  for(zx = A->data; zx < A->data + (A->rows * A->cols); zx++)
    (*zx).imaginary = -(*zx).imaginary;

  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *N = A->cols;
  *OB = A->cols;
  *incy = 1;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zhico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zhico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      zhisl(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      zhisl_(A->data, LDA, N, Z->ipvt, Z->work);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_upper_tri_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out)
#else
void z_upper_tri_ms_factor(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");

  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  zx = A->data;
  for(*OB = 1; *OB <= *LDA; (*OB)++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(OB,zx,N,Tdata,incy);
#else
    zcopy_(OB,zx,N,Tdata,incy);
#endif
    Tdata += *LDA;
    zx++;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols; 
  *JOB=1;

  for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(N,zx,OB,Z->work,incy);
    ztrsl(Tdata, LDA, N, Z->work, JOB, INFO);
    zcopy(N,Z->work,incy,zx,OB);
#else
    zcopy_(N,zx,OB,Z->work,incy);
    ztrsl_(Tdata, LDA, N, Z->work, JOB, INFO);
    zcopy_(N,Z->work,incy,zx,OB);
#endif
  }
  if(*INFO != 0)
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_upper_tri_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out)
#else
void z_upper_tri_ms_condition(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  int INFO[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  zx = A->data;
  for(*OB=1; *OB <= *LDA; (*OB)++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(OB,zx,N,Tdata,incy);
#else
    zcopy_(OB,zx,N,Tdata,incy);
#endif
    Tdata += *LDA;
    zx++;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols; 
  *JOB=1;

#ifndef UNDERSCOREFORTRAN
  ztrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  ztrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);
    
    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      ztrsl(Tdata, LDA, N, Z->work, JOB, INFO);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      ztrsl_(Tdata, LDA, N, Z->work, JOB, INFO);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_lower_tri_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out)
#else
void z_lower_tri_ms_factor(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  zx = A->data;
  for(*OB=*LDA; *OB > 0; (*OB)--) {
#ifndef UNDERSCOREFORTRAN
    zcopy(OB,zx,N,Tdata,incy);
#else
    zcopy_(OB,zx,N,Tdata,incy);
#endif
    Tdata++;
    Tdata += *LDA;
    zx++;
    zx += *LDA;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols; 
  *JOB=0;

  
  for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
    zcopy(N,zx,OB,Z->work,incy);
    ztrsl(Tdata, LDA, N, Z->work, JOB, INFO);
    zcopy(N,Z->work,incy,zx,OB);
#else
    zcopy_(N,zx,OB,Z->work,incy);
    ztrsl_(Tdata, LDA, N, Z->work, JOB, INFO);
    zcopy_(N,Z->work,incy,zx,OB);
#endif
  }
  if(*INFO != 0)
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_lower_tri_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out)
#else
void z_lower_tri_ms_condition(A, B, Z, cond_out)
ComplexDoubleMatrix *A;
ComplexDoubleMatrix *B;
WorkSpace *Z;
FILE *cond_out;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  int INFO[1];
  int JOB[1];
  int OB[1];
  int incy[1];
  int i;
  void *p;
  ComplexDouble *zx;
  ComplexDouble *Tdata;

  if(A->cols != A->rows || B->rows != A->cols)
    error("MatrixSolve: the matrices can not be multiplied together");
	
  *N = A->cols*(A->rows +1);
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("MatrixSolve: work space realloc failed");
    Z->Wsize = *N*2;
  }
  *N = A->cols;
  *LDA = A->rows; /* A->cols == B->rows or error results after get_matrix */
  *incy = 1;
  p = Z->work;
  Tdata = p;
  Tdata += *N;
  zx = A->data;
  for(*OB=*LDA; *OB > 0; (*OB)--) {
#ifndef UNDERSCOREFORTRAN
    zcopy(OB,zx,N,Tdata,incy);
#else
    zcopy_(OB,zx,N,Tdata,incy);
#endif
    Tdata++;
    Tdata += *LDA;
    zx++;
    zx += *LDA;
  }
  Tdata = p;
  Tdata += *N;
  *OB = B->cols; 
  *JOB=0;

#ifndef UNDERSCOREFORTRAN
  ztrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  ztrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(cond_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);
    
    for(zx = B->data; zx < B->data + B->cols; zx++) {
#ifndef UNDERSCOREFORTRAN
      zcopy(N,zx,OB,Z->work,incy);
      ztrsl(Tdata, LDA, N, Z->work, JOB, INFO);
      zcopy(N,Z->work,incy,zx,OB);
#else
      zcopy_(N,zx,OB,Z->work,incy);
      ztrsl_(Tdata, LDA, N, Z->work, JOB, INFO);
      zcopy_(N,Z->work,incy,zx,OB);
#endif
    }
  }
  else
    error("MatrixSolve: Singular matrix encountered");
}

/*========================================================================*/
