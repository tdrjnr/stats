/* HISTOGRAMMING ROUTINES
 *
 * Commands:
 * histogram plot [= <column>]    plot the histogram
 * histogram bins [= <nbins>]    set number of bins
 * hist min [= <min>]
 * hist max [= <max>]
 */

#include "interdef.h"
#include "menudef.h"
#include "iodefs.h"
#include <math.h>
void *calloc(), *malloc();
extern int atoi();
extern double atof();

static char *argptr;

#define MAXBINS 1000
double *hist;                   /* histogram */
static double hmin, hmax;       /* limits of histogram */
static double binsize;
extern int hbins;               /* number of bins (default = 100) */
extern double hist_norm;        /* histogram normalization */
extern int weighthist, wcol;

static int histminset = 0;
static int histmaxset = 0;
static double histmin, histmax;
static int loghist = 0;
static int autohist = 0;
static int baseline = 0;

extern CURVE **curve;
extern int ncols;
extern double xmin, xmax, ymin, ymax;
extern int plotted, fileplotted, moviemode;
extern FILE *inputfile;


histogram() {
    int hcol;   /* column to make into histogram */

    if(inputfile == NULL) {
        fprintf(stderr," No input file!\n");
        return 1;
    }
    
    /* get first column number */
    if((hcol = getcol()) == 0) {
        hcol = igetd("column from which to make histogram", ncols==1?1:2);
        if(hcol < 1 || hcol > ncols) {
           fprintf(stderr,"There is no column %d!\n",hcol);
           return 1;
       }
    }
    else if(hcol == -1)  /* Error in getcol */
        return 1;

    hist = (double *) calloc((unsigned) hbins, sizeof(*hist)); /* get space */
    
    do {    
        hcol -= 1;  /* Adjust for array indexing */
    
        limithist(hcol);                    /* find limits */
    
        if(!weighthist)
            makehist(hcol);
        else
            makewhist(hcol);
    
        if(moviemode) printf("#axes\n#clear\n");
        if(!loghist)
           printhist();
        else
           printloghist();
    
        fileplotted = plotted = 1;  /* Set flag */
    
    } while((hcol = getcol()) > 0);
    
    free( (char *) hist);               /* free up space */
    histminset = histmaxset = 0;
    return 1;
}

histweight() {

    if((argptr=arg()) == NULL) {  /* find wcol */
       if(!weighthist)
           wcol = igetd("column containing weights", ncols);
       else {
           weighthist = 0;
           fprintf(stderr, "  Histogram weights are all unity.\n");
           return 1;
       }
    }
    else {
       wcol = atoi(argptr);
       weighthist = 1;
    }
       
    if(wcol > ncols) {
       fprintf(stderr,"There is no column %d!\n",wcol);
       weighthist = 0;
       return 1;
    }

    wcol -= 1;
    return 1;
}

limithist(hcol)
int hcol;
{
    DATAPT *hptr;

    hmin = HUGE;
    hmax = -HUGE;
    hptr = curve[hcol]->firstpt;

    while (hptr != NULL) {
        if(hptr->x < hmin) hmin = hptr->x;
        if(hptr->x > hmax) hmax = hptr->x;
        hptr = hptr->next;
    }

    if(histminset)
       hmin = histmin;
    else if(!autohist)
       hmin = dgetd("minimum value for histogram",hmin);

    if(histmaxset)
       hmax = histmax;
    else if(!autohist)
       hmax = dgetd("maximum value for histogram",hmax);

    if(xmin > hmin) xmin = hmin;
    if(xmax < hmax) xmax = hmax;
    return 1;
}

sethistmin() {
   histminset = 1;
   if((argptr = arg()) == NULL)
      histmin = dget("minimum value for histogram");
   else
      histmin = atof(argptr);
    return 1;
}

sethistmax() {
   histmaxset = 1;
   if((argptr = arg()) == NULL)
      histmax = dget("maximum value for histogram");
   else
      histmax = atof(argptr);
    return 1;
}

