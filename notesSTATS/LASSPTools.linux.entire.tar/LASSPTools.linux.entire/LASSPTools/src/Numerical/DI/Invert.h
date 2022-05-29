# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include "linDI.h"

#define TRUE 1
#define FALSE 0

void flag_error();

int  timing=FALSE;

#ifndef _NO_PROTO

typedef int (*GetMatrix)();
typedef void (*PutMatrix)();

void get_options(int argc, char **argv, FILE **Ain,  FILE **detinv_out,
		 DetInvFctn *det_fctn, GetMatrix *matrix_in,
		 PutMatrix *matrix_out, int *type);

#else

typedef int (*GetMatrix)();
typedef void (*PutMatrix)();

void get_options();

#endif
