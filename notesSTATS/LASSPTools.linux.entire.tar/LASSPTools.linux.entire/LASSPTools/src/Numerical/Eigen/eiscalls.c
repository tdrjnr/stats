#include "eiscalls.h"

/*===========================================================================*/

#ifndef _NO_PROTO
void real_eigen(Matrix A, WorkSpace *Z, FILE *value_out, FILE *vector_out)
#else
void real_eigen(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *z;
  double *ZR;
  double *ZI;
  double *WR;
  double *WI;
  double *fv1;
  double tmp;
  int *iv1;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  z = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  WR = (double*)malloc((size_t)(40*sizeof(double)));
 *  WI = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 *  iv1 = (int*)malloc((size_t)(40*sizeof(int)));
 *  A = *create_default_matrix();
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 3*A.rows*A.cols + 3*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  z = Z->work;
  ZR = z + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  WR = ZI + A.rows * A.cols;
  WI = WR + A.rows;
  fv1 = WI + A.rows;

  if(Z->Isize < A.rows)
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)A.rows*sizeof(int));

  iv1 = Z->ipvt;

  *N = A.rows;
  *NM = A.rows;
  
  for(i=0; i<A.rows; i++){
    for(j=0; j<i; j++){
      tmp=A.data[i*(*N)+j];
      A.data[i*(*N)+j]=A.data[j*(*N)+i];
      A.data[j*(*N)+i]=tmp;
    }
  }

#ifndef UNDERSCOREFORTRAN
  balanc(NM,N,A.data,is1,is2,fv1);
  elmhes(NM,N,is1,is2,A.data,iv1);
  eltran(NM,N,is1,is2,A.data,iv1,z);
  hqr2(NM,N,is1,is2,A.data,WR,WI,z,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    balbak(NM,N,is1,is2,fv1,N,z);
#else
  balanc_(NM,N,A.data,is1,is2,fv1);
  elmhes_(NM,N,is1,is2,A.data,iv1);
  eltran_(NM,N,is1,is2,A.data,iv1,z);
  hqr2_(NM,N,is1,is2,A.data,WR,WI,z,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    balbak_(NM,N,is1,is2,fv1,N,z);
#endif

  for(i=0;i<*N;i++) {
    if(WI[i]==0) {
      for(j=0;j<*N;j++) {
	ZR[(i*(*N)+j)] = z[(i*(*N)+j)];
	ZI[(i*(*N)+j)] = 0;
      }
    }
    else {
      for(j=0;j<*N;j++) {
	ZR[(i*(*N)+j)]=z[(i*(*N)+j)];
	ZI[(i*(*N)+j)]=z[((i+1)*(*N)+j)];
      }
    }
  }
  
  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf+%lf I, ",
	    WR[i],WI[i]);
  }
  fprintf(value_out,"\n\n");
  
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void real_vectors(Matrix A, WorkSpace *Z, FILE *value_out, FILE *vector_out)
#else
void real_vectors(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *z;
  double *ZR;
  double *ZI;
  double *WR;
  double *WI;
  double *fv1;
  double tmp;
  int *iv1;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  z = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  WR = (double*)malloc((size_t)(40*sizeof(double)));
 *  WI = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 *  iv1 = (int*)malloc((size_t)(40*sizeof(int)));
 *  A = *create_default_matrix();
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 3*A.rows*A.cols + 3*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  z = Z->work;
  ZR = z + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  WR = ZI + A.rows * A.cols;
  WI = WR + A.rows;
  fv1 = WI + A.rows;

  if(Z->Isize < A.rows)
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)A.rows*sizeof(int));

  iv1 = Z->ipvt;

  *N = A.rows;
  *NM = A.rows;
  
  for(i=0; i<A.rows; i++){
    for(j=0; j<i; j++){
      tmp=A.data[i*(*N)+j];
      A.data[i*(*N)+j]=A.data[j*(*N)+i];
      A.data[j*(*N)+i]=tmp;
    }
  }