makehist(hcol)          /* Compile the histogram. */
int hcol;
{
    double scale;
    int i,binno;
    DATAPT *hptr;


    for(i=0; i<hbins; i++) hist[i] = 0; /* initialize */
    binsize = (hmax - hmin)/hbins;              /* size of bins */
    if(hist_norm != 0.)                         /* set scale factor */
        scale = hist_norm/(binsize * curve[hcol]->npts);
    else
        scale = 1.;                             /* y = # of counts */

    hptr = curve[hcol]->firstpt;                        /* ptr to first point */

    while (hptr != NULL) {                              /* make histogram */
        binno = (hptr->x - hmin)/binsize;
        if(binno < hbins && binno >= 0)
            hist[binno] += scale;
        else if(hptr->x == hmax) /* Count points at upper limit */
              hist[hbins-1] += scale;
        hptr = hptr->next;
    }
}

makewhist(hcol)         /* Compile the weighted histogram. */
int hcol;
{
    double scale;
    int i,binno;
    DATAPT *hptr,*wptr;


    for(i=0; i<hbins; i++) hist[i] = 0.; /* initialize */
    binsize = (hmax - hmin)/hbins;              /* size of bins */
    if(hist_norm != 0.)                         /* set scale factor */
        scale = hist_norm/(binsize * curve[hcol]->npts);
    else
        scale = 1.;                             /* y = # of counts */

    hptr = curve[hcol]->firstpt;                /* ptr to first point */
    wptr = curve[wcol]->firstpt;                /* ptr to first weight */

    while (hptr != NULL && wptr != NULL) { /* make histogram */
        binno = (hptr->x - hmin)/binsize;
        if(binno < hbins && binno >= 0)
            hist[binno] += scale * wptr->x;
        else if(hptr->x == hmax) /* Count points at upper limit */
              hist[hbins-1] += scale * wptr->x;
        hptr = hptr->next;
        wptr = wptr->next;
    }
}

printhist() {
    int i;

    printf("#\n");
    if(baseline) printf("%e %e\n",hmin,0.);
    for(i=0; i<hbins; i++) {
        printf("%e %e\n", hmin+i*binsize, hist[i]);
        printf("%e %e\n", hmin+(i+1)*binsize, hist[i]);
        if(hist[i] > ymax) ymax = hist[i];
    }
    if(baseline) {
	printf("%e %e\n",hmax,0.0);
	/* printf("%e %e\n",hmin,0.0); */   /* print baseline */
    }
    if(ymin > 0.) ymin = 0.;
}

printloghist() {
    int i;
    double lh;

    printf("#\n");
    for(i=0; i<hbins; i++) {
        if(hist[i] > 0.0) {
           lh = log(hist[i]);
           printf("%e %e\n", hmin+i*binsize, lh);
           printf("%e %e\n", hmin+(i+1)*binsize, lh);
        }
        else {
           puts("#");
        }
        if(hist[i] > ymax) ymax = hist[i];
    }
    if(ymin > 0.) ymin = 0.;
}

histbins() {
    if((argptr=arg()) == NULL)
       hbins = igetd("number of bins in histogram",hbins);
    else
       hbins = atoi(argptr);
    return 1;
}

hist_normal() {
    if((argptr=arg()) == NULL) {
       fprintf(stderr,"  Set normalization to 0 to make y = # of counts\n");
       hist_norm = dgetd("normalization of histogram",hist_norm);
    }
    else
       hist_norm = atof(argptr);
    return 1;
}

sethistlog() {
   if(loghist ^= 1)
      fputs("Histogram y axis is logarithmic.\n", stderr);
   else
      fputs("Histogram y axis is linear.\n", stderr);
    return 1;
}

sethistauto() {
   if(autohist ^= 1)
      fputs("Histogram limits will be set automatically.\n", stderr);
   else
      fputs("Histogram limits will not be set automatically.\n", stderr);
    return 1;
}

sethistbaseline() {
    if(baseline ^= 1)
	fputs("Histogram baseline is ON.\n", stderr);
    else
	fputs("Histogram baseline is OFF.\n", stderr);
    return 1;
}

/***************************************************************/

/* data adjustment */

extern double scalefactor, shift, shiftincrement;
extern int automatic;
static int firstshift = 1;
#define DEFAULTCOL (automatic? 1 : (ncols > 1 ? 2 : 1))

adjustdata() {
    int col;
    if((col = getcol()) == 0) {
        adjustcol( igetd("column to adjust", DEFAULTCOL) );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
        adjustcol(col);
    } while ((col = getcol()) > 0);

    return 1;
}

