#include "xy_plot.h"

#ifndef _NO_PROTO
   void draw_axis_rec(Widget w, ImageDataStruct *data)
#else
   void draw_axis_rec(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  double x, y;
  double x_tick, y_tick;
  int x_pixel, y_pixel;
  extern WindowStruct windows;
  XyiRecStruct *rect;
  
  rect = (XyiRecStruct *) &(windows.all->data);
  
  axis_tickinc_calculate(rect->x_min, rect->x_max, &x, &x_tick);
  axis_tickinc_calculate(rect->y_min, rect->y_max, &y, &y_tick);
  
  x_pixel = (int)((x - rect->x_min)/rect->x_scale);
  y_pixel = (int)((y - rect->y_max)/rect->y_scale);
  
  draw_axis_private(w, data, x_pixel, y_pixel, 0);
  
}


#ifndef _NO_PROTO
   void draw_axis_polar(Widget w, ImageDataStruct *data)
#else
   void draw_axis_polar(w,data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  extern WindowStruct windows;
  XyiPolarStruct *polar;
  
  polar = (XyiPolarStruct *) &(windows.all->data);
  
  draw_axis_private(w, data, polar->x_zero, polar->y_zero, 0);

}


#ifndef _NO_PROTO
   void draw_axis_spher(Widget w, ImageDataStruct *data)
#else
   void draw_axis_spher(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  extern WindowStruct windows;
  XyiSphericalStruct *sphere;
  
  sphere = (XyiSphericalStruct *) &(windows.all->data);
  
  draw_axis_private(w,data, sphere->x_zero, sphere->y_zero, 0);
}


#ifndef _NO_PROTO
   void axis_tickinc_calculate(double min, double max, double *axis, double *tick)
#else 
   void axis_tickinc_calculate(min, max, axis, tick)
      double min, max, *axis, *tick;
#endif
{
  double absmax, absmin, sign;
  int logmax, logmin;

  *tick = (double) pow(10, floor(log10(max - min)));

  if ((max*min) <= 0) 
    *axis = 0;
  else {
    if (max > 0) {
      absmax = max;
      absmin = min;
      sign = 1;
    }
    else {
      absmax = - min;
      absmin = - max;
      sign = -1;
    }
    logmax = (int) floor(log10(absmax));
    logmin = (int) floor(log10(absmin));
    if (logmin != logmax) 
      *axis = (double) pow(10, floor(log10(absmax))) * sign;
    else
      *axis = ceil(absmin/(*tick)) * (*tick) * sign;
  }
  if (((absmax - absmin)/(*tick)) <= 4) 
    if (((absmax - absmin)/(*tick)) <= 2)
      *tick = (double) (*tick / 5);
    else
      *tick = (double) (*tick / 2);			 
  
}


