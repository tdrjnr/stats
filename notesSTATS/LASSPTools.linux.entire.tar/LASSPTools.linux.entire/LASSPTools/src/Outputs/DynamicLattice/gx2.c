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
   gX2.c:  X-windows graphics for two-dimensional DynamicLattice 
*/

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "DynamicLattice.h"
#include "Xwindowdefs.h"

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::    graphics calls    ::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*:::::::::::::::::::::::::::    site graphics     ::::::::::::::::::::::::*/

int plot_updated_site (gpxwdw, i,j, z)
pix_window *gpxwdw;
int i,j;
double z;
{
   double XUL, YUL;
   int level, dX, dY;

   XUL = (double) i; YUL = (double) j;

   dX = SCALEX(gpxwdw,1.); dY = SCALEY(gpxwdw,1.);
   /* draw squares one pixel smaller to simulate lattice grid */
   if (graphics.grid) { dX -= 1; dY -= 1; }
     
   /* set color_val based on where z lies in zrange */
   level = floor(map.Nlevels*(z-zscale.zmin)/zscale.zrange);
   if (level < 0)             level = 0;
   if (level > map.Nlevels-1) level = map.Nlevels-1;
   /* set color or gray bitmap based on level */
   if (graphics.gray) 
   {
      XSetFillStyle(display, gpxwdw->gc, FillTiled);
      XSetTile(display, gpxwdw->gc, map.tilepix[level]);
   }
   else 
   {
      XSetForeground(display,gpxwdw->gc,map.fgd[level]);
   }

   XFillRectangle(display,gpxwdw->pixmap,gpxwdw->gc,
		  X(gpxwdw,XUL),Y(gpxwdw,YUL), dX, dY);
   if (dynamics.mode==0)
   {
       XFillRectangle(display,gpxwdw->window,gpxwdw->gc,
		      X(gpxwdw,XUL),Y(gpxwdw,YUL), dX, dY);
   }
   return (1);
}

erase_site (gpxwdw, i,j)
pix_window *gpxwdw;
int i,j;
{
   double XUL, YUL;
   int dX, dY;

   XUL = (double) i; YUL = (double) j;

   dX = SCALEX(gpxwdw,1.); dY = SCALEY(gpxwdw,1.);
      
   if (graphics.gray == 1)
   {
      XSetFillStyle(display,gpxwdw->gc,FillSolid);
   }
   XSetForeground(display,gpxwdw->gc,map.bgd);

   XFillRectangle(display,gpxwdw->pixmap,gpxwdw->gc,
		  X(gpxwdw,XUL),Y(gpxwdw,YUL), dX, dY);
   if (dynamics.mode == 0)
   {
       XFillRectangle(display,gpxwdw->window,gpxwdw->gc,
		      X(gpxwdw,XUL),Y(gpxwdw,YUL), dX, dY);
   }
}

/*:::::::::::::::::::::::::::    arrow graphics     :::::::::::::::::::::::*/

plot_updated_arrow (gpxwdw, i,j, dir, z, c)
pix_window *gpxwdw;
int i,j, c;
double dir,z;
{
   double XUL, YUL, xa, ya, length;
   int color_val, dX, dY, ncolor;

   XUL = (double) (i+.5);
   YUL = (double) (j+.5);

   /* scale arrow length by range */
   length = (z-zscale.zmin)/zscale.zrange;
   if (length < 0.) length = 0.;
   if (length > 1.) length = 1.;

   xa =   length * .50*sin(2.*M_PI*dir);
   ya =  -length * .50*cos(2.*M_PI*dir);

   XSetForeground(display, gpxwdw->gc, map.bgd);
   XFillRectangle(display, gpxwdw->pixmap, gpxwdw->gc,
		  X(gpxwdw,XUL-.5), Y(gpxwdw,YUL-.5), 
		  SCALEX(gpxwdw,1.), SCALEY(gpxwdw,1.));
   if (dynamics.mode == 0)
   {
       XFillRectangle(display, gpxwdw->window, gpxwdw->gc,
		      X(gpxwdw,XUL-.5), Y(gpxwdw,YUL-.5), 
		      SCALEX(gpxwdw,1.), SCALEY(gpxwdw,1.));
   }

   /* set foreground color based on color_val */
   if (c <= 0) c = 0; 
   if (c >= map.Nlevels) c = map.Nlevels-1;
   XSetForeground(display,gpxwdw->gc,map.fgd[c]);

   draw_arrow (gpxwdw, XUL-xa, YUL-ya, XUL+xa, YUL+ya);
}

erase_arrow (gpxwdw, i,j)
pix_window *gpxwdw;
int i,j;
{
   double XUL, YUL;
   int dX, dY;

   XUL = (double) (i+.5); 
   YUL = (double) (j+.5);

      XSetForeground(display, gpxwdw->gc, map.bgd);
   XFillRectangle(display, gpxwdw->pixmap, gpxwdw->gc,
		  X(gpxwdw,XUL-.5), Y(gpxwdw,YUL-.5), 
		  SCALEX(gpxwdw,1.0), SCALEY(gpxwdw,1.0));
      if (dynamics.mode == 0) 
      {
	    XFillRectangle(display, gpxwdw->window, gpxwdw->gc,
			   X(gpxwdw,XUL-.5), Y(gpxwdw,YUL-.5), 
			   SCALEX(gpxwdw,1.0), SCALEY(gpxwdw,1.0));
      }
}

