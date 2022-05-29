#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <math.h>
#include "rotate.h"
#define DELTA 1e-6

static double gRotationMatrix[3][3]={{1,0,0},
		       	            {0,1,0},
			            {0,0,1}};
static double gStartPoint[3];
static double gAxisEnds[3][3]={{1,0,0},
                            {0,1,0},
                            {0,0,1}};

static double gRadiusSquared;

extern int  update; 	/* Update is one (default) for continuous updating. */
                        /* Update zero (-noupdate) if new matrix on release */






/*===========================================================================*/

#ifndef _NO_PROTO
int set_initial(char **args)
#else
int set_initial(args)
char **args;
#endif
{
  static double alpha = 0.0;
  static double beta = 0.0;
  static double gamma = 0.0;
  int i, to_return;

  to_return = 1;
  if(args) {
    if(!(sscanf(*(args++),"%lf", &alpha) && alpha < 3.2 && alpha > -3.2 &&
	 sscanf(*(args++),"%lf", &beta) && beta < 3.2 && beta > -3.2 &&
	 sscanf(*args,"%lf", &gamma) && gamma < 3.2 && gamma > -3.2))
      {
	alpha = 0.0;
	beta = 0.0;
	gamma = 0.0;
	to_return = 0;
      }
  }
  
  gRotationMatrix[0][0]=cos(alpha)*cos(beta);
  gRotationMatrix[0][1]=cos(beta)*sin(alpha);
  gRotationMatrix[0][2]=-sin(beta);

  gRotationMatrix[1][0]=sin(gamma)*sin(beta)*cos(alpha) -sin(alpha)*cos(gamma);
  gRotationMatrix[1][1]=sin(gamma)*sin(beta)*sin(alpha) +cos(alpha)*cos(gamma);
  gRotationMatrix[1][2]=cos(beta)*sin(gamma);

  gRotationMatrix[2][0]=cos(gamma)*sin(beta)*cos(alpha) +sin(gamma)*sin(alpha);
  gRotationMatrix[2][1]=cos(gamma)*sin(beta)*sin(alpha) -sin(gamma)*cos(alpha);
  gRotationMatrix[2][2]=cos(beta)*cos(gamma);

  for(i=X_coOrd; i<=Z_coOrd; i++) {
    gAxisEnds[i][X_coOrd] = gRotationMatrix[i][X_coOrd];
    gAxisEnds[i][Y_coOrd] = gRotationMatrix[i][Y_coOrd];
    gAxisEnds[i][Z_coOrd] = gRotationMatrix[i][Z_coOrd];
  }
  return(to_return);
}

/*===========================================================================*/

#ifndef _NO_PROTO
static double Mod3(double *vector)
#else
static double Mod3(vector)
double *vector;
#endif
{
  int i;
  double sumSq=0;

  sumSq+= vector[X_coOrd]*vector[X_coOrd];
  sumSq+= vector[Y_coOrd]*vector[Y_coOrd];
  sumSq+= vector[Z_coOrd]*vector[Z_coOrd];
  return sumSq;
}

/*===========================================================================*/

#ifndef _NO_PROTO
static void Cross(double *vector1, double* vector2, double *vector3)
#else
static void Cross(vector1, vector2, vector3)
double *vector1;
double *vector2;
double *vector3;
#endif
{
  vector3[X_coOrd]= vector1[Y_coOrd]*vector2[Z_coOrd] -
    vector1[Z_coOrd]*vector2[Y_coOrd];

  vector3[Y_coOrd]= vector1[Z_coOrd]*vector2[X_coOrd] -
    vector1[X_coOrd]*vector2[Z_coOrd];

  vector3[Z_coOrd]= vector1[X_coOrd]*vector2[Y_coOrd] -
    vector1[Y_coOrd]*vector2[X_coOrd];
}

/*===========================================================================*/

#ifndef _NO_PROTO
static void CalculateRotationMatrix(double *nHat, double sinPhi, 
				    double cosPhi)
