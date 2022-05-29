/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 
/*
   DynamicLattice: 
   X-windows program to animate dynamics on square or rectangular lattices

   see enclosed man page and online help via "DynamicLattice -h"

   Copyright (C) 1990 Christopher R. Myers (with the exception of
   math_io.c, from the LASSPTools distribution).  All rights reserved.
   Feel free to copy, modify and redistribute this program, but do not
   sell it.  Include this notice in any copy.

   written: 10/90; latest revision: 2/93 (v. 3.0)
*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 

/*
   xinit.c:  initialization and allocation routines for DynamicLattice
*/

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "DynamicLattice.h"
#include "Xwindowdefs.h"

XSetWindowAttributes mw_attr;
unsigned int border_width = 4;
Font text_font;

#define NAMESIZE 64

char bdrname[64], bgdname[64];

/* default colormap: blue to red, via whitish-gray */

#define DEFAULT_NCOLORS 128

char *blue_to_red[128] = {
"#0000ff", "#0404ff", "#0808ff", "#0c0cff",
"#1010ff", "#1414ff", "#1818ff", "#1c1cff",
"#2020ff", "#2424ff", "#2828ff", "#2c2cff",
"#3030ff", "#3434ff", "#3838ff", "#3c3cff",
"#4040ff", "#4444ff", "#4848ff", "#4c4cff",
"#5050ff", "#5454ff", "#5858ff", "#5c5cff",
"#6060ff", "#6464ff", "#6868ff", "#6c6cff",
"#7070ff", "#7474ff", "#7878ff", "#7c7cff",
"#8080ff", "#8484ff", "#8888ff", "#8c8cff",
"#9090ff", "#9494ff", "#9898ff", "#9c9cff",
"#a0a0ff", "#a4a4ff", "#a8a8ff", "#acacff",
"#b0b0ff", "#b4b4ff", "#b8b8ff", "#bcbcff",
"#c0c0ff", "#c4c4ff", "#c8c8ff", "#ccccff",
"#d0d0ff", "#d4d4ff", "#d8d8ff", "#dcdcff",
"#e0e0ff", "#e4e4ff", "#e8e8ff", "#ececff",
"#f0f0ff", "#f4f4ff", "#f8f8ff", "#fcfcff",
"#fffcfc", "#fff8f8", "#fff4f4", "#fff0f0",
"#ffecec", "#ffe8e8", "#ffe4e4", "#ffe0e0",
"#ffdcdc", "#ffd8d8", "#ffd4d4", "#ffd0d0",
"#ffcccc", "#ffc8c8", "#ffc4c4", "#ffc0c0",
"#ffbcbc", "#ffb8b8", "#ffb4b4", "#ffb0b0",
"#ffacac", "#ffa8a8", "#ffa4a4", "#ffa0a0",
"#ff9c9c", "#ff9898", "#ff9494", "#ff9090",
"#ff8c8c", "#ff8888", "#ff8484", "#ff8080",
"#ff7c7c", "#ff7878", "#ff7474", "#ff7070",
"#ff6c6c", "#ff6868", "#ff6464", "#ff6060",
"#ff5c5c", "#ff5858", "#ff5454", "#ff5050",
"#ff4c4c", "#ff4848", "#ff4444", "#ff4040",
"#ff3c3c", "#ff3838", "#ff3434", "#ff3030",
"#ff2c2c", "#ff2828", "#ff2424", "#ff2020",
"#ff1c1c", "#ff1818", "#ff1414", "#ff1010",
"#ff0c0c", "#ff0808", "#ff0404", "#ff0000"};

/* default graymap: 4x4 ordered dither */

