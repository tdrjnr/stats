/* c     Machine constants for little-endian (binary) IEEE arithmetic
   c     JPS
   c
   c     mcheps = dmach(1) = dpmpar(1) = b**(1-t) = d1mach(4) = diver
   C      DATA DIVER(1),DIVER(2) /          0, 1018167296 /
   c     minmag = dmach(2) = dpmpar(2) = b**(emin - 1) = d1mach(1) = small 
   C      DATA SMALL(1),SMALL(2) /          0,    1048576 /
   c     maxmag = dmach(3) = dpmpar(3) = b**emax*(1 - b**(-t)) 
   c		= d1mach(2) = large
   C      DATA LARGE(1),LARGE(2) /         -1, 2146435071 /
   c
   c     Machine constants for little-endian (binary) IEEE arithmetic
   c
        data mcheps(1),mcheps(2) /         0, 1018167296 /
        data minmag(1),minmag(2) /         0,    1048576 /
        data maxmag(1),maxmag(2) /        -1, 2146435071 /
*/

/* C source for dpmpar: adapted from d1mach from blas Sethna 6/14/96 */
#include <stdio.h>
#include <float.h>
#include <math.h>

double dpmpar_(long *i)
{
	switch(*i){
/* Original d1mach
	  case 1: return DBL_MIN;
	  case 2: return DBL_MAX;
	  case 3: return DBL_EPSILON/FLT_RADIX;
	  case 4: return DBL_EPSILON;
	  case 5: return log10(FLT_RADIX);
*/
	  case 1: return DBL_EPSILON;
	  case 2: return DBL_MIN;
	  case 3: return DBL_MAX;
	  }

	fprintf(stderr, "invalid argument: d1mach(%ld)\n", *i);
	exit(1);
	return 0; /* for compilers that complain of missing return values */
}
