#include <stdio.h>
#include "interdef.h"

extern CURVE **curve;
extern int ncols, xcol;
extern int inputskip, inputspacing;
extern int automatic;
extern int fileplotted, plotted;
extern double xmin, xmax, ymin, ymax;
static int autooffset;
static double offset = 0.0;

char *calloc(), *malloc();

#define LOW -1.e300   /* Should be less than all data */

/* Linked list of points on the curve at max y */
typedef struct POINT {
   struct POINT *next;
   struct POINT *prev;
   double x;
   double y;
   int used;
} POINT;

static POINT *newsky(), *findcross();
static void delsky();

#define NEWCURVE printf("#\n")

hiddenplot() {
   int npts;
   int col, i;
   int spacecount, skipcount;
   DATAPT *xptr, *yptr;  /* pointers to data elements */
   double lastx, lasty, nextx, nexty;
   double crossx, crossy;
   POINT *skyline;     /* Array of skyline segments */
   POINT *tempsky, *crossptr;
   int first, startvisible;
   extern FILE *inputfile;

    if(inputfile == NULL) {     /* Is there a current file? */
        fprintf(stderr," No input file!\n");
        trashline();
        return 1;
    }

   /* Initialize -- make skyline struct for each x point */
   /* skyline struct is a linked list of (x,y) points giving the
    * maximum of all plotted curves at all points between a plotted
    * x value and the PREVIOUS plotted x value.
    */

   npts = curve[0]->npts;
   spacecount = skipcount = 0;
   xptr = curve[xcol]->firstpt;
   skyline = (POINT *) calloc((unsigned) npts, sizeof(POINT));
   tempsky = (POINT *) 0;
   i = 0;
   while(skipcount++ < inputskip) {  /* Skip initial data lines */
      xptr = xptr->next;
      skyline[i].used = 0;
      skyline[i].next = skyline[i].prev = NULL;
      i++;
   }
   while(xptr != NULL) {            /* Loop over data lines */
      skyline[i].next = skyline[i].prev = NULL;
      if(++spacecount == inputspacing) {   /* skip unused lines */
         spacecount = 0;
         skyline[i].y = LOW;         /* initial skyline is below all data */
         skyline[i].used = 1;
         tempsky = skyline[i].next = newsky(&skyline[i]);
         tempsky->prev = &skyline[i];
         tempsky->next = NULL;
         tempsky->y = LOW;
         tempsky->x = xptr->x;
      }
      else {
         skyline[i].used = 0;
      }
      xptr = xptr->next;
      i++;
   }
   /* Load x values for end of skyline segments */
   tempsky = NULL;
   for(i=npts-1; i>=0; i--) {
      if(!skyline[i].used) continue;
      if(tempsky) tempsky->x = skyline[i].next->x;
      tempsky = &skyline[i];
   }

   while((col = getcol()) > 0) {   /* Loop over columns given in arg list */

      /* New column of data */
      xptr = curve[xcol]->firstpt;
      yptr = curve[col-1]->firstpt;
      first = 1;
      NEWCURVE;

      for(i=0; i<npts; i++) {
         if(skyline[i].used) {  /* Only look at used points */
            nextx = xptr->x;
            nexty = yptr->x;
            if(first) {
               if(yptr->x >= skyline[i].y) printf("%e %e\n", nextx, nexty);
               if(nextx < xmin) xmin = nextx;      /* Find limits */
               if(nexty < ymin) ymin = nexty;
               if(nextx > xmax) xmax = nextx;
               if(nexty > ymax) ymax = nexty;
            }
            else {
               /* Draw new line from last(x,y) to next(x,y) */
               crossptr = NULL;
               startvisible = (lasty >= skyline[i].y);
               /*  Find where skyline crosses new line */
               crossptr = findcross(&skyline[i], lastx, lasty, nextx, nexty);
               if(!crossptr && startvisible) {
                  /* Line doesn't cross and is always above */
                  printf("%e %e\n", nextx, nexty);
                  if(nextx < xmin) xmin = nextx;      /* Find limits */
                  if(nexty < ymin) ymin = nexty;
                  if(nextx > xmax) xmax = nextx;
                  if(nexty > ymax) ymax = nexty;
                  /* Make skyline segment straight */
                  skyline[i].x = lastx;
                  skyline[i].y = lasty;
                  tempsky = skyline[i].next;
                  tempsky->x = nextx;
                  tempsky->y = nexty;
                  while(tempsky->next) delsky(tempsky->next);
               }
               else if(!crossptr && !startvisible) {
                  /* Line is completely invisible */
               }
               else if(crossptr && startvisible) {
                  /* Line dives into skyline */
                  intersect(lastx, lasty, nextx, nexty,
                            crossptr->x, crossptr->y,
                            crossptr->next->x, crossptr->next->y,
                            &crossx, &crossy);
                  printf("%e %e\n", crossx, crossy);
                  if(crossx < xmin) xmin = crossx;      /* Find limits */
                  if(crossy < ymin) ymin = crossy;
                  if(crossx > xmax) xmax = crossx;
                  if(crossy > ymax) ymax = crossy;
                  skyline[i].x = lastx;
                  skyline[i].y = lasty;
                  tempsky = newsky(&skyline[i]);
                  tempsky->x = crossx;
                  tempsky->y = crossy;
                  while(tempsky->next && tempsky->next->x < crossx)
                     delsky(tempsky->next);
               }
               else if(crossptr && !startvisible) {
                  /*line emerges from skyline */
                  intersect(lastx, lasty, nextx, nexty,
                            crossptr->x, crossptr->y,
                            crossptr->next->x, crossptr->next->y,
                            &crossx, &crossy);
                  NEWCURVE;
                  printf("%e %e\n", crossx, crossy);
                  if(crossx < xmin) xmin = crossx;      /* Find limits */
                  if(crossy < ymin) ymin = crossy;
                  if(crossx > xmax) xmax = crossx;
                  if(crossy > ymax) ymax = crossy;
                  tempsky = newsky(crossptr);
                  tempsky->x = crossx;
                  tempsky->y = crossy;
                  /* Is there another crossing? */
                  crossptr = findcross(tempsky->next,
                                lastx, lasty, nextx, nexty);
                  if(!crossptr) { /* Make rest of skyline straight */
                     printf("%e %e\n", nextx, nexty);
                     tempsky->next->x = nextx;
                     tempsky->next->y = nexty;
                     tempsky = tempsky->next;
                     while(tempsky->next) delsky(tempsky->next);
                  }
                  else {  /* line disappears again */
                     intersect(lastx, lasty, nextx, nexty,
                            crossptr->x, crossptr->y,
                            crossptr->next->x, crossptr->next->y,
                            &crossx, &crossy);
                     printf("%e %e\n", crossx, crossy);
                     if(crossx < xmin) xmin = crossx;      /* Find limits */
                     if(crossy < ymin) ymin = crossy;
                     if(crossx > xmax) xmax = crossx;
                     if(crossy > ymax) ymax = crossy;
                     while(tempsky->next != crossptr) delsky(tempsky->next);
                     crossptr->x = crossx;
                     crossptr->y = crossy;
                  }
               }
            }
            lastx = nextx;
            lasty = nexty;
            first = 0;
         }
         xptr = xptr->next;
         yptr = yptr->next;
      }

   }  /* End of loop over columns */
   
   for(i=0; i<npts; i++) freeskyline(&skyline[i]);
   free((char *) skyline);
   fileplotted = plotted = 1;
   return 1;
}

