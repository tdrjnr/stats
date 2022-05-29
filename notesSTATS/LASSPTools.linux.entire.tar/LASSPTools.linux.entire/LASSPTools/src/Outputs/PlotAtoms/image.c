#include"plotatoms.h"

#define NCOLORS 256

/* This routine does LOTS of work - color allocations, image generation,
   pixmap creation */

#ifndef _NO_PROTO 
 void create_atoms(Display *display, int screen, int depth, Options *options,
		   unsigned long option_mask, unsigned long *foreground,
		   unsigned long *background)
#else
 void create_atoms(display, screen, depth, options, option_mask, foreground, 
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
  Colormap                colormap;
  
  XGCValues	          gc_image_values, gc_mask_values;
  XImage                  *image,*mask_image;
  XColor                  calc_color,parsed_color;
  XColor                  *atom_colors;
  XColor                  default_color;
  Visual                  *default_visual;

  unsigned long		  gc_image_mask, gc_mask_mask;
  unsigned long           alloc_pixels[NCOLORS];
  unsigned long           black,white;
  unsigned long           **pixels;
  unsigned long           *default_pixels;
  int i,j, read_write;
  int *failure;
  int                     r0sq,rsq,value,nc,k,l;
  int                     radius,size,xzero,yzero;
  int total_radii=0;
  int *number_of_colors;
  int default_noc;
  int angle;
  float aspsq, invaspsq;
  double sin_ang, cos_ang;

  colormap = DefaultColormap(display,screen);

/* Check Visual Class to determine color allocation scheme */
  default_visual = DefaultVisual(display,screen);
  if( ( default_visual->class == DirectColor) || 
      ( default_visual->class == PseudoColor) || 
      ( default_visual->class == GrayScale) ) {
    read_write = 1;  /* Colormap has read/write cells */
  } else {
    read_write = 0;  /* Colormap has read-only cells */
  }

#ifdef DEBUG
  fprintf(stderr,"read_write = %d\n", read_write);
  fflush(stderr);
#endif

/* get black and white here */
  black = BlackPixel(display,screen);
  white = WhitePixel(display,screen);

/* allocate foreground color */
  if( XParseColor(display,colormap,(options->foreground),
		  &parsed_color ) == 0 ) {
    fprintf(stderr,"%s: Error parsing foreground color. Using white.\n",name);
    *foreground = white;
  } else {
    XAllocColor(display,colormap,&parsed_color);
    *foreground = parsed_color.pixel;
  }

/* allocate background color */
  if( XParseColor(display,colormap,(options->background),
		  &parsed_color ) == 0 ) {
    fprintf(stderr,"%s: Error parsing background color. Using black.\n",name);
    *background = black;
  } else {
    XAllocColor(display,colormap,&parsed_color);
    *background = parsed_color.pixel;
  }

/* malloc storage for atom data */
  atom_colors = (XColor *) malloc( (options->natoms)*sizeof(XColor) );
  failure = (int *) malloc((options->natoms)*sizeof(int) );
  pixels =(unsigned long **) malloc((options->natoms)*sizeof(unsigned long *));
  number_of_colors = (int *) malloc((options->natoms)*sizeof(int));

  for( i=0;i<(options->natoms);++i) {
    failure[i] = 0;
    if( (option_mask & PAShade) == 0 ) {
      pixels[i] = (unsigned long *) malloc( 1*sizeof(unsigned long));
      number_of_colors[i] = 1;
    } else {
      pixels[i] = (unsigned long *) malloc( (options->atoms[i].radius)*
					   sizeof(unsigned long) );
    }
  }

/* set default noc here ? */  
  if( (option_mask & PAShade) == 0 ) {
    default_pixels = (unsigned long *) malloc( 1*sizeof(unsigned long));
    default_noc = 1;
  } else {
    default_pixels =(unsigned long *) malloc((options->default_atom.radius) *
					     sizeof(unsigned long));
  }

  if( XParseColor(display,colormap,(options->default_atom.color),
		  &default_color ) == 0 ) {
    fprintf(stderr,"%s: Error parsing default atom color. Exiting.\n",name);
    exit(-1);
  } else {
    XAllocColor(display,colormap,&default_color);
    default_pixels[0] = default_color.pixel;
    total_radii += options->default_atom.radius; 
  }

  if( (options->natoms) > 1 ) {
    for(i=0;i<options->natoms;++i) {
      if( XParseColor(display,colormap,(options->atoms[i].color),
		      &(atom_colors[i]) ) == 0 ) {
       fprintf(stderr,"%s:Error parsing atom %i color. Using default color.\n",
		name, i+1);
	atom_colors[i] = default_color;
	failure[i] = -1;
      } else {
	total_radii += options->atoms[i].radius;
      }    
    }
  } else {
    failure[0] = -1;
  }

  for(i=0;i<(options->natoms);++i) {
    if( failure[i] != -1 ) {
      XAllocColor(display,colormap,&(atom_colors[i]) );
    }
    pixels[i][0] = atom_colors[i].pixel;
  }


  if( (option_mask & PAShade) != 0 ) {  /* shading set */


    if( read_write == 1) {
/* Allocate read/write color cells */
      for ( nc = total_radii ; nc > 0; nc-- ) {
	if ( XAllocColorCells(display,colormap,0,0,0, alloc_pixels, nc)!=0)
	  break;
      }
      if ( nc == total_radii ) {
	default_noc = options->default_atom.radius;
	for(i=0;i<options->natoms;++i) {
	  number_of_colors[i] = options->atoms[i].radius;
	}
      } else {

#ifdef DEBUG
	fprintf(stderr, "%s: Only able to allocate %d out of %d colors\n",
		name,nc,total_radii);
	fflush(stderr);
#endif

	if ( nc == 0 ) {
	  fprintf(stderr,"%s: Couldn't allocate any colors.\n",name);
	  fflush(stderr);
	  exit(-1);
	}
	default_noc = (int) ( ( (float)( options->default_atom.radius )/
			       (float)total_radii)*nc );
	for(i=0;i<options->natoms;++i) {
	  number_of_colors[i] = (int) ( ( (float)( options->atoms[i].radius )/
					 (float)total_radii)*nc);
	}
      }
    } else {
/* Colorcells are read-only --- going for broke here */
      default_noc = options->default_atom.radius;
      for(i=0;i<options->natoms;++i) {
	number_of_colors[i] = options->atoms[i].radius;
      }
    } /* ends if( read_write == 1 ) */

    calc_color.flags=DoRed|DoBlue|DoGreen;
    
    j = 0;

    for(nc=1;nc<default_noc;nc++) {
      calc_color.red = (int) (default_color.red * 
	sqrt(1 - (float)nc/(float)(default_noc)) );
      calc_color.green = (int) (default_color.green * 
	sqrt(1 - (float)nc/(float)(default_noc)));
      calc_color.blue = (int) (default_color.blue * 
	sqrt(1 - (float)nc/(float)(default_noc)));

      if(read_write ==1 ) {
	default_pixels[nc] = alloc_pixels[j];
	calc_color.pixel=alloc_pixels[j];
	j++;
	XStoreColor(display,colormap,&calc_color);
      } else {
	XAllocColor(display, colormap, &calc_color);
	default_pixels[nc] = calc_color.pixel;
      }

    }
    
    for(i=0;i<options->natoms;++i) {
      if( failure[i] != -1 ) {
	for(nc=1;nc<number_of_colors[i];nc++) {
	  calc_color.red = (int) (atom_colors[i].red * 
	   /*SQUARE*/sqrt(1 - (float)nc/(float)(number_of_colors[i])));
	  calc_color.green = (int) (atom_colors[i].green * 
	   /*SQUARE*/sqrt(1 - (float)nc/(float)(number_of_colors[i])));
	  calc_color.blue = (int) (atom_colors[i].blue * 
	   /*SQUARE*/sqrt(1 - (float)nc/(float)(number_of_colors[i])));
	  
	  if(read_write == 1) {
	    pixels[i][nc] = alloc_pixels[j];
	    calc_color.pixel=alloc_pixels[j];
	    j++;
	    XStoreColor(display,colormap,&calc_color);
	  } else {
	    XAllocColor(display, colormap, &calc_color);
	    pixels[i][nc] = calc_color.pixel;
	  }

	}
      }
    }
  }  /* ends if( (option_mask & PAShade) != 0 ) */


  atom_shapes = (Pixmap *) malloc( (options->natoms+1)*sizeof(Pixmap) );
  atom_masks = (Pixmap *) malloc( (options->natoms+1)*sizeof(Pixmap) );

  for(i=0;i<=options->natoms;++i) {
    if(i>0) {
      if(failure[i] != -1 ) {
	atom_shapes[i] = XCreatePixmap(display,RootWindow(display,screen),
				      2*(options->atoms[i-1].radius)+1,
				      2*(options->atoms[i-1].radius)+1, depth);
	atom_masks[i] = XCreatePixmap(display,RootWindow(display,screen),
				     2*(options->atoms[i-1].radius)+1,
				     2*(options->atoms[i-1].radius)+1,1);
      } else {
	atom_shapes[i] = atom_shapes[0];
	atom_masks[i] = atom_masks[0];
      }
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
      if(rsq<r0sq) {
	if( (option_mask & PAShade) != 0 ) {
/*	  value=(int)(sqrt((1-(double)(rsq)/(double)(r0sq)))*
		      (default_noc)); */
	  value=(int)(sqrt((double)(rsq)/(double)(r0sq))*
		      (default_noc));
/*	  value=(int)((double)(rsq)/(double)(r0sq)*
		      (default_noc));*/
	  if(value < 1 ) value = 1;
	  XPutPixel(image,k,l,default_pixels[value-1]);
	} else {
	  XPutPixel(image,k,l,default_pixels[0]);
	}
	XPutPixel(mask_image,k,l,1);

/* CHANGED 7-17-91 */
/*	if(white==1) XPutPixel(mask_image,k,l,white);
	else XPutPixel(mask_image,k,l,black); */

      } else { 
	XPutPixel(image,k,l,*foreground);
	XPutPixel(mask_image,k,l,0);

/* CHANGED 7-17-91 */
/*	if(white==1) XPutPixel(mask_image,k,l,black);
	else XPutPixel(mask_image,k,l,white); */

      }
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
    if(failure[i] != -1 ) {
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


	  if(rsq<r0sq) {
	    if( (option_mask & PAShade) != 0 ) {
/*	      value=(int)(sqrt((1-(double)(rsq)/(double)(r0sq)))*
			  (number_of_colors[i-1]));*/
	      value=(int)( /*SQUARE*/ sqrt((double)(rsq)/(double)(r0sq))*
			  (number_of_colors[i-1]));
	      if(value < 1) value = 1;
	      XPutPixel(image,k,l,pixels[i-1][value-1]);
	    } else {
	      XPutPixel(image,k,l,pixels[i-1][0]);
	    }
	    XPutPixel(mask_image,k,l,1);

/* CHANGED 7-17-91 */
/*	    if(white==1) XPutPixel(mask_image,k,l,white);
	    else XPutPixel(mask_image,k,l,black); */

	  } else { 
	    XPutPixel(image,k,l,*foreground);
	    XPutPixel(mask_image,k,l,0);

/* CHANGED 7-17-91 */
/*	    if(white==1) XPutPixel(mask_image,k,l,black);
	    else XPutPixel(mask_image,k,l,white);*/

	  }
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
    } /* ends if(failure[i]!=-1) */
  } /* ends for(i=1;i<=options->natoms;++i) */

  XFreeGC(display,gc_image);
  XFreeGC(display,gc_mask);
  free(atom_colors);
  free(failure);
  free(pixels);
  free(default_pixels);
  free(number_of_colors);

  fflush(stderr);



	
/* free lots of things here */


   } 

/* TTD:
   1.Fix the case of 1 atom - there we shouldn't create both a
     default and a first atom 
   2.Free variables after we finish using them, not at the end 
*/   


/* image.c */
/* last modified 3-31-91 */





