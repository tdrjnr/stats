#include "RotateStream.h"

#define ROTATE(x,y,z,r,xf,yf,zf) \
  (xf)=(r)[0][0]*(x) + (r)[0][1]*(y) + (r)[0][2]*(z); \
  (yf)=(r)[1][0]*(x) + (r)[1][1]*(y) + (r)[1][2]*(z); \
  (zf)=(r)[2][0]*(x) + (r)[2][1]*(y) + (r)[2][2]*(z)
#define VECT_SUBTRACT(ax,ay,az,bx,by,bz,c) \
  (c).x = (ax) - (bx); \
  (c).y = (ay) - (by); \
  (c).z = (az) - (bz)
#define CROSS(a,b,c) \
  (c).x = (a).y * (b).z - (a).z * (b).y; \
  (c).y = (a).z * (b).x - (a).x * (b).z; \
  (c).z = (a).x * (b).y - (a).y * (b).x
#define LENGTH(a) sqrt(DOT(a,a))
#define DOT(a,b) (a).x*(b).x + (a).y*(b).y + (a).z*(b).z

typedef struct { /* used in lighten_and_output_frame */
  double x,y,z;
} position;

#define DEFAULTSIDES 20 /* lighten_and_output_frame */
#define LIGHT_AMT 0.25 /* lighten_and_output_frame */

extern int sorting;
extern char averaging;
extern char two_D_out;

static double *ave_z = NULL;
static size_t ave_array_length = 0;

/*-----------------------------------------------------------------------------
 * compare_ave_z()
 */
#ifndef _NO_PROTO
int compare_ave_z(int *a, int *b)
#else
int compare_ave_z(a, b)
int *a;
int *b;
#endif
{
  if (ave_z[*a]>ave_z[*b])
    return(1);
  else
    return(-1);
}

/*---------------------------------------------------------------------------
 * rotate_and_sort_frame(frame F, double rot[3][3])
 */
#ifndef _NO_PROTO
void rotate_and_sort_frame(frame F, double rot[3][3])
#else
void rotate_and_sort_frame(F, rot)
frame F;
double rot[3][3];
#endif
{
  int j;
  size_t i;

  /* now that we have the rotation matrix,  go through the polygon
   * arrays and rotate, putting the rotated versions in vx, vy, vz
   */

  if(sorting != NOSORT) {

    /* make sure that ave_z is big enough */
    if(ave_array_length < F.numpoly && ave_z != NULL) {
      free((void *)ave_z);
      ave_z = NULL;
    }
    if(ave_z == NULL) {
      ave_z = (double *)malloc(F.numpoly * sizeof(double));
      if(ave_z == NULL) {
	fprintf(stderr,"RotateStream : could not allocate space for average array\n");
	exit(1);
      }
      ave_array_length = F.numpoly;
    }
    
    /* start sorting */
    if(averaging) { /* sort by the average of the points on each line */
      for (i=0; i<F.numpoly; i++) {
	F.ordering[i] = i;
	ave_z[i] = 0.0;
	for (j=0; j<F.num[i]; j++) {
	  ROTATE(F.Act[X][i][j],F.Act[Y][i][j],F.Act[Z][i][j],rot,
		 F.Rot[X][i][j],F.Rot[Y][i][j],F.Rot[Z][i][j]);
	  ave_z[i] += F.Rot[sorting][i][j];
	}
	ave_z[i] /= F.num[i];
      }
    }
    else { /* sort by the first point on each line */
      for (i=0; i<F.numpoly; i++) {
	F.ordering[i] = i;
	ave_z[i] = F.Rot[sorting][i][0];
	for (j=0; j<F.num[i]; j++) {
	  ROTATE(F.Act[X][i][j],F.Act[Y][i][j],F.Act[Z][i][j],rot,
		 F.Rot[X][i][j],F.Rot[Y][i][j],F.Rot[Z][i][j]);
	}
      }
    }

    /* output_frame uses F.ordering to determine the out ordering of the lines
     * of the frame now sort this array in order of ascending z coordinate of
     *  the polygon with that index
     */
    qsort((void *)F.ordering,F.numpoly,sizeof(int),compare_ave_z);
  }
  else {
    /* rotate_frame(); */
    for (i=0; i<F.numpoly; i++) {
      F.ordering[i] = i;  /* sorting array ordering controls the output order */
      for (j=0; j<F.num[i]; j++) {
	ROTATE(F.Act[X][i][j],F.Act[Y][i][j],F.Act[Z][i][j],rot,
	       F.Rot[X][i][j],F.Rot[Y][i][j],F.Rot[Z][i][j]);
      }
    }
  }
}

