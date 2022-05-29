/*-----------------------------------------------------------------------------
 * circledraw.c
 * This is the main source file for a program to throw open a window and
 * plot circles in it.  The circles come from stdin in the form (for
 * a red circle followed by a blue circle)
 *  x1 y1 radius1 rgb 65535 0 0
 *  x2 y2 radius2 rgb 0 0 65535
 * The coordinates coming in are mapped in such a way that x=-1 is the left
 * edge of the window, x=1 is the right edge of the window, y=-1 is the bottom
 * edge of the window, and y=1 is the top edge of the window unless overidden
 * by options.
 */ 

#ifndef _NO_PROTO
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>
#include <math.h>
#include "line_io.h"  /* which also includes Xlib.h */
#include <X11/Xutil.h>
#include "circledraw.h"
#include "parse_argument.h"

/*
 * global variables
 * used as a simple way of keeping the function calls from
 * being completely ugly and obscure.  relies largely on the
 * fact that circledraw has exactly one window
 */

/* the boolean flags for options */
static char back_store_on, help;
static char framethem, interactive, binarymode, buffermode,noheal;

/* background and foreground pixel values */
static unsigned long myforeground, mybackground;

/* window variables */
static Display *mydisplay;
static Window mywindow;
static Pixmap mypixmap;
static Colormap colormap;
static GC mygc;

/* width & height of window */
static int width, height;
static int widthd2, heightd2;

/*-----------------------------------------------------------------------------
 * getRGB()
 * this routine parses a string of the form "rgb 65535 0 0" into red, green,
 * and blue components which are stored in the corresponding fields of the
 * passed XColor.
 */
#ifndef _NO_PROTO
  int getRGB(char *thestring, XColor *thecolor)
#else
  int getRGB(thestring, thecolor)
char *thestring;
  XColor *thecolor;
#endif
{
	int red, green, blue;
	int numgot;
	char dummy[255];

	numgot = sscanf(thestring, "%s %d %d %d", dummy, &red, &green, &blue);

	if (numgot!=4) {
		return(False);
	} else {
		thecolor->red   = (unsigned short)red;
		thecolor->green = (unsigned short)green;
		thecolor->blue  = (unsigned short)blue;
		return(True);
	}
}

/*-----------------------------------------------------------------------------
 * clear_pixmap()
 * this routine erases a Pixmap to a solid color
 */
#ifndef _NO_PROTO
  void clear_pixmap(Pixmap *pix, int width, int height, Display *disp,
		GC *gc, unsigned long background)
#else
  void clear_pixmap(pix, width, height, disp, gc, background)
  Pixmap *pix;
  int width, height;
  Display *disp;
  GC *gc;
  unsigned long background;
#endif
{
	XSetForeground(disp, *gc, background);
	XFillRectangle(disp, *pix, *gc, 0, 0, width, height); 
}

/*-----------------------------------------------------------------------------
 * display_help()
 * this routine displays help on circledraw options
 */
void display_help()
{
	fprintf(stderr,
		"\n\n\n\n\n");
	fprintf(stderr,
		"circledraw - a program for diplaying colored circles in an X window\n");
	fprintf(stderr,
		"\noptions (none are required)\n");
	fprintf(stderr,
		"	-noframe	:	do not frame the circles\n");
	fprintf(stderr,
		"	-i		:	'interactive' - see EACH circle as it is read\n");
	fprintf(stderr,
		"	-buffer		:	accumulate circles - draw as 'frames'\n");
	fprintf(stderr,
		"	-b		:	binary mode input\n");
	fprintf(stderr,
		"	-noheal		:	skip steps taken to preserve window image (for speed)\n");
	fprintf(stderr,
		"	-xmin		:	set x value for left   edge (default -1)\n");
	fprintf(stderr,
		"	-xmax		:	set x value for right  edge (default  1)\n");
	fprintf(stderr,
		"	-ymin		:	set y value for top    edge (default -1)\n");
	fprintf(stderr,
		"	-ymax		:	set y value for bottom edge (default  1)\n");
	fprintf(stderr,
		"	-interval	:	set interval for checking for expose events while\n");
	fprintf(stderr,
		"				drawing. Default 100-checks after each 100 circles \n");
	fprintf(stderr,
		"	-geometry	:	request window geometry (in standard X form)\n");
	fprintf(stderr,
		"	-title		:	set title of the window (default is circledraw)\n");
	fprintf(stderr,
		"	-h 		:	display this help message\n");
	fprintf(stderr,
		"	-help\n");
}

