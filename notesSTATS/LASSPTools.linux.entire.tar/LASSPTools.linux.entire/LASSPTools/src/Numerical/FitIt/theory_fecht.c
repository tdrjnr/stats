#include <math.h>

#ifndef _NO_PROTO
#define CONST const
#else
#define CONST
#endif

#ifndef _NO_PROTO
    double theory(double temp, const double params[])
#else
        double theory(temp, params)
        double temp;
        double params[];
#endif
/* theory takes the temp from the data, and the array of params,
 * and returns the value of the function at x with this set
 * of params */
{
    CONST double T_0 = 1.0;
    double A, B, A_prime, B_prime, alpha;
    double reduced_temp;
    
    A = params[0];
    B = params[1];
    A_prime = params[2];
    B_prime = params[3];
    alpha = params[4];
    reduced_temp = fabs((temp - T_0)/(T_0));  
    
    if (temp > T_0) {
	return B + A*pow(reduced_temp, -alpha);
    } else {
	return B_prime + A_prime*pow(reduced_temp, -alpha);
    }
}
	
    
	     