#ifndef _NO_PROTO
   void axistick(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void axistick(w,client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
 
  extern WindowStruct windows;
  XyiAllDataStruct *all;
  ImageDataStruct *graph;
  XyiRecStruct *rect;
  XyiPolarStruct *polar;
  XyiSphericalStruct *sphere;
  int selection;
  
  selection = (int) client_data;
  all = windows.all;
  graph = windows.image;

  switch (all->coordinate) {
  case XyiRect:
    axistick_rec(selection, graph);
    break;
  case XyiPolar:
    axistick_polar(selection, graph);
    break;
  case XyiSpherical:
    axistick_spherical(selection, graph);
    break;    
  }
  (*(all->resize))(windows.cartesian, &(all->data), NULL);
}


#ifndef _NO_PROTO
   void axistick_rec(int selection, ImageDataStruct *graph)
#else
   void axistick_rec(selection, graph)
      int selection;
      ImageDataStruct *graph;
#endif
{
  switch (selection) {
    case XyiAxis:
      graph->draw_axis = draw_axis_rec;
      graph->draw_grid = DGdummy;
      break;
    case XyiTick:
      graph->draw_axis = draw_axis_rec;
      graph->draw_grid = draw_tick_rec;
      break;
    case XyiGrid:
      graph->draw_axis = draw_axis_rec;
      graph->draw_grid = draw_grid_rec;
      break;
    case XyiNoAxis:
      graph->draw_axis = DAdummy;
      graph->draw_grid = DGdummy;
      break;
    }
}


#ifndef _NO_PROTO
   void axistick_polar (int selection, ImageDataStruct *graph)
#else
   void axistick_polar(selection, graph)
      int selection;
      ImageDataStruct *graph;
#endif
{
  switch (selection) {
    case XyiAxis:
      graph->draw_axis = draw_axis_polar;
      graph->draw_grid = DGdummy;
      break;
    case XyiTick:
      graph->draw_axis = draw_axis_polar;
      graph->draw_grid = draw_tick_polar;
      break;
    case XyiGrid:
      graph->draw_axis = draw_axis_polar;
      graph->draw_grid = draw_grid_polar;
      break;
    case XyiNoAxis:
      graph->draw_axis = DAdummy;
      graph->draw_grid = DGdummy;
      break;
    }
}


#ifndef _NO_PROTO
   void axistick_spherical(int selection, ImageDataStruct *graph)
#else
   void axistick_spherical(selection, graph)
      int selection;
      ImageDataStruct *graph;
#endif
{
  switch (selection) {
    case XyiAxis:
      graph->draw_axis = draw_axis_spher;
      graph->draw_grid = DGdummy;
      break;
    case XyiTick:
      graph->draw_axis = draw_axis_spher;
      graph->draw_grid = draw_tick_spher;
      break;
    case XyiGrid:
      graph->draw_axis = draw_axis_spher;
      graph->draw_grid = draw_grid_spher;
      break;
    case XyiNoAxis:
      graph->draw_axis = DAdummy;
      graph->draw_grid = DGdummy;
      break;
    }
}


#ifndef _NO_PROTO
   void draw_axis_private (Widget w, ImageDataStruct *data, int x, int y, int unused)
#else
   void draw_axis_private(w,data,x,y,unused)
      Widget w;
      ImageDataStruct *data;
      int x, y, unused;
#endif
{
  extern WindowStruct windows;

  /*change_color(w, "brown", data);*/
  color_change(w, data, windows.color.axis);

  XDrawLine(XtDisplay(w), data->pix, data->gc, 5, y, data->width - 5, y);
  XDrawLine(XtDisplay(w), data->pix, data->gc, x, 5, x, data->height - 5);
  if (XtIsRealized(w))  {
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc, 5, y, data->width - 5, y);
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc, x, 5, x, data->height - 5);
  }
  color_change(w, data, windows.color.point);
}


#ifndef _NO_PROTO
   void draw_tick_rec(Widget w, ImageDataStruct *data)
#else
   void draw_tick_rec(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  double x,y;
  double x0,y0;
  int x_axis, y_axis;
  double x_inc, y_inc;
  double x_tick, y_tick;
  int x_pixel;
  extern WindowStruct windows;
  XyiRecStruct *rect;

  rect = (XyiRecStruct *) &(windows.all->data);

  /*  change_color(w, "dark green", data);  */
  color_change(w, data, windows.color.axis);

  axis_tickinc_calculate(rect->x_min, rect->x_max, &x0, &x_tick);
  axis_tickinc_calculate(rect->y_min, rect->y_max, &y0, &y_tick);

  x_axis = (int)((x0-rect->x_min)/rect->x_scale);
  y_axis = (int)((y0-rect->y_max)/rect->y_scale);

  x_inc = x_tick/rect->x_scale;
  y_inc = -y_tick/rect->y_scale;

  x = (double) x_axis + x_inc;
  while ((int) x < data->width-5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
	       (int) x, y_axis - 3, (int) x, y_axis + 3);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
		 (int) x, y_axis - 3, (int) x, y_axis +3);
    x = x + x_inc;
  }

  x = (double) x_axis - x_inc;
  while ((int) x > 5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
               (int) x, y_axis - 3, (int) x, y_axis + 3);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
                 (int) x, y_axis - 3, (int) x, y_axis + 3);
    x = x - x_inc;
  }
  
  y = (double) y_axis + y_inc;
  while ((int) y < (data->height - 5)) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
	       x_axis - 3, (int) y, x_axis + 3, (int) y);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
		 x_axis - 3, (int) y, x_axis + 3, (int) y);
    y = y + y_inc;
  }

  y = (double) y_axis - y_inc;
  while (y > 5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
	       x_axis - 3, (int) y, x_axis + 3, (int) y);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
		 x_axis - 3, (int) y, x_axis + 3, (int) y);
    y = y - y_inc;
  }
  color_change(w, data, windows.color.point);
}


#ifndef _NO_PROTO
   void draw_tick_polar(Widget w, ImageDataStruct *data)
