#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "linDI.h"


#define TRUE 1
#define FALSE 0

extern char pgmname[];

/*========================================================================*/

#ifndef _NO_PROTO
void d_gen_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_gen_di_factor(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int *INFO[1];
  double det[2];

  
  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error(strcat(pgmname,": workspace realloc failed"));
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dgefa(A->data, LDA, N, Z->ipvt, INFO);
#else
  dgefa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {
#ifndef UNDERSCOREFORTRAN
    dgedi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#else
    dgedi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#endif

  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}


/*========================================================================*/

#ifndef _NO_PROTO
void d_gen_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_gen_di_condition(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  double RCOND[1];
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dgeco(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  dgeco_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

#ifndef UNDERSCOREFORTRAN
    dgedi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#else
    dgedi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#endif

  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_sym_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_sym_di_factor(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int INERT[3];
  int incx[1], incy[1];
  double *dx;
  double *dy;
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows; /* A->cols == A->rows or error results after get_matrix */
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dsifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  dsifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0) {

#ifndef UNDERSCOREFORTRAN
    dsidi(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#else
    dsidi_(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#endif

    if(*JOB & 1) {
      dx = A->data;
      *incx = *LDA;
      dy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  dy++;
	  dx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  dcopy(N,dx,incx,dy,incy);
#else
	  dcopy_(N,dx,incx,dy,incy);
#endif
	  dy += *LDA;
	  dx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_sym_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_sym_di_condition(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int INERT[3];
  int incx[1], incy[1];
  double RCOND[1];
  double *dx;
  double *dy;
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  dsico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  dsico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif

  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

#ifndef UNDERSCOREFORTRAN
    dsidi(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#else
    dsidi_(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#endif

    if(*JOB & 1) {
      dx = A->data;
      *incx = *LDA;
      dy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  dy++;
	  dx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  dcopy(N,dx,incx,dy,incy);
#else
	  dcopy_(N,dx,incx,dy,incy);
#endif
	  dy += *LDA;
	  dx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_upper_tri_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_upper_tri_di_factor(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  *INFO = 0;
  JOB[0] = JOB[0]*10; /* 100 tells dtrdi to calculate the determinant *
		       * this routine calls the linpack lower triangle*
		       * routine be cause the matrix gets transposed  *
		       * when it is passed from c to frotran          */
#ifndef UNDERSCOREFORTRAN
  dtrdi(A->data, LDA, N, det, JOB, INFO);
#else
  dtrdi_(A->data, LDA, N, det, JOB, INFO);
#endif

  if(*INFO != 0) {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_upper_tri_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_upper_tri_di_condition(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int INFO[1];
  double RCOND[1];
  int LDA[1];
  int N[1];
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
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
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }

#ifndef UNDERSCOREFORTRAN
  dtrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  dtrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

    JOB[0] = JOB[0]*10; /* 100 tells dtrdi to calculate the determinant *
			 * this routine calls the linpack lower triangle*
			 * routine be cause the matrix gets transposed  *
			 * when it is passed from c to frotran          */
#ifndef UNDERSCOREFORTRAN
    dtrdi(A->data, LDA, N, det, JOB, INFO);
#else
    dtrdi_(A->data, LDA, N, det, JOB, INFO);
#endif

    JOB[0] = JOB[0]/10;
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_lower_tri_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_lower_tri_di_factor(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  *INFO = 0;
  JOB[0] *=10;
  if(JOB[0] & 10) {
    (JOB[0])++;
  }            /* 111 tells dtrdi to calculate the inverse of the upper
		* triangular matrix because the matrix gets transposed
		* when it is passed to linpack
		* 100 tells dtrdi to calculate the determinant */
#ifndef UNDERSCOREFORTRAN
  dtrdi(A->data, LDA, N, det, JOB, INFO);
#else
  dtrdi_(A->data, LDA, N, det, JOB, INFO);
#endif

  if(*INFO != 0) {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  JOB[0] = JOB[0]/10;
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void d_lower_tri_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void d_lower_tri_di_condition(A, Z, data_out, JOB)
Matrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  double RCOND[1];
  int power;
  double det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  JOB[0] *=10;
  if(JOB[0] & 10) {
    (JOB[0])++;
  }
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
      error("DeterminantInverse: workspace realloc failed");
  }

#ifndef UNDERSCOREFORTRAN
  dtrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  dtrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif

  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

#ifndef UNDERSCOREFORTRAN
    dtrdi(A->data, LDA, N, det, JOB, INFO);
#else
    dtrdi_(A->data, LDA, N, det, JOB, INFO);
#endif

  }
  else {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0]=0;
    det[1]=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[0] = det[0];
  Z->work[1] = det[1];
}

/*========================================================================*/

/* the definitions for the complex fucntions */

/*========================================================================*/

#ifndef _NO_PROTO
void z_gen_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z,
		     FILE *data_out, int *JOB)
#else
void z_gen_di_factor(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int INFO[1];
  int LDA[1];
  int N[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zgefa(A->data, LDA, N, Z->ipvt, INFO);
#else
  zgefa_(A->data, LDA, N, Z->ipvt, INFO);
#endif

  if(*INFO == 0)

#ifndef UNDERSCOREFORTRAN
    zgedi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);    
#else
    zgedi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);    
#endif

  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_gen_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_gen_di_condition(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zgeco(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zgeco_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);

#ifndef UNDERSCOREFORTRAN
    zgedi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#else
    zgedi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#endif
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_sym_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_sym_di_factor(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  int incx[1], incy[1];
  ComplexDouble *zx;
  ComplexDouble *zy;
  int power;
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zsifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  zsifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif
  if(*INFO == 0) {
#ifndef UNDERSCOREFORTRAN
    zsidi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#else
    zsidi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#endif

    if(*JOB & 1) {
      zx = A->data;
      *incx = *LDA;
      zy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  zy++;
	  zx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  zcopy(N,zx,incx,zy,incy);
#else
	  zcopy_(N,zx,incx,zy,incy);
#endif
	  zy += *LDA;
	  zx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_sym_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_sym_di_condition(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int incx[1], incy[1];
  double RCOND[1];
  ComplexDouble *zx;
  ComplexDouble *zy;
  int power;
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N*2) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zsico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zsico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is %lf\n\n",*RCOND);
#ifndef UNDERSCOREFORTRAN
    zsidi(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#else
    zsidi_(A->data, LDA, N, Z->ipvt, det, Z->work, JOB);
#endif

    if(*JOB & 1) {
      zx = A->data;
      *incx = *LDA;
      zy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  zy++;
	  zx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  zcopy(N,zx,incx,zy,incy);
#else
	  zcopy_(N,zx,incx,zy,incy);
#endif
	  zy += *LDA;
	  zx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_hermitian_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_hermitian_di_factor(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INERT[3];
  int INFO[1];
  int i;
  int incx[1], incy[1];
  ComplexDouble *zx;
  ComplexDouble *zy;
  ComplexDouble *zz;
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zhifa(A->data, LDA, N, Z->ipvt, INFO);
#else
  zhifa_(A->data, LDA, N, Z->ipvt, INFO);
#endif
  if(*INFO == 0) {
#ifndef UNDERSCOREFORTRAN
    zhidi(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#else
    zhidi_(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#endif

    if(*JOB & 1) {
      zx = A->data;
      *incx = *LDA;
      zy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  zy++;
	  zx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  zcopy(N,zx,incx,zy,incy);
#else
	  zcopy_(N,zx,incx,zy,incy);
#endif
	  for(zz=zy;zz<zy+(*N);zz++)
	    (*zz).imaginary = -(*zz).imaginary;
	  zy += *LDA;
	  zx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/

#ifndef _NO_PROTO
void z_hermitian_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_hermitian_di_condition(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INERT[3];
  double RCOND[1];
  int i;
  int incx[1], incy[1];
  ComplexDouble *zx;
  ComplexDouble *zy;
  ComplexDouble *zz;
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Wsize = *N*2;
  }
  if(*N > Z->Isize) {
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)(*N) * sizeof(int));
    if(Z->ipvt == NULL)
      error("DeterminantInverse: workspace realloc failed");
    Z->Isize = *N;
  }

#ifndef UNDERSCOREFORTRAN
  zhico(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#else
  zhico_(A->data, LDA, N, Z->ipvt, RCOND, Z->work);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is%lf\n\n",*RCOND);
#ifndef UNDERSCOREFORTRAN
    zhidi(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#else
    zhidi_(A->data, LDA, N, Z->ipvt, det, INERT, Z->work, JOB);
#endif
  
    if(*JOB & 1) {
      zx = A->data;
      *incx = *LDA;
      zy = A->data;
      *incy = 1;
      while((--(*N))>0)
	{
	  zy++;
	  zx += *LDA;
#ifndef UNDERSCOREFORTRAN
	  zcopy(N,zx,incx,zy,incy);
#else
	  zcopy_(N,zx,incx,zy,incy);
#endif
	  for(zz=zy;zz<zy+(*N);zz++)
	    (*zz).imaginary = -(*zz).imaginary;
	  zy += *LDA;
	  zx++;
	}
    }
  }
  else {
    if(*JOB & 1) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_upper_tri_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_upper_tri_di_factor(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  *INFO = 0;
  JOB[0] *= 10; /* 100 tells ztrdi to calculate the determinant *
		 * this routine calls the linpack lower triangle*
		 * routine be cause the matrix gets transposed  *
		 * when it is passed from c to frotran          */
#ifndef UNDERSCOREFORTRAN
  ztrdi(A->data, LDA, N, det, JOB, INFO);
#else
  ztrdi_(A->data, LDA, N, det, JOB, INFO);
#endif
  if(*INFO != 0) {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_upper_tri_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_upper_tri_di_condition(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  int INFO[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  INFO[0] = 0;
  /* 0 tells ztrco to use the lower matrix. the matrix gets transposed
   *  when it is passed to linpack because fortran is column oriented
   * but c is row oriented. ie in c first comes row 1 element 1 thru
   * row 1 element n and then comes row 2 element 1. In fortran the
   * order is row 1 element 1 row 2 element 1 thru row n element 1 and 
   * then comes row 1 element 2.
   */
  JOB[0] *= 10; /* 100 tells ztrdi to calculate the determinant *
		 * this routine calls the linpack lower triangle*
		 * routine be cause the matrix gets transposed  *
		 * when it is passed from c to frotran          */
  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
  }

#ifndef UNDERSCOREFORTRAN
  ztrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  ztrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is%lf\n\n",*RCOND);
#ifndef UNDERSCOREFORTRAN
    ztrdi(A->data, LDA, N, det, JOB, INFO);
#else
    ztrdi_(A->data, LDA, N, det, JOB, INFO);
#endif
  }
  else {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_lower_tri_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_lower_tri_di_factor(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  int LDA[1];
  int N[1];
  int INFO[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  *INFO =0;
  JOB[0] *=10;
  if(JOB[0] & 10) {
    (JOB[0])++;
  }            /* 111 tells dtrdi to calculate the inverse of the upper
		* triangular matrix because the matrix gets transposed
		* when it is passed to linpack
		* 100 tells dtrdi to calculate the determinant */
#ifndef UNDERSCOREFORTRAN
  ztrdi(A->data, LDA, N, det, JOB, INFO);
#else
  ztrdi_(A->data, LDA, N, det, JOB, INFO);
#endif
  if(*INFO != 0) {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
 
#ifndef _NO_PROTO
void z_lower_tri_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out, int *JOB)
#else
void z_lower_tri_di_condition(A, Z, data_out, JOB)
ComplexDoubleMatrix *A;
WorkSpace *Z;
FILE *data_out;
int *JOB;
#endif
{
  double RCOND[1];
  int LDA[1];
  int N[1];
  int INFO[1];
  ComplexDouble det[2];

  if(A->rows != A->cols)
    error("DeterminantInverse: the LHS matrix input must be square");
  /* For a square matrix lda and n should be equal        *
   * and the matrix MUST be square to have a determinant. */

  *LDA = A->rows;
  *N = A->cols;
  INFO[0] = 0;
  /* 0 tells ztrco to use the lower matrix. the matrix gets transposed
   *  when it is passed to linpack because fortran is column oriented
   * but c is row oriented. ie in c first comes row 1 element 1 thru
   * row 1 element n and then comes row 2 element 1. In fortran the
   * order is row 1 element 1 row 2 element 1 thru row n element 1 and 
   * then comes row 1 element 2.
   */

  if(*N*2 > Z->Wsize) {
    Z->work = (double *)realloc(Z->work, (size_t)(*N*2) * sizeof(double));
    if(Z->work == NULL)
      error("DeterminantInverse: workspace realloc failed");
  }
  JOB[0] *=10;
  if(JOB[0] & 10) {
    (JOB[0])++;
  }            /* 111 tells dtrdi to calculate the inverse of the upper
		* triangular matrix because the matrix gets transposed
		* when it is passed to linpack
		* 100 tells dtrdi to calculate the determinant */
#ifndef UNDERSCOREFORTRAN
  ztrco(A->data, LDA, N, RCOND, Z->work, INFO);
#else
  ztrco_(A->data, LDA, N, RCOND, Z->work, INFO);
#endif
  if(*RCOND != 0.0) {

    fprintf(data_out,"the matrix reciprocal condition is%lf\n\n",*RCOND);
#ifndef UNDERSCOREFORTRAN
    ztrdi(A->data, LDA, N, det, JOB, INFO);
#else
    ztrdi_(A->data, LDA, N, det, JOB, INFO);
#endif
  }
  else {
    if(*JOB & 10) {
      error(strcat(pgmname,": Singular matrix encountered"));
    }
    det[0].real=0;
    det[0].imaginary=0;
    det[1].real=0;
  }
  JOB[0]=JOB[0]/10;
  Z->work[2] = det[1].real;
  Z->work[0] = det[0].real;
  Z->work[1] = det[0].imaginary;
}

/*========================================================================*/
