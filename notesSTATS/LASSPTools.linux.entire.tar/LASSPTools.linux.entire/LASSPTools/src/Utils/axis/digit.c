#include <math.h>
#define max(a,b) ((a)>(b)?(a):(b))

digit (xlo, xhi, nlab, width, digits)
 float xlo, xhi;
 int nlab;
 int *width, *digits;
{
   double delta, x, y;
   int p,q;

   delta = (xhi-xlo)/(nlab-1);
   x = log10 (delta);
   p = (x >= 0) ? (int) x : (int) (x - 0.9);

   y = pow ((double)10, (double)(-p));
   q = (int)(1+log10 (max(fabs((double)(xlo*y)), fabs((double)(xhi*y)))));

   if (p >= 0 ) {
      *digits = 0;
      *width = p+q;
   }
   else {
      *digits = -p;
      *width = 1+q+((-p)>q)*(1+(-p)-q);
   }

   if ((xlo < 0) || (xhi < 0)) (*width) ++;
   if (q == (-p)) (*width) ++;
}
