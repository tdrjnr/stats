/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 
/*

   DynamicLattice: X-windows program to animate dynamics on square 
   or rectangular lattices

   see enclosed man page (DynamicLattice.l) and online help 
   via "DynamicLattice -h"

   Copyright (C) 1990 Christopher R. Myers (with the exception of
   math_io.c, from the LASSPTools distribution).  All rights reserved.
   Feel free to copy, modify and redistribute this program, but do not
   sell it.  Include this notice in any copy.

   written: 10/90; latest revision: 11/92 (v. 3.0)

*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 

#ifdef XINIT
#define XEXTERN
#else 
#define XEXTERN extern
#endif

XEXTERN XSizeHints wdw, wdwg, wdw2g, twdw;
XEXTERN Display *display;
XEXTERN Window main_window;
XEXTERN Drawable g_drawable;
XEXTERN XSetWindowAttributes window_attr;
XEXTERN GC main_gc;
XEXTERN XEvent event1, event2;
XEXTERN int screen, depth, discard, window_open;
XEXTERN Visual *visual;
XEXTERN Colormap syscolormap;
XEXTERN char geomstr[128];

#define X(pxwdw,x) ((int) floor(pxwdw->xscale*(x-pxwdw->xmin)+pxwdw->xoff+.5))
#define Y(pxwdw,y) ((int) floor(pxwdw->yscale*(y-pxwdw->ymin)+pxwdw->yoff+.5))
#define InvX(pxwdw,X) ((int) floor(((X)/(pxwdw->xscale)) + pxwdw->xmin + .5))
#define InvY(pxwdw,Y) ((int) floor(((Y)/(pxwdw->yscale)) + pxwdw->ymin + .5))
#define SCALEX(pxwdw,x) ((int) floor(pxwdw->xscale*(x) + .5))
#define SCALEY(pxwdw,y) ((int) floor(pxwdw->yscale*(y) + .5))

typedef struct {
   Window window;
     unsigned int width, height;
     double xmin, ymin, xmax, ymax, xoff, yoff, xscale, yscale;
   Pixmap pixmap;
   GC gc;
} pix_window;

XEXTERN pix_window *g1_pixwdw, *g2_pixwdw, *text_wdw, *make_pix_window();

typedef struct {
   int depth;
   int Nlevels;
   unsigned long *fgd;
   unsigned long bgd;
   unsigned long bdr;
   char **fgdname;
   char **bitmap;
   Pixmap *tilepix;
   unsigned int tilepix_width;
   unsigned int tilepix_height;
   int position;          /* 0 = horiz. on bottom, 1 = vert. on right */
} MAP;

XEXTERN MAP map;

typedef struct {
   XPoint *points;
   double length;
   double headlength;
   double headwidth;
} ARROW;

XEXTERN ARROW arrow;
