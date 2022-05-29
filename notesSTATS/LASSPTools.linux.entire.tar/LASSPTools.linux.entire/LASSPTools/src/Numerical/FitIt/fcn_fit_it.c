#include <stdio.h>
#include <math.h>
#include "math_io.h"

#ifndef _NO_PROTO
#define ARGS(a) a
#else
#define ARGS(a) ()
#endif

#ifdef UNDERSCOREFORTRAN
#define FCN fcn_
#else
#define FCN fcn
#endif

#ifndef _NO_PROTO
    void FCN (int *m, int *n, const double params[], 
	      double fvec[], int *iflag)
#else
    FCN (m, n, params, fvec, iflag)
        int *m;
        int *n;
        double params[];
        double fvec[];
        int *iflag;
#endif
/* this assigns to fvec the difference between experiment and theory;
 * note that this doesn't use a lot of the parameters (mainly the number
 * of parameters and the number of data points) passed because
 * theory() knows what they are */
{
    extern double theory ARGS((double, const double[]));
    extern Matrix data_array;
    double data_x, data_y;
    int i;
    
    
    for (i = 0; i < *m; ++i) {
	data_x = data_array.el[i][0];
	data_y = data_array.el[i][1];
	fvec[i-1] = data_y - theory(data_x, params);
    }
}