#else
   void draw_tick_polar(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  extern WindowStruct windows;
  XyiPolarStruct *polar;
  double bound;
  double inc;
  double r;
  double unused;
  
  polar = (XyiPolarStruct *) &(windows.all->data);
  axis_tickinc_calculate((double)0, (double)polar->max, &unused, &inc);

  bound = 0.5 + 0.5/sqrt((double)2);
  inc = inc/polar->scale;

  color_change (w, data, windows.color.grid);

  r = inc;
  while (r*polar->scale < polar->max/bound) {
    XDrawArc(XtDisplay(w), data->pix, data->gc, 
	     polar->x_zero - (int) r, polar->y_zero - (int) r,
	     (unsigned int) r * 2,(unsigned int) r * 2,
	     0, 64*360);
    if(XtIsRealized(w)) 
      XDrawArc(XtDisplay(w), XtWindow(w), data->gc,
	       polar->x_zero - (int) r, polar->y_zero - (int) r,
	       (unsigned int) r * 2, (unsigned int) r * 2,
	       0, 64*360);
    r = r + inc;
  }
  color_change(w, data, windows.color.point);
}


#ifndef _NO_PROTO
   void draw_tick_spher(Widget w, ImageDataStruct *data)
#else
   void draw_tick_spher(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  double one;
  int radius,i;
  extern WindowStruct windows;
  XyiSphericalStruct *sphere;
  char color[10];
  int ncolors = XDisplayCells(XtDisplay(w), XDefaultScreen(XtDisplay(w)));
  
  sphere = (XyiSphericalStruct *) &(windows.all->data);

  one = 1/sphere->scale;

  color_change(w, data, windows.color.grid);
  for (i = 1; i <= 9; i++) {
    radius = (int)(one * sin(i * PI/18));
    sprintf(color, "gray%d", (100-5*i));
    /* change_color(w, color, data); */
    XDrawArc(XtDisplay(w), data->pix, data->gc,
	     sphere->x_zero - radius, sphere->y_zero - radius,
	     radius * 2, radius * 2, 
	     0, 64*360);
    if (XtIsRealized(w))
      XDrawArc(XtDisplay(w), XtWindow(w), data->gc,
	       sphere->x_zero - radius, sphere->y_zero - radius,
	       radius * 2, radius * 2,
	       0, 64*360);
  }
  color_change(w, data, windows.color.point);
}



#ifndef _NO_PROTO
   void draw_grid_rec(Widget w, ImageDataStruct *data)
#else
   void draw_grid_rec(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  double x,y;
  double x0,y0;
  int x_axis, y_axis;
  double x_inc, y_inc;
  double x_tick, y_tick;
  int x_pixel;
  extern WindowStruct windows;
  XyiRecStruct *rect;

  rect = (XyiRecStruct *) &(windows.all->data);

  axis_tickinc_calculate(rect->x_min, rect->x_max, &x0, &x_tick);
  axis_tickinc_calculate(rect->y_min, rect->y_max, &y0, &y_tick);

  x_axis = (int)((x0-rect->x_min)/rect->x_scale);
  y_axis = (int)((y0-rect->y_max)/rect->y_scale);

  x_inc = x_tick/rect->x_scale;
  y_inc = -y_tick/rect->y_scale;

  /*  change_color(w, "sea green", data); */
  color_change(w, data, windows.color.grid);

  x = (double) x_axis + x_inc;
  while ((int) x < data->width-5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
               (int) x, data->height - 5, (int) x, 5);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
                 (int) x, data->height - 5, (int) x, 5);
    x = x + x_inc;
  }

  x = (double) x_axis - x_inc;
  while ((int) x > 5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
               (int) x, data->height - 5, (int) x, 5);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
                 (int) x, data->height - 5, (int) x, 5);
    x = x - x_inc;
  }
  
  y = (double) y_axis + y_inc;
  while ((int) y < (data->height - 5)) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
               5, (int) y, data->width - 5, (int) y);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
                 5, (int) y, data->width - 5, (int) y);
    y = y + y_inc;
  }

  y = (double) y_axis - y_inc;
  while (y > 5) {
    XDrawLine(XtDisplay(w), data->pix, data->gc,
               5, (int) y, data->width - 5, (int) y);
    if (XtIsRealized(w))
      XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
                 5, (int) y, data->width - 5, (int) y);
    y = y - y_inc;
  }
  color_change(w, data, windows.color.point);
}


