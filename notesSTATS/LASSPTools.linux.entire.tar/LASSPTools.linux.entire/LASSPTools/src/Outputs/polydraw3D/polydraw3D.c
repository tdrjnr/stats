/*****************************************************************************
 *polydraw3D.c
 *This is the main source file for a program to throw open a window and
 *plot polygons in it.
 *
 *BLAH, BLAH,BLAH.... 
 ****************************************************************************/

#ifndef _NO_PROTO 
#include <stdlib.h>
#endif
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "polydraw3D.h"
#include "parseargument.h"
#include "processdata.h"

/****************************************************************************/
/*These are global variable to make function calls much smoother*/ 

/*Boolean flags for options */
char binary, buffer, colors, fill, frames, help, interactive, didit;
char backstore, perspective;

/* background and foreground pixel values */
static unsigned long polyfg, polybg;

/* window variables */

static Display *polydisplay;
static Window polywin;
static Pixmap polypix;
static Colormap colormap;
static GC polygc;

/****************************************************************************/
#ifndef _NO_PROTO
  void readoptions(int argc, char **argv, double *maxvalue, char *geometry,
 		char *title, double *perspval)
#else
  void readoptions(argc, argv, maxvalue, geometry, title, perspval)
	int argc;
	char **argv;
	double *maxvalue;
	char *geometry;
	char *title;
        double *perspval;
#endif

{
char *temp;

/*Initialize variables*/
binary = FALSE;
buffer = FALSE;
colors = TRUE;
fill = TRUE;
frames = TRUE;
help = FALSE;
interactive = FALSE;
perspective = FALSE;
*maxvalue = DEFMAX;
strcpy(title, DEFTITLE);
strcpy(geometry, DEFGEOM);
*perspval = DEFPVAL;

/*Use Parseargument to read command line options*/
if (parseargument(argc,argv,0,&temp,"-binary") != -1)
	binary = TRUE;

if (parseargument(argc,argv,0,&temp,"-b") != -1)
	binary = TRUE;

if(parseargument(argc,argv,0,&temp,"-buffer") != -1)
	buffer = TRUE;

if(parsenextargument(argc,argv,0,&temp,"-geometry") != -1)
	strcpy(geometry, temp);

if(parsenextargument(argc,argv,0,&temp,"-geom") != -1)
	strcpy(geometry, temp);

if(parsenextargument(argc,argv,0,&temp,"-g") != -1)
	strcpy(geometry, temp);

if(parseargument(argc,argv,0,&temp,"-help") != -1)
	help = TRUE;

if(parseargument(argc,argv,0,&temp,"-h") != -1)
	help = TRUE;

if(parseargument(argc,argv,0,&temp,"-interactive") != -1)
	interactive = TRUE;

if(parseargument(argc,argv,0,&temp,"-i") != -1)
	interactive = TRUE;

if(parsenextargument(argc,argv,0,&temp,"-max") != -1)
	sscanf(temp,"%lf",maxvalue);

if(parseargument(argc,argv,0,&temp,"-nocolors") != -1)
	colors = FALSE;

if(parseargument(argc,argv,0,&temp,"-nofill") != -1)
	fill = FALSE;

if(parseargument(argc,argv,0,&temp,"-noframes") != -1)
	frames = FALSE;

if(parseargument(argc,argv,0,&temp,"-perspective") != -1)
	perspective = TRUE;

if(parseargument(argc,argv,0,&temp,"-p") != -1)
	perspective = TRUE;

if(parsenextargument(argc,argv,0,&temp,"-pval") != -1) {
	sscanf(temp,"%lf",perspval);
	perspective = TRUE;
	}

if(parsenextargument(argc,argv,0,&temp,"-title") != -1)
	strcpy(title, temp);

}
/****************************************************************************/
/*rewite usage statement*/


#ifndef _NO_PROTO
  void displayusage()
#else
  void displayusage()