#else
static void CalculateRotationMatrix(nHat, sinPhi, cosPhi)
double *nHat;
double sinPhi;
double cosPhi;
#endif
/* try using small angles approximations */
{
  static int leviCivita[3][3][3]={ { {0,0,0},{0,0,1},{0,-1,0} },
				   { {0,0,-1},{0,0,0},{1,0,0} },
				   { {0,1,0},{-1,0,0},{0,0,0} } };
  
  int	i, j, k;
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

  for(i=X_coOrd; i<=Z_coOrd; i++){
    for(j=X_coOrd; j<=Z_coOrd; j++){

      tmp1[i][j]= (i==j?cosPhi:0.0);
      tmp1[i][j]+= nHat[i]*nHat[j]*(1-cosPhi);

      tmp=0;
      for(k=X_coOrd; k<=Z_coOrd; k++) {  
	tmp += leviCivita[i][j][k]*nHat[k];
      }
      tmp1[i][j]+= tmp*sinPhi;
    }
  }

  for(i=X_coOrd; i<=Z_coOrd; i++){
    for(j=X_coOrd; j<=Z_coOrd; j++){
      tmp2[i][j]=0;
      for(k=X_coOrd; k<=Z_coOrd; k++) {  
	tmp2[i][j]+=tmp1[i][k]*gRotationMatrix[k][j];
      }
    }
  }

  for(i=X_coOrd; i<=Z_coOrd; i++){
    for(j=X_coOrd; j<=Z_coOrd; j++){
      gRotationMatrix[i][j]=tmp2[i][j];
    }
  }
}

/*==========================================================================*/