#define DEFAULT_NGRAY 16
#define BITMAPSIZE 1024
#define defaultgraymap_width 4
#define defaultgraymap_height 4
char four_by_four_dither[][BITMAPSIZE] = {
{0xfe, 0xff, 0xff, 0xff},
{0xfe, 0xff, 0xfb, 0xff},
{0xfa, 0xff, 0xfb, 0xff},
{0xfa, 0xff, 0xfa, 0xff},
{0xfa, 0xfd, 0xfa, 0xff},
{0xfa, 0xfd, 0xfa, 0xf7},
{0xfa, 0xf5, 0xfa, 0xf7},
{0xfa, 0xf5, 0xfa, 0xf5},
{0xf8, 0xf5, 0xfa, 0xf5},
{0xf8, 0xf5, 0xf2, 0xf5},
{0xf0, 0xf5, 0xf2, 0xf5},
{0xf0, 0xf5, 0xf0, 0xf5},
{0xf0, 0xf4, 0xf0, 0xf5},
{0xf0, 0xf4, 0xf0, 0xf1},
{0xf0, 0xf0, 0xf0, 0xf1},
{0xf0, 0xf0, 0xf0, 0xf0}};


/***************************************************************************/
/****************       initialize display variables      ******************/
/***************************************************************************/

initialize_display()
{
    XColor exact_def;

    display = XOpenDisplay("");
    screen  = DefaultScreen(display);
    
    map.depth    = DisplayPlanes(display, screen);
    visual       = DefaultVisual(display, screen);
    syscolormap  = DefaultColormap(display, screen);

    if (map.depth == 1) graphics.gray = 1;
}

/***************************************************************************/
/****************     get color/grayscale information     ******************/
/***************************************************************************/

input_map_info()
{
   int i = 0, j = 0;

      set_window_sizes();

      if (!set.map || ((graphics.mapfd = fopen(graphics.mapf, "r")) == NULL)) 
      {
		if (set.map) 
		   fprintf(stderr, "Warning: colormap file not found.\n");
		if (graphics.gray) 
		{
		    install_default_graymap();
		}
	        else 
		{
		    install_default_colormap();
		}
      }
      else
      {
		fscanf (graphics.mapfd, "background color: %s\n", bgdname);
		fscanf (graphics.mapfd, "border color: %s\n", bdrname);
		fscanf (graphics.mapfd, "number of levels: %d\n", &map.Nlevels);
		map.fgdname = (char **) malloc (map.Nlevels * sizeof(char *));
		map.fgd     = (unsigned long *) 
		  malloc ((map.Nlevels+1) * sizeof(unsigned long)) 
		    + 1;
		for (i = 0; i < map.Nlevels; i++) 
		{
		    map.fgdname[i] = (char *) malloc (NAMESIZE*sizeof(char));
		    fscanf (graphics.mapfd, "%s\n", map.fgdname[i]);
		}

		fclose(graphics.mapfd);
      }

   /* parse names to allocate colors/grays in map */
   initialize_map(); 

   return (1);
}

/***************      allocate colors based on colornames    ***************/

initialize_map()
{
   XColor exact_def;
   int i, j;

      if (!XParseColor (display, syscolormap, bdrname, &exact_def)) {
         printf("basic: color %s not in database", bdrname);
         exit(0);
      }
      if (!XAllocColor(display, syscolormap, &exact_def)) {
         printf("basic: all colorcells allocated and read/write \n");
         exit (0);
      }
      map.bdr = exact_def.pixel;

      if (!XParseColor (display, syscolormap, bgdname, &exact_def)) {
         printf("basic: color %s not in database", bgdname);
         exit(0);
      }
      if (!XAllocColor(display, syscolormap, &exact_def)) {
         printf("basic: all colorcells allocated and read/write \n");
         exit (0);
      }
      map.bgd = exact_def.pixel;

   if (graphics.gray) 
   {
      for (i = 0; i < map.Nlevels; i++) 
      {
	    create_included_stipple(i, &map.tilepix[i], 
			    &map.tilepix_width, &map.tilepix_height);
      }
   }
   else 
   {
      for (i = 0; i < map.Nlevels; i++) 
      {
	  if(!XParseColor (display, syscolormap, map.fgdname[i], &exact_def)) 
	  {
	      printf("basic: color fgdname %s not in database", 
		     map.fgdname[i]);
	      exit(0);
	  }
	  if (!XAllocColor(display, syscolormap, &exact_def)) 
	  {
	      printf("basic: all colorcells allocated and read/write \n");
	      exit (0);
	  }
	  map.fgd[i] = exact_def.pixel;
      }
   }
   map.fgd[-1] = map.bgd;
}