#endif
{ 
fprintf(stderr,"Usage:\tPolydraw [-options] < data \n\n");
fprintf(stderr,"Options: \n\n");
fprintf(stderr,"  -binary            or -b   Reads data from a binary file.\n");
fprintf(stderr,"  -buffer            Writes the data to a buffer first and then prints\n");
fprintf(stderr,"                     the entire image at once to the screen.\n");
fprintf(stderr,"  -geometry WxW+X+Y  or -geom spec or -g spec\n");
fprintf(stderr,"  -help              or -h    You already know.\n");
fprintf(stderr,"  -interactive       or -i    Interactive mode draws each polygon separately\n");
fprintf(stderr,"  -max maxval        maxval is the maximum value corresponding to the window\n");
fprintf(stderr,"  -nocolors          no colors\n");
fprintf(stderr,"  -nofill            wire frame\n");
fprintf(stderr,"  -noframes          no frames\n");
fprintf(stderr,"  -perspective       or -p    Draw image in perspective\n");
fprintf(stderr,"  -pval perval       perval is the degree of perspectivity\n");
fprintf(stderr,"  -title name        name is the title of the window\n\n\n");
fprintf(stderr,"Other info: \n");
fprintf(stderr,"  1) B,C,F,I,P,Q can be pressed while the cursor is over the polydraw3D\n");
fprintf(stderr,"window.  B, C, F, I, P toggle the buffer, color, fill, interactive, and\n");
fprintf(stderr,"perspective flags respectively. Q quits.\n");
fprintf(stderr,"  2) + and - can be pressed to zoom in and out. Be careful, zooming in on\n");
fprintf(stderr,"perspective mode has little effect, and toggling the perspective flag after\n");
fprintf(stderr,"zooming in a lot will cause a large orthogonal projection.\n");
fprintf(stderr,"  3) Use the mouse to rotate the image.\n\n");
}
/****************************************************************************/
#ifndef _NO_PROTO
  void drawpolygon(XPoint *vertices, int numsides, XColor *color)
#else
  void drawpolygon(vertices, numsides, color)
	XPoint *vertices;
	int numsides;
	XColor *color;
#endif

{
/*Set pixel color*/
if (colors) {
	if (!XAllocColor(polydisplay, colormap, color)) {
		fprintf(stderr,"polydraw: failure in XAllocColor.\n");
		exit(1);
		}
	XSetForeground(polydisplay, polygc, color->pixel);
	} 
else 
	XSetForeground(polydisplay, polygc, polybg); 

/*Draw polygon*/

if (fill){ 
	if (buffer || !backstore)
		XFillPolygon(polydisplay, polypix, polygc, vertices,
			     numsides, Nonconvex, CoordModeOrigin);
	if (!buffer)
		XFillPolygon(polydisplay, polywin, polygc, vertices,
			     numsides, Nonconvex, CoordModeOrigin);
	}

if (frames) {
	if (!fill && colors)
		XSetForeground(polydisplay, polygc, color->pixel);
	else 
		XSetForeground(polydisplay, polygc, polyfg); 
	if (buffer || !backstore)
		XDrawLines(polydisplay, polypix, polygc, vertices,
			     numsides+1, CoordModeOrigin);
	if (!buffer)
		XDrawLines(polydisplay, polywin, polygc, vertices,
			     numsides+1, CoordModeOrigin);
      }

if (interactive && !buffer)
	XFlush(polydisplay);
}

/****************************************************************************/
#ifndef _NO_PROTO
  void scalepoints(XPoint *vertices, polygon side, unsigned int width,
		 double max, double pval)
#else
  void scalepoints(vertices, side, width, max, pval)
	XPoint *vertices;
	polygon side;
	unsigned int width;
	double max;
        double pval;
#endif

{
int i;
int dim;

dim = width / 2;
if (!perspective)
	for (i = 0; i < side.numsides; i++) {
		vertices[i].x = (short) floor(dim * (1 + (side.x[i] / max))
					      + 0.5);
		vertices[i].y = (short) floor(dim * (1 - (side.y[i] / max)) 
					      + 0.5);
		}
else
	for (i = 0; i < side.numsides; i++) {
	vertices[i].x = (short) floor(dim * (1 + (side.x[i] / 
						 (max-side.z[i]+pval))) + 0.5);
	vertices[i].y = (short) floor(dim * (1 - (side.y[i] / 
						 (max-side.z[i]+pval))) + 0.5);
		}
vertices[side.numsides].x = vertices[0].x;
vertices[side.numsides].y = vertices[0].y;
}
/****************************************************************************/
#ifndef _NO_PROTO
  void displaydata(polygon *data, int numpolygons, 
		   unsigned int width, unsigned int height, 
		   double max, double pval)
#else
  void displaydata(data, numpolygons, width, height, max, pval)
	polygon *data;
	int numpolygons;
	unsigned int width;
	unsigned int height;
	double max;
        double pval;
#endif

