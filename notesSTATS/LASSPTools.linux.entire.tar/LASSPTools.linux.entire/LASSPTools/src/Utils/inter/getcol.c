/* Get a column number from arg().
 *  * means all columns
 *  x means x column
 *  $ means last column
 *  number means the given column
 * 
 * Return column number if successful
 * Return 0 if no more args
 * Return -1 if error
 */

#include <stdio.h>
#include "menudef.h"
extern int ncols, xcol, automatic, returnplot;

#define ALLCOLUMNS      '*'
#define LASTCOLUMN      '$'
#define XCOLUMN         'x'

static int allcols = 0;

int getcol() {
   char *arg(), *argptr;
   static int col = 0;
   if(allcols) {   /* previous call found ALLCOLUMNS */
      col++;
      if(!automatic && !returnplot && col == xcol+1)
         col++;    /* xcol starts at 0, col at 1! */
      if(col <= ncols) return col;
      allcols = 0;       /* all columns plotted */
   }
   if((argptr = arg()) == NULL) return 0;  /* No more args */
   if(*argptr == ALLCOLUMNS) {
      allcols = 1;
      col = 0;
      return getcol();
   }
   if(*argptr == XCOLUMN) return xcol+1;
   if(*argptr == LASTCOLUMN) return ncols;
   col = atoi(argptr);
   if(col > 0 && col <= ncols) return col;
   fprintf(stderr, "There is no column %d!\n",col);
   return -1;
}

resetgetcol() {
   allcols = 0;
}
