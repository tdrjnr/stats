/*-----------------------------------------------------------------------------
 * polydraw.c
 * This is the main source file for a program to throw open a window and
 * plot polygons in it.  The polygons come from stdin in the form (for
 * a red polygon)
 *  x1 y1 x2 y2 .... xn yn rgb 65535 0 0
 * The coordinates coming in are mapped in such a way that x=-1 is the left
 * edge of the window, x=1 is the right edge of the window, y=-1 is the bottom
 * edge of the window, and y=1 is the top edge of the window unless overidden
 * by options.
 *
 * Note that if no colors are to be specifies in the file, the the "-nocolors"
 * option must be used.  Requiring this is necessary since in binary mode it
 * is impossible to "check" if there is a colors on the line or not.
 */ 

#ifndef _NO_PROTO
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>
#include <math.h>
#include "line_io.h"  /* which also includes Xlib.h */
#include <X11/Xutil.h>
#include "polydraw.h"
#include <malloc.h>
#include "parse_argument.h"

/*
 * global variables
 * used as a simple way of keeping the function calls from
 * being completely ugly and obscure.  relies largely on the
 * fact that polydraw has exactly one window
 */

/* the boolean flags for options */
static char back_store_on, help;
static char lines, fillthem, framethem, interactive, binarymode, buffermode;
static char colors, noheal;

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
    thecolor->red = (unsigned short)red;
    thecolor->green = (unsigned short)green;
    thecolor->blue = (unsigned short)blue;
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
 * this routine displays help on polydraw options
 */
void display_help()
{
  fprintf(stderr,
	  "\n\n\n\n\n");
  fprintf(stderr,
	  "polydraw - a program for diplaying colored polygons in an X window\n");
  fprintf(stderr,
	  "\noptions (none are required)\n");
  fprintf(stderr,
	  "	-lines		:	plots lines rather than closed polygons (doesn't fill)\n");
  fprintf(stderr,
	  "	-thickness	:	set line thickness in pixels (default 0)\n");
  fprintf(stderr,
	  "	-nofill		:	do not fill the polygons (wireframe)\n");
  fprintf(stderr,
	  "	-noframe	:	do not frame the polygons\n");
  fprintf(stderr,
	  "	-i		:	'interactive' - see EACH polygon as it is read\n");
  fprintf(stderr,
	  "	-buffer		:	accumulate polygons - draw as 'frames'\n");
  fprintf(stderr,
	  "	-b		:	binary mode input\n");
  fprintf(stderr,
	  "	-nocolors	:	do not read colors from file\n");
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
	  "				drawing. Default 100-checks after each 100 polygons \n");
  fprintf(stderr,
	  "	-length 	:	sets the maximum input line lenght. Default 2000.\n");
  fprintf(stderr,
	  "	-max_sides	:	sets the maximum number of polygon sides. Default 60.\n");
  fprintf(stderr,
	  "	-geometry	:	request window geometry (in standard X form)\n");
  fprintf(stderr,
	  "	-title		:	set title of the window (default is polydraw)\n");
  fprintf(stderr,
	  "	-h 		:	display this help message\n");
  fprintf(stderr,
	  "	-help\n");
}

/*-----------------------------------------------------------------------------
 * handle_polygon()
 * this routine does everything necessary to process one actual polygon.
 * in "buffer mode" the polygon is drawn only to the pixmap and not the screen
 * while in the normal mode the polygon is drawn to the pixmap if backing
 * store id not supported (for healing the window) and always one to the
 * window.  The frames are only drawn if the framethem variable is True.
 * Wireframes (faster) are given if fillthem is false.
 * If lines is true, the last line closing the polygon is not drawn, 
 * and the polygons are not filled.
 */
#ifndef _NO_PROTO
  void handle_polygon(XPoint *thepoints, int numsides, XColor *pcolor)
#else
  void handle_polygon(thepoints, numsides, pcolor)
  XPoint *thepoints;
  int numsides;
  XColor *pcolor;