draw_arrow(gpxwdw, Xi,Yi,Xf,Yf)
pix_window *gpxwdw;
double Xi,Yi,Xf,Yf;
{
  double ahlength = arrow.headlength, ahwidth = arrow.headwidth;

        arrow.points[0].x = X (gpxwdw,Xf);
        arrow.points[0].y = Y (gpxwdw,Yf);
        arrow.points[1].x = X (gpxwdw,Xf-ahlength*(Xf-Xi-ahwidth*(Yi-Yf)));
        arrow.points[1].y = Y (gpxwdw,Yf-ahlength*(Yf-Yi-ahwidth*(Xf-Xi)));
        arrow.points[2].x = X (gpxwdw,Xf-ahlength*(Xf-Xi+ahwidth*(Yi-Yf)));
        arrow.points[2].y = Y (gpxwdw,Yf-ahlength*(Yf-Yi+ahwidth*(Xf-Xi)));

	    XDrawLine(display,gpxwdw->pixmap,gpxwdw->gc,
			X(gpxwdw,Xi), Y(gpxwdw,Yi), 
			X(gpxwdw,Xf), Y(gpxwdw,Yf));
	    XFillPolygon(display,gpxwdw->pixmap,gpxwdw->gc,
			arrow.points,3,Convex,CoordModeOrigin);

      if (dynamics.mode == 0) 
      {
	    XDrawLine(display,gpxwdw->window,gpxwdw->gc,
			X(gpxwdw,Xi), Y(gpxwdw,Yi), 
			X(gpxwdw,Xf), Y(gpxwdw,Yf));
	    XFillPolygon(display,gpxwdw->window,gpxwdw->gc,
			arrow.points,3,Convex,CoordModeOrigin);
      }
}

/*:::::::::::::::::::::::::::    bond graphics     ::::::::::::::::::::::::*/

plot_updated_bond (gpxwdw, i,j, dir, len, c)
pix_window *gpxwdw;
int i,j, c;
double dir, len;
{
   double XUL, YUL, xb, yb;
   int color_val, dX, dY, ncolor;

   XUL = (double) i; YUL = (double) j;
      
   XSetForeground(display,gpxwdw->gc,map.fgd[c]);

   xb =  len*sin(2.*M_PI*dir);
   yb = -len*cos(2.*M_PI*dir);

       XDrawLine(display,gpxwdw->pixmap,gpxwdw->gc,
		 X(gpxwdw,XUL),Y(gpxwdw,YUL), 
		 X(gpxwdw, XUL+xb), Y(gpxwdw,YUL+yb));
   if (dynamics.mode == 0) 
   {
       XDrawLine(display,gpxwdw->window,gpxwdw->gc,
		 X(gpxwdw,XUL), Y(gpxwdw,YUL), 
		 X(gpxwdw, XUL+xb), Y(gpxwdw,YUL+yb));
   }
}
	
erase_bond (gpxwdw, i,j, dir)
pix_window *gpxwdw;
int i,j;
double dir;
{
   double XUL, YUL, xb, yb;
   int dX, dY;

   XUL = (double) i; YUL = (double) j;
   
   XSetForeground(display,gpxwdw->gc,map.bgd);
   
   xb =  sin(2.*M_PI*dir);
   yb = -cos(2.*M_PI*dir);
   
       XDrawLine(display,gpxwdw->pixmap,gpxwdw->gc,
		 X(gpxwdw,XUL),Y(gpxwdw,YUL), 
		 X(gpxwdw, XUL+xb), Y(gpxwdw,YUL+yb));
   if (dynamics.mode == 0) 
   {
       XDrawLine(display,gpxwdw->window,gpxwdw->gc,
		 X(gpxwdw,XUL),Y(gpxwdw,YUL), 
		 X(gpxwdw, XUL+xb), Y(gpxwdw,YUL+yb));
   }
}
	
/*::::::::::::::::::::::::::    misc graphics     :::::::::::::::::::::::*/

clear_graphics_window(gpxwdw)
pix_window *gpxwdw;
{
   int i,j;

   /* fill graphics window with background color */

   if (graphics.gray) 
   {
      XSetFillStyle(display,gpxwdw->gc,FillSolid);
   }
   XSetForeground(display, gpxwdw->gc, map.bgd);

   XFillRectangle(display, gpxwdw->pixmap, gpxwdw->gc,
		  X(gpxwdw,0.), Y(gpxwdw,0.), 
		  SCALEX(gpxwdw,lattice.nx), SCALEY(gpxwdw,lattice.ny));
   if (dynamics.mode == 0) 
   {
       XFillRectangle(display, gpxwdw->window, gpxwdw->gc,
		      X(gpxwdw,0.), Y(gpxwdw,0.), 
		      SCALEX(gpxwdw,lattice.nx), SCALEY(gpxwdw,lattice.ny));
   }
}