/*---------------------------------------------------------------------------
 * output_frame(frame F, char *last_line)
 */
#ifndef _NO_PROTO
void output_frame(frame F, char *last_line)
#else
void output_frame(F, last_line)
frame F;
char *last_line;
#endif
{
  size_t i, j, curr;
  char towrite[256];

  /* now that they are sorted, send the rotated polygons to polydraw
   * in back to front order, without the z-coordinates [i.e. isometric
   * projection]
   */

  if(two_D_out) {
    for (i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      for(j=0; j<F.num[curr]; j++) {
	sprintf(towrite,"%g %g ", F.Rot[X][curr][j], F.Rot[Y][curr][j]);
	fputs(towrite,stdout);
      }
      fputs(F.remains[curr],stdout);
    }
  }
  else {
    for (i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      for(j=0; j<F.num[curr]; j++) {
	sprintf(towrite,"%g %g %g ", F.Rot[X][curr][j], F.Rot[Y][curr][j],
		F.Rot[Z][curr][j]);
	fputs(towrite,stdout);
      }
      fputs(F.remains[curr],stdout);
    }
  }

  fputs(last_line, stdout);
  fflush(stdout);
}

/*----------------------------------------------------------------------------
 * z_part_norm()
 * this routine computes the average outward normal for a polygon
 * the "right hand rule" going around the ring determines the sign of
 * the normal vector
 */
#ifndef _NO_PROTO
double z_part_norm(double *x, double *y, double *z, int sides)
#else
double z_part_norm(x, y, z, sides)
double *x;
double *y;
double *z;
int sides;
#endif
{
  int i, next, nextnext;
  position norm, curr_out;
  position first, second;
  double len;

  /* compute the outward normal */

  norm.x = 0;  norm.y = 0;  norm.z = 0;
  for (i=0; i<sides; i++) {
    next = i+1;
    if (next==sides) next = 0;
    nextnext = next+1;
    if (nextnext==sides) nextnext=0;
    VECT_SUBTRACT(x[next],y[next],z[next],x[i],y[i],z[i],first);
    VECT_SUBTRACT(x[nextnext],y[nextnext],z[nextnext],
		  x[next],y[next],z[next],second);
    CROSS(first,second,curr_out);

    len = LENGTH(curr_out);
    norm.x += (curr_out.x/len);
    norm.y += (curr_out.y/len);
    norm.z += (curr_out.z/len);
  }

  len = LENGTH(norm);
/*  norm.x /= len;
 *  norm.y /= len;
 */
  norm.z /= len;
  return(norm.z);
}

/*---------------------------------------------------------------------------
 * lighten_and_output_frame(frame F,char *last_line)
 */
#ifndef _NO_PROTO
void lighten_and_output_frame(frame F, char *last_line)
#else
void lighten_and_output_frame(F, last_line)
frame F;
char *last_line;
#endif
{
  size_t i, j, curr;
  int red, grn, blu;
  char towrite[256];
  double norm;
  
  /* now that they are sorted, send the rotated polygons to polydraw
   * in back to front order, without the z-coordinates [i.e. isometric
   * projection]
   */  
  if(two_D_out) {
    for (i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      for(j=0; j<F.num[curr]; j++) {
	sprintf(towrite,"%g %g ", F.Rot[X][curr][j], F.Rot[Y][curr][j]);
	fputs(towrite,stdout);
      }
      /* light the polygon and ouput the color */
      sscanf(F.remains[curr], "%s %d %d %d", towrite, &red, &grn, &blu);
      norm = z_part_norm(F.Rot[X][curr],F.Rot[Y][curr],
			 F.Rot[Z][curr],F.num[curr]);

      red += (int)(LIGHT_AMT*norm*(double)red);
      if (red>65535) red = 65535;
      
      grn += (int)(LIGHT_AMT*norm*(double)grn);
      if (grn>65535) grn = 65535;
      
      blu += (int)(LIGHT_AMT*norm*(double)blu);
      if (blu>65535) blu = 65535;

      fputs(towrite,stdout);
      sprintf(towrite," %d %d %d\n",red,grn,blu);
      fputs(towrite,stdout);
    }
  }
  else {
    for (i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      for(j=0; j<F.num[curr]; j++) {
	sprintf(towrite,"%g %g %g ", F.Rot[X][curr][j], F.Rot[Y][curr][j],
		F.Rot[Z][curr][j]);
	fputs(towrite,stdout);
      }
      /* light the polygon and ouput the color */
      sscanf(F.remains[curr], "%s %d %d %d", towrite, &red, &grn, &blu);
      norm = z_part_norm(F.Rot[X][curr],F.Rot[Y][curr],
			 F.Rot[Z][curr],F.num[curr]);

      red += (int)(LIGHT_AMT*norm*(double)red);
      if (red>65535) red = 65535;
      
      grn += (int)(LIGHT_AMT*norm*(double)grn);
      if (grn>65535) grn = 65535;
      
      blu += (int)(LIGHT_AMT*norm*(double)blu);
      if (blu>65535) blu = 65535;

      fputs(towrite,stdout);
      sprintf(towrite," %d %d %d\n",red,grn,blu);
      fputs(towrite,stdout);
    }
  }
  
  fputs(last_line, stdout);
  fflush(stdout);
}


