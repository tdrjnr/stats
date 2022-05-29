/*
** PlotMolecule -
**    An X-windows and Motif program to display ball and stick
**    representations of molecules along with interactive
**    control for scaling and rotating.
**
** Feel free to copy, modify and redistribute this program, but do not
** sell it. Include this notice in any copy.
**
** Author: Jim Frankenberger 1993
**         Laboratory of Atomic and Solid State Physics
**         Cornell University
**
*/

#include <stdio.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>

#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/MainW.h>
#include <Xm/Separator.h>
#include <Xm/BulletinB.h>
#include "show.h"

/*
** sphere_depth_adj()
**
** Try to make adjustments in the pixmap to account
** for 3D 
**
*/
#ifndef _NO_PROTO
void sphere_depth_adj(OBJECT *object,int width,int height)
#else
void sphere_depth_adj(object,width,height)
OBJECT *object;
int width;
int height;
#endif
{
   double range,depth_fac,offset;
   Display *dpy;

   if (depth_cues) {
      range = zmax - zmin;
      offset = zmax - object->z1;
      depth_fac = offset/range;
      dpy = XtDisplay(toplevel);
      if (object->use_this == NULL) {
	 if (depth_fac < 0.5)
	   return;
	 else {
	   XSetFillStyle(dpy,object->pixmap_gc,FillOpaqueStippled);
	   XCopyArea(dpy,object->save_pixmap,object->pixmap,object->pixmap_gc,0,0,width,height,
		     0,0);
	   XSetFillStyle(dpy,object->pixmap_gc,FillSolid);
         }
      } else {
	 if (depth_fac < 0.5)
	   return;
	 else {
	   XSetFillStyle(dpy,object->pixmap_gc,FillOpaqueStippled);
	   XCopyArea(dpy,object->use_this->save_pixmap,object->use_this->pixmap,
                 object->use_this->pixmap_gc,0,0,width,height,
		     0,0);
	   XSetFillStyle(dpy,object->pixmap_gc,FillSolid);
         }
      }

   } else
      return;
}



