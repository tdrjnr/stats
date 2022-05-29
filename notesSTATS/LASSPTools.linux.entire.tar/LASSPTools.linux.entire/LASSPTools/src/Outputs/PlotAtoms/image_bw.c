#include"plotatoms.h"

#define NCOLORS 256

#ifndef _NO_PROTO 
void create_atoms_bw(Display *display, int screen, int depth, Options *options,
		   unsigned long option_mask, unsigned long *foreground,
		   unsigned long *background)
#else
void create_atoms_bw(display, screen, depth, options, option_mask, foreground, 
		  background)
 Display *display; 
 int screen, depth; 
 Options *options;
 unsigned long option_mask, *foreground, *background;
#endif
{
  extern char             name[];
  extern Pixmap           *atom_shapes;
  extern Pixmap           *atom_masks;
  
  GC			  gc_image,gc_mask;
  XGCValues	          gc_image_values, gc_mask_values;
  XImage                  *image,*mask_image;
  unsigned long		  gc_image_mask, gc_mask_mask;
  unsigned long           black,white;
  int                     i,r0sq,rsq,k,l;
  int                     radius,angle,size,xzero,yzero;
  float                   aspsq, invaspsq;
  double                  sin_ang, cos_ang;

/* get black and white here */
  black = BlackPixel(display,screen);
  white = WhitePixel(display,screen);

  *foreground = white;
  *background = black;

/* Create the pixmaps */

  atom_shapes = (Pixmap *) malloc( (options->natoms+1)*sizeof(Pixmap) );
  atom_masks = (Pixmap *) malloc( (options->natoms+1)*sizeof(Pixmap) );

  for(i=0;i<=options->natoms;++i) {
    if(i>0) {
	atom_shapes[i] = XCreatePixmap(display,RootWindow(display,screen),
				      2*(options->atoms[i-1].radius)+1,
				      2*(options->atoms[i-1].radius)+1, depth);
	atom_masks[i] = XCreatePixmap(display,RootWindow(display,screen),
				     2*(options->atoms[i-1].radius)+1,
				     2*(options->atoms[i-1].radius)+1,1);
    } else {
      atom_shapes[0] = XCreatePixmap(display,RootWindow(display,screen),
				    2*(options->default_atom.radius)+1,
				    2*(options->default_atom.radius)+1, depth);
      atom_masks[0] = XCreatePixmap(display,RootWindow(display,screen),
				    2*(options->default_atom.radius)+1,
				    2*(options->default_atom.radius)+1, 1);
    }
  }

  gc_image_mask = (GCForeground | GCBackground );
  gc_image_values.foreground = *foreground;
  gc_image_values.background = *background;
  gc_image=XCreateGC(display,RootWindow(display,screen),gc_image_mask,
			     &gc_image_values);

  gc_mask_mask = (GCForeground | GCBackground );
  gc_mask_values.foreground = black;
  gc_mask_values.background = white;
  gc_mask=XCreateGC(display,atom_masks[0],gc_mask_mask,
		    &gc_mask_values);

/* DEFAULT ATOM SHAPE CREATION */
/* create default image */

  size = 2*(options->default_atom.radius)+1;
  radius = options->default_atom.radius;
  xzero = radius;
  yzero = radius;
  r0sq = SQUARE(radius);

  if((image=XCreateImage(display, DefaultVisual(display,screen), depth,
			 ZPixmap, 0, (char*) 0, (unsigned int) size,
			 (unsigned int) size, 32 ,0 ) )
     == (XImage *) 0) {
    fprintf(stderr,"%s: XCreateImage failed on default atom.\n",name);
    XCloseDisplay(display);
    exit(-1);
  }
  image->data = (char *) malloc(SQUARE(size) * sizeof(unsigned long));
  
  if((mask_image=XCreateImage(display, DefaultVisual(display,screen), 1, 
			      ZPixmap, 0, (char*) 0, (unsigned int) size,
			      (unsigned int) size, 8, 0 ) )
     == (XImage *) 0) {
    fprintf(stderr,"%s: XCreateImage failed on default atom mask.\n",name);
    XCloseDisplay(display);
    exit(-1);
  }
  mask_image->data = (char *) malloc( SQUARE(size) * sizeof(unsigned long) );
  
  /* create Image data for the atom */
  
  for(k=0;k<size;++k) {
    for(l=0;l<size;++l) {
      rsq=SQUARE(k-xzero)+SQUARE(l-yzero);

/* the -2 adds a black circle around the outer edge */
      if(sqrt((double)rsq)<sqrt((double)r0sq)-2) 
	XPutPixel(image,k,l,*foreground);
      else XPutPixel(image,k,l,*background);
      if(rsq<r0sq) XPutPixel(mask_image,k,l,1);
      else XPutPixel(mask_image,k,l,0);
    }
  }
  XPutImage(display,atom_shapes[0],gc_image,image,0,0,0,0,size,size);
  XPutImage(display,atom_masks[0],gc_mask,mask_image,0,0,0,0,size,size);

/* end of default Image generation - clean up variables used */ 

  free(image->data);
  image->data = NULL;
  free(mask_image->data);
  mask_image->data = NULL;
  XFree(image);
  XFree(mask_image);

/* create atom images */
	
  for(i=1;i<=options->natoms;++i) {
    size = 2*(options->atoms[i-1].radius)+1;
    radius = options->atoms[i-1].radius;
    xzero = radius;
    yzero = radius;
    aspsq = SQUARE( options->atoms[i-1].aspect_ratio );
    invaspsq = 1/aspsq;
    r0sq = SQUARE(radius);
    angle = options->atoms[i-1].angle;
    sin_ang = sin( (double)angle*(PI/180.0));
    cos_ang = cos( (double)angle*(PI/180.0));
    
    if((image=XCreateImage(display, DefaultVisual(display,screen), depth,
			   ZPixmap, 0, (char*) 0, (unsigned int) size,
			   (unsigned int) size, 32 ,0 ) )
       == (XImage *) 0) {
      fprintf(stderr,"%s: XCreateImage failed on default atom.\n",name);
      XCloseDisplay(display);
      exit(-1);
    }
    image->data = (char *) malloc(SQUARE(size) * sizeof(unsigned long));
    
    if((mask_image=XCreateImage(display, DefaultVisual(display,screen), 1, 
				ZPixmap, 0, (char*) 0, (unsigned int) size,
				(unsigned int) size, 8, 0 ) )
       == (XImage *) 0) {
      fprintf(stderr,"%s:XCreateImage failed on default atom mask.\n",name);
      XCloseDisplay(display);
      exit(-1);
    }
    mask_image->data = (char *) malloc(SQUARE(size)*sizeof(unsigned long) );
    
    /* create Image data for the atom */
    
    for(k=0;k<size;++k) {
      for(l=0;l<size;++l) {
	
	
/* ELLIPSES HERE ......................................... */
	
	if(aspsq >= 1) {
	  rsq = (int) ( aspsq * SQUARE(cos_ang*(k-xzero)-sin_ang*(l-yzero))+ 
		       SQUARE(sin_ang*(k-xzero)+cos_ang*(l - yzero) ) );
	} else {
	  rsq = (int) ( SQUARE(cos_ang*(k-xzero)-sin_ang*(l-yzero) ) + 
		       invaspsq*SQUARE(sin_ang * (k-xzero) + 
				       cos_ang * (l-yzero) ) );
	}
	
/* the -2 adds a black circle around the outer edge */
	if(sqrt((double)rsq)<sqrt((double)r0sq)-2) 
		XPutPixel(image,k,l,*foreground);
	else XPutPixel(image,k,l,*background);
	if(rsq<r0sq) XPutPixel(mask_image,k,l,1);
	else XPutPixel(mask_image,k,l,0);	
      }
    }
    XPutImage(display,atom_shapes[i],gc_image,image,0,0,0,0,size,size);
    XPutImage(display,atom_masks[i],gc_mask,mask_image,0,0,0,0,size,size);
    
/* end of atoms Image generation - clean up variables used */ 
    
    free(image->data);
    image->data = NULL;
    free(mask_image->data);
    mask_image->data = NULL;
    XFree(image);
    XFree(mask_image);
  } /* ends for(i=1;i<=options->natoms;++i) */
  
/* free lots of things here */

  XFreeGC(display,gc_image);
  XFreeGC(display,gc_mask);

  fflush(stderr);

} 

/* image_bw.c */
/* last modified 9-3-92 */





