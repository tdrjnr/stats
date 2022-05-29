#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h> 
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <X11/Shell.h>
#include "rotate.h"
#include <stdio.h>

GC		gRotateGC;
Widget		gTopLevel;
Widget		gRotateArea;
Widget		gQuitButton;
Widget          gResetButton;
Dimension	gHeight2, gWidth2, gCircleBorder;
Pixel		gbgColor;
ApplicationData gRotateAreaData;

int  update; 	/* Update is one (default) for continuous updating. */
			/* Update zero (-noupdate) if new matrix on release */

#ifndef _NO_PROTO
int set_initial(char **args);
#else
int set_initial();
#endif

/*==========================================================================*/

#ifndef _NO_PROTO
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
  extern Widget gTopLevel, gRotateArea, gQuitButton;

  int i;
  Widget entireArea;
  Display *dpy;
  XGCValues tempGCValues;
  
  gTopLevel= XtInitialize(argv[0], "Rotate", NULL, 0,(Cardinal *)&argc, argv);

  update = 0; /* the default is to output only on button release */
/*
  if(argc>2) {
    fprintf(stderr,"Rotate: too many options\n");
    exit(1);
  }
*/
  i = 0;
  while(++i < argc) {
    if((strcmp(argv[i],"-u") == 0) || 
       (strcmp(argv[i],"-update") == 0))
      {
	update = 1;
      }
    else if(strcmp(argv[i],"-initial") == 0) {
      if(!set_initial(&(argv[++i]))) {
	i--;
	fprintf(stderr,"rotate : Warning, ignoring invalid initial values\n");
      }
      else
	i += 2;
    }
  }



  gRotateArea= XtCreateManagedWidget("rotateArea", xmDrawingAreaWidgetClass,
				     gTopLevel, NULL, 0);

  GetResources();

  gQuitButton= CreateQuitButton(gRotateArea);

  gResetButton = CreateResetButton(gRotateArea);

  CreateMouseTracker(gRotateArea);

  XtRealizeWidget(gTopLevel);
  
  setvbuf(stdout, NULL, _IONBF, 512);
  
  tempGCValues.line_width=2;
  dpy=XtDisplay(gRotateArea);
  gRotateGC=XCreateGC(dpy, XtWindow(gRotateArea), GCLineWidth, &tempGCValues);
  XtMainLoop();
  }
/*===========================================================================*/

#ifndef _NO_PROTO
void GetResources()
#else
void GetResources()
#endif
{

#define XtNxColor "xColor"
#define XtNyColor "yColor"
#define XtNzColor "zColor"
#define XtNcircleColor "circleColor"

  static XtResource RotateAreaResources[4]={
    {XtNxColor, XtCForeground, XtRPixel, sizeof(Pixel),
       XtOffset(ApplicationDataPtr, xColor), XtRString, "red"},
    {XtNyColor, XtCForeground, XtRPixel, sizeof(Pixel),
       XtOffset(ApplicationDataPtr, yColor), XtRString, "forest green"},
    {XtNzColor, XtCForeground, XtRPixel, sizeof(Pixel),
       XtOffset(ApplicationDataPtr, zColor), XtRString, "blue"},
    {XtNcircleColor, XtCForeground, XtRPixel, sizeof(Pixel),
       XtOffset(ApplicationDataPtr, circleColor), XtRString, "black"},
  };

  int     n=0;
  Arg wargs[10];

  /*Make sure sensible defaults exist for rotateArea*/
  n=0;
  XtSetArg(wargs[n], XtNminAspectX, 1); n++;
  XtSetArg(wargs[n], XtNminAspectY, 1); n++;
  XtSetArg(wargs[n], XtNmaxAspectX, 1); n++;
  XtSetArg(wargs[n], XtNmaxAspectY, 1); n++;
  XtSetArg(wargs[n], XtNminWidth, ROTATE_AREA_DEFAULT_WIDTH); n++;
  XtSetArg(wargs[n], XtNminHeight, ROTATE_AREA_DEFAULT_HEIGHT); n++;
  XtSetArg(wargs[n], XtNwidth, ROTATE_AREA_DEFAULT_WIDTH); n++;
  XtSetArg(wargs[n], XtNheight, ROTATE_AREA_DEFAULT_HEIGHT); n++;

  XtSetValues(gTopLevel, wargs, n);

  n=0;
  XtSetArg(wargs[n], XtNbackground, &gbgColor); n++;
  XtGetValues(gRotateArea, wargs, n);

  n=0;
  XtSetArg(wargs[n], XmNmarginWidth, 0); n++;
  XtSetArg(wargs[n], XmNmarginHeight, 0); n++;

  XtSetValues(gRotateArea, wargs, n);

  gWidth2 = ROTATE_AREA_DEFAULT_WIDTH;
  gHeight2 = ROTATE_AREA_DEFAULT_HEIGHT;
  gWidth2 = gWidth2/2;
  gHeight2 = gHeight2/2;
  gCircleBorder = 3*gWidth2/8;
  gWidth2 = gWidth2 - gCircleBorder;
  gHeight2 = gHeight2 - gCircleBorder;

  /*Get the colors of the rotation axes etc.*/
  XtGetApplicationResources(gTopLevel, (XtPointer)&gRotateAreaData,
			    RotateAreaResources,  
			    XtNumber(RotateAreaResources), NULL,0);
}
/*===========================================================================*/