/***************************************************************************/
/********************   allocate and initialize graphics    ****************/
/***************************************************************************/

open_Xwindow()
{
   mw_attr.backing_store = Always;

   /* MAIN WINDOW */

     /* make main_window */
     main_window = XCreateSimpleWindow (display, DefaultRootWindow(display),
		 wdw.x, wdw.y, wdw.width, wdw.height,
	         border_width, map.bdr, map.bgd);
     main_gc     = XCreateGC (display, main_window, 0, 0);
     XChangeWindowAttributes (display, main_window, CWBackingStore, &mw_attr);
     XSetStandardProperties (display, main_window, "DynamicLattice", 
	NULL, NULL, NULL, 0, &wdw);

     /* map main_window */
     XMapRaised (display, main_window);

   /* SUBWINDOWS */

   /* GRAPHICS SUBWINDOWS */

   /* make graphics pix_windows */
   g1_pixwdw = make_pix_window (display, main_window, wdwg,
				0., 0., 
				(double)lattice.nx, (double)lattice.ny, 
				0., 0., map.bdr, map.bgd);
   g2_pixwdw = make_pix_window (display, main_window, wdw2g,
				0., 0., 1., 1., 0., 0., map.bdr, map.bgd);
   /* TEXT SUBWINDOW */

   /* make text pix_windows */
   text_wdw = make_pix_window (display, main_window, twdw,
			       0., 0., 1., 1., 0., 0., map.bgd, map.bgd);

   if (wdwg.width < 100)      text_font = XLoadFont (display, "6x12");
   else if (wdwg.width < 200) text_font = XLoadFont (display, "8x13");
   else                       text_font = XLoadFont (display, "9x15");

   XSetFont (display, text_wdw->gc, text_font);

/*
   XChangeWindowAttributes (display, g1_pixwdw->window, CWBackingStore,
			    &mw_attr);
   XChangeWindowAttributes (display, g2_pixwdw->window, CWBackingStore,
			    &mw_attr);
   XChangeWindowAttributes (display, text_wdw->window, CWBackingStore,
			    &mw_attr);
*/
   XSetWindowBackgroundPixmap(display, g1_pixwdw->window, g1_pixwdw->pixmap);
   XSetWindowBackgroundPixmap(display, g2_pixwdw->window, g2_pixwdw->pixmap);
   XSetWindowBackgroundPixmap(display, text_wdw->window,  text_wdw->pixmap);

   /* map subwindows */
   XMapSubwindows (display, main_window);

  XSelectInput(display,g1_pixwdw->window, ExposureMask | KeyPressMask |
               ButtonPressMask | StructureNotifyMask );

   clear_graphics_window(g1_pixwdw);
   clear_graphics_window(text_wdw);
   if (graphics.writez) write_zrange();
   write_text(graphics.title);
   draw_map(g2_pixwdw);

   window_open = 1;
}

/***************************************************************************/

close_Xwindow()
{
   /* free graphics window, pixmap, gc */
   XFreeGC (display, g1_pixwdw->gc);
   XFreePixmap (display, g1_pixwdw->pixmap);
   XDestroyWindow (display, g1_pixwdw->window);

   /* free main window, gc */
   XFreeGC (display, main_gc);
   XDestroyWindow (display, main_window);

   /* close display */
   XCloseDisplay (display);
}

/***************************************************************************/
/******************       create pix_window structs        *****************/
/***************************************************************************/

