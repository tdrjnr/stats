#include <strings.h>
#include <stdio.h>
#include "line_io.h"
#include <math.h>
 
#define MAXLENGTH 1000 /* get_frame and main */

#define INITIALPOLY 500 /* initialize_frame and get_frame */
#define INITIALTEXT 30 /* initialize_frame and get_frame */
#define LONGTEXT 300 /* initialize_frame and get_frame */

#define NOSORT -1 /* parse_options and rotate_and_sort_frame */
#define X 0  /* all except main */
#define Y 1  /* all except main */
#define Z 2  /* all except main */

typedef struct { /* all */
  unsigned numpoly;
  unsigned maxpoly;
  int *num;
  int *ordering;
  double **Act[3];
  double **Rot[3];
  char **remains;
  XColor *color;
} frame;