/****************************************************************/

static POINT *newsky(curpt)  /* Add new point after curpt */
POINT *curpt;
{
   POINT *newpt = (POINT *) malloc(sizeof(POINT));
   POINT *nextpt = curpt->next;
   curpt->next = newpt;
   newpt->prev = curpt;
   newpt->next = nextpt;
   newpt->used = 1;
   if(nextpt != 0) nextpt->prev = newpt;
   return newpt;
}

static void delsky(curpt) /* Delete skyline point and reconnect list */
POINT *curpt;
{
   POINT *nextpt = curpt->next;
   POINT *lastpt = curpt->prev;
   if(lastpt == 0 && nextpt == 0) return;  /* No curve left */
   if(nextpt != 0) nextpt->prev = lastpt;
   if(lastpt != 0) lastpt->next = nextpt;
   free((char *) curpt);
}

freeskyline(skyptr)
POINT *skyptr;
{
  POINT *next;
  next = skyptr->next;
  while(next) {
     skyptr = next;
     next = skyptr->next;
     free((char *) skyptr);
  }
}

/***************************************************************/

/* Is point (xx,yy) to the left of the vector (x1, y1)-(x0, y0) ? */
#define LEFTSIDE(xx, yy, x0, y0, x1, y1) \
       (((xx) - (x0))*((y1) - (y0)) < ((yy) - (y0))*((x1) - (x0)))

static POINT *findcross(skyptr, x0, y0, x1, y1)
POINT *skyptr;
double x0, y0, x1, y1;
{
   int below, belowlast;
   belowlast = LEFTSIDE(skyptr->x, skyptr->y, x0, y0, x1, y1);
   skyptr = skyptr->next;
   while(skyptr) {
      below = LEFTSIDE(skyptr->x, skyptr->y, x0, y0, x1, y1);
      if(below != belowlast) return skyptr->prev;
      belowlast = below;
      skyptr = skyptr->next;
   }
   return NULL;
}

/***************************************************************/

/* Find the intersection of two lines (not line segments).
 * Return 1 if they intersect, 0 otherwise */

int intersect(x0, y0, x1, y1, xa, ya, xb, yb, ix, iy)
double x0, y0, x1, y1;   /* One line segment */
double xa, ya, xb, yb;   /* Another line segment */
double *ix, *iy;         /* The point where the LINES intersect */
{
   double x, y;
   double dx0 = x1 - x0;
   double dy0 = y1 - y0;
   double dxa = xb - xa;
   double dya = yb - ya;

   if(dya*dx0 == dy0*dxa) /* Parallel lines */
      return 0;

   x = ((y0-ya)*dx0*dxa - x0*dy0*dxa + xa*dya*dx0)/
         (dya*dx0 - dy0*dxa);
   if(dx0 != 0)
      y = y0 + (x-x0)*dy0/dx0;
   else
      y = ya + (x-xa)*dya/dxa;
   *ix = x;
   *iy = y;
   return 1;
}
