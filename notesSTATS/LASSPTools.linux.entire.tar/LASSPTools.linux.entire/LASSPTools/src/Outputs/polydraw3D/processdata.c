/*****************************************************************************
 processdata.c
 uses lineio to read a data stream and to build the data structures
 *****************************************************************************/


#include <X11/Xlib.h>
#include "line_io.h"
#include "processdata.h"

extern char binary;
#ifndef _NO_PROTO
extern void swap(polygon *poly_list, int i, int j);
#else
extern void swap();
#endif

/****************************************************************************/

#ifndef _NO_PROTO
  int readdata(polygon *polylist, int *numpolygons)
#else 
  int readdata(polylist, numpolygons)
	polygon *polylist;
	int *numpolygons;
#endif

{
double *x, *y, *z;
char *remains;
char input[MAXLENGTH];
int num, i, j;
double zsum;
char dummy[255];
XColor color;

x = NULL;
y = NULL;
z = NULL;

j = -1;
if (!binary) {
	while (fgets(input,MAXLENGTH,stdin) != NULL) {
		j++;
		if (read_triples(input, &x, &y, &z, &num, &remains)) {
		     	polylist[j].numsides = num;
			polylist[j].x = x;
			polylist[j].y = y;
			polylist[j].z = z;
			i = sscanf(remains,"%s %d %d %d",dummy,
			&polylist[j].red,&polylist[j].green,&polylist[j].blue);
			zsum = 0;
			for (i=0; i<num; i++) 
				zsum = zsum + z[i]; 
			polylist[j].depth = zsum/num;
			}
	      }
      }
else {
	while (read_binary_triples(stdin, &x, &y, &z, &num)) {
		j++;
		polylist[j].numsides = num;
		polylist[j].x = x;
		polylist[j].y = y;
		polylist[j].z = z;
		read_binary_rgb_color(stdin, &color);
		polylist[j].red = (int)color.red;
		polylist[j].green = (int)color.green;
		polylist[j].blue = (int)color.blue;
		zsum = 0;
		for (i=0; i<num; i++) 
			zsum = zsum + z[i]; 
		polylist[j].depth = zsum/num;
		}
	}
*numpolygons = j;
if (x!=NULL) free(x);
if (y!=NULL) free(y);
if (z!=NULL) free(z);
fflush(stdout);
}

/****************************************************************************/

#ifndef _NO_PROTO
  int sortdata(polygon *polylist, int left, int right)
#else
  int sortdata(polylist, left, right)
	polygon *polylist;
	int left;
	int right;
#endif

{
int i, last;


if (left >= right)
   return 1;
swap(polylist, left, (left + right)/2);
last = left;
for (i= left+1; i <= right; i++) {
	if (polylist[i].depth < polylist[left].depth)
	        swap(polylist, ++last, i);
	}
swap(polylist, left, last);
sortdata(polylist, left, last-1);
sortdata(polylist, last+1, right);
}

/****************************************************************************/

#ifndef _NO_PROTO
  void swap(polygon *polylist, int i, int j)
#else
  void swap(polylist, i, j)
	polygon *polylist;
	int i;
	int j;
#endif
{
polygon temp;
 
temp = polylist[i];
polylist[i] = polylist[j];
polylist[j] = temp;
}

/****************************************************************************/

#ifndef _NO_PROTO
  int rotatedata(polygon *polylist, double rotmatrix[3][3], int numpolygons)
#else
  int rotatedata(polylist, rotmatrix, numpolygons)
	polygon *polylist;
	double rotmatrix[3][3];
	int numpolygons;
#endif


{
int i, j;
double tempx, tempy, tempz, zsum;

for (i = 0; i <= numpolygons; i++){
   	zsum = 0;
	for (j = 0; j < polylist[i].numsides; j++){
		tempx = polylist[i].x[j];
		tempy = polylist[i].y[j];
		tempz = polylist[i].z[j];
		polylist[i].x[j] = rotmatrix[0][0] * tempx + rotmatrix [0][1]
		  * tempy + rotmatrix [0][2] * tempz;
		polylist[i].y[j] = rotmatrix[1][0] * tempx + rotmatrix [1][1]
		  * tempy + rotmatrix [1][2] * tempz;
		polylist[i].z[j] = rotmatrix[2][0] * tempx + rotmatrix [2][1]
		  * tempy + rotmatrix [2][2] * tempz;
		zsum=zsum+polylist[i].z[j];
		}
  	polylist[i].depth=zsum/polylist[i].numsides;
	}
}