/*------------------------------------------------------------------------------
 * handle_circle()
 * this routine does everything necessary to process one actual circle.
 * in "buffer mode" the circle is drawn only to the pixmap and not the screen
 * while in the normal mode the circle is drawn to the pixmap if backing
 * store id not supported (for healing the window) and always one to the
 * window.  The frames are only drawn if the framethem variable is True.
 */
#ifndef _NO_PROTO
  void handle_circle(int x, int y, unsigned int r_width, unsigned int r_height, XColor *pcolor)
#else
  void handle_circle(x, y, r_width, r_height,  pcolor)
  int x, y;
  unsigned int r_width, r_height;
  XColor *pcolor;
#endif
{
	if (!XAllocColor(mydisplay, colormap, pcolor)) {
		fprintf(stderr,"circledraw: failure in XAllocColor.\n");
		exit(1);
	}
	XSetForeground(mydisplay, mygc, pcolor->pixel);
	
	if ((!noheal && !back_store_on) || buffermode) {
		XFillArc(mydisplay, mypixmap, mygc, x, y,
			r_width, r_height, 0, 360 * 64);
	}

	if (!buffermode) {	
		XFillArc(mydisplay, mywindow, mygc, x, y,
			r_width, r_height, 0, 360 * 64);
	}

	if (framethem) {
		XSetForeground(mydisplay, mygc, myforeground);

		if ((!noheal && !back_store_on) || buffermode) {
			XDrawArc(mydisplay, mypixmap, mygc, x, y,
				r_width, r_height, 0, 360 * 64);
		}

		if (!buffermode) {
			XDrawArc(mydisplay, mywindow, mygc, x, y,
				r_width, r_height, 0, 360 * 64);
		}
	}

	if (interactive && !buffermode) {
		/* in interactive mode, flush the buffer manually
		 * so the user sees output from last input */
		XFlush(mydisplay);
	}	
}

/*-----------------------------------------------------------------------------
 * read_options()
 * this program parses the program options and sets appropriate flags and
 * variables.  the defaults for xmin, xmax, ymin, and ymax are also handled
 * here.
 */
#ifndef _NO_PROTO
  void read_options(int argc, char **argv, double *xmin, double *xmax,
		double *ymin, double *ymax, int *interval, char *ret_title_str,
		char *ret_geometry_str)
#else
  void read_options(argc, argv, xmin, xmax, ymin, ymax, interval, ret_title_str,
		ret_geometry_str)
  int argc;
  char **argv;
  double *xmin, *xmax, *ymin, *ymax;
  int *interval;
  char *ret_title_str, *ret_geometry_str;
