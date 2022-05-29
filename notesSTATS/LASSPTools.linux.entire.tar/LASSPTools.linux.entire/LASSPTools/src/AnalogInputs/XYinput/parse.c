#include "xy_plot.h"

#define DOUBLE   1
#define PROC     2
#define OPTIONS  3
#define FUNCTION 4


typedef union parseunion {
  int flag;
  void *pointer;
#ifndef _NO_PROTO
  void (*function1)(XyiAllDataStruct *);
  void (*function2)(Widget, ImageDataStruct *, int, int, int);
#else
  void (*function)();
#endif
} ParseUnion;

typedef struct parseSct {
  char *option;
  int type;
  ParseUnion *pointer;
  int flag;
} ParseStruct;


#ifndef _NO_PROTO
   void parse_option(XyiAllDataStruct *all, unsigned int argc, char **argv)
#else
   void parse_option(all, argc, argv)
      XyiAllDataStruct *all;
      unsigned int argc;
      char **argv;
#endif
{
  XyiRecStruct *data = (XyiRecStruct *) &(all->data);
  XyiPolarStruct *polar = (XyiPolarStruct *) &(all->data);
  XyiSphericalStruct *sphere = (XyiSphericalStruct *) &(all->data);
  static   int angle = 0;
  static   int display = 0;

/* static was included to compile on sun - this is twisted, the foolowing
   was changed - added static to angle and display, added code below to
   initialize the structure below.
*/
static   ParseStruct options[21] = {
    {"-xmin",DOUBLE,(ParseUnion *)0,0},
    {"-xmax",DOUBLE,(ParseUnion *)0,0},
    {"-ymin",DOUBLE,(ParseUnion *)0,0},
    {"-ymax",DOUBLE,(ParseUnion *)0,0},
    {"-rmax", DOUBLE,(ParseUnion *)0, 0},
    {"-rec", PROC, (ParseUnion *)default_rec, 0},
    {"-polar",PROC, (ParseUnion *)default_polar,0},
    {"-sphere", PROC, (ParseUnion *)default_spherical, 0},
    {"-deg",OPTIONS, (ParseUnion *)&angle, XyiDegree},
    {"-rad",OPTIONS, (ParseUnion *)&angle, XyiRadian},
    {"-radtimepi", OPTIONS, (ParseUnion *)&angle, XyiRadTimePi},
    {"-axis", OPTIONS, (ParseUnion *)&display, XyiAxis},
    {"-grid", OPTIONS, (ParseUnion *)&display, XyiGrid},
    {"-tick", OPTIONS, (ParseUnion *)&display, XyiTick},
    {"-plain", OPTIONS, (ParseUnion *)&display, XyiNoAxis},
    {"-point", FUNCTION, (ParseUnion *)draw_one_point, 0},
    {"-square", FUNCTION, (ParseUnion *)draw_square, 0},
    {"-circle", FUNCTION, (ParseUnion *)draw_circle, 0},
    {"-cross", FUNCTION, (ParseUnion *)draw_cross, 0},
    {"-nodisp", FUNCTION, (ParseUnion *)DPdummy, 0},
    {"-help", PROC, (ParseUnion *)initial_help, 0}
  };

  char **arg;
  int i,j;
#ifdef _NO_PROTO
     void (*proc_call) ();
#else 
  void (*proc_call)(XyiAllDataStruct *);
#endif

  extern WindowStruct windows;
  ImageDataStruct *graph = windows.image;

  argc --;
  arg = argv;
  arg ++;
  
  options[0].pointer = (ParseUnion *)&(data->x_min);
  options[1].pointer = (ParseUnion *)&(data->x_max);
  options[2].pointer = (ParseUnion *)&(data->y_min);
  options[3].pointer = (ParseUnion *)&(data->y_max);
  options[4].pointer = (ParseUnion *)&(polar->max);

  while (argc)  {
    i = 0;
    while (i < XtNumber(options)) {
      if (!strcmp(arg[0], options[i].option)) {
	switch (options[i].type) {
          case DOUBLE:
	     arg ++;
	     argc --;
             sscanf(arg[0],"%f",(double *) options[i].pointer);
             break;
          case PROC:
             proc_call = (void (*)) options[i].pointer;
             (*proc_call)(all);
	     break;
          case OPTIONS:
             *((int *) options[i].pointer) = options[i].flag;
	     break;
          case FUNCTION:
             graph->draw_point = (void (*)) options[i].pointer;
	     break;
	   }
	i = XtNumber(options);
      }
      else i++;
    }
    arg ++;
    argc --;
  }

  if (angle) {
    if (all->coordinate = XyiPolar) 
      polar->deg_rad = angle;
    if (all->coordinate = XyiSpherical)
      sphere->deg_rad = angle;
  }
  if (display) {
    if (all->coordinate = XyiRect)
      axistick_rec(display, graph);
    if (all->coordinate = XyiPolar)
      axistick_polar(display, graph);
    if (all->coordinate = XyiSpherical)
      axistick_spherical(display, graph);
   
  }
}








