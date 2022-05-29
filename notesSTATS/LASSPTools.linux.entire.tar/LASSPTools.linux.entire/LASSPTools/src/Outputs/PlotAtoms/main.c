/* PlotAtoms: a program for displaying images of atoms on the screen , 
 * with the capability of concatenating the images into a movie.
 *
 * I will give here an overview of how the program is structured in case
 * (god forbid!) you want to modify it, or are just interested.
 *       
 *
 	      main 
		|
	 declare variables
		|
	   parse options
	 	|
            open display
		|
	    create atom images
		|
	create window, pixmaps, and graphics contexts
		|
	read in first frame and set scale
		| 
	plot first frame
		|
	plot frames and process events
		|
	       exit			 


*/

/* runge adds workaround for exposure event stack memory growth */
#define IGNORE_XCOPYAREA_EXPOSURES 1

#include"plotatoms.h"

#define HUNKSIZE 50     /* initial array size for reading in first frame */

#ifndef _NO_PROTO
 main(int argc, char **argv)
#else
 main(argc,argv)
 int argc;
 char **argv;
#endif
{

/************************ Declare Variables *********************************/

  extern int              carg;
  extern char             **varg;
  extern char             name[];
  extern Pixmap           *atom_shapes;
  extern Pixmap           *atom_masks;
  
  Display		  *display;
  Window		  window,root;
  KeySym                  key1;
  GC			  gc_window,gc_erase;
  GC                      *gc_atoms;
  Pixmap		  atom1_shape,atom1_shape_mask,buffer,eraser,
                          icon_pixmap;
  
  XSetWindowAttributes	  window_attr;
  XGCValues	          gc_atom_values, gc_window_values, gc_erase_values;
  XSizeHints              size_hints;
  XEvent                  event1,event2;
  
  int			  screen,depth,ready_flag,j,scale_flag=0;
  unsigned long		  foreground, background,window_mask,
                          gc_atom_mask,gc_window_mask,gc_erase_mask;
  unsigned long           black,white;


/* Variables used for speed up */  
#ifndef NOSPEEDUP
  int                     xdraw_max=0, xdraw_min=0, ydraw_min=0,
                          ydraw_max=0, set=UNSET, old_xmax, old_xmin,
                          old_ymax, old_ymin;
#endif

  int                     atom_width,atom_height, atom_mask_width,
                          atom_mask_height,x_hot_spot_dummy,
                          y_hot_spot_dummy,xpix,ypix;
  
  char                    *window_name="PlotAtoms", *icon_name="Plot";
  
  char                    text[10],xypositions[MAXLINELENGTH];
  double		  xposition,yposition,xmax=0,xmin=0,ymax=0,
                          ymin=0,xscale,yscale;

/* list for first frame */
  double *first_x, *first_y;
  int *first_n;
  int num_in_first;
  int first_limit;

  int                     i,new_xpos,new_ypos;
  unsigned int            new_width,new_height,new_border_width,new_depth;
  
  int                     atom_number=0;

  unsigned long           option_mask = 0L;
  Options                 *options;
  XrmDatabase             options_db;

/* End of variable declarations */

  carg = argc;
  varg = argv;
  strcpy(name,argv[0]);

  fprintf(stderr,"%s : Type 'q' in the graphics window to exit.\n",name);
  fflush(stderr);

  options = (Options *) malloc( sizeof(Options) );
  options_db = parse(&argc, argv);
  set_options(&options_db, &option_mask, options);

#ifdef DEBUG
  debug_options(options);
#endif

  XFree(options_db); 


/*************************** Set Up Window **********************************/

  if ((display=XOpenDisplay(""))==NULL) {
    fprintf(stderr,"%s: Cannot connect to server.\n",name);
    fflush(stderr);
    exit(-1);
  }

#ifdef DEBUG
/* turn on synchronization for debugging */
  XSynchronize(display,1);  
  fprintf(stderr,"%s: Sychronization turned on for debugging.\n",name);
  fflush(stderr);
#endif

  new_width=options->geometry.width;
  new_height=options->geometry.height;
  screen= DefaultScreen(display);
  depth = DefaultDepth(display,screen);
  black = BlackPixel(display,screen);
  white = WhitePixel(display,screen);
  root = RootWindow(display,screen);

/* set-up color */

  if(depth>MONO) {
/* Create "fancy" atoms using color */

    create_atoms(display, screen, depth, options, option_mask, 
		 &foreground, &background);
  } else {
/* Only have black and white - darn */

    create_atoms_bw(display,screen,depth,options, option_mask,
		    &foreground, &background);

  }
  
  window_attr.border_pixel = black;
  window_attr.background_pixel = background;
  window_attr.override_redirect = False;
  window_attr.bit_gravity = CenterGravity;
  window_mask = (CWBackPixel|CWBorderPixel|CWOverrideRedirect|CWBitGravity);
  window = XCreateWindow(display,root,
			  options->geometry.xpos,options->geometry.ypos,
			  options->geometry.width,options->geometry.height,
			  2,depth,InputOutput, CopyFromParent,
			  window_mask,&window_attr);

  XSelectInput(display,window,ExposureMask | KeyPressMask |
	       ButtonPressMask | StructureNotifyMask );

  buffer = XCreatePixmap(display,root,
			 options->geometry.width,options->geometry.height,
			 depth);
  eraser = XCreatePixmap(display,root,
			 options->geometry.width, options->geometry.height,
			 depth);


/*************************** Set WM Hints ***********************************/

  size_hints.flags = USPosition | USSize;
  size_hints.x = options->geometry.xpos;
  size_hints.y = options->geometry.ypos;
  size_hints.width = options->geometry.width;
  size_hints.height = options->geometry.height;
  icon_pixmap = atom_masks[0];
  XSetStandardProperties(display,window,window_name,icon_name,
			 icon_pixmap,varg,carg,&size_hints);


/************************* Create Graphics Contexts *************************/

  
  gc_atom_mask = (GCForeground | GCBackground | GCClipMask );
  gc_atom_values.foreground = foreground;
  gc_atom_values.background = background;
  gc_atoms = (GC *) malloc( ((options->natoms)+1)*sizeof(GC) );

  for(i=0;i<=(options->natoms);++i) {
    gc_atom_values.clip_mask = atom_masks[i];
    gc_atoms[i] = XCreateGC(display,window,gc_atom_mask,&gc_atom_values);
    #if IGNORE_XCOPYAREA_EXPOSURES
	XSetGraphicsExposures(display, gc_atoms[i], False);
    #endif
  }

  gc_window_mask = GCForeground | GCBackground;
  gc_window_values.foreground = foreground;
  gc_window_values.background = background;
  gc_window=XCreateGC(display,window,gc_window_mask,&gc_window_values);

  gc_erase_mask = GCForeground | GCBackground ;
  gc_erase_values.foreground = background;
  gc_erase_values.background = foreground;
  gc_erase = XCreateGC(display,buffer,gc_erase_mask,&gc_erase_values);
#if IGNORE_XCOPYAREA_EXPOSURES
	XSetGraphicsExposures(display, gc_window, False);
	XSetGraphicsExposures(display, gc_erase, False);
#endif

/* black and white allow faster clearing of buffer                */
/* Couldn't get this to work on the Sun's, so I dropped them.     */ 
/* Could be a server bug.                                         */ 

#ifndef SUN
  if(background == 1) XSetFunction(display,gc_erase,GXset);   
  if(background == 0) XSetFunction(display,gc_erase,GXclear); 
#endif

  XFillRectangle(display,eraser,gc_erase,0,0,options->geometry.width,
		 options->geometry.height);
  XMapRaised(display,window);

/********************** Get scale from first frame-fix later ****************/

/* malloc space for first frame */
  first_x = (double *) malloc( HUNKSIZE * sizeof(double) );
  first_y = (double *) malloc( HUNKSIZE * sizeof(double) );
  first_n = (int *) malloc( HUNKSIZE * sizeof(int) );
  first_limit = HUNKSIZE;

  num_in_first = 0;
  while(whitespace(fgets(xypositions,MAXLINELENGTH,stdin))!=NEWFRAME){
    ++num_in_first;
    sscanf(xypositions,"%lf %lf %i",&xposition,&yposition,&atom_number);
 
    if(num_in_first>=first_limit) {
      first_x = (double *) realloc(first_x, (first_limit + HUNKSIZE) * 
				   sizeof(double));
      first_y = (double *) realloc(first_y, (first_limit + HUNKSIZE) * 
				   sizeof(double));
      first_n = (int *) realloc(first_n, (first_limit + HUNKSIZE) * 
				sizeof(int));
      first_limit += HUNKSIZE;
    }
    first_x[num_in_first-1] = xposition;
    first_y[num_in_first-1] = yposition;
    first_n[num_in_first-1] = atom_number;

    if(scale_flag==UNSET) {
      xmax=xposition;
      xmin=xposition;
      ymax=yposition;
      ymin=yposition;
      scale_flag=SET;
    }
    else {
      xmax =MAX(xposition,xmax);
      xmin =MIN(xposition,xmin);
      ymax =MAX(yposition,ymax);
      ymin =MIN(yposition,ymin);
    }
  }
  if(num_in_first==1){ 
    xmax=(xposition<0)?-1*xposition:xposition;
    xmin=(xposition<0)?xposition:-1*xposition;
    ymax=(yposition<0)?-1*yposition:yposition;
    ymin=(yposition<0)?yposition:-1*yposition;
  }

/* use user x and y limits, if supplied */

  if((option_mask & PAXMinSet) != 0L ) xmin = options->limits.xmin;
  if((option_mask & PAXMaxSet) != 0L ) xmax = options->limits.xmax;
  if((option_mask & PAYMinSet) != 0L ) ymin = options->limits.ymin;
  if((option_mask & PAYMaxSet) != 0L ) ymax = options->limits.ymax;

/* error checking for limits */
  if(xmin > xmax) {
    fprintf(stderr,"%s: WARNING: xmin > xmax.\n",name);
    fflush(stderr);
  }
  if(ymin > ymax) {
    fprintf(stderr,"%s: WARNING: ymin > ymax.\n",name);
    fflush(stderr);
  }

/* Set the user data to window scale factors */
  xscale =  (1/(xmax - xmin))*(options->geometry.width)*
            (1-2*(options->border));
  yscale =  (1/(ymax - ymin))*(options->geometry.height)*
            (1-2*(options->border));
  
  if( (option_mask & PAScale) == 0 ) {
    xscale = MIN(xscale,yscale);
    yscale = xscale;
  } /* scale x and y coordinates identically, if desired */
  
/* Wait for window to be ready - necessary so that we don't lose any frames */
  XFlush(display);
  for(ready_flag=0;ready_flag!=1;) {
    XNextEvent(display,&event1);
    if (event1.type == Expose)
      ready_flag = 1;
  } 
  XCopyArea(display,window,buffer,gc_erase,0,0,options->geometry.width,
	    options->geometry.height,0,0);

/* Draw first frame */
  for(j=0;j<num_in_first;j++) {
    atom_number = first_n[j];
    xposition = first_x[j];
    yposition = first_y[j];
    if((atom_number>0)&&(atom_number<=options->natoms)&&(options->natoms > 1)){
	atom_width = 2*(options->atoms[atom_number-1].radius)+1;
	atom_height = atom_width;
      } else {
	atom_number = 0;
	atom_width = 2*(options->default_atom.radius)+1;
	atom_height = atom_width;
      }
    xpix =(int)((xposition-xmin)*xscale+((options->border)*
		(options->geometry.width)) - atom_width*(0.5) );
    ypix =(int)((ymax-yposition)*yscale+((options->border)*
		(options->geometry.height)) - atom_width*(0.5) );

    if((xpix>-1*atom_width)&&(ypix>-1*atom_width)&&
       (xpix<options->geometry.width) && (ypix < options->geometry.height)) {
      XSetClipOrigin(display,gc_atoms[atom_number],xpix,ypix);
      XCopyArea(display,atom_shapes[atom_number],buffer, 
		gc_atoms[atom_number], 0,0,atom_width,atom_height,xpix,ypix);
    }      

/* this section is for speed-up of the drawing */ 
#ifndef NOSPEEDUP
    if(set==UNSET) {         
      old_xmax=xdraw_max;    
      old_xmin=xdraw_min;    
      old_ymax=ydraw_max;    
      old_ymin=ydraw_min;    
      xdraw_max=MAX( MIN(xpix+atom_width,options->geometry.width) ,0 ); 
      xdraw_min=xdraw_max - atom_width;   
      ydraw_max=MAX( MIN(ypix+atom_height,options->geometry.height) ,0); 
      ydraw_min=ydraw_max - atom_height;   
      set=SET;               
    } else {                 
      if((xpix > -1*atom_width) && (xpix < options->geometry.width)) {  
	xdraw_max=MAX( xdraw_max, xpix+atom_width); 
	xdraw_min=MIN( xdraw_min, MAX( xpix,0 ) ); 
      } 
      if((ypix > -1*atom_height) && (ypix < options->geometry.height)) { 
	ydraw_max=MAX( ydraw_max, ypix+atom_height );
	ydraw_min=MIN( ydraw_min, MAX( ypix,0 ) ); 
      } 
    } 
#endif
/* end of speed-up code section */
  } /* ends for(j=0;j<num_in_first... */

#ifndef NOSPEEDUP
  old_xmax=xdraw_max; 
  old_xmin=xdraw_min; 
  old_ymax=ydraw_max; 
  old_ymin=ydraw_min; 
  XCopyArea(display, buffer, window, gc_window, MIN(old_xmin,xdraw_min) , 
	    MIN(old_ymin,ydraw_min) , (MAX(old_xmax,xdraw_max) - 
	    MIN(old_xmin,xdraw_min) + 2 ) , (MAX(old_ymax,ydraw_max) - 
            MIN(old_ymin,ydraw_min) + 2 ) , MIN(old_xmin,xdraw_min), 
	    MIN(old_ymin,ydraw_min) ); 
  set=UNSET; 
#else
  XCopyArea(display,buffer,window,gc_window,0,0,
	    options->geometry.width,options->geometry.height,0,0);
#endif

  if( (option_mask & PAEntire) == 0 ) {

#ifndef NOSPEEDUP
    XCopyArea(display,eraser,buffer,gc_erase,xdraw_min,ydraw_min,
	      xdraw_max-xdraw_min, ydraw_max-ydraw_min,xdraw_min, ydraw_min); 
#else
	XCopyArea(display,eraser,buffer,gc_erase,0,0,
		  options->geometry.width,options->geometry.height,0,0);
#endif

  }
  XFlush(display);

/* end of drawing first frame */
/* free up storage space of first frame */
  free(first_x);
  free(first_y);
  free(first_n);

/*********************** Input Atom Motions **********************************/

  while( (fgets(xypositions,MAXLINELENGTH,stdin))!= NULL) {
    if(whitespace(xypositions)!=NEWFRAME) {
      atom_number = 0;
      sscanf(xypositions,"%lf %lf %i",&xposition,&yposition,&atom_number);
      if( (atom_number>0) && (atom_number<=options->natoms) 
	 && (options->natoms > 1) ) {
	atom_width = 2*(options->atoms[atom_number-1].radius)+1;
	atom_height = atom_width;
      } else {
	atom_number = 0;
	atom_width = 2*(options->default_atom.radius)+1;
	atom_height = atom_width;
      }
      
      xpix =(int)((xposition-xmin)*xscale+((options->border)*
					   (options->geometry.width)) - 
		  atom_width*(0.5) );
      ypix =(int)((ymax-yposition)*yscale+((options->border)*
					   (options->geometry.height))- 
		  atom_width*(0.5) );
	    
      if((xpix > -1*atom_width) && (ypix > -1*atom_width)
	 && (xpix < options->geometry.width) 
	 && (ypix < options->geometry.height)) {
	      
	XSetClipOrigin(display,gc_atoms[atom_number],xpix,ypix);
	XCopyArea(display,atom_shapes[atom_number],buffer,
		  gc_atoms[atom_number], 0,0,atom_width,atom_height,xpix,ypix);
      }

/* this section is for speed-up of the drawing */ 
#ifndef NOSPEEDUP
    if(set==UNSET) {         
      old_xmax=xdraw_max;    
      old_xmin=xdraw_min;    
      old_ymax=ydraw_max;    
      old_ymin=ydraw_min;    
      xdraw_max=MAX( MIN(xpix+atom_width,options->geometry.width) ,0 ); 
      xdraw_min=xdraw_max - atom_width;   
      ydraw_max=MAX( MIN(ypix+atom_height,options->geometry.height) ,0); 
      ydraw_min=ydraw_max - atom_height;   
      set=SET;               
    } else {                 
      if((xpix > -1*atom_width) && (xpix < options->geometry.width)) {  
	xdraw_max=MAX( xdraw_max, xpix+atom_width); 
	xdraw_min=MIN( xdraw_min, MAX( xpix,0 ) ); 
      } 
      if((ypix > -1*atom_height) && (ypix < options->geometry.height)) { 
	ydraw_max=MAX( ydraw_max, ypix+atom_height );
	ydraw_min=MIN( ydraw_min, MAX( ypix,0 ) ); 
      } 
    } 
#endif
/* end of speed-up code section */

    } else {
      if((option_mask & PAEntire) != 0L ) {

/* plot entire trajectory - don't erase between frames */
	XCopyArea(display,buffer,window,gc_window,0,0,
		  options->geometry.width,options->geometry.height,0,0);

      } else {

#ifndef NOSPEEDUP
	XCopyArea(display, buffer, window, gc_window, 
		  MIN(old_xmin,xdraw_min) , 
		  MIN(old_ymin,ydraw_min) , 
		  (MAX(old_xmax,xdraw_max)-MIN(old_xmin,xdraw_min) + 2 ) , 
		  (MAX(old_ymax,ydraw_max)-MIN(old_ymin,ydraw_min) + 2 ) , 
		  MIN(old_xmin,xdraw_min), 
		  MIN(old_ymin,ydraw_min) ); 
	set=UNSET; 
#else
	XCopyArea(display,buffer,window,gc_window,0,0,
		  options->geometry.width,options->geometry.height,0,0);
#endif

      }

/* EVENT handling */

      if (XCheckMaskEvent(display, ButtonPressMask | KeyPressMask | 
			  StructureNotifyMask, &event1)==True) { 
	switch(event1.type) {
	case MappingNotify:
	  XRefreshKeyboardMapping(&event1);
	  break;
	case KeyPress:
	  i=XLookupString(&event1,text,10,&key1,0);
	  if(i==1 && *text == 'q') {
	    XDestroyWindow(display,window);
	    XCloseDisplay(display);
	    exit(0);
	  }
	  break;
	  
	case ButtonPress:
	  for(ready_flag=0;!ready_flag;) {
	    XNextEvent(display,&event2);
	    switch(event2.type) {
	    case ButtonPress:
	      ready_flag=1;
	      break;
	    case KeyPress:
	      i=XLookupString(&event2,text,10,&key1,0);
	      if(i==1 && *text == 'q') {
		XDestroyWindow(display,window);
		XCloseDisplay(display);
		exit(0);
	      }
	      break;
	    }
	  }
	  break;
	case ConfigureNotify:
	  XGetGeometry(display,window,&root,&new_xpos, &new_ypos,&new_width,
		       &new_height, &new_border_width,&new_depth);

	  options->geometry.width = new_width;
	  options->geometry.height = new_height;

	  XFlush(display);
	  XFreePixmap(display,buffer);
	  XFreePixmap(display,eraser);
	  buffer = XCreatePixmap(display,root, new_width,new_height,depth);
	  eraser = XCreatePixmap(display,root, new_width,new_height,depth);
	  XFillRectangle(display,eraser,gc_erase,0,0,options->geometry.width,
			 options->geometry.height);
	  XFillRectangle(display,buffer,gc_erase,0,0,options->geometry.width,
			 options->geometry.height);
	  XFlush(display);
	  break;
	case DestroyNotify:
	  shutdown(display,window);
	  break;

	} /* ends switch(event1.type) */
	
      }  /* ends if(XCheckMask... */
      
/* End of EVENT loop */
      if( (option_mask & PAEntire) == 0 ) {

#ifndef NOSPEEDUP
	XCopyArea(display,eraser,buffer,gc_erase,xdraw_min,ydraw_min,
		  xdraw_max-xdraw_min, ydraw_max-ydraw_min,xdraw_min, 
		  ydraw_min); 
#else
	XCopyArea(display,eraser,buffer,gc_erase,0,0,
		  options->geometry.width,options->geometry.height,0,0);
#endif
	
      }
    }
  }

/* wait for typed "q" then EXIT */
  while(1) {
    XMaskEvent(display, KeyPressMask, &event1);
    switch(event1.type) {
    case KeyPress:
      i=XLookupString(&event1,text,10,&key1,0);
      if(i==1 && *text == 'q') {
	shutdown(display, window);
      }
      break;
    default:
      break;
    }
  }

/* Shouldn't reach this, but... */
  shutdown(display,window);
  return 0;
}
/* main */


#ifndef _NO_PROTO
  void shutdown(Display *display, Window window)
#else
  void shutdown(display,window)
    Display *display;
    Window window;
#endif
{
  XDestroyWindow(display,window);
  XCloseDisplay(display);
  exit(0);
}
/* shutdown */

/* PlotAtoms (Bruce W. Roberts) */
/* last modification: 9-4-1992 */