adjustcol(col)
int col;
{
    DATAPT *xptr;

    xptr = curve[col-1]->firstpt;

    if(!firstshift) shift += shiftincrement;
    firstshift = 0;

    while (xptr != NULL) {
        xptr->x *= scalefactor;
        xptr->x += shift;
        xptr = xptr->next;
    }
}

adjust_scale() {
   if((argptr=arg()) == NULL)
      scalefactor = dgetd("scale factor", scalefactor);
   else
      scalefactor = atof(argptr);
    return 1;
}

adjust_shift() {
   if((argptr=arg()) == NULL)
      shift = dgetd("shift", shift);
   else
      shift = atof(argptr);
    return 1;
}

adjust_shift_increment() {
   if((argptr=arg()) == NULL)
      shiftincrement = dgetd("shift_increment", shift);
   else
      shiftincrement = atof(argptr);
    firstshift = 1;
    return 1;
}

/***************************************************************/

addcurves() {
   return addsubcurves(1);
}

subcurves() {
   return addsubcurves(-1);
}

addsubcurves(sign) /* Add or subtract two columns of data */
int sign;
{
    int curveA, curveB;
    DATAPT *aptr, *bptr;

    curveA = getcol();
    curveB = getcol();
    if(curveA == 0 && curveB == 0) {
       if(sign == 1)
          fputs("Adding curve A to curve B...\n", stderr);
       else
          fputs("Subtracting curve A from curve B...\n",stderr);
        curveA = igetd("column number of curve A",2);
        curveB = igetd("column number of curve B",ncols);
    }
    else if(curveA == 0) {
       if(sign == 1)
          fprintf(stderr, "Adding curve A to curve B=%d...\n", curveB);
       else
          fprintf(stderr, "Subtracting curve A from curve B=%d...\n", curveB);
       curveA = igetd("column number of curve A",ncols);
    }
    else if(curveB == 0) {
       if(sign == 1)
           fprintf(stderr,"Adding curve A=%d to curve B...\n", curveA);
       else
           fprintf(stderr, "Subtracting curve A=%d from curve B...\n", curveA);
       curveB = igetd("column number of curve B",ncols);
    }

    if (curveA <= 0 || curveB <= 0) {
       if(sign == 1)
          fputs("Not adding curves.\n", stderr);
       else
          fputs("Not subtracting curves.\n", stderr);
       return 1;
    }

    aptr = curve[curveA-1]->firstpt;
    bptr = curve[curveB-1]->firstpt;

    while (aptr != NULL && bptr != NULL) {
        bptr->x += sign*aptr->x;
        aptr = aptr->next;
        bptr = bptr->next;
    }

    while((curveB = getcol()) > 0) {
       if(curveB <= 0) {
          return 1;
       }
       aptr = curve[curveA-1]->firstpt;
       bptr = curve[curveB-1]->firstpt;
       while (aptr != NULL && bptr != NULL) {
          bptr->x += sign*aptr->x;
          aptr = aptr->next;
          bptr = bptr->next;
       }
    }

    return 1;
}

multiplycurves() /* Multiply two columns of data */
{
    int curveA, curveB;
    DATAPT *aptr, *bptr;

    curveA = getcol();
    curveB = getcol();
    if(curveA == 0 && curveB == 0) {
        fputs("Multiplying curve B by curve A...\n", stderr);
        curveA = igetd("column number of curve A",2);
        curveB = igetd("column number of curve B",ncols);
    }
    else if(curveA == 0) {
       fprintf(stderr, "Multiplying curve B=%d by curve A...\n", curveB);
       curveA = igetd("column number of curve A",ncols);
    }
    else if(curveB == 0) {
       fprintf(stderr,"Multiplying curve B by curve A=%d...\n", curveA);
       curveB = igetd("column number of curve B",ncols);
    }

    if (curveA <= 0 || curveB <= 0) {
       fputs("Not multiplying curves.\n", stderr);
       return 1;
    }

    aptr = curve[curveA-1]->firstpt;
    bptr = curve[curveB-1]->firstpt;

    while (aptr != NULL && bptr != NULL) {
        bptr->x *= aptr->x;
        aptr = aptr->next;
        bptr = bptr->next;
    }

    while((curveB = getcol()) > 0) {
       if(curveB <= 0) {
          return 1;
       }
       aptr = curve[curveA-1]->firstpt;
       bptr = curve[curveB-1]->firstpt;
       while (aptr != NULL && bptr != NULL) {
          bptr->x *= aptr->x;
          aptr = aptr->next;
          bptr = bptr->next;
       }
    }

    return 1;
}