pix_window *make_pix_window(dpy, parent, wdw, 
			    xmin, ymin, xmax, ymax, xoff, yoff, fc, bc)
Display *dpy;
Window parent;
XSizeHints wdw;
unsigned long fc, bc;
double xmin, ymin, xmax, ymax, xoff, yoff;
{
   pix_window *pxwdw;

   pxwdw = (pix_window *) malloc (sizeof(pix_window));
   if (pxwdw == (pix_window *) 0) return pxwdw;

   /* check for legal bounds */
   if (xmin >= xmax || ymin >= ymax) return (pix_window *) 0;

   window_attr.override_redirect = True;
   pxwdw->window = XCreateSimpleWindow (display, parent, wdw.x, wdw.y,
      wdw.width, wdw.height, border_width, fc, bc);
   pxwdw->pixmap = XCreatePixmap (display, pxwdw->window, 
      wdw.width, wdw.height, DefaultDepth(display,DefaultScreen(display)));
   pxwdw->gc     = XCreateGC (display, pxwdw->pixmap, 0,0);

   pxwdw->width  = wdw.width;
   pxwdw->height = wdw.height;
   pxwdw->xmin   = xmin;
   pxwdw->ymin   = ymin;
   pxwdw->xmax   = xmax;
   pxwdw->ymax   = ymax;
   pxwdw->xoff   = xoff;
   pxwdw->yoff   = yoff;
   pxwdw->xscale = wdw.width/(xmax-xmin);
   pxwdw->yscale = wdw.height/(ymax-ymin);

   return (pxwdw);
}

/*************************************************************************/
/***********************  encoded defaults   *****************************/
/*************************************************************************/

set_window_sizes()
{
   int oxmargin = 40, oymargin = 40, ixmargin = 40, iymargin = 40,
   map_height = 0, map_width = 0, text_height, nmax, gw, gh;

   /* set main window position */
   wdw.x = 0; wdw.y = 0;

   /* set graphics window position and size */
   wdwg.x = oxmargin; wdwg.y = oymargin;
   if (!set.pix) 
   {
      nmax = (lattice.nx >= lattice.ny) ? lattice.nx : lattice.ny;
      if      (nmax <= 16)
      {
	  wdwg.width  = lattice.nx*32; 
	  wdwg.height = lattice.ny*32; 	  
      }
      else if (nmax <= 32)  
      { 
	  wdwg.width  = lattice.nx*16; 
	  wdwg.height = lattice.ny*16; 
      }
      else if (nmax <= 64)  
      { 
	  wdwg.width  = lattice.nx*8;  
	  wdwg.height = lattice.ny*8;  
      }
      else if (nmax <= 256) 
      { 
	  wdwg.width  = lattice.nx*4;  
	  wdwg.height = lattice.ny*4;  
      }
      else if (nmax <= 512) 
      { 
	  wdwg.width  = lattice.nx*2;  
	  wdwg.height = lattice.ny*2;  
      }
      else  
      { 
	  wdwg.width  = lattice.nx;    
	  wdwg.height = lattice.ny;    
      }
   }
   else 
   {
	wdwg.width  = lattice.nx*graphics.pix;
	wdwg.height = lattice.ny*graphics.pix;
   }

   /* set secondary graphics window (colormap) position and size */
   if (map.position == 0)
   {
	wdw2g.x = wdwg.x; 
	wdw2g.y = wdwg.y + wdwg.height + iymargin + 2*border_width;
	wdw2g.width = wdwg.width; 
	if (wdw2g.width >= 20) wdw2g.height = map_height = 20;
	else                   wdw2g.height = map_height = 8;
   }
   else if (map.position == 1)
   {
	wdw2g.y = wdwg.y; 
	wdw2g.x = wdwg.x + wdwg.width + ixmargin + 2*border_width;
	wdw2g.height = wdwg.height; 
	if (wdw2g.height >= 20) wdw2g.width = map_width = 20;
	else                    wdw2g.width = map_width = 8;
   }

   /* set text window position and size */
   if (map.position == 0)
   {
	twdw.x = wdwg.x; 
	twdw.y = wdw2g.y + wdw2g.height + iymargin + 2*border_width;
	twdw.width = wdwg.width; 
	if (twdw.width >= 20) twdw.height = text_height = 40;
	else                  twdw.height = text_height = 16;
   }
   else if (map.position == 1)
   {
	twdw.x = wdwg.x; 
	twdw.y = wdwg.y + wdwg.height + iymargin + 2*border_width;
	twdw.width = wdwg.width + ixmargin + wdw2g.width; 
	if (twdw.width >= 20) twdw.height = text_height = 40;
	else                  twdw.height = text_height = 16;
   }

   /* set main window size based on size of graphics windows */
   /*wdw.width  = wdwg.width  + 2*xmargin + map_width + 2*border_width;*/
   if (map.position == 0)
   {
	wdw.width  = wdwg.x + wdwg.width + oxmargin;
	wdw.height = twdw.y + twdw.height + oymargin/2;
   }
   else if (map.position == 1)
   {
	wdw.width  = wdw2g.x + wdw2g.width + oxmargin;
	wdw.height = twdw.y + twdw.height + oymargin/2;
   }

   if (strcmp(geomstr,"")==0)
       wdw.flags |= PPosition;
   else
   {
       XParseGeometry(geomstr, &wdw.x, &wdw.y, &gw, &gh);
       wdw.flags |= USPosition;
   }    

   /* set arrow attributes based on window size */
   if (lattice.nx < 30 || lattice.ny < 30) 
   { 
       arrow.headlength = .30; arrow.headwidth = .25; 
   }
   else
   { 
       arrow.headlength = .50; arrow.headwidth = .45;
   }
}

