/*****************************************************************************
 tracking.c
 This contains all the messy code to track the mouse around the window and to 
 change the rotation matrix appropriatly
 *****************************************************************************/


#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include "tracking.h"

/*****************************************************************************/

#ifndef _NO_PROTO
  static double mod3(double *vector)
#else
  static double mod3(vector)
	double *vector;
#endif
{
double sumSq=0;

sumSq+= vector[Xcoord]*vector[Xcoord];
sumSq+= vector[Ycoord]*vector[Ycoord];
sumSq+= vector[Zcoord]*vector[Zcoord];
return sumSq;
}

/*****************************************************************************/

#ifndef _NO_PROTO
  static void cross(double *vector1, double* vector2, double *vector3)
#else
  static void cross(vector1, vector2, vector3)
	double *vector1;
	double *vector2;
	double *vector3;
#endif
{
vector3[Xcoord]= vector1[Ycoord]*vector2[Zcoord] -
	vector1[Zcoord]*vector2[Ycoord];

vector3[Ycoord]= vector1[Zcoord]*vector2[Xcoord] -
	vector1[Xcoord]*vector2[Zcoord];

vector3[Zcoord]= vector1[Xcoord]*vector2[Ycoord] -
	vector1[Ycoord]*vector2[Xcoord];
}

/*****************************************************************************/

#ifndef _NO_PROTO
  static void calcrotmatrix(double *nHat, double sinPhi, 
				      double cosPhi, double rotmatrix[3][3])
#else
  static void calcrotmatrix(nHat, sinPhi, cosPhi, rotmatrix)
	double *nHat;
	double sinPhi;
	double cosPhi;
	double rotmatrix[3][3];
#endif
/* try using small angles approximations */
{
static int leviCivita[3][3][3]={ { {0,0,0},{0,0,1},{0,-1,0} },
				 { {0,0,-1},{0,0,0},{1,0,0} },
				 { {0,1,0},{-1,0,0},{0,0,0} } };
int i, j, k;
double tmp, tmp1[3][3], tmp2[3][3];

/* Calculate the rotation matrix using the generators Sx, Sy, and Sz
 * such that :
 *
 * new_matrix = exp[i*Phi*(Nx*Sx + Ny*Sy + Nz*Sz)]*old_matrix
 *
 * where Nx is the x component of nHat, Ny is the y component of nHat,  
 * Nz is the z component of nHat, and
 *
 *      ( 0  0  0)        ( 0  0  i)        ( 0 -i  0) 
 * Sx = ( 0  0 -i)   Sy = ( 0  0  0)   Sz = ( i  0  0) 
 *      ( 0  i  0)        (-i  0  0)        ( 0  0  0) 
 */

for(i=Xcoord; i<=Zcoord; i++){
	for(j=Xcoord; j<=Zcoord; j++){
		tmp1[i][j]= (i==j?cosPhi:0.0);
		tmp1[i][j]+= nHat[i]*nHat[j]*(1-cosPhi);
		tmp=0;
		for(k=Xcoord; k<=Zcoord; k++)  
			tmp += leviCivita[i][j][k]*nHat[k];
		tmp1[i][j]+= tmp*sinPhi;
		}
	}

for(i=Xcoord; i<=Zcoord; i++){
	for(j=Xcoord; j<=Zcoord; j++){
		tmp2[i][j]=0;
		for(k=Xcoord; k<=Zcoord; k++) 
			tmp2[i][j]+=tmp1[i][k]*rotmatrix[k][j];
		}
	}

for(i=Xcoord; i<=Zcoord; i++)
	for(j=Xcoord; j<=Zcoord; j++)
     		rotmatrix[i][j]=tmp2[i][j];
}

/*****************************************************************************/

#ifndef _NO_PROTO
  void initializematrix(double rotmatrix[3][3])
#else
  void initializematrix(rotmatrix)
	double rotmatrix[3][3];
#endif


{
rotmatrix[0][0] = 1;
rotmatrix[0][1] = 0;
rotmatrix[0][2] = 0;
rotmatrix[1][0] = 0;
rotmatrix[1][1] = 1;
rotmatrix[1][2] = 0;
rotmatrix[2][0] = 0;
rotmatrix[2][1] = 0;
rotmatrix[2][2] = 1;
}

/*****************************************************************************/

#ifndef _NO_PROTO
  void starttracking(XEvent *startevent, double start[3], double radius,
		     unsigned int width)
#else
  void starttracking(startevent, start, radius, width)
	XEvent *startevent;
	double start[3];
	double radius;
	unsigned int width;
#endif

{
int dim;
int x, y;

dim = width / 2;
x = startevent->xbutton.x;
y = startevent->xbutton.y;

start[Xcoord] = (double)(x - dim) / dim;
start[Ycoord] = (double)(dim - y) / dim;
radius = start[Xcoord] * start[Xcoord] + start[Ycoord] * start[Ycoord];

if (radius < 1 - Delta)
	start[Zcoord] = sqrt(1 - radius);
else
	start[Zcoord] = 0;
}
/*****************************************************************************/

#ifndef _NO_PROTO
  void trackmotion(XEvent *moveevent, double rotmatrix[3][3], double start[3],
		   double radius, unsigned int width)
#else
  void trackmotion(moveevent, rotmatrix,start, radius, width)
	XEvent *moveevent;
	double rotmatrix[3][3];
	double start[3];
	double radius;
	unsigned int width;
#endif

{
int dim;
int x, y;
int i;
double min;
double new[3];
double newradius;
double perp[3], nhat[3];
double phi, norm, sinphi, cosphi;

dim =width / 2;
x = moveevent->xbutton.x;
y = moveevent->xbutton.y;
min = 1- Delta;

new[Xcoord] = (double)(x - dim) / dim;
new[Ycoord] = (double)(dim - y) / dim;
newradius = new[Xcoord] * new[Xcoord] + new[Ycoord] * new[Ycoord];

if (newradius < min)
	new[Zcoord] = sqrt(1 - newradius);
else
	new[Zcoord] = 0;

if (x > width || y > width || start[Xcoord] > dim || start[Ycoord] > dim) {
	for (i = Xcoord; i <= Zcoord; i++)
		start[i] = new[i];
	return;
	}

if ((newradius < min && radius < min) || (newradius > min && radius > min)) {
	for (i = Xcoord; i <= Zcoord; i++)
		perp[i] = start[i] - new[i];
	phi = -sqrt(mod3(perp)) / (radius > 1 ? sqrt(radius) : 1);
	if (fabs(phi) < EPS)
		return;
	cross(new, start, nhat);
	sinphi = mod3(nhat);
	norm = sqrt(sinphi);
	if (radius > 1)
		sinphi = sinphi / (radius * newradius);
	cosphi = sqrt(1 - sinphi);
	sinphi = sqrt(sinphi);
	if (norm != 0) {
		norm = 1 / norm;
		nhat[Xcoord] *= norm;
		nhat[Ycoord] *= norm;
		nhat[Zcoord] *= norm;
		}

	calcrotmatrix(nhat, sinphi, cosphi, rotmatrix);
	}

for (i = Xcoord; i <= Zcoord; i++)
	start[i] = new[i];
radius = newradius;
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


