#include "xy_plot.h"

#ifndef _NO_PROTO
   void init_graphics(Widget w, ImageDataStruct *graph)
#else
   void init_graphics(w, graph)
      Widget w;
      ImageDataStruct *graph;
#endif
{
  extern WindowStruct windows;
  GC gc;
  int n;
  Arg warg[2];
  XGCValues values;

  w = windows.cartesian;

  if (graph->pix) 
    XFreePixmap(XtDisplay(w), graph->pix); 

  if(!XtIsRealized(w)) 
    return;
  
  XClearWindow(XtDisplay(w),XtWindow(w));

#ifdef DEBUG
  fprintf(stderr,"Creating Pixmap.\nHeight:%d\nWidth:%d\n\n",
	  graph->height, graph->width);
  fflush(stderr);
#endif

  graph->pix = XCreatePixmap(XtDisplay(w),RootWindowOfScreen(XtScreen(w)), 
			     graph->width, graph->height, 
			     DefaultDepthOfScreen(XtScreen(w)));

  n=0;
  XtSetArg(warg[n], XmNforeground, &values.foreground); n++;
  XtSetArg(warg[n], XmNbackground, &values.background); n++;
  XtGetValues (w, warg, n);

  values.foreground = values.background;

  gc = XtGetGC(w, GCForeground | GCBackground, &values);

  XFillRectangle(XtDisplay(w), graph->pix, gc, 0, 0, graph->width, graph->height);

  (*(graph->draw_axis))(w,graph);
  (*(graph->draw_grid))(w,graph);
}


#ifndef _NO_PROTO
   void default_graphics (Widget w, ImageDataStruct *graph)
#else
   void default_graphics (w, graph)
      Widget w;
      ImageDataStruct *graph;
#endif
{
  int n;
  Dimension x,y;
  Arg warg[5];
  extern WindowStruct windows;
  XGCValues gcv;

  graph->gc = XtGetGC(w,NULL, NULL);
  gcv.line_style = LineSolid;
  gcv.line_width = 0;
  XChangeGC(XtDisplay(w), graph->gc, GCLineStyle | GCLineWidth, &gcv);

  graph->pix = NULL;
  graph->size = 10;
  
 
  n = 0;
 
  /* Changed 8-18-92 jwf
  **   - Made x,y of type Dimension to make code work with X11R4
  */
  XtSetArg(warg[n], XmNwidth, &x); n++;
  XtSetArg(warg[n], XmNheight, &y); n++;
  XtGetValues(w, warg, n);
  graph->flag = 0;
  
  graph->width = x;
  graph->height = y;

  XtAddCallback(w, XmNexposeCallback, redisplay, graph);
}


/*#ifndef _NO_PROTO
   unsigned long change_color(Widget w, char *colorname, ImageDataStruct *graph)
#else
   unsigned long change_color(w, colorname,graph)
      Widget w;
      char *colorname;
      ImageDataStruct *graph;
#endif
{
  Display *dpy = XtDisplay(w);
  Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
  XColor  color, exact;

  if (XAllocNamedColor(dpy, cmap, colorname, &color, &exact))  
    XSetForeground(XtDisplay(w), graph->gc, color.pixel);
  return (color.pixel);
}
*/
  

#ifndef _NO_PROTO
   void draw_one_point(Widget w, ImageDataStruct *data, int x, int y, int unused)
#else
   void draw_one_point(w, data, x, y, unused)
      Widget w;
      ImageDataStruct *data;
      int x,y,unused;
#endif
{
  /* Added 4 pixel wide filled circles to replace dots - 6-8-93 jwf */
 XFillArc(XtDisplay(w), data->pix, data->gc, x, y, 4, 4, 0, 64*360);
 if (XtIsRealized(w))
   XFillArc(XtDisplay(w), XtWindow(w), data->gc, x, y, 4, 4, 0, 64*360);
}


#ifndef _NO_PROTO
   void draw_square (Widget w, ImageDataStruct *data, int x, int y, int size)
#else
   void draw_square (w, data, x, y, size)
      Widget w;
      ImageDataStruct *data;
      int x, y, size;
#endif
{
  XDrawRectangle(XtDisplay(w), data->pix, data->gc, 
		 x - size/2, y-size/2, size, size);
  if (XtIsRealized(w))
    XDrawRectangle(XtDisplay(w), XtWindow(w), data->gc,
		   x - size/2, y - size/2, size, size);
}


#ifndef _NO_PROTO
   void draw_circle (Widget w, ImageDataStruct *data, int x, int y, int size)
#else
   void draw_circle (w, data, x, y, size)
      Widget w;
      ImageDataStruct *data;
      int x, y, size;
#endif
{
  XDrawArc(XtDisplay(w), data->pix, data->gc, x- size/2 ,y - size/2, size, size, 0, 64*360);
  if (XtIsRealized(w))
    XDrawArc(XtDisplay(w), XtWindow(w), data->gc, x- size/2, y- size/2, size, size, 0, 64*360);
}


#ifndef _NO_PROTO
   void draw_cross (Widget w, ImageDataStruct *data, int x, int y, int size)
#else
   void draw_cross (w, data, x, y, size)
      Widget w;
      ImageDataStruct *data;
      int x, y, size;
#endif
{
  XDrawLine(XtDisplay(w), data->pix, data->gc, x-size/2, y+size/2, x+size/2, y-size/2);
  XDrawLine(XtDisplay(w), data->pix, data->gc, x-size/2, y-size/2, x+size/2, y+size/2);
  if (XtIsRealized(w)) {
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc, x-size/2, y+size/2, x+size/2, y-size/2);
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc, x-size/2, y-size/2, x+size/2, y+size/2);
  }
}


#ifndef _NO_PROTO
   void DPdummy (Widget w, ImageDataStruct *graph, int x, int y, int size)
#else
   void DPdummy(w, graph, x, y, size)
      Widget w;
      ImageDataStruct *graph;
      int x, y, size;
#endif
{
  return;
}


#ifndef _NO_PROTO
   void DAdummy (Widget w, ImageDataStruct *graph)
#else
   void DAdummy (w, graph)
      Widget w;
      ImageDataStruct *graph;
#endif
{
  return;
}


#ifndef _NO_PROTO
   void DGdummy (Widget w, ImageDataStruct *graph)
#else
   void DGdummy (w, graph)
      Widget w;
      ImageDataStruct *graph;
#endif
{
  return;
}


#ifndef _NO_PROTO
   void change_point (Widget w, caddr_t client_data, caddr_t call_data)
#else
   void change_point (w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  extern WindowStruct windows;
  ImageDataStruct *graph = (ImageDataStruct *) windows.image;

  if (graph->draw_point == DPdummy) 
    graph->flag = graph->flag & 6;
  else
    graph->flag = graph->flag | 1;

  graph->draw_point = (void (*)) client_data;
 
}


#ifndef _NO_PROTO
   void redisplay(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void redisplay(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  ImageDataStruct *data;
  XExposeEvent *event;
  XmDrawingAreaCallbackStruct *draw;

  data = (ImageDataStruct *) client_data;
  draw = (XmDrawingAreaCallbackStruct *) call_data;
  event = (XExposeEvent *) draw->event;

  XCopyArea(XtDisplay(w), data->pix, XtWindow(w), data->gc, 
	    event->x, event->y, event->width, event->height, 
	    event->x,event->y);

}