install_default_colormap()
{
   int i;

   strcpy (bdrname, "#777777");  /* medium gray */ 
   strcpy (bgdname, "#000000");  /* black */

   /* allocate and set color names */
   map.Nlevels = DEFAULT_NCOLORS;
   if (!set.defc)  graphics.defc  = map.Nlevels/2;
   if (!set.textc) graphics.textc = map.Nlevels/2;

   map.fgdname = (char **) malloc (map.Nlevels * sizeof(char *));
   map.fgd = (unsigned long *) malloc (map.Nlevels * sizeof(unsigned long));
	
   for (i = 0; i < map.Nlevels; i++) 
   {
       map.fgdname[i] = (char *) malloc (NAMESIZE*sizeof(char));
       strcpy(map.fgdname[i], blue_to_red[i]);
   }
}

install_default_graymap()
{
   int i,j;

   strcpy(bdrname, "#ffffff");  /* white */
   strcpy(bgdname, "#000000");  /* black */

   /* allocate and set gray levels */
   map.Nlevels = DEFAULT_NGRAY;

   map.bitmap  = (char **)  malloc (map.Nlevels * sizeof(char *));
   map.tilepix = (Pixmap *) malloc (map.Nlevels * sizeof(Pixmap));
   map.fgd = (unsigned long *) malloc (map.Nlevels * sizeof(unsigned long));

   for (i = 0; i < map.Nlevels; i++) 
   {
       map.bitmap[i] = (char *) malloc (BITMAPSIZE*sizeof(char));
       strcpy(map.bitmap[i], four_by_four_dither[i]);
       map.fgd[i] = WhitePixel(display,screen);
   }
}

/***************************************************************************/

create_included_stipple(stipno, stip, width, height)
int stipno;
Pixmap *stip;
unsigned int *width, *height;
{
  if ((*stip = XCreateBitmapFromData(display,
	    RootWindow(display, screen), four_by_four_dither[stipno],
	    defaultgraymap_width, defaultgraymap_height)) 
      == False) return (False);
  *width  = defaultgraymap_width;
  *height = defaultgraymap_height;
  return (True);
}

/***************************************************************************/