#endif
{
	int i;
	char *dummy_string;
	char *xmin_string, *xmax_string;
	char *ymin_string, *ymax_string;
	char *interval_string, *title_string, *geometry_string;
 
	/* read options */
	i = parse_argument(argc,argv,0,&dummy_string,"-i");
	if (i!=-1)
		interactive = True;
	else
		interactive = False;

	i = parse_argument(argc,argv,0,&dummy_string,"-noframe");
	if (i!=-1)
		framethem = False;
	else
		framethem = True;
	
	i = parse_argument(argc,argv,0,&dummy_string,"-b");
	if (i!=-1)
		binarymode = True;
	else
		binarymode = False;
	
	i = parse_argument(argc,argv,0,&dummy_string,"-buffer");
	if (i!=-1)
		buffermode = True;
	else
		buffermode = False;

	i = parse_argument(argc,argv,0,&dummy_string,"-noheal");
	if (i!=-1)
		noheal = True;
	else
		noheal = False;

	(*xmin) = (*ymin) = -1.0;
	(*xmax) = (*ymax) =  1.0;

	i = parse_next_argument(argc,argv,0,&xmin_string,"-xmin");
	if (i != -1)
		sscanf(xmin_string,"%lf",xmin);

	i = parse_next_argument(argc,argv,0,&xmax_string,"-xmax");
	if (i != -1)
		sscanf(xmax_string,"%lf",xmax);

	i = parse_next_argument(argc,argv,0,&ymin_string,"-ymin");
	if (i != -1)
		sscanf(ymin_string,"%lf",ymin);

	i = parse_next_argument(argc,argv,0,&ymax_string,"-ymax");
	if (i != -1)
		sscanf(ymax_string,"%lf",ymax);
	
	i = parse_next_argument(argc,argv,0,&interval_string,"-interval");
	if (i != -1)
		sscanf(interval_string,"%d",interval);
	else
		*interval = DEF_INTERVAL;

	i = parse_next_argument(argc,argv,0,&title_string,"-title");
	if (i == -1)
		strcpy(ret_title_str,"circledraw");
	else 
		strcpy(ret_title_str,title_string);

	
	i = parse_next_argument(argc,argv,0,&geometry_string,"-geometry");
	if (i == -1)
		sprintf(ret_geometry_str,"=%dx%d+%d+%d",DEF_WIDTH,DEF_HEIGHT,DEF_X,DEF_Y);
	else
		strcpy(ret_geometry_str,geometry_string);
	
	help = False;
	i = parse_argument(argc,argv,0,&dummy_string,"-h");
	if (i!=-1)
		help = True;
	i = parse_argument(argc,argv,0,&dummy_string,"-help");
	if (i!=-1)
		help = True;
}

/*-----------------------------------------------------------------------------
 * handle_clear()
 * this routine deals with the clear/'new frame' command which is indicated by
 * a blank line in ascii mode and a circle of zero radius in binary mode.
 * essentially, in "buffer mode" the buffer is copied to the window and then
 * cleared.  In the usual mode the window is cleared and the pixmap is cleared
 * if it is being used to keep a copy of the window for expose events (i.e.
 * if backing store not available)
 */
void handle_clear()
{
	XSetForeground(mydisplay, mygc, myforeground);
	if (!buffermode) {
		XClearWindow(mydisplay, mywindow);	
	} else {
		XCopyArea (mydisplay, mypixmap, mywindow, mygc, 0, 0,
				width, height,0,0); 
	}
	if ((!noheal && !back_store_on) || buffermode) {
		clear_pixmap(&mypixmap,width,height,
			mydisplay,&mygc,mybackground);
	}
	XFlush(mydisplay);
}

/*-----------------------------------------------------------------------------
 * scale_circle()
 * this routine scales the circle so that xmin is the leftmost edge
 * of the window, xmax is the rightmost edge of the window, ymin is the bottom
 * and ymax is the top.
 */
#ifndef _NO_PROTO
  void scale_circle(double *x, double *y, double *radius, double xmin, double xspand2,
			double ymax, double yspand2, int *rect_x, int *rect_y,
			unsigned int *r_width, unsigned int *r_height)
#else
  void scale_circle(x, y, radius, xmin, xspand2, ymax, yspand2, rect_x, rect_y,
			r_width, r_height)
  double *x, *y, *radius, xmin, xspand2, ymax, yspand2;
  int *rect_x, *rect_y;
  unsigned int *r_width, *r_height;
#endif

{
	double scaled_x, scaled_y;
	double d_r_width, d_r_height;

	scaled_x =  (*x - xmin) * (widthd2  / xspand2);
	scaled_y = -(*y - ymax) * (heightd2 / yspand2);

	d_r_width  = 2 * (*radius) * (widthd2  / xspand2);
	d_r_height = 2 * (*radius) * (heightd2 / yspand2);

	*r_width = (unsigned int)floor(d_r_width + 0.5);
	*r_height = (unsigned int)floor(d_r_height + 0.5);

	/* what we really need is the upper left corner of the superscribed(?)
	 * rectangle */
	*rect_x = (int) floor( scaled_x - d_r_width/2.0  + 0.5);
	*rect_y = (int) floor( scaled_y - d_r_height/2.0 + 0.5);
}

