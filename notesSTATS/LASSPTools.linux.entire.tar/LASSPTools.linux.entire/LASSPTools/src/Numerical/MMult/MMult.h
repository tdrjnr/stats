# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include "math_io.h"

#define TRUE 1
#define FALSE 0

#ifndef UNDERSCOREFORTRAN
void dgemm();
void zgemm();

typedef int (*GetMatrix)();
typedef void (*ResultOut)();

void flag_error();
void get_options(int argc, char **argv, FILE **Ain, FILE **Bin, FILE **out,
		 int *type, GetMatrix *matrix_Ain, GetMatrix *matrix_Bin,
		 ResultOut *result_out);

void real_matrix_mult(Matrix A, Matrix B, Matrix *result);
void complex_matrix_mult(ComplexDoubleMatrix A, ComplexDoubleMatrix B,
			 ComplexDoubleMatrix *result);

void put_short_matrix(Matrix M, FILE *out);
void put_short_complex_matrix(Matrix A, FILE *out);

#else
void dgemm_();
void zgemm_();

typedef int (*GetMatrix)();
typedef void (*ResultOut)();

void flag_error();
void get_options();
void real_matrix_mult();
void complex_matrix_mult();
void put_short_complex_matrix();
void put_short_matrix();
#endif
