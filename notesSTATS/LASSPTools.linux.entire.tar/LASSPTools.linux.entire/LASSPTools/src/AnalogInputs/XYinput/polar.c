#include "xy_plot.h" 

#ifndef _NO_PROTO
   void track_position_polar(Widget w, XyiAllDataStruct  *all  , XEvent *event)
#else
   void track_position_polar(w, all, event)
      Widget w;
      XyiAllDataStruct *all;
      XEvent *event;
#endif
{
  char tmp1[20], tmp2[20];
  XmString x_message, y_message;
  Arg wargs[2];
  double x,y;

  XyiPolarStruct *data;
  data = (XyiPolarStruct *) &(all->data);

  data->x_pixel = event->xmotion.x;
  data->y_pixel = event->xmotion.y;

  x = data->scale * (double) (event->xmotion.x - data->x_zero);
  y = data->scale * (double) (data->y_zero - event->xmotion.y);

  data->r = sqrt(x*x + y*y);
  data->theta = atan2(y,x);
  
  if(data->deg_rad == XyiDegree)
    data->theta = data->theta * 180 / PI;
  else if (data->deg_rad == XyiRadTimePi)
    data->theta = data->theta / PI;

  sprintf(tmp1, "r= %04f", data->r);
  x_message = XmStringCreate(tmp1, XmSTRING_DEFAULT_CHARSET);
  sprintf(tmp2, "t= %04f", data->theta);
  y_message = XmStringCreate(tmp2, XmSTRING_DEFAULT_CHARSET);

  XtSetArg(wargs[0], XmNlabelString, x_message);
  XtSetValues(all->tmp1, wargs, 1);
  XtSetArg(wargs[0], XmNlabelString, y_message);
  XtSetValues(all->tmp2, wargs, 1);
}


#ifndef _NO_PROTO
   void send_data_polar(Widget w, XyiDataStruct *data0, XEvent *event)
#else
   void send_data_polar(w, data0, event)
      Widget w;
      XyiDataStruct *data0;
      XEvent *event;
#endif
{
  XyiPolarStruct *data;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  data = (XyiPolarStruct *) data0;
  graph = windows.image;

  fprintf(stdout,"%f\t%f\n", data->r, data->theta);
  fflush(stdout);

  (*(graph->draw_point))(windows.cartesian, windows.image, 
			 data->x_pixel, data->y_pixel, graph->size);
}


#ifndef _NO_PROTO
   void resize_polar(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void resize_polar(w,client_data,call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  Dimension x,y;
  int n;
  Arg arg[10];
  XyiPolarStruct *data;
  double small;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  graph = (ImageDataStruct *) windows.image;
  data = (XyiPolarStruct *) client_data;

  n=0;

  /* Changed 8-18-92 jwf
  **  - Changed x,y to type Dimension for X11R4
  */ 
  XtSetArg(arg[n],XmNwidth,&x); n++;
  XtSetArg(arg[n],XmNheight,&y); n++;
  XtGetValues(w,arg,n);

  graph->width = x;
  graph->height = y;
  
  data->x_zero = x/2;
  data->y_zero = y/2;

  if (x > y) 
    small = (double) y/2;
  else 
    small = (double) x/2;

  data->scale = data->max / small;

  init_graphics(windows.cartesian, windows.image);
}


#ifndef _NO_PROTO
   void default_polar(XyiAllDataStruct *all)
#else
   void default_polar(all)
      XyiAllDataStruct *all;
#endif
{
  XyiPolarStruct *polar;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  polar = (XyiPolarStruct *) &(all->data);
  graph = (ImageDataStruct *) windows.image;

  all->track_position = track_position_polar;
  all->send_data = send_data_polar;
  all->resize = resize_polar;
  all->coordinate = XyiPolar;

  polar->max = 1;
  polar->deg_rad = XyiDegree;

  polar->r = 0;
  polar->theta = 0;

  if (graph->draw_point == NULL)
    graph->draw_point = draw_one_point;
  graph->draw_axis = draw_axis_polar;
  graph->draw_grid = draw_tick_polar;
  


}


#ifndef _NO_PROTO
   void default_polar_in_radian(XyiAllDataStruct *all)
#else
   void default_polar_in_radian(all)
      XyiAllDataStruct *all;
#endif
{
  XyiPolarStruct *polar;
  default_polar(all);
  polar = (XyiPolarStruct *) &(all->data);

  polar->deg_rad = XyiRadian;
}