/*---------------------------------------------------------------------------
 * output_binary_frame(frame F)
 */
#ifndef _NO_PROTO
void output_binary_frame(frame F, char put_zero)
#else
void output_binary_frame(F, put_zero)
frame F;
char put_zero;
#endif
{
  int j;
  size_t i, curr;

  if(two_D_out)
    for(i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      write_binary_pairs(stdout,F.Rot[X][curr],F.Rot[Y][curr],F.num[curr]);
      write_binary_rgb_color(stdout,&(F.color[curr]));
    }
  else
    for(i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      write_binary_triples(stdout,F.Rot[X][curr],F.Rot[Y][curr],
			   F.Rot[Z][curr],F.num[curr]);
			   
      write_binary_rgb_color(stdout,&(F.color[curr]));
    }
  if(put_zero) {
    j = 0;
    fwrite((void *)&j, sizeof(int), (size_t)1, stdout);
  }
  fflush(stdout);
}

/*---------------------------------------------------------------------------
 * lighten_and_output_binary_frame(frame F)
 */
#ifndef _NO_PROTO
void lighten_and_output_binary_frame(frame F, char put_zero)
#else
void lighten_and_output_binary_frame(F, put_zero)
frame F;
char put_zero;
#endif
{
  int j;
  int red, grn, blu;
  size_t i, curr;
  double norm;
  XColor c_out;

  if(two_D_out)
    for(i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      write_binary_pairs(stdout,F.Rot[X][curr],F.Rot[Y][curr],F.num[curr]);
			   
      c_out = F.color[curr];
      norm = z_part_norm(F.Rot[X][curr],F.Rot[Y][curr],
			 F.Rot[Z][curr],F.num[curr]);
      red = c_out.red;
      grn = c_out.green;
      blu = c_out.blue;

      red += (int)(LIGHT_AMT*norm*(double)red);
      if (red>65535) red = 65535;
      
      grn += (int)(LIGHT_AMT*norm*(double)grn);
      if (grn>65535) grn = 65535;
      
      blu += (int)(LIGHT_AMT*norm*(double)blu);
      if (blu>65535) blu = 65535;

      c_out.red = red;
      c_out.green = grn;
      c_out.blue = blu;
      write_binary_rgb_color(stdout,&c_out);
    }
  else
    for(i=0; i<F.numpoly; i++) {
      curr = F.ordering[i];
      write_binary_triples(stdout,F.Rot[X][curr],F.Rot[Y][curr],
			   F.Rot[Z][curr],F.num[curr]);
			   
      c_out = F.color[curr];
      norm = z_part_norm(F.Rot[X][curr],F.Rot[Y][curr],
			 F.Rot[Z][curr],F.num[curr]);
      red = c_out.red;
      grn = c_out.green;
      blu = c_out.blue;

      red += (int)(LIGHT_AMT*norm*(double)red);
      if (red>65535) red = 65535;
      
      grn += (int)(LIGHT_AMT*norm*(double)grn);
      if (grn>65535) grn = 65535;
      
      blu += (int)(LIGHT_AMT*norm*(double)blu);
      if (blu>65535) blu = 65535;

      c_out.red = red;
      c_out.green = grn;
      c_out.blue = blu;
      write_binary_rgb_color(stdout,&c_out);
    }
  if(put_zero) {
    j = 0;
    fwrite((void *)&j, sizeof(int), (size_t)1, stdout);
  }
  fflush(stdout);
}