dividecurves() /* Divide two columns of data */
{
    int curveA, curveB;
    DATAPT *aptr, *bptr;

    curveA = getcol();
    curveB = getcol();
    if(curveA == 0 && curveB == 0) {
        fputs("Dividing curve B by curve A...\n", stderr);
        curveA = igetd("column number of curve A",2);
        curveB = igetd("column number of curve B",ncols);
    }
    else if(curveA == 0) {
       fprintf(stderr, "Dividing curve B=%d by curve A...\n", curveB);
       curveA = igetd("column number of curve A",ncols);
    }
    else if(curveB == 0) {
       fprintf(stderr,"Dividing curve B by curve A=%d...\n", curveA);
       curveB = igetd("column number of curve B",ncols);
    }

    if (curveA <= 0 || curveB <= 0) {
       fputs("Not multiplying curves.\n", stderr);
       return 1;
    }

    aptr = curve[curveA-1]->firstpt;
    bptr = curve[curveB-1]->firstpt;

    while (aptr != NULL && bptr != NULL) {
        bptr->x /= aptr->x;
        aptr = aptr->next;
        bptr = bptr->next;
    }

    while((curveB = getcol()) > 0) {
       if(curveB <= 0) {
          return 1;
       }
       aptr = curve[curveA-1]->firstpt;
       bptr = curve[curveB-1]->firstpt;
       while (aptr != NULL && bptr != NULL) {
          bptr->x /= aptr->x;
          aptr = aptr->next;
          bptr = bptr->next;
       }
    }

    return 1;
}

/***************************************************************/

static void applyfunc(col, f)
int col;
double (*f)();
{
    DATAPT *lptr;
    if(col <= 0 || col > ncols) {
        fprintf(stderr, "There is no column %d!\n", col);
        return;
    }
    lptr = curve[col-1]->firstpt;
    while (lptr != NULL) {
        lptr->x = (*f)(lptr->x);
        lptr = lptr->next;
    }
}

/********************************************************************/

logdata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc( igetd("column to take log of", DEFAULTCOL), log10 );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, log10);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

double invert(x) double x; { return 1./x; }

invertdata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc( igetd("column to invert", DEFAULTCOL), invert );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, invert);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

sindata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc( igetd("column to take sine of", DEFAULTCOL), sin );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, sin);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

cosdata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc( igetd("column to take cosine of", DEFAULTCOL), cos );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, cos);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

absdata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc(igetd("column to take absolute value of", DEFAULTCOL),
                  fabs);
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, fabs);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

sqrtdata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc( igetd("column to take square root of", DEFAULTCOL), sqrt );
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, sqrt);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

double square(x) double x; { return x*x; }

squaredata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc(igetd("column to take square of", DEFAULTCOL), square);
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, square);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

double cube(x) double x; { return x*x*x; }

cubedata() {
    int col;
    if((col = getcol()) == 0) {
        applyfunc(igetd("column to take cube of", DEFAULTCOL), cube);
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       applyfunc(col, cube);
    } while ((col = getcol()) > 0);

    return 1;
}

/*******************************************************************/

static double modulus = 1;

setmod() {
   if((argptr = arg()) == NULL)
      modulus = dgetd("modulus", modulus);
   else
      modulus = atof(argptr);
}

double modfloor(x, mod)
double x, mod;
{
   double r;
   r = floor(x/mod)*mod;
   return x - r;
}

moddata() {
    int col;
    if((col = getcol()) == 0) {
        col = igetd("column to take modulus of", DEFAULTCOL);
        if(col < 1 || col > ncols) 
           fprintf(stderr,"There is no column %d!\n", col);
        else
           moddcol(col);
        return 1;
    }
    if(col == -1) return 1;  /* Error in getcol */

    do {
       moddcol(col);
    } while ((col = getcol()) > 0);

    return 1;
}

moddcol(col)
int col;
{
    DATAPT *lptr;
    lptr = curve[col-1]->firstpt;
    while (lptr != NULL) {
        lptr->x = modfloor(lptr->x, modulus);
        lptr = lptr->next;
    }
}
