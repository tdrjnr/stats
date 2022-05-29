#include<stdio.h>

#ifndef _NO_PROTO
void help(void)
#else
void help()
#endif
{
  printf("\nUsage:\t\tPlotAtoms < datafile\n\n");
  printf("Description: \tCreates a movie of atom motions by reading x,y ");
  printf("coordinates from\n");
  printf("\t\tstandard in and plotting them in an X-Window. ");
  printf("The options \n\t\tavailable include setting window geometry, ");
  printf("shading the atoms, \n");
  printf("\t\tsetting scales, and plotting entire trajectories.  See the\n");
  printf("\t\tmanual pages for details.\n");
  printf("\n");
  fflush(stdout);
  return;
}

/* help */

/* PlotAtoms (Bruce W. Roberts) */
/* last modification: 3-29-91 */