{
int i;
XPoint vertices[MAXSIDESp1];
XColor color;


for (i = 0; i <= numpolygons; i++) {
	scalepoints(vertices, data[i], width, max, pval);
	color.red = (unsigned short)data[i].red;
	fprintf(stderr,"");
	fprintf(stderr,""); 
	color.green = (unsigned short)data[i].green;
	fprintf(stderr,""); 
	color.blue = (unsigned short)data[i].blue;	
	fprintf(stderr,""); 
	drawpolygon(vertices, data[i].numsides, &color);
	fprintf(stderr,""); 
      }

if (buffer) {
	XClearWindow(polydisplay, polywin);
	XCopyArea(polydisplay, polypix, polywin, polygc, 0, 0,
		  width, height, 0, 0);
      }
}

/*
**   main()
**
**
*/

#ifndef _NO_PROTO
  int main(int argc, char **argv)
#else
  int main(argc, argv)
	int argc;
	char **argv;
#endif
{
   int polyscreen;
   XSizeHints polyhint;
   XEvent polyevent;
   XWindowAttributes winattr;
   XSetWindowAttributes attrib;
   KeySym polykey;

   double maxvalue;	/*maximal coordinate value(unscaled)*/
   double perspval;
   char geometry[20];	/*window geometry*/
   char title[80];		/*window title*/
   int posx, posy;		/*position of window*/ 
   unsigned int width, height;	/*window size*/
   unsigned int maxwinsize;	/*maximum windowsize*/
   int borderwidth;
   char done;
   int i;
   char text[10];
   polygon data[MAXPOLY];
   int numpolygons;
   double radius;
   double rotmatrix[3][3];
   double start[3];

   /*Read in options and set appropriate variables*/

   readoptions(argc, argv, &maxvalue, geometry, title, &perspval);

   if (help) {
	displayusage();
	exit(0);
   }


   /*Initialization */

   if( (polydisplay = XOpenDisplay("")) == NULL ) {
	fprintf(stderr,"Cannot connect to server.\n");
        fflush(stderr);
        exit(-1);
	}

   polyscreen = DefaultScreen(polydisplay);
   width = DisplayWidth(polydisplay, polyscreen);
   height = DisplayHeight(polydisplay, polyscreen);
   if (width > height) 
	maxwinsize = height;
   else
	maxwinsize = width;

   /*Set default pixel values */

   polybg = WhitePixel(polydisplay, polyscreen);
   polyfg = BlackPixel(polydisplay, polyscreen);

   /*Parse geometry and set xhints values*/

   XParseGeometry(geometry, &posx, &posy, &width, &height);
   polyhint.flags = PPosition | PSize | PMaxSize | PAspect;
   polyhint.x = posx;
   polyhint.y = posy;
   polyhint.width = (int)width;
   polyhint.height = (int)height;
   polyhint.max_width = (int)maxwinsize;
   polyhint.max_height = (int)maxwinsize;
   polyhint.min_aspect.x = 1;	/*set aspect ratios forcing*/
   polyhint.min_aspect.y = 1;	/*window to be square*/
   polyhint.max_aspect.x = 1;
   polyhint.max_aspect.y = 1;
   borderwidth = 5;

   /*Create window*/

   polywin = XCreateSimpleWindow(polydisplay, DefaultRootWindow(polydisplay),
			posx, posy, width, height, borderwidth,
			polyfg, polybg);
   XSetWindowBackground(polydisplay, polywin, polybg);
   XSetStandardProperties(polydisplay, polywin, title, title, None,
		       argv, argc, &polyhint);

   /*Create GC and intializite it*/

   polygc = XCreateGC(polydisplay, polywin, 0, 0);
   XSetBackground(polydisplay, polygc, polybg);
   XSetForeground(polydisplay, polygc, polyfg);

   /*Select event types wanted*/

   XSelectInput(polydisplay, polywin, KeyPressMask | ExposureMask |
	     ButtonPressMask | ButtonReleaseMask | Button1MotionMask |
	     StructureNotifyMask);

   /*Display Window*/

   XMapRaised(polydisplay, polywin);

   /*Set up color map*/

   colormap = DefaultColormap(polydisplay, polyscreen);
   XSetWindowColormap(polydisplay, polywin, colormap);

   /*Get actual window attributes*/

   XGetWindowAttributes(polydisplay, polywin, &winattr);
   width = winattr.width;
   height = winattr.height;

   /*Generate and clear buffer pixmap*/
   polypix = XCreatePixmap(polydisplay, polywin, maxwinsize, maxwinsize,
			DefaultDepth(polydisplay, polyscreen));
   XSetForeground(polydisplay, polygc, polybg);
   XFillRectangle(polydisplay, polypix, polygc, 0, 0, maxwinsize, maxwinsize);

   /*Wait for initial expose event*/
   done = FALSE;
   while(!done) {
	XNextEvent(polydisplay, &polyevent);
	if (polyevent.type == Expose)
		done = TRUE;
	}

   /*Turn on backing store*/
   backstore = DoesBackingStore(DefaultScreenOfDisplay(polydisplay));
	if (backstore) {
		attrib.backing_store = Always;
		XChangeWindowAttributes (polydisplay, polywin, CWBackingStore,
			&attrib);
	}

   /*Display window again*/

   XMapRaised(polydisplay, polywin);
   XSetForeground(polydisplay, polygc, polyfg);
   XClearWindow(polydisplay, polywin);
   XFlush(polydisplay);

   /*Read in data stream*/
   readdata(data, &numpolygons);
   sortdata(data, 0, numpolygons);
   /*displaydata(data, numpolygons, width, height, maxvalue, perspval);*/

   /*Event loop*/
   done = FALSE;
   while(!done) {
	didit=FALSE;
	XNextEvent(polydisplay, &polyevent);
	switch(polyevent.type) {
		case KeyPress:
			didit=TRUE;
			i = XLookupString((XKeyEvent *)&polyevent, text,
					  10, &polykey, 0);

			if (i == 1 && (text[0] == 'q' || text [0] == 'Q'))
			  	done = TRUE;

			if (i == 1 && text[0] == '+')
			  	maxvalue = maxvalue / 2;

			if (i == 1 && text[0] == '-')
			  	maxvalue = maxvalue * 2;

			if (i == 1 && (text[0] == 'b' || text [0] == 'B')){
				if (!buffer)
					buffer = TRUE;
				else
				  	buffer = FALSE;
			  }

			if (i == 1 && (text[0] == 'c' || text [0] == 'C')){
				if (!colors)
					colors = TRUE;
				else
				  	colors = FALSE;
			  }

			if (i == 1 && (text[0] == 'i' || text [0] == 'I')){
				if (!interactive)
					interactive = TRUE;
				else
				  	interactive = FALSE;
			  }

			if (i == 1 && (text[0] == 'p' || text [0] == 'P')){
				if (!perspective)
					perspective = TRUE;
				else
				  	perspective = FALSE;
			  }

			if (i == 1 && (text[0] == 'f' || text [0] == 'F')){
				if (!fill)
					fill = TRUE;
				else
				  	fill = FALSE;
			  }

			if (!done){
			  XSetForeground(polydisplay, polygc, polybg);
			  XFillRectangle(polydisplay, polypix, polygc,
					 0, 0, width, height);
			  if (!buffer)
			    XClearWindow(polydisplay, polywin);
			  displaydata(data, numpolygons, width,
				      height, maxvalue, perspval);
			}

		case Expose:
			didit = TRUE;
			XCopyArea(polydisplay, polypix, polywin, polygc,
				  0, 0, width, height, 0, 0);
		case ButtonPress:
			didit = TRUE;
			if (polyevent.xbutton.button == Button1) {
				initializematrix(rotmatrix);
				starttracking(&polyevent, start, radius, 
					      width);
				while (!done) {
				  XNextEvent(polydisplay, &polyevent);
				  if (polyevent.type == MotionNotify) 
				    trackmotion(&polyevent, rotmatrix, start, 
					    radius, width);
				  else if (polyevent.type == ButtonRelease &&
					   polyevent.xbutton.button==Button1) {
					rotatedata(data, rotmatrix, 
						   numpolygons);
					sortdata(data, 0, numpolygons);
					XSetForeground(polydisplay, polygc,
						       polybg);
					XFillRectangle(polydisplay, polypix,
						  polygc, 0, 0, width, height);
					if (!buffer)
					  XClearWindow(polydisplay, polywin);
					displaydata(data, numpolygons, width,
						    height, maxvalue,perspval);
					done = TRUE;
		      			}
				}
				done = FALSE; 
			}

		case ConfigureNotify:
			if (!didit) {
				width = polyevent.xconfigure.width;
				height = polyevent.xconfigure.height;
				XSetForeground(polydisplay, polygc, polybg);
				XFillRectangle(polydisplay, polypix, polygc, 0,
					       0, width, width);
				XClearWindow(polydisplay, polywin);
				displaydata(data, numpolygons, width, height,
					    maxvalue, perspval);
				didit = TRUE;
			        }
		}
      }

   /*Clean up and exit*/
   XFreeGC(polydisplay,polygc);
   XDestroyWindow(polydisplay,polywin);
   XFreePixmap(polydisplay, polypix);
   XCloseDisplay(polydisplay);
   exit(0);
   return 1;
}



