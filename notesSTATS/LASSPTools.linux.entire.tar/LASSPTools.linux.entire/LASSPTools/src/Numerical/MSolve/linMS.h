# include "lincalls.h"
# include "math_io.h"

/* This is the header file for the MatrixSolve routines that call Linpack */

#ifndef _NO_PROTO
typedef void (*SolveFctn)();

/* These are the c wrappers for the Linpack routines */

void d_gen_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);
void d_gen_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);

void d_sym_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);
void d_sym_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);

void d_upper_tri_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);
void d_upper_tri_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);

void d_lower_tri_ms_factor(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);
void d_lower_tri_ms_condition(Matrix *A, Matrix *B, WorkSpace *Z, FILE *cond_out);

void z_gen_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
		     WorkSpace *Z, FILE *cond_out);
void z_gen_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			WorkSpace *Z, FILE *cond_out);

void z_sym_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
		     WorkSpace *Z, FILE *cond_out);
void z_sym_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			WorkSpace *Z, FILE *cond_out);

void z_hermitian_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out);
void z_hermitian_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out);

void z_upper_tri_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out);
void z_upper_tri_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out);

void z_lower_tri_ms_factor(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			   WorkSpace *Z, FILE *cond_out);
void z_lower_tri_ms_condition(ComplexDoubleMatrix *A, ComplexDoubleMatrix *B,
			      WorkSpace *Z, FILE *cond_out);


#else

typedef void (*SolveFctn)();

/* These are the c wrappers for the Linpack routines */

void d_gen_ms_factor();
void d_gen_ms_condition();

void d_sym_ms_factor();
void d_sym_ms_condition();

void d_upper_tri_ms_factor();
void d_upper_tri_ms_condition();

void d_lower_tri_ms_factor();
void d_lower_tri_ms_condition();

void z_gen_ms_factor();
void z_gen_ms_condition();

void z_sym_ms_factor();
void z_sym_ms_condition();

void z_hermitian_ms_factor();
void z_hermitian_ms_condition();

void z_upper_tri_ms_factor();
void z_upper_tri_ms_condition();

void z_lower_tri_ms_factor();
void z_lower_tri_ms_condition();

#endif
