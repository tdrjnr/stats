/*--------------------------------------------------------------------
 * 3D-2D.h
 */

#include <strings.h>
#include <stdio.h>
#include "line_io.h"
#define TRUE 1
#define FALSE 0

#define MAXLENGTH 2000

#ifndef _NO_PROTO
   int parse_argument( int argc, char **argv, int start, char **s,
		      char *option_flag	);
#else
   int parse_argument();
#endif

