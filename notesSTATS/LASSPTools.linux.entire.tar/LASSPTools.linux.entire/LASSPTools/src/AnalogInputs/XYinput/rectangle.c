#include "xy_plot.h"

#ifndef _NO_PROTO
   void track_position_rec(Widget w, XyiAllDataStruct  *all  , XEvent *event)
#else
   void track_position_rec(w,all,event)
      Widget w;
      XyiAllDataStruct *all;
      XEvent *event;
#endif
{
  char tmp1[20], tmp2[20];
  XmString x_message, y_message;
  Arg wargs[2];

  XyiRecStruct *data;
  data = (XyiRecStruct *) &(all->data);

  data->x_pixel = event->xmotion.x;
  data->y_pixel = event->xmotion.y;

  data->x_value = data->x_min + data->x_scale * (double) event->xmotion.x;
  data->y_value = data->y_max + data->y_scale * (double) event->xmotion.y;

  sprintf(tmp1, "x = %4f", data->x_value);
  x_message = XmStringCreate(tmp1, XmSTRING_DEFAULT_CHARSET);
  sprintf(tmp2, "y = %4f", data->y_value);
  y_message = XmStringCreate(tmp2, XmSTRING_DEFAULT_CHARSET);

  XtSetArg(wargs[0], XmNlabelString, x_message);
  XtSetValues(all->tmp1, wargs, 1);
  XtSetArg(wargs[0], XmNlabelString, y_message);
  XtSetValues(all->tmp2, wargs, 1);
}


#ifndef _NO_PROTO
   void send_data_rec(Widget w, XyiDataStruct *data0, XEvent *event)
#else
   void send_data_rec(w,data0,event)
      Widget w;
      XyiDataStruct *data0;
      XEvent *event;
#endif
{
  XyiRecStruct *data;
  ImageDataStruct *graph;
  extern WindowStruct windows;

  data = (XyiRecStruct *) data0;
  graph = windows.image;

  fprintf(stdout,"%f\t%f\n", data->x_value, data->y_value);
  fflush(stdout);
  (*(graph->draw_point))(windows.cartesian, graph, 
			 data->x_pixel, data->y_pixel, graph->size);

}


#ifndef _NO_PROTO
   void resize_rec(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void resize_rec(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  
  int n;
  Arg arg[10];
  XyiRecStruct *data;
  extern WindowStruct windows;
  ImageDataStruct *graph;
  Dimension x,y;

  graph = (ImageDataStruct *) windows.image;
  data = (XyiRecStruct *) client_data;

  n=0;
  
  
  /* Changed 8-18-92, jwf
  **   - Cleaned up 
  **   - Make x,y of type Dimension to match function prototype this
  **     was needed because X11R4 is different from R3.
  */
  XtSetArg(arg[n],XmNwidth, &x); n++;
  XtSetArg(arg[n],XmNheight,&y); n++;
  XtGetValues(w,arg,n);
  
  /* Set the values for the size of the graph */ 
  graph->width = x;
  graph->height = y;
  

#ifdef DEBUG
  fprintf(stderr,"In rectangle.c, x = %d, y = %d\n",x,y);
  fprintf(stderr,"width = %d, height = %d\n",(windows.image)->width,
	  (windows.image)->height);
  fflush(stderr);
#endif

  data->x_scale = (data->x_max - data->x_min) / (double) x;
  data->y_scale = (data->y_min - data->y_max) / (double) y;

  init_graphics(windows.cartesian, windows.image); 
}


#ifndef _NO_PROTO
   void default_rec(XyiAllDataStruct *all)
#else
   void default_rec(all)
      XyiAllDataStruct *all;
#endif
{
  XyiRecStruct *rect;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  rect = (XyiRecStruct *) &(all->data);
  graph = (ImageDataStruct *) windows.image;

  all->track_position = track_position_rec;
  all->send_data = send_data_rec;
  all->resize = resize_rec;
  all->coordinate = XyiRect;
  
  rect->x_min = -1;
  rect->x_max =  1;
  rect->y_min = -1;
  rect->y_max =  1;

  rect->x_value = 0;
  rect->y_value = 0;

  if (graph->draw_point == NULL)
    graph->draw_point = draw_one_point;
  graph->draw_axis = draw_axis_rec;
  graph->draw_grid = draw_tick_rec;
  
}