#endif
{
  static int error_count=0;

  /*
   * First set the color to be the color used for filling polygons
   * If colors are being passed, it's the color that was
   * read in from stdin, otherwise it's mybackground.  This is done
   * even if the polygons are not being filled, since in that case
   * color will be used for drawing the something else.
   */

  if (colors) {
    if (!XAllocColor(mydisplay, colormap, pcolor)) {
      if(error_count < 2) {
	fprintf(stderr,"polydraw: Failure in XAllocColor.\n");
	fprintf(stderr,"polydraw: Using closest available color.\n");
	fflush(stderr);
	error_count++;
      } else {
	if(error_count < 3) {
	  fprintf(stderr,"polydraw: Failure in XAllocColor.\n");
	  fprintf(stderr,"polydraw: Using closest available color.\n");
	  fprintf(stderr,
		  "polydraw: Further output of XAllocColor error will be suppressed.\n"); 
	  fflush(stderr);
	  error_count++;
	}
      }

    }
    XSetForeground(mydisplay, mygc, pcolor->pixel);
  } else {
    XSetForeground(mydisplay, mygc, mybackground);
  } 
  
  /* note that is lines is True then fillthem and framethem are gauranteed
   * to be False */
  
  if (fillthem) {	
    if ((!noheal && !back_store_on) || buffermode) {
      XFillPolygon(mydisplay, mypixmap, mygc, thepoints,
		   numsides, Nonconvex, CoordModeOrigin);
    }
    
    if (!buffermode) {	
      XFillPolygon(mydisplay, mywindow, mygc, thepoints,
		   numsides, Nonconvex, CoordModeOrigin);
    }
  }
  
  if (lines) {
    if (colors)
      XSetForeground(mydisplay, mygc, pcolor->pixel);
    else
      XSetForeground(mydisplay, mygc, myforeground);
    
    if ((!noheal && !back_store_on) || buffermode) {
      XDrawLines(mydisplay,mypixmap,mygc,thepoints,
		 numsides, CoordModeOrigin);
    }
    
    if (!buffermode) {
      XDrawLines(mydisplay,mywindow,mygc,thepoints,
		 numsides, CoordModeOrigin);
    }
  }
  
  if (framethem) {
    /* if colors are specified and the polygons were not filled
     * then make the frames the color passed, otherwise make
     * them the foreground color */
    if (!fillthem && colors)
      XSetForeground(mydisplay, mygc, pcolor->pixel);
    else
      XSetForeground(mydisplay, mygc, myforeground);
    if ((!noheal && !back_store_on) || buffermode) {
      XDrawLines(mydisplay,mypixmap,mygc,thepoints,
		 numsides+1, CoordModeOrigin);
    }
    
    if (!buffermode) {
      XDrawLines(mydisplay,mywindow,mygc,thepoints,
		 numsides+1, CoordModeOrigin);
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
 * variables.  the defaults for xmin, xmax, ymin, ymax, and line_thickness are 
 * also handled here.
 */
#ifndef _NO_PROTO
  void read_options(int argc, char **argv, long *length, long *max_vert,
  double *xmin, double *xmax, double *ymin, double *ymax, int *interval,
  char *ret_title_str, char *ret_geometry_str, int *line_thickness)
#else
  void read_options(argc, argv, length, max_vert, xmin, xmax, ymin, ymax,
		    interval, ret_title_str, ret_geometry_str, line_thickness)
  int argc;
  char **argv;
  double *xmin, *xmax, *ymin, *ymax;
  int *line_thickness;
  long *length;
  long *max_vert;
  int *interval;
  char *ret_title_str, *ret_geometry_str;
#endif
{
  int i;
  char *dummy_string;
  char *xmin_string, *xmax_string;
  char *ymin_string, *ymax_string;
  char *interval_string, *title_string, *geometry_string;
  char *line_thickness_string;
  
  /* read options */
  i = parse_argument(argc,argv,0,&dummy_string,"-i");
  if (i!=-1)
    interactive = True;
  else
    interactive = False;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-lines");
  if (i!=-1)
    lines = True;
  else
    lines = False;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-nofill");
  if (i!=-1)
    fillthem = False;
  else
    fillthem = True;
  
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
  
  i = parse_argument(argc,argv,0,&dummy_string,"-nocolors");
  if (i!=-1)
    colors = False;
  else
    colors = True;
  
  (*xmin) = (*ymin) = -1.0;
  (*xmax) = (*ymax) =  1.0;
  (*line_thickness) = 0;
  
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
 
  i = parse_next_argument(argc,argv,0,&line_thickness_string,"-thickness");
  if (i!=-1)
    sscanf(line_thickness_string, "%d", line_thickness);
  
  i = parse_next_argument(argc,argv,0,&interval_string,"-interval");
  if (i != -1)
    sscanf(interval_string,"%d",interval);
  else
    *interval = DEF_INTERVAL;

  i = parse_next_argument(argc,argv,0,&interval_string,"-length");
  if (i != -1)
    sscanf(interval_string,"%ld",length);
  else
    *length = DEFAULT_MAXLENGTH;

  i = parse_next_argument(argc,argv,0,&interval_string,"-max_sides");
  if (i != -1)
    sscanf(interval_string,"%ld",max_vert);
  else
    *max_vert = DEFAULT_MAX_SIDES;
  
  i = parse_next_argument(argc,argv,0,&title_string,"-title");
  if (i == -1)
    strcpy(ret_title_str,"polydraw");
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

/* now cope with conflicting options! */
  if (lines) {
    framethem = False;
    fillthem = False;
  }
}

/*-----------------------------------------------------------------------------
 * handle_clear()
 * this routine deals with the clear/'new frame' command which is indicated by
 * a blank line in ascii mode and a a polygon of zero sides in binary mode.
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
 * fill_scaled_array()
 * this routine scales the polygons points so that xmin is the leftmost edge
 * of the window, xmax is the rightmost edge of the window, ymin is the bottom
 * and ymax is the top.  It also adds the original point to the end of the
 * new array so that the frame can be drawn with one call to XDrawLines() if
 * the framethem flag is True.
 */
#ifndef _NO_PROTO
void fill_scaled_array(XPoint *thepoints, int numsides, double *x, double *y,
		       double xmin, double xspand2, double ymax, double yspand2)
#else
void fill_scaled_array(thepoints, numsides, x, y, xmin, xspand2, ymax, yspand2)
   XPoint *thepoints;
   int numsides;
   double *x, *y, xmin, xspand2, ymax, yspand2;
#endif

{
  int i;

  for (i=0; i<numsides; i++) {
    thepoints[i].x = (short) floor ( (x[i] - xmin)/xspand2 * widthd2  + 0.5 );
    thepoints[i].y = (short) floor (-(y[i] - ymax)/yspand2 * heightd2 + 0.5);
  }
  thepoints[numsides].x = thepoints[0].x;
  thepoints[numsides].y = thepoints[0].y;
}

/*-----------------------------------------------------------------------------
 * main()
 * this is the main program.
 * it contains all the messy X code to open the window
 * then there is the drawing loop where polygons are read in and processed
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
  
  double *x,*y;
  int got_color;
  char text[10];
  int numsides;
  XPoint *thepoints;
  XColor thecolor;
  XWindowAttributes wind_attr;
  XSetWindowAttributes attr_settings;
  char *skippedpolyErr = 
    "polydraw: non-fatal error - skipped polygon due to missing color\n";
  char *input;
  char *remains;
  double xmin, xmax, ymin, ymax;
  int line_thickness;
  double xspand2, yspand2;
  char just_got_clear_signal = False;
  char just_cleared = True;
  char title[255], geometry[255];
  char ready_to_go, doesbackstore;
  long input_length, max_vertices;
  int countdown, interval;
  
  read_options(argc, argv, &input_length, &max_vertices, &xmin, &xmax,
	       &ymin, &ymax, &interval, title, geometry, &line_thickness);
  
  if (help) {
    display_help();
    exit(0);
  }
  
  /* initialization */
  thepoints = (XPoint *)malloc((max_vertices+1)*sizeof(XPoint));
  if (thepoints==NULL) {
    fprintf(stderr,"polydraw: fatal err - allocation of polygon storage failed\n");
    exit(1);
  }

  input = (char *)malloc(input_length*sizeof(char));
  if (input==NULL) {
    fprintf(stderr,"polydraw: fatal err - allocation of storage for input lines failed.");
    exit(1);
  }
  if( (mydisplay = XOpenDisplay("")) == NULL ) {
    fprintf(stderr,"Cannot connect to server.\n");
    fflush(stderr);
    exit(-1);
  }
  
  myscreen  = DefaultScreen(mydisplay);
  
  /* default pixel values */
  
  mybackground = WhitePixel(mydisplay, myscreen);
  myforeground = BlackPixel(mydisplay, myscreen);
  
  /* default program-specified window position and size */
  myhint.x = DEF_X;		myhint.y = DEF_Y;
  myhint_width = DEF_WIDTH;	myhint_height = DEF_HEIGHT;

  /* Changed 11-9-92 J. Marko - fixed so that program respomds to offset
  **         part of geometry specified by -geometry flag. Now uses
  **         US. 
  */
  myhint.flags = USPosition | USSize;
  
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
  XSetLineAttributes(mydisplay,mygc,line_thickness,LineSolid,CapButt,JoinMiter);
  
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
    while (fgets(input,input_length,stdin) != NULL) {
      if (read_pairs(input, &x, &y, &numsides, &remains)) {
	if (numsides != 0) {
	  
	  if (just_got_clear_signal && !buffermode)
	    handle_clear();
	  
	  just_got_clear_signal = False;
	  
	  fill_scaled_array(thepoints,numsides,x,y,xmin,xspand2,ymax,
			    yspand2);
	  
	  if (colors)
	    got_color = getRGB(remains, &thecolor);
	  else
	    got_color = False;
	  
	  if (!(colors && !got_color))
	    handle_polygon(thepoints,numsides,&thecolor);
	  else 
	    fprintf(stderr, skippedpolyErr);
	  
	} else {
	  just_got_clear_signal = True;
	  if (buffermode)
	    handle_clear();
	  else
	    XFlush(mydisplay);
	}
	countdown--;
	free(x); free(y);
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
    while (read_binary_pairs(stdin,&x,&y,&numsides)) {
      if (numsides!=0) {
	
	if (just_got_clear_signal && !buffermode)
	  handle_clear();
	
	just_got_clear_signal = False;
	
	fill_scaled_array(thepoints,numsides,x,y,xmin,xspand2,ymax,
			  yspand2);
	if (colors)
	  got_color = read_binary_rgb_color(stdin, &thecolor);
	
	handle_polygon(thepoints,numsides,&thecolor);
	
      } else {
	just_got_clear_signal = True;
	if (buffermode)
	  handle_clear();
	else
	  XFlush(mydisplay);
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
      free(x); free(y);
    }
  }
  
  if (buffermode && !just_got_clear_signal) {
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