/*-----------------------------------------------------------------------------
 * main()
 * this is the main program.
 * it contains all the messy X code to open the window
 * then there is the drawing loop where circles are read in and processed
 * finally there is an event loop that is entered after an eof has been reached.
 */
#ifndef _NO_PROTO
  int main(int argc, char **argv)
#else
  int main(argc, argv)
  int argc;
  char **argv;
#endif
{
	KeySym mykey;
	XEvent myevent;
	XSizeHints myhint;
	unsigned int myhint_width, myhint_height;
	int myscreen;
	int i, done;
	
	double *x,*y,*radius;
	int status;
	char text[10];
	int numtriples;
	XColor thecolor;
	XWindowAttributes wind_attr;
	XSetWindowAttributes attr_settings;
	char *skippedcircleErr = 
		"circledraw: non-fatal error - skipped circle due to missing color\n";
	char input[MAXLENGTH];
	char *remains;
	double xmin, xmax, ymin, ymax;
	double xspand2, yspand2;
	int scaled_x, scaled_y;
	unsigned int rect_width, rect_height;
	char just_cleared = True;
	char title[255], geometry[255];
	char ready_to_go, doesbackstore;
	int countdown, interval;
	
	read_options(argc, argv, &xmin, &xmax, &ymin, &ymax, &interval, title, geometry);

	if (help) {
		display_help();
		exit(0);
	}

	/* initialization */
	mydisplay = XOpenDisplay("");
	myscreen  = DefaultScreen(mydisplay);

	/* default pixel values */

	mybackground = WhitePixel(mydisplay, myscreen);
	myforeground = BlackPixel(mydisplay, myscreen);

	/* default program-specified window position and size */
	myhint.x = DEF_X;		myhint.y = DEF_Y;
	myhint_width = DEF_WIDTH;	myhint_height = DEF_HEIGHT;
	myhint.flags = PPosition | PSize;

	XParseGeometry(geometry,&(myhint.x),&(myhint.y),
			&myhint_width, &myhint_height);

	myhint.width = (int)myhint_width;
	myhint.height= (int)myhint_height;

	/* window creation */
	mywindow = XCreateSimpleWindow(mydisplay,
		DefaultRootWindow(mydisplay),
		myhint.x, myhint.y, myhint.width, myhint.height,
		5, myforeground, mybackground );

	XSetWindowBackground(mydisplay,mywindow,mybackground);

		
	XSetStandardProperties (mydisplay, mywindow, title, title, 
		None, argv, argc, &myhint);
	
	/* GC creation and initialization */
	mygc = XCreateGC(mydisplay,mywindow, 0, 0);
	XSetBackground(mydisplay, mygc, mybackground);
	XSetForeground(mydisplay, mygc, myforeground);

	/* input event selection */
	XSelectInput(mydisplay,mywindow, KeyPressMask | ExposureMask);
	
	/* window mapping */
	XMapRaised(mydisplay, mywindow);

	/* set line attributes */
	XSetLineAttributes(mydisplay,mygc,0,LineSolid,CapButt,JoinMiter);
	
	/* set up the color map */
	colormap = DefaultColormap( mydisplay, myscreen );
	XSetWindowColormap( mydisplay, mywindow, colormap );
 
	/* get the window attributes */
	XGetWindowAttributes(mydisplay, mywindow, &wind_attr);
	width  = wind_attr.width;
	height = wind_attr.height;
	widthd2  = width  / 2;
	heightd2 = height / 2;

	/* make the buffer pixmap */ 
	mypixmap = XCreatePixmap (mydisplay, mywindow, width, height,
			DefaultDepth(mydisplay,myscreen));
	clear_pixmap(&mypixmap,width,height,mydisplay,&mygc,mybackground);

	/* now WAIT for the expose event, this means that the user has
	 * actually positioned the window
	 */
	ready_to_go = False;
	while (!ready_to_go) {
		XNextEvent(mydisplay,&myevent);
		if (myevent.type == Expose)
			ready_to_go = True;
	}
			
	/*
	 * turn on backing store if available 
     	 * if this is done before the window appears, then no initial Expose
	 * event is ever generated.  so it must be done here.
	 */
	doesbackstore = DoesBackingStore(DefaultScreenOfDisplay(mydisplay));
	if (doesbackstore && !noheal) {
		attr_settings.backing_store = Always;
		XChangeWindowAttributes (mydisplay, mywindow, CWBackingStore,
			&attr_settings);
		back_store_on = True;
	}

	/* window mapping(again) this seems to make the backing store come
	 * on immediately rather than eventually (if just requested) */
	XMapRaised(mydisplay, mywindow);

	XSetForeground(mydisplay, mygc, myforeground);
	XClearWindow(mydisplay, mywindow);	

	XFlush(mydisplay);

	xspand2 = (xmax - xmin)/2;
	yspand2 = (ymax - ymin)/2;

	countdown = interval;

	if (!binarymode) {
		while (fgets(input,MAXLENGTH,stdin) != NULL) {
			if (read_triples(input, &x, &y, &radius, &numtriples, &remains)) {
				if (numtriples != 0) {

					just_cleared = False;

					scale_circle(x, y, radius, xmin, xspand2, ymax, yspand2,
						&scaled_x, &scaled_y, &rect_width, &rect_height);

					status = getRGB(remains, &thecolor);

					if (status) 
						handle_circle(scaled_x, scaled_y, rect_width, rect_height, &thecolor);
					else 
						fprintf(stderr, skippedcircleErr);

				} else {
					handle_clear();
					just_cleared = True;
				}
				countdown--;
				free(x);
				free(y);
				free(radius);
			}

			if (!noheal && !back_store_on) {
				if (countdown==0) {
					if (XCheckTypedWindowEvent(mydisplay, mywindow,
							Expose, &myevent)) {
						XCopyArea (mydisplay, mypixmap, mywindow, mygc, 0, 0,
								width, height,0,0); 
					}
					countdown = interval;
				}
			}
		}
	} else { /* in binary mode */
		while (read_binary_triples(stdin,&x,&y,&radius,&numtriples)) {
			if (numtriples!=0) {

				just_cleared = False;

				scale_circle(x, y, radius, xmin, xspand2, ymax, yspand2,
					&scaled_x, &scaled_y, &rect_width, &rect_height);

				status = read_binary_rgb_color(stdin, &thecolor);

				if (status) 
					handle_circle(scaled_x, scaled_y, rect_width, rect_height, &thecolor);
				else
					fprintf(stderr, skippedcircleErr);

			} else {
				handle_clear();
				just_cleared = True;
			}

			countdown--;

			if (!noheal && !back_store_on) {
				if (countdown==0) {
					if (XCheckTypedWindowEvent(mydisplay, mywindow,
							Expose, &myevent)) {
						XCopyArea (mydisplay, mypixmap, mywindow, mygc, 0, 0,
								width, height,0,0); 
					}
					countdown = interval;
				}
			}
		}
	}
	
	if (buffermode && !just_cleared) {
		/* copy the last frame to the screen */
		XCopyArea(mydisplay,mypixmap,mywindow,mygc,0,0,width,height,0,0);
	}

	XFlush(mydisplay);
	
	done = 0;
	while(done==0) {
		/* read the next event */
		XNextEvent(mydisplay,&myevent);
		switch (myevent.type) {
			/* quit if the window is sent a keystroke */
			case KeyPress:
				i = XLookupString((XKeyEvent *)&myevent,text,10,&mykey,0);
				if (i==1 && (text[0]=='q' || text[0]=='Q'))
					done = 1;
				break;
			case Expose:
				/* if backing store is on then execution never gets to here */
				if (!noheal) {
					XCopyArea (mydisplay, mypixmap, mywindow, mygc, 0, 0,
						width, height,0,0); 
				}
				break;
		}
	}
	
	/* termination */
	XFreeGC(mydisplay,mygc);
	XDestroyWindow(mydisplay,mywindow);
	XFreePixmap(mydisplay, mypixmap);
	XCloseDisplay(mydisplay);

	/* Successful exit */
	exit(0);
}
