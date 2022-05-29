#include "xy_plot.h"

#ifndef _NO_PROTO
   void init_color(WindowStruct *window, Widget top)
#else
   void init_color(window, top)
      WindowStruct *window;
      Widget top;
#endif
{
  Display *dpy = XtDisplay(top);
  XColor exact, shadow;
  ColorPaletteStruct *color;
  int ncolor;
  Colormap color_def;
  XColor Colors[NCOLORS];
  int i,n;
  Arg arg[1];
  XGCValues values;
  int num;

#ifdef DEBUG
  XSynchronize(dpy, 1);
#endif

  color = (ColorPaletteStruct *) &(window->color);
  ncolor = DisplayCells(dpy, DefaultScreen(dpy));

  if (ncolor > NCOLORS + 129) 
    ncolor = NCOLORS - 129;
  else
    ncolor = ncolor - 129;

  color_def = DefaultColormap(dpy, DefaultScreen(dpy));
  for (i = 0; i< ncolor; i++) {
    Colors[i].pixel = i;
    Colors[i].flags = DoRed | DoGreen | DoBlue;
  }
  XQueryColors(dpy, color_def, Colors, ncolor);

  XtSetArg(arg[0], XmNbackground, &values.background);
  XtGetValues(top, arg, 1);
    
  color->background.pixel = values.background;

  XQueryColor(dpy, color_def, &(color->background));

/****** Create shadow for spherical coordinate ***********/

  window->shadow_pixel_number = 10;
  
  window->shadow_pixel_array = (int *) malloc (window->shadow_pixel_number * sizeof(int));

  shadow.red = 0;
  shadow.blue = 0;
  shadow.flags = DoRed | DoGreen | DoBlue;

  for (n=0; n < window->shadow_pixel_number; n++) {
    shadow.green = 65535 - 5000 * n;
    XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &shadow);
    window->shadow_pixel_array[n] = shadow.pixel;
  }

/*
  for (n=0; n <window->shadow_pixel_number; n++)  {
    shadow.green = 65000 - 3500 * n;
    shadow.pixel = window->shadow_pixel_array[n];
    XStoreColor(dpy, color->cmap, &shadow);
  }
*/
  
  
  
  XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), 
	      &(color->background));
  XtSetArg(arg[0], XmNbackground, values.background);
  XtSetValues(top, arg, 1);

  XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), 
		   "brown", &(color->axis), &exact);
  XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), 
		   "dark green", &(color->grid), &exact);
  XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), 
		   "black", &(color->point), &exact);


}


#ifndef _NO_PROTO
   void color_change(Widget w, ImageDataStruct *graph, XColor color)
#else
   void color_change(w, graph, color)
      Widget w;
      ImageDataStruct *graph;
      XColor color;
#endif
{
  XSetForeground(XtDisplay(w), graph->gc, color.pixel);
}























