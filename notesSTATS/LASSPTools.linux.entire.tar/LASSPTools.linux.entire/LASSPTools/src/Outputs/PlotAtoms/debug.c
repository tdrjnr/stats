#include"plotatoms.h"

#ifndef _NO_PROTO
void debug_options(Options *options)
#else 
void debug_options(options)
  Options *options;
#endif
{
  FILE *fp;
  int i;

  fp = fopen("debug.options","w");

  fprintf(fp,"Options debugging output:\n\n");
  fprintf(fp,"Geometry:\n");
  fprintf(fp,"\tWidth = %i\n",options->geometry.width);
  fprintf(fp,"\tHeight = %i\n",options->geometry.height);
  fprintf(fp,"\tXpos = %i\n",options->geometry.xpos);
  fprintf(fp,"\tYpos = %i\n\n",options->geometry.ypos);
  fprintf(fp,"Limits:\n");
  fprintf(fp,"\tXmin = %f\n",options->limits.xmin);
  fprintf(fp,"\tXmax = %f\n",options->limits.xmax);
  fprintf(fp,"\tYmin = %f\n",options->limits.ymin);
  fprintf(fp,"\tYmax = %f\n\n",options->limits.ymax);
  fprintf(fp,"Natoms = %i\n",options->natoms);
  fprintf(fp,"Border fraction = %f\n",options->border);
  fprintf(fp,"Foreground = %s\n",options->foreground);
  fprintf(fp,"Background = %s\n\n",options->background);
  fprintf(fp,"Default Atom:\n");
  fprintf(fp,"\tRadius = %i\n",options->default_atom.radius);
  fprintf(fp,"\tColor = %s\n\n",options->default_atom.color);
  fprintf(fp,"Other Atoms:\n");
  for(i=0;i<options->natoms;++i) {
    fprintf(fp,"\tAtom %i:\n",i+1);
    fprintf(fp,"\t\tRadius = %i\n",options->atoms[i].radius);
    fprintf(fp,"\t\tColor = %s\n",options->atoms[i].color);
  }
  fprintf(fp,"\n\n");
  fflush(fp);
  fclose(fp);
  return;
}

/* debug.c */
/* last modified 3-29-91 */








