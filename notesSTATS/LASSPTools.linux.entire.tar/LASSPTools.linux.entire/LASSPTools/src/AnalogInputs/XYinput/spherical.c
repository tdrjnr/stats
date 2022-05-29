#include "xy_plot.h"

#ifndef _NO_PROTO
   void track_position_spherical(Widget w, XyiAllDataStruct *all  , XEvent *event)
#else
   void track_position_spherical(w ,all, event)
      Widget w;
      XyiAllDataStruct *all;
      XEvent *event;
#endif
{
  char tmp1[20], tmp2[20];
  XmString x_message, y_message;
  Arg wargs[2];
  double x,y,r;

  XyiSphericalStruct *data;
  data = (XyiSphericalStruct *) &(all->data);

  data->x_pixel = event->xmotion.x;
  data->y_pixel = event->xmotion.y;

  x = data->scale * (double) (event->xmotion.x - data->x_zero);
  y = data->scale * (double) (data->y_zero - event->xmotion.y);

  r = sqrt(x*x + y*y);
  
  if ( r < 1) 
    data->phi = asin(sqrt(x*x + y*y));
  else
    data->phi = PI / 2;
  data->theta = atan2(y,x);
  
  if(data->deg_rad == XyiDegree) {
    data->phi = data->phi * 180 / PI;
    data->theta = data->theta * 180 / PI;
  }
  else if (data->deg_rad == XyiRadTimePi) {
    data->phi = data->phi / PI;
    data->theta = data->theta / PI;
  }

  sprintf(tmp1, "t=%04f", data->theta);
  x_message = XmStringCreate(tmp1, XmSTRING_DEFAULT_CHARSET);
  sprintf(tmp2, "p=%04f", data->phi);
  y_message = XmStringCreate(tmp2, XmSTRING_DEFAULT_CHARSET);

  XtSetArg(wargs[0], XmNlabelString, x_message);
  XtSetValues(all->tmp1, wargs, 1);
  XtSetArg(wargs[0], XmNlabelString, y_message);
  XtSetValues(all->tmp2, wargs, 1);
}


#ifndef _NO_PROTO
   void send_data_spherical(Widget w, XyiDataStruct *data0, XEvent *event)
#else
   void send_data_spherical(w, data0, event)
      Widget w;
      XyiDataStruct *data0;
      XEvent *event;
#endif
{
  XyiSphericalStruct *data;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  data = (XyiSphericalStruct *) data0;
  graph = windows.image;

  fprintf(stdout,"%f\t%f\n", data->theta, data->phi);
  fflush(stdout);

  (*(graph->draw_point))(windows.cartesian, graph, 
                         data->x_pixel, data->y_pixel, graph->size);

}


#ifndef _NO_PROTO
   void resize_spherical(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void resize_spherical(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  Dimension x,y;
  int i,n;
  Arg arg[10];
  XyiSphericalStruct *data;
  double small;
  extern WindowStruct windows;
  ImageDataStruct *graph;
  int narc,radius;
  double step,one;
  Display *dpy;
  

  graph = (ImageDataStruct *) windows.image;
  data = (XyiSphericalStruct *) client_data;


  n=0;
  /* Changed 8-18-92 jwf
  **  - Changed x,y to type dimension for X11R4
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

  data->scale = (double) 1 / small;

  init_graphics(windows.cartesian, graph);

/******  Draw the shadow *******/

  narc = windows.shadow_pixel_number;
  step = PI/18;
  dpy = XtDisplay(w);
  one = 1/data->scale;

  if (graph->pix != NULL) {
    for(i = 9; i>= 0; i--)  {
      XSetForeground(dpy, graph->gc, windows.shadow_pixel_array[i]);
      radius = (int)(one * sin(i*step));
      XFillArc(dpy, graph->pix, graph->gc,
	       data->x_zero - radius, data->y_zero - radius,
	       radius * 2, radius * 2,
	       0, 64*360);
      if (XtIsRealized(w))
	XFillArc(dpy, XtWindow(w), graph->gc,
		 data->x_zero - radius, data->y_zero - radius,
		 radius * 2, radius * 2,
		 0, 64*360);
    };

    (*(graph->draw_axis))(windows.cartesian,graph);
    (*(graph->draw_grid))(windows.cartesian,graph);
  }
}


#ifndef _NO_PROTO
   void default_spherical(XyiAllDataStruct *all)
#else
   void default_spherical(all)
      XyiAllDataStruct *all;
#endif
{
  XyiSphericalStruct *sphere;
  extern WindowStruct windows;
  ImageDataStruct *graph;

  sphere = (XyiSphericalStruct *) &(all->data);
  graph = (ImageDataStruct *) windows.image;

  all->track_position = track_position_spherical;
  all->send_data = send_data_spherical;
  all->resize = resize_spherical;
  all->coordinate = XyiSpherical;

  sphere->deg_rad = XyiDegree;

  sphere->phi = 0;
  sphere->theta = 0;

  if (graph->draw_point == NULL)
    graph->draw_point = draw_one_point;
  graph->draw_axis = draw_axis_spher;
  graph->draw_grid = draw_tick_spher;

}

#ifndef _NO_PROTO
   void default_spherical_in_radian(XyiAllDataStruct *all)
#else
   void default_spherical_in_radian(all)
      XyiAllDataStruct *all;
#endif
{
  XyiSphericalStruct *sphere;
  default_spherical(all);
  sphere = (XyiSphericalStruct *) &(all->data);

  sphere->deg_rad = XyiRadian;
}
