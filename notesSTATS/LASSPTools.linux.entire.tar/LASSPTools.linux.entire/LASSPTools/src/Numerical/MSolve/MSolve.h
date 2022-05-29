# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include "linMS.h"

#define TRUE 1
#define FALSE 0

void flag_error();

int  timing=FALSE;

#ifndef _NO_PROTO

typedef int (*GetMatrix)();
typedef void (*PutMatrix)();

void get_options(int argc, char **argv, FILE **Ain, FILE **Bin, FILE **Xout,
		 SolveFctn *solve_fctn, int *type, GetMatrix *matrix_Ain,
		 GetMatrix *matrix_Bin, PutMatrix *data_out);

#else

typedef int (*GetMatrix)();
typedef void (*PutMatrix)();
void get_options();

#endif