#ifndef UNDERSCOREFORTRAN
  balanc(NM,N,A.data,is1,is2,fv1);
  elmhes(NM,N,is1,is2,A.data,iv1);
  eltran(NM,N,is1,is2,A.data,iv1,z);
  hqr2(NM,N,is1,is2,A.data,WR,WI,z,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    balbak(NM,N,is1,is2,fv1,N,z);
#else  
  balanc_(NM,N,A.data,is1,is2,fv1);
  elmhes_(NM,N,is1,is2,A.data,iv1);
  eltran_(NM,N,is1,is2,A.data,iv1,z);
  hqr2_(NM,N,is1,is2,A.data,WR,WI,z,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    balbak_(NM,N,is1,is2,fv1,N,z);
#endif

  for(i=0;i<*N;i++) {
    if(WI[i]==0) {
      for(j=0;j<*N;j++) {
	ZR[(i*(*N)+j)] = z[(i*(*N)+j)];
	ZI[(i*(*N)+j)] = 0;
      }
    }
    else {
      for(j=0;j<*N;j++) {
	ZR[(i*(*N)+j)]=z[(i*(*N)+j)];
	ZI[(i*(*N)+j)]=z[((i+1)*(*N)+j)];
      }
    }
  }
  
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void real_values(Matrix A, WorkSpace *Z, FILE *value_out, FILE *vector_out)
#else
void real_values(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *WR;
  double *WI;
  double *fv1;
  double tmp;
  int *iv1;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  WR = (double*)malloc((size_t)(40*sizeof(double)));
 *  WI = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 *  iv1 = (int*)malloc((size_t)(40*sizeof(int)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 3*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  WR = Z->work;
  WI = WR + A.rows;
  fv1 = WI + A.rows;

  if(Z->Isize < A.rows)
    Z->ipvt = (int *)realloc(Z->ipvt, (size_t)A.rows*sizeof(int));

  iv1 = Z->ipvt;

  *N = A.rows;
  *NM = A.rows;

  for(i=0; i<A.rows; i++){
    for(j=0; j<i; j++){
      tmp=A.data[i*(*N)+j];
      A.data[i*(*N)+j]=A.data[j*(*N)+i];
      A.data[j*(*N)+i]=tmp;
    }
  }
  
#ifndef UNDERSCOREFORTRAN
  balanc(NM,N,A.data,is1,is2,fv1);
  elmhes(NM,N,is1,is2,A.data,iv1);
  hqr(NM,N,is1,is2,A.data,WR,WI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#else
  balanc_(NM,N,A.data,is1,is2,fv1);
  elmhes_(NM,N,is1,is2,A.data,iv1);
  hqr_(NM,N,is1,is2,A.data,WR,WI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#endif  

  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf+%lf I, ",
	    WR[i],WI[i]);
  }
  fprintf(value_out,"\n\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void symmetric_eigen(Matrix A, WorkSpace *Z, FILE *value_out,
		     FILE *vector_out)
#else
void symmetric_eigen(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *z;
  double *W;
  double *fv1;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  z = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  W = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = A.rows*(A.cols + 2);
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  z = Z->work;
  W = z + A.rows * A.cols;
  fv1 = W + A.rows;

  *N = A.rows;
  *NM = A.rows;

#ifndef UNDERSCOREFORTRAN
  tred2(NM,N,A.data,W,fv1,z);
  tql2(NM,N,W,fv1,z,ierr);
#else
  tred2_(NM,N,A.data,W,fv1,z);
  tql2_(NM,N,W,fv1,z,ierr);
#endif

  if(*ierr) error("Eispack error"); /* error if ierr != 0 */
      
  for(i=0;i<*N;i++)
    fprintf(value_out,"%lf, ",W[i]);
  fprintf(value_out,"\n\n");

  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%9.6f ", z[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}
/*===========================================================================*/

#ifndef _NO_PROTO
void symmetric_vectors(Matrix A, WorkSpace *Z, FILE *value_out,
		       FILE *vector_out)
#else
void symmetric_vectors(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *z;
  double *W;
  double *fv1;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  z = (double*)malloc((size_t)(40*40*sizeof(double)));
 *  W = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = A.rows*(A.cols + 2);
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  z = Z->work;
  W = z + A.rows * A.cols;
  fv1 = W + A.rows;

  *N = A.rows;
  *NM = A.rows;

#ifndef UNDERSCOREFORTRAN
  tred2(NM,N,A.data,W,fv1,z);
  tql2(NM,N,W,fv1,z,ierr);
#else
  tred2_(NM,N,A.data,W,fv1,z);
  tql2_(NM,N,W,fv1,z,ierr);
#endif

  if(*ierr) error("Eispack error"); /* error if ierr != 0 */
      
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%9.6f ", z[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void symmetric_values(Matrix A, WorkSpace *Z, FILE *value_out,
		      FILE *vector_out)
#else
void symmetric_values(A, Z, value_out, vector_out)
Matrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *W;
  double *fv1;
  double *fv2;
  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  W = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(40*sizeof(double)));
 *  fv2 = (double*)malloc((size_t)(40*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 3*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  W = Z->work + A.rows;
  fv1 = W + A.rows;
  fv2 = fv1 + A.rows;

  *N = A.rows;
  *NM = A.rows;

#ifndef UNDERSCOREFORTRAN
  tred1(NM,N,A.data,W,fv1,fv2);
  tqlrat(N,W,fv2,ierr);
#else
  tred1_(NM,N,A.data,W,fv1,fv2);
  tqlrat_(N,W,fv2,ierr);
#endif

  if(*ierr) error("Eispack error"); /* error if ierr != 0 */
      
  for(i=0;i<*N;i++)
    fprintf(value_out,"%lf, ",W[i]);
  fprintf(value_out,"\n\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void complex_eigen(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		   FILE *vector_out)
#else
void complex_eigen(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *AR;
  double *AI;
  double *ZR;
  double *ZI;
  double *WR;
  double *WI;
  double *fv1;
  double *fv2;
  double *fv3;

  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  WR = (double*)malloc((size_t)(space*sizeof(double)));
 *  WI = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv2 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv3 = (double*)malloc((size_t)(space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 4*A.rows*A.cols + 5*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  ZR = AI + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  WR = ZI + A.rows * A.cols;
  WI = WR + A.rows;
  fv1 = WI + A.rows;
  fv2 = fv1 + A.rows;
  fv3 = fv2 + A.rows;

  *N = A.rows;
  *NM = A.rows;
  
  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  cbal(NM,N,AR,AI,is1,is2,fv1);
  corth(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr2(NM,N,is1,is2,fv2,fv3,AR,AI,WR,WI,ZR,ZI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    cbabk2(NM,N,is1,is2,fv1,N,ZR,ZI);
#else
  cbal_(NM,N,AR,AI,is1,is2,fv1);
  corth_(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr2_(NM,N,is1,is2,fv2,fv3,AR,AI,WR,WI,ZR,ZI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    cbabk2_(NM,N,is1,is2,fv1,N,ZR,ZI);
#endif

  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf+%lf I, ",
	    WR[i],WI[i]);
  }
  fprintf(value_out,"\n\n");
  
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void complex_vectors(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		     FILE *vector_out)
#else
void complex_vectors(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *AR;
  double *AI;
  double *ZR;
  double *ZI;
  double *WR;
  double *WI;
  double *fv1;
  double *fv2;
  double *fv3;

  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  WR = (double*)malloc((size_t)(space*sizeof(double)));
 *  WI = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv2 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv3 = (double*)malloc((size_t)(space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 4*A.rows*A.cols + 5*A.rows;
  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  ZR = AI + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  WR = ZI + A.rows * A.cols;
  WI = WR + A.rows;
  fv1 = WI + A.rows;
  fv2 = fv1 + A.rows;
  fv3 = fv2 + A.rows;

  *N = A.rows;
  *NM = A.rows;
  
  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  cbal(NM,N,AR,AI,is1,is2,fv1);
  corth(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr2(NM,N,is1,is2,fv2,fv3,AR,AI,WR,WI,ZR,ZI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    cbabk2(NM,N,is1,is2,fv1,N,ZR,ZI);
#else
  cbal_(NM,N,AR,AI,is1,is2,fv1);
  corth_(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr2_(NM,N,is1,is2,fv2,fv3,AR,AI,WR,WI,ZR,ZI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    cbabk2_(NM,N,is1,is2,fv1,N,ZR,ZI);
#endif

  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void complex_values(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		    FILE *vector_out)
#else
void complex_values(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  
  double *AR;
  double *AI;
  double *WR;
  double *WI;
  double *fv1;
  double *fv2;
  double *fv3;

  int NM[1];
  int N[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  WR = (double*)malloc((size_t)(space*sizeof(double)));
 *  WI = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv2 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv3 = (double*)malloc((size_t)(space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 2*A.rows*A.cols + 5*A.rows;

  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  WR = AI + A.rows * A.cols;
  WI = WR + A.rows;
  fv1 = WI + A.rows;
  fv2 = fv1 + A.rows;
  fv3 = fv2 + A.rows;

  *N = A.rows;
  *NM = A.rows;

  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  cbal(NM,N,AR,AI,is1,is2,fv1);
  corth(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr(NM,N,is1,is2,AR,AI,WR,WI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#else
  cbal_(NM,N,AR,AI,is1,is2,fv1);
  corth_(NM,N,is1,is2,AR,AI,fv2,fv3);
  comqr_(NM,N,is1,is2,AR,AI,WR,WI,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#endif

  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf+%lf I, ",
	    WR[i],WI[i]);
  }
  fprintf(value_out,"\n\n");
  
}

/*===========================================================================*/

#ifndef _NO_PROTO
void hermitian_eigen(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		     FILE *vector_out)
#else
void hermitian_eigen(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *AR;
  double *AI;
  double *ZR;
  double *ZI;
  double *W;
  double *fv1;
  double *fm1;

  int NM[1];
  int N[1];
  int l[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  W = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fm1 = (double*)malloc((size_t)(2*space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 4*A.rows*A.cols + 4*A.rows;

  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  ZR = AI + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  W = ZI + A.rows * A.cols;
  fv1 = W + A.rows;
  fm1 = fv1 + A.rows;

  *N = A.rows;
  *NM = A.rows;

  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  htridi(NM,N,AR,AI,W,fv1,fv1,fm1);
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      ZR[i*(*N)+j] = 0.0;
    ZR[i*(*N)+i] = 1.0;
  }
  tql2(NM,N,W,fv1,ZR,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    htribk(NM,N,AR,AI,fm1,N,ZR,ZI);
#else
  htridi_(NM,N,AR,AI,W,fv1,fv1,fm1);
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      ZR[i*(*N)+j] = 0.0;
    ZR[i*(*N)+i] = 1.0;
  }
  tql2_(NM,N,W,fv1,ZR,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    htribk_(NM,N,AR,AI,fm1,N,ZR,ZI);
#endif

  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf, ", W[i]);	  
  }
  fprintf(value_out,"\n\n");
  
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void hermitian_vectors(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		       FILE *vector_out)
#else
void hermitian_vectors(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *AR;
  double *AI;
  double *ZR;
  double *ZI;
  double *W;
  double *fv1;
  double *fm1;

  int NM[1];
  int N[1];
  int l[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  ZI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  W = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fm1 = (double*)malloc((size_t)(2*space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 4*A.rows*A.cols + 4*A.rows;

  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  ZR = AI + A.rows * A.cols;
  ZI = ZR + A.rows * A.cols;
  W = ZI + A.rows * A.cols;
  fv1 = W + A.rows;
  fm1 = fv1 + A.rows;

  *N = A.rows;
  *NM = A.rows;

  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  htridi(NM,N,AR,AI,W,fv1,fv1,fm1);
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      ZR[i*(*N)+j] = 0.0;
    ZR[i*(*N)+i] = 1.0;
  }
  tql2(NM,N,W,fv1,ZR,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    htribk(NM,N,AR,AI,fm1,N,ZR,ZI);
#else
  htridi_(NM,N,AR,AI,W,fv1,fv1,fm1);
  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      ZR[i*(*N)+j] = 0.0;
    ZR[i*(*N)+i] = 1.0;
  }
  tql2_(NM,N,W,fv1,ZR,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
  else
    htribk_(NM,N,AR,AI,fm1,N,ZR,ZI);
#endif

  for(i=0;i<*N;i++) {
    for(j=0;j<*N;j++)
      fprintf(vector_out,"%lf+%lfI ", ZR[(i*(*N)+j)], ZI[(i*(*N)+j)]);
    fprintf(vector_out,"\n");
  }
  fprintf(vector_out,"\n");
}

/*===========================================================================*/

#ifndef _NO_PROTO
void hermitian_values(ComplexDoubleMatrix A, WorkSpace *Z, FILE *value_out,
		      FILE *vector_out)
#else
void hermitian_values(A, Z, value_out, vector_out)
ComplexDoubleMatrix A;
WorkSpace *Z;
FILE *value_out;
FILE *vector_out;
#endif
{
  double *AR;
  double *AI;
  double *W;
  double *fv1;
  double *fv2;
  double *fm1;

  int NM[1];
  int N[1];
  int l[1];
  int ierr[1];
  int is1[1];
  int is2[1];
  int space;
  int i,j;

/*  AR = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  AI = (double*)malloc((size_t)(space*space*sizeof(double)));
 *  W = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv1 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fv2 = (double*)malloc((size_t)(space*sizeof(double)));
 *  fm1 = (double*)malloc((size_t)(2*space*sizeof(double)));
 */

  if(A.rows != A.cols)
    error("RealEigen: can not continue");

  space = 2*A.rows*A.cols + 5*A.rows;

  if(Z->Wsize < space)
    Z->work = (double *)realloc(Z->work, (size_t)space*sizeof(double));
  AR = Z->work;
  AI = AR + A.rows * A.cols;
  W = AI + A.rows * A.cols;
  fv1 = W + A.rows;
  fv2 = fv1 + A.rows;
  fm1 = fv2 + A.rows;

  *N = A.rows;
  *NM = A.rows;

  for(i=0; i<A.rows; i++){
    for(j=0; j<A.rows; j++){
      AR[i*(*N)+j] = A.data[j*(*N)+i].real;
      AI[i*(*N)+j] = A.data[j*(*N)+i].imaginary;
    }
  }

#ifndef UNDERSCOREFORTRAN
  htridi(NM,N,AR,AI,W,fv1,fv2,fm1);
  tqlrat(N,W,fv2,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#else
  htridi_(NM,N,AR,AI,W,fv1,fv2,fm1);
  tqlrat_(N,W,fv2,ierr);
  if(*ierr) /* error if ierr != 0 */
    error("Eispack error");
#endif

  for(i=0;i<*N;i++) {
    fprintf(value_out,"%lf, ", W[i]);
  }
  fprintf(value_out,"\n\n");
}

/*===========================================================================*/