#ifndef _NO_PROTO
   void draw_grid_polar(Widget w, ImageDataStruct *data)
#else
   void draw_grid_polar(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  extern WindowStruct windows;
  XyiPolarStruct *polar;
  double bound;
  double inc;
  double r;
  double unused;
  int gridlength;
  
  polar = (XyiPolarStruct *) &(windows.all->data);
  axis_tickinc_calculate(0, polar->max, &unused, &inc);

  bound = 0.5 + 0.5/sqrt(2);
  inc = inc/polar->scale;
  
/*  change_color(w, "dark green", data);*/
  color_change(w, data, windows.color.grid);

  r = inc;
  while (r*polar->scale < polar->max/bound) {
    XDrawArc(XtDisplay(w), data->pix, data->gc, 
             polar->x_zero - (int) r, polar->y_zero - (int) r,
             (unsigned int) r * 2,(unsigned int) r * 2,
             0, 64*360);
    if(XtIsRealized(w)) 
      XDrawArc(XtDisplay(w), XtWindow(w), data->gc,
               polar->x_zero - (int) r, polar->y_zero - (int) r,
               (unsigned int) r * 2, (unsigned int) r * 2,
               0, 64*360);
    r = r + inc;
  }

/*  change_color(w, "sea green", data);*/
  
  gridlength = sin(PI/4) * (double)((polar->x_zero + polar->y_zero) / 2);
  XDrawLine(XtDisplay(w), data->pix, data->gc,
	    polar->x_zero - gridlength, polar->y_zero - gridlength,
	    polar->x_zero + gridlength, polar->y_zero + gridlength);
  XDrawLine(XtDisplay(w), data->pix, data->gc,
	    polar->x_zero - gridlength, polar->y_zero + gridlength,
	    polar->x_zero + gridlength, polar->y_zero - gridlength);
  if(XtIsRealized(w)) {
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
	      polar->x_zero - gridlength, polar->y_zero - gridlength,
	      polar->x_zero + gridlength, polar->y_zero + gridlength);
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
	      polar->x_zero - gridlength, polar->y_zero + gridlength,
	      polar->x_zero + gridlength, polar->y_zero - gridlength);
  }
  color_change(w, data, windows.color.point);
}


#ifndef _NO_PROTO
   void draw_grid_spher(Widget w, ImageDataStruct *data)
#else
   void draw_grid_spher(w, data)
      Widget w;
      ImageDataStruct *data;
#endif
{
  double one;
  int radius,i;
  extern WindowStruct windows;
  XyiSphericalStruct *sphere;
  int gridlength;
  char color[10];

  sphere = (XyiSphericalStruct *) &(windows.all->data);

  one = 1/sphere->scale;

  color_change(w, data, windows.color.grid);
  for (i = 1; i <= 9; i++) {
    sprintf(color, "gray%d", (100-5*i));
/*    change_color(w, color, data);*/
    radius = (int)(one * sin(i * PI/18));
    XDrawArc(XtDisplay(w), data->pix, data->gc,
             sphere->x_zero - radius, sphere->y_zero - radius,
             radius * 2, radius * 2, 
             0, 64*360);
    if (XtIsRealized(w))
      XDrawArc(XtDisplay(w), XtWindow(w), data->gc,
               sphere->x_zero - radius, sphere->y_zero - radius,
               radius * 2, radius * 2,
               0, 64*360);
  }
  
/*  change_color(w, "sea green", data);*/

  gridlength = sin(PI/4) * (double)((sphere->x_zero + sphere->y_zero) / 2);
  XDrawLine(XtDisplay(w), data->pix, data->gc,
            sphere->x_zero - gridlength, sphere->y_zero - gridlength,
            sphere->x_zero + gridlength, sphere->y_zero + gridlength);
  XDrawLine(XtDisplay(w), data->pix, data->gc,
            sphere->x_zero - gridlength, sphere->y_zero + gridlength,
            sphere->x_zero + gridlength, sphere->y_zero - gridlength);
  if(XtIsRealized(w)) {
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
              sphere->x_zero - gridlength, sphere->y_zero - gridlength,
              sphere->x_zero + gridlength, sphere->y_zero + gridlength);
    XDrawLine(XtDisplay(w), XtWindow(w), data->gc,
              sphere->x_zero - gridlength, sphere->y_zero + gridlength,
              sphere->x_zero + gridlength, sphere->y_zero - gridlength);
  }
  color_change(w, data, windows.color.point);
}