draw_map(gpxwdw)
pix_window *gpxwdw;
{
   int i = 0;
   double x, oldx=0., y, oldy=1.;

   if (map.position == 0)
   {
	for (i = 0; i < map.Nlevels; i++) 
	{
	     x = X(gpxwdw,((double)i+1.)/map.Nlevels);
	     if (graphics.gray) 
	     {
		  XSetFillStyle(display,gpxwdw->gc,FillTiled);
		  XSetTile(display,gpxwdw->gc, map.tilepix[i]);
	     }
	     else XSetForeground(display, gpxwdw->gc, map.fgd[i]);
	     XFillRectangle(display, gpxwdw->pixmap, gpxwdw->gc,
			    (int) oldx, Y(gpxwdw, 0.), 
			    (int)(x-oldx), SCALEY(gpxwdw,1.));
	     oldx = x;
	}
   }
   else if (map.position == 1)
   {
	for (i = 0; i < map.Nlevels; i++) 
	{
	     y = Y(gpxwdw,1.-(((double)i+1)/map.Nlevels));
	     if (graphics.gray) 
	     {
		  XSetFillStyle(display,gpxwdw->gc,FillTiled);
		  XSetTile(display,gpxwdw->gc, map.tilepix[i]);
	     }
	     else XSetForeground(display, gpxwdw->gc, map.fgd[i]);
	     XFillRectangle(display, gpxwdw->pixmap, gpxwdw->gc,
			    X(gpxwdw,0.), (int) y, 
			    SCALEX(gpxwdw,1.), (int) (oldy-y));
	     oldy = y;
	}
   }

   copy_pix2win(gpxwdw);
   XFlush(display);
}


copy_pix2win(pxwdw)
pix_window *pxwdw;
{
   XCopyArea (display, pxwdw->pixmap, pxwdw->window, pxwdw->gc,
          0,0, pxwdw->width, pxwdw->height, 0,0);
   XRaiseWindow (display, pxwdw->window);
}

copy_win2pix(pxwdw)
pix_window *pxwdw;
{
   XCopyArea (display, pxwdw->window, pxwdw->pixmap, pxwdw->gc,
          0,0, pxwdw->width, pxwdw->height, 0,0);
   XRaiseWindow (display, pxwdw->window);
}

write_text(str)
char **str;
{
    char line[256];

    XSetForeground(display, text_wdw->gc, map.bgd);
    XFillRectangle(display, text_wdw->window, text_wdw->gc,
		       X(text_wdw,0.), Y(text_wdw,0.), 
		       SCALEX(text_wdw,1.), SCALEY(text_wdw,0.45));
    XFillRectangle(display, text_wdw->pixmap, text_wdw->gc,
		       X(text_wdw,0.), Y(text_wdw,0.), 
		       SCALEX(text_wdw,1.), SCALEY(text_wdw,0.45));
    XSetBackground (display, text_wdw->gc, map.bgd);
    if (graphics.textc != -1)
    {
	 XSetForeground (display, text_wdw->gc, map.fgd[graphics.textc]);
	 sprintf(line, "%s ", str);
	 XDrawImageString (display, text_wdw->window, text_wdw->gc,
			   X(text_wdw, 0.0), Y(text_wdw, 0.33),
			   line, strlen(line));
	 sprintf(line, "%s ", str);
	 XDrawImageString (display, text_wdw->pixmap, text_wdw->gc,
			   X(text_wdw, 0.0), Y(text_wdw, 0.33),
			   line, strlen(line));
	 XFlush(display);
    }
}

write_zrange(str)
char **str;
{
    char line[256];

    XSetForeground(display, text_wdw->gc, map.bgd);
    XFillRectangle(display, text_wdw->window, text_wdw->gc,
		       X(text_wdw,0.), Y(text_wdw,0.45), 
		       SCALEX(text_wdw,1.), SCALEY(text_wdw,0.55));
    XFillRectangle(display, text_wdw->pixmap, text_wdw->gc,
		       X(text_wdw,0.), Y(text_wdw,0.45), 
		       SCALEX(text_wdw,1.), SCALEY(text_wdw,0.55));
    XSetBackground (display, text_wdw->gc, map.bgd);
    if (graphics.textc != -1)
    {
	 XSetForeground (display, text_wdw->gc, map.fgd[graphics.textc]);
	 sprintf(line, "z: [%.1e, %.1e]", zscale.zmin, zscale.zmax);
	 XDrawImageString (display, text_wdw->window, text_wdw->gc,
			   X(text_wdw, 0.0), Y(text_wdw, 0.73),
			   line, strlen(line));
	 sprintf(line, "z: [%.1e, %.1e]", zscale.zmin, zscale.zmax);
	 XDrawImageString (display, text_wdw->pixmap, text_wdw->gc,
			   X(text_wdw, 0.0), Y(text_wdw, 0.73),
			   line, strlen(line));
	 XFlush(display);
    }
}

