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
*/ 
#include <stdio.h>
#include <Xm/Xm.h>
#include <math.h>
#include <X11/StringDefs.h>

#include "show.h"

/*
** show_mouse_pos()
**
** This function is called when the the left mouse button is clicked
** and the sprite is in the drawing area canvas. If the sprite is
** over a sphere its cordinates are displayed in the top window.
**
**
*/
#ifndef _NO_PROTO
void show_mouse_pos(Widget w, Widget client, XEvent *event)
#else
void show_mouse_pos(w,client,event)
Widget w;
Widget client;
XEvent *event;
#endif
{
     OBJECT *object;
     int i;
     char str[20];

     object = object_head.next;

      /* Look through all the objects and find the one(s) that are
      ** spehres and are also in the same area as the sprite
      */
      for (i=0;i<object_count;i++) {
          if (object->type == SPHERE) {
             if ((event->xbutton.x >= (object->x1r-object->radius_scaled)) &&
                 (event->xbutton.x <= (object->x1r+object->radius_scaled)) &&
                 (event->xbutton.y >= (object->y1r-object->radius_scaled)) &&
                 (event->xbutton.y <= (object->y1r+object->radius_scaled))) {
                 sprintf(str,"%f(%f)",object->x1,object->x1_orig);
                 XmTextSetString(x_text,str);
                 sprintf(str,"%f(%f)",object->y1,object->y1_orig);
                 XmTextSetString(y_text,str);
                 sprintf(str,"%f(%f)",object->z1,object->z1_orig);
                 XmTextSetString(z_text,str);
		 XmTextSetString(name_text,object->name);
		 update_atom_window(w,object);
	      }
	   }
           object = object->next;
       }
}

/*
** redraw_canvas()
**
** When the drawing area is exposed this function will cause the 
** objects to be redrawn by copying the off-screen image to the
** display.
**
*/
#ifndef _NO_PROTO
void redraw_canvas(Widget w, Widget client, XExposeEvent *event)
#else
void redraw_canvas(w,client,event)
Widget w;
Widget client;
XExposeEvent *event;
#endif
{
    
    XCopyArea(XtDisplay(canvas),canvas_pix,XtWindow(canvas),canvas_gc,event->x,event->y,
	      event->width,event->height, event->x,event->y);
    XFlush(XtDisplay(canvas));

}

/*
** resize_canvas()
**
** When the toplevel window is resized this function is called to handle resizing
** the drawing area.
**
**
*/
#ifndef _NO_PROTO
void resize_canvas(Widget w, Widget client, XEvent *event)
#else
void resize_canvas(w,client,event)
Widget w;
Widget client;
XEvent *event;
#endif
{
    XConfigureEvent *event_config;
    float ratio,range;
    int new_canvas_height,new_canvas_width;

    if (event->type == ConfigureNotify) {
       event_config = (XConfigureEvent *)event;

       /* Get the size of the new window */
       new_canvas_width = event_config->width;
       new_canvas_height = event_config->height;


       /* Free the old pixmap */
       XFreePixmap(XtDisplay(canvas),canvas_pix);

       /* Create a new pix map */
       canvas_pix = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)),
			new_canvas_width,new_canvas_height, DefaultDepthOfScreen(XtScreen(canvas)));

       XFillRectangle(XtDisplay(canvas),canvas_pix,canvas_gc,0,0,canvas_width,canvas_height);

       
       /* Figure out much the window was enlarged or shrunk and adjust
       ** the limits accordingly keeping the drawing centered.
       */
       if (new_canvas_height != new_canvas_width) {
          ratio = (float)new_canvas_height / (float)canvas_height;
	  range = ((ymax - ymin) * ratio) - (ymax - ymin);
	  ymin = ymin - (range/2);
	  ymax = ymax + (range/2);
	  ratio = (float)new_canvas_width / (float)canvas_width;
	  range = ((xmax - xmin) * ratio) - (xmax - xmin);
	  xmin = xmin - (range/2);
	  xmax = xmax + (range/2);
	  
       }

       canvas_width = new_canvas_width;
       canvas_height = new_canvas_height;

       /* Free all the old objects (they will have changed size) */
       do_down_scale(w,5,NULL);

    } 
  }













