#include <string.h>
#include "math_io.h"
# include "lincalls.h"

/* This is the header file for the DeterminantInverse program
   that calls Linpack */

#ifndef _NO_PROTO
typedef void (*DetInvFctn)();

/* These are the c wrappers for the Linpack routines */

void d_gen_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB);
void d_gen_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB);

void d_sym_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB);
void d_sym_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out, int *JOB);

void d_upper_tri_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out,
			   int *JOB);
void d_upper_tri_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out,
			      int *JOB);

void d_lower_tri_di_factor(Matrix *A, WorkSpace *Z, FILE *data_out,
			   int *JOB);
void d_lower_tri_di_condition(Matrix *A, WorkSpace *Z, FILE *data_out,
			      int *JOB);

void z_gen_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out,
		     int *JOB);
void z_gen_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out,
			int *JOB);

void z_sym_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out,
		     int *JOB);
void z_sym_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z, FILE *data_out,
			int *JOB);

void z_hermitian_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z,
			   FILE *data_out, int *JOB);
void z_hermitian_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z,
			      FILE *data_out, int *JOB);

void z_upper_tri_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z,
			   FILE *data_out, int *JOB);
void z_upper_tri_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z,
			      FILE *data_out, int *JOB);

void z_lower_tri_di_factor(ComplexDoubleMatrix *A, WorkSpace *Z,
			   FILE *data_out,int *JOB);
void z_lower_tri_di_condition(ComplexDoubleMatrix *A, WorkSpace *Z,
			      FILE *data_out, int *JOB);


#else

typedef void (*DetInvFctn)();

/* These are the c wrappers for the Linpack routines */

void d_gen_di_factor();
void d_gen_di_condition();

void d_sym_di_factor();
void d_sym_di_condition();

void d_upper_tri_di_factor();
void d_upper_tri_di_condition();

void d_lower_tri_di_factor();
void d_lower_tri_di_condition();

void z_gen_di_factor();
void z_gen_di_condition();

void z_sym_di_factor();
void z_sym_di_condition();

void z_hermitian_di_factor();
void z_hermitian_di_condition();

void z_upper_tri_di_factor();
void z_upper_tri_di_condition();

void z_lower_tri_di_factor();
void z_lower_tri_di_condition();

#endif
