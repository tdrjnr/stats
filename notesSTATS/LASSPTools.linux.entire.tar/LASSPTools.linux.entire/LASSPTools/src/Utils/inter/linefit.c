/* Find and plot the best fit line through the data */

#include "interdef.h"
#include "menudef.h"
#include "iodefs.h"
#include <math.h>
void *calloc();

extern int errorson, automatic;
extern int ncols, xcol;
extern CURVE **curve;
static char *argptr;
extern int atoi();
extern FILE *inputfile;

extern int plotted, fileplotted;

linefit() { /* Get column number, and fit a line */
   int col;
   double a, b, siga, sigb; /* Intercept, slope, and errors */

   if(inputfile == NULL) {
       fprintf(stderr," No input file!\n");
       return 1;
   }

   while((col = getcol()) > 0 || moreargs) {
      if(!col) col = igetd("column to fit", (automatic? 1 : 2));

/*
    if((argptr = arg()) == NULL)
        col = igetd("column to fit",(automatic? 1 : 2));
    else
        col = atoi(argptr);

    if(col > ncols) {
        fprintf(stderr,"There is no column %d!\n",col);
        return 1;
    }
*/
      if(errorson && col > ncols-1) {
         fprintf(stderr,"There is no column %d with error values!\n",col+1);
         resetgetcol();
         return 1;
      }

      fitline(col, &a, &b, &siga, &sigb);
      printfit(a, b, siga, sigb);

   }
   return 1;
}

/************************************************************/

fitline(col, pa, pb, psiga, psigb)
int col;
double *pa, *pb, *psiga, *psigb;
{
    double xsum = 0.;   /* Sum of x's */
    double xsum2 = 0.;  /* Sum of x*x's */
    double ysum = 0.;   /* Sum of y's */
    double xysum = 0.;  /* Sum of xy's */
    double sig2 = 0.;   /* Sum of inverse sigma**2's */
    int i;
    double x, y, sigma;
    DATAPT *xptr, *yptr, *eptr;
    double siggy, siggy2, delta, d1; /* Intermediate stuff */

    xptr = curve[xcol]->firstpt;
    yptr = curve[col-1]->firstpt;
    if(errorson) eptr = curve[col]->firstpt;

    while (xptr != NULL) {      /* Compute sums */
        x = xptr->x;
        y = yptr->x;
        xptr = xptr->next;
        yptr = yptr->next;
        if (errorson) {
            sigma = eptr->x;
            eptr = eptr->next;
        }
        else
            sigma = 1;

        siggy = 1./sigma;
        siggy2 = siggy * siggy;
        xsum += x * siggy2;
        ysum += y * siggy2;
        xsum2 += x * x * siggy2;
        xysum += x * y * siggy2;
        sig2 += siggy2;
    }

    delta = sig2*xsum2 - xsum*xsum;
    d1 = 1./delta;
    *pa = (xsum2*ysum - xsum*xysum) * d1;       /* Y intercept */
    *pb = (sig2*xysum - xsum*ysum) * d1;        /* slope */
    *psiga = sqrt(xsum2 * d1);          /* error in intercept */
    *psigb = sqrt(sig2 * d1);           /* error in slope */
}

/************************************************************/

printfit(a, b, siga, sigb)
double a, b, siga, sigb;
{
    int i;
    int npts;
    DATAPT *xptr;
    double *sortx;      /* Sorted x data, needed to plot nicely */
    int qsortcompare();

    fprintf(stderr,"Y intercept = %g +- %g\n", a, siga);
    fprintf(stderr,"      Slope = %g +- %g\n", b, sigb);

/* sort x data */
    npts = curve[xcol]->npts;
    sortx = (double *) calloc((unsigned) npts, sizeof(double));
    xptr = curve[xcol]->firstpt;
    i=0;
    while (xptr != NULL) {
        sortx[i] = xptr->x; /* Copy */
        i++;
        xptr = xptr->next;
    }
    qsort((char *) sortx, npts, sizeof(sortx[0]), qsortcompare);

/* Print fitted line */
    if(errorson) printf("#e0\n");       /* Turn off errors temporarily */
    printf("#\n");
    for(i=0; i<npts; i++)
        printf("%e %e\n", sortx[i], a + b*sortx[i]);
    free((char *) sortx);
    plotted = fileplotted = 1;
    if(errorson) printf("#e\n");       /* Turn errors back on */
}

/************************************************************/

int qsortcompare(a,b)
double *a, *b;
{
    double diff;
    diff = (*a - *b);
    if (diff == 0) return (0);
    return( (diff < 0) ? -1 : 1);
}