#ifndef _NO_PROTO
void PrintRotationMatrix()
#else
void PrintRotationMatrix()
#endif
 /* Puts rotation matrix to stdout */
{  
  int i, j;

  for(i=X_coOrd; i<=Z_coOrd; i++){
    for(j=X_coOrd; j<=Z_coOrd; j++){
      printf("%9.6lf ", gRotationMatrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

/*==========================================================================*/

#ifndef _NO_PROTO
void StartTracking(Widget gRotateArea, XtPointer clientData, 
		   XEvent *theEventPtr)
#else
void StartTracking(gRotateArea, clientData, theEventPtr)
Widget gRotateArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{
  gStartPoint[X_coOrd]=(double)
    (theEventPtr->xbutton.x-(int)(gWidth2+gCircleBorder))/(double)gWidth2;
  gStartPoint[Y_coOrd]=(double)
    ((int)(gHeight2+gCircleBorder)-theEventPtr->xbutton.y)/(double)gHeight2;
  gRadiusSquared = gStartPoint[X_coOrd] * gStartPoint[X_coOrd] +
    gStartPoint[Y_coOrd]*gStartPoint[Y_coOrd];
  
  if(gRadiusSquared<1-DELTA) {
    if((1-gRadiusSquared)>0)
      gStartPoint[Z_coOrd]=sqrt(1-gRadiusSquared);
    else {
      fprintf(stderr,"rotate : StartTracking : negative sqrt\n");
      exit(1);
    }
  } else {	
    gStartPoint[Z_coOrd]=0;
  }
}
/*===========================================================================*/

#ifndef _NO_PROTO
void EndTracking(Widget gRotateArea, XtPointer clientData, 
		 XEvent *theEventPtr)
#else
void EndTracking(gRotateArea, clientData, theEventPtr)
Widget gRotateArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{
  if(update==0) PrintRotationMatrix();
}

/*==========================================================================*/

#ifndef _NO_PROTO
void DrawNewAxes(void)
#else
void DrawNewAxes()
#endif
{
  int 		 i, j, k;
  XSegment  	 axes[3];
  static Display *dpy;

  dpy = XtDisplay(gRotateArea);
  
  for(i=X_coOrd; i<=Z_coOrd; i++) {	
    axes[i].x1=gWidth2+gCircleBorder;
    axes[i].y1=gHeight2+gCircleBorder;
    axes[i].x2=(short)((.9*gAxisEnds[X_coOrd][i]+1)*gWidth2+gCircleBorder);
    axes[i].y2=(short)((1-.9*gAxisEnds[Y_coOrd][i])*gHeight2+gCircleBorder);
  }
  XSetForeground(dpy, gRotateGC, gbgColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, axes, 3);
  
  for(i=X_coOrd; i<=Z_coOrd; i++)
    for(j=X_coOrd; j<=Z_coOrd; j++)
      gAxisEnds[i][j]=0.0;

  for(i=X_coOrd; i<=Z_coOrd; i++)
    for(j=X_coOrd; j<=Z_coOrd; j++)
      for(k=X_coOrd; k<=Z_coOrd; k++)
        gAxisEnds[i][j]+=gRotationMatrix[i][k]*(k==j);
  
  for(i=0; i<3; i++) {
    axes[i].x1=gWidth2+gCircleBorder;
    axes[i].y1=gHeight2+gCircleBorder;
    axes[i].x2=(short)((.9*gAxisEnds[0][i]+1)*gWidth2+gCircleBorder);
    axes[i].y2=(short)((1-.9*gAxisEnds[1][i])*gHeight2+gCircleBorder);
  }

  XSetForeground(dpy, gRotateGC, gRotateAreaData.xColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, axes, 1);
  XSetForeground(dpy, gRotateGC, gRotateAreaData.yColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, &axes[1], 1);
  XSetForeground(dpy, gRotateGC, gRotateAreaData.zColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, &axes[2], 1);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void TrackMousePosition(Widget gRotateArea, XtPointer clientData, 
			XEvent *theEventPtr)
#else
void TrackMousePosition(gRotateArea, clientData, theEventPtr)
Widget gRotateArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
# define EPS	    (1.0/200)
{
  static num_rotations = 0;
  static double tmp2[3][3] = { {0, 0, 0},
			       {0, 0, 0},
			       {0, 0, 1} };
  double perpVector[3], nHat[3], presentPoint[3];
  double presentRadiusSquared, phi, norm, cosPhi, sinPhi;
  int i, j;
  
  presentPoint[X_coOrd]=(double)
    (theEventPtr->xbutton.x-(int)(gWidth2+gCircleBorder))/(double)gWidth2;
  presentPoint[Y_coOrd]=(double)
    ((int)(gHeight2+gCircleBorder)-theEventPtr->xbutton.y)/(double)gHeight2;
  presentRadiusSquared = presentPoint[X_coOrd] * presentPoint[X_coOrd] +
    presentPoint[Y_coOrd] * presentPoint[Y_coOrd];
  
  if(presentRadiusSquared<1-DELTA) {	
    presentPoint[Z_coOrd]=sqrt(1-presentRadiusSquared);
  } else {	
    presentPoint[Z_coOrd]=0;
  }

  if(gStartPoint[X_coOrd]>(double)(gWidth2 + gCircleBorder) ||
     gStartPoint[Y_coOrd]>(double)(gWidth2 + gCircleBorder) ||
     theEventPtr->xbutton.x>2*(gWidth2+gCircleBorder)       ||
     theEventPtr->xbutton.y>2*(gHeight2+gCircleBorder))

    {
      gStartPoint[X_coOrd] = presentPoint[X_coOrd];
      gStartPoint[Y_coOrd] = presentPoint[Y_coOrd];
      gStartPoint[Z_coOrd] = presentPoint[Z_coOrd];
      return;
    }

  /* Calculate the rotation matrix */
  if((presentRadiusSquared<1-DELTA && gRadiusSquared<1-DELTA) ||
     (presentRadiusSquared>1-DELTA && gRadiusSquared>1-DELTA)) {

    for(i=X_coOrd; i<=Z_coOrd; i++)
      perpVector[i]=-presentPoint[i]+gStartPoint[i];

    phi = -sqrt(Mod3(perpVector))/
      (gRadiusSquared>1?sqrt(gRadiusSquared):1);

    if( fabs(phi)< EPS ) return;

    Cross(presentPoint, gStartPoint, nHat);

    sinPhi = Mod3(nHat);
    norm = sqrt(sinPhi);
    if(gRadiusSquared>1) {
      sinPhi = sinPhi/(gRadiusSquared * presentRadiusSquared);
    }
    cosPhi = sqrt(1 - sinPhi);
    sinPhi = sqrt(sinPhi);

    if(norm!=0) {   
      norm=1/norm;
      nHat[X_coOrd]*=norm;
      nHat[Y_coOrd]*=norm;
      nHat[Z_coOrd]*=norm;
    }

    CalculateRotationMatrix(nHat, sinPhi, cosPhi);

    if(update==1) PrintRotationMatrix();
  }

  /* Set gStartPoint and gRadiusSquared for the next call to this callback */
  for(i=X_coOrd; i<=Z_coOrd; i++)
    gStartPoint[i]=presentPoint[i];
  gRadiusSquared=presentRadiusSquared;
  
  DrawNewAxes();
}

/*==========================================================================*/

#ifndef _NO_PROTO
void HandleExposure(Widget w, caddr_t clientData, caddr_t callData)
#else
void HandleExposure(w, clientData, callData)
Widget w;
caddr_t clientData;
caddr_t callData;
#endif
{
  int i;
  XSegment       axes[3];
  static Display *dpy;

  if(!XtIsRealized(gRotateArea)) return;

  dpy = XtDisplay(gRotateArea);

  for(i=0; i<3; i++) {	
    axes[i].x1=gWidth2+gCircleBorder;
    axes[i].y1=gHeight2+gCircleBorder;
    axes[i].x2=(short)((.9*gAxisEnds[0][i]+1)*gWidth2
		       +gCircleBorder);
    axes[i].y2=(short)((1-.9*gAxisEnds[1][i])*gHeight2
		       +gCircleBorder);}
  
  XSetForeground(dpy, gRotateGC, gRotateAreaData.xColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, axes, 1);
  XSetForeground(dpy, gRotateGC, gRotateAreaData.yColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, &axes[1], 1);
  XSetForeground(dpy, gRotateGC, gRotateAreaData.zColor);
  XDrawSegments(dpy, XtWindow(gRotateArea), gRotateGC, &axes[2], 1);

  XSetForeground(dpy, gRotateGC, gRotateAreaData.circleColor);
  XDrawArc(dpy, XtWindow(gRotateArea), gRotateGC, gCircleBorder,
	   gCircleBorder, 2*gWidth2, 2*gHeight2, 0, 360*64);
}

/*==========================================================================*/

#ifndef _NO_PROTO
void HandleResize(Widget w, caddr_t clientData, caddr_t callData)
#else
void HandleResize(w, clientData, callData)
Widget w;
caddr_t clientData;
caddr_t callData;
#endif
{
  int n;
  Arg wargs[2];
  Dimension width, height, rwidth, rborder, xpos;
  Position x, y;
  static Display *dpy;

  dpy = XtDisplay(gRotateArea);

  /* Arrange the quit button widget on the form*/
  n = 0;
  XtSetArg(wargs[n], XmNmarginWidth, &width); n++;
  XtSetArg(wargs[n], XmNmarginHeight, &height); n++;
  XtGetValues(gQuitButton, wargs, n);
  x = (Position)width;
  y = (Position)height;
  
  n = 0;
  XtSetArg(wargs[n], XmNheight, &height); n++;
  XtSetArg(wargs[n], XmNwidth, &width); n++;
  XtGetValues(gRotateArea, wargs, n);

  n = 0;
  XtSetArg(wargs[n], XmNx, x); n++;
  XtSetArg(wargs[n], XmNy, y); n++;
  XtSetValues(gQuitButton, wargs, n);

  n=0;
  XtSetArg(wargs[n], XmNwidth, &rwidth); n++;
  XtSetArg(wargs[n], XmNborderWidth, &rborder); n++;
  XtGetValues(gResetButton, wargs,n);

  /* Change the location of the pushbutton */
  xpos = width-rwidth-rborder;
  XtSetArg(wargs[0],XmNx,xpos);
  XtSetValues(gResetButton,wargs,1);
  
  if(XtIsRealized(gRotateArea)) {
    XSetForeground(dpy, gRotateGC, gbgColor);
    XFillRectangle(dpy,XtWindow(gRotateArea),gRotateGC, 0,0, width, height);
    XSetForeground(dpy, gRotateGC, gRotateAreaData.xColor);
  }
  if(width > height) {   
    width = height;
    if(XtIsRealized(gRotateArea))
      XSetForeground(dpy, gRotateGC, gRotateAreaData.circleColor);
      XDrawLine(dpy,XtWindow(gRotateArea),gRotateGC, width,0,width,height);
  }
  if(height > width) {
    height = width;
    if(XtIsRealized(gRotateArea))
      XSetForeground(dpy, gRotateGC, gRotateAreaData.circleColor);
      XDrawLine(dpy,XtWindow(gRotateArea),gRotateGC, 0,height,width,height);
  }
  /* now width = height */
  width = width/2;
  height = width;
  gCircleBorder = 3*width/8;
  gWidth2 = width - gCircleBorder;
  gHeight2 = gWidth2;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void CreateMouseTracker(Widget gRotateArea)
#else
void CreateMouseTracker(gRotateArea)
Widget gRotateArea;
#endif
{
  XtAddEventHandler(gRotateArea, ButtonPressMask, FALSE,
		    (XtEventHandler) StartTracking, NULL);
  XtAddEventHandler(gRotateArea, ButtonMotionMask, FALSE,
		    (XtEventHandler) TrackMousePosition, NULL);
  XtAddEventHandler(gRotateArea, ButtonReleaseMask, FALSE, 
                     (XtEventHandler) EndTracking, NULL);
  XtAddCallback(gRotateArea, XmNexposeCallback, HandleExposure, NULL);
  XtAddCallback(gRotateArea, XmNresizeCallback, HandleResize, NULL);
  XtAddCallback(gRotateArea, XmNresizeCallback, HandleExposure, NULL);
}

/*==========================================================================*/

