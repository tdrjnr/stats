/* xy_plot.h - header file for xy_plot.c */

#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <stdio.h>
#ifndef _NO_PROTO
   #include <stdlib.h>
#endif
#include <math.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>

#define PI 3.1415926535            /* definition of PI */

#define XyiRect         1          /* Constant for Coordinate Systems */
#define XyiPolar        2
#define XyiSpherical    3

#define XyiDegree       1          /* Constant for Angle Representation */
#define XyiRadian       2
#define XyiRadTimePi    3
 
#define XyiAxis         1          /* Constant for Display Configuration */
#define XyiTick         2
#define XyiGrid         3
#define XyiNoAxis       4

#define NCOLORS         256        /* Maximum number of colors supported */


typedef struct _XyiRecStruct {     /* Data Struct for Rectangular Coordinate */
  int type;               /* unused */
  int x_pixel, y_pixel;   /* location of pixels on the screen */
  double x_min,x_max;     /* max and min bounds on x-axis */
  double y_min,y_max;     /* max and min bounds on y-axis */
  double x_scale,y_scale; /* conversion scale from pixel scale to real value */
  double x_value,y_value; /* real value */
} XyiRecStruct;

typedef struct _XyiPolarStruct {   /* Data Struct for Polar Coordinate */
  int type;               /* unused */
  int x_pixel, y_pixel;   /* location of pixels on the screen */
  double r,theta;         /* values of display */
  double max;             /* max bound in radius */
  double scale;           /* conversion scale from pixel scale to real value */
  int x_zero,y_zero;      /* location of (0,0) in pixel scale */
  int deg_rad;            /* method of representation of angles */
} XyiPolarStruct;

typedef struct _XyiSphericalStruct { /* Data Struct for Spherical Coordinate */
  int type;               /* unused */
  int x_pixel, y_pixel;   /* location of pixels on the screen */
  double theta, phi;      /* values of display */
  double scale;           /* conversion scale from pixel scale ot real value */
  int x_zero, y_zero;     /* location of (0,0) in pixel scale */
  int deg_rad;            /* method of representation of angles */
} XyiSphericalStruct;

typedef union _XyiDataStruct {  /* a union data Struct containing all coordinates */
  int type;
  XyiRecStruct xy_rec;
  XyiPolarStruct xy_polar;
  XyiSphericalStruct xy_sphere;
} XyiDataStruct;

typedef struct _XyiAllDataStruct XyiAllDataStruct;
typedef struct _WindowStruct WindowStruct;
typedef struct _ImageDataStruct ImageDataStruct;

struct _XyiAllDataStruct {     /* Structure containing critical information */
  XyiDataStruct data;  
  Widget tmp1,tmp2;
  int coordinate;
  WindowStruct *window;
#ifndef _NO_PROTO
     void (*track_position)(Widget, XyiAllDataStruct *, XEvent *);
     void (*send_data)(Widget, XyiDataStruct *, XEvent *);
     void (*resize)(Widget,caddr_t,caddr_t);
#else
     void (*track_position)();
     void (*send_data)();
     void (*resize)();
#endif
};

typedef struct _MenuStruct MenuStruct;
struct _MenuStruct {                 /* For use in the pop-up menus */
  char *name;
#ifndef _NO_PROTO
     void (*func)(Widget, caddr_t, caddr_t);
#else
     void (*func)();
#endif
  caddr_t data;
  MenuStruct *submenu;
  int submenu_entries;
  char *submenu_title;
};

typedef struct _ImageDataStruct {    /* For use in drawing window */
  GC gc;
  Pixmap pix;
  int width, height;
  int size;
  char flag;
#ifndef _NO_PROTO
     void (*draw_point)(Widget w, ImageDataStruct *, int, int, int);
     void (*draw_axis) (Widget w, ImageDataStruct *);
     void (*draw_grid) (Widget w, ImageDataStruct *);
#else
     void (*draw_point)();
     void (*draw_axis)();
     void (*draw_grid)();
#endif
} image_data;

typedef struct _ColorPaletteStruct { /* Structure containing the colors */
  XColor point;
  XColor axis;
  XColor grid;
  XColor background;
  Colormap cmap;
} ColorPaletteStruct;

struct _WindowStruct {            /* A Structure for global variable */
  Widget cartesian;
  Widget quit;
  XyiAllDataStruct *all;
  ImageDataStruct *image;
  ColorPaletteStruct color; 
  int *shadow_pixel_array, shadow_pixel_number;
};

typedef struct _RangeChangeStruct {  /* A Structure for Range Change Routines */
  char *label;
  double *data;
  double *greater_than;
} RangeChangeStruct;

typedef struct _RangeChangeWindowStruct { /* Structure for Range Change Display*/ 
  int number;
  Widget board0;
  Widget *heading;
  Widget *text;
  Widget *row;
  RangeChangeStruct *change;
} RangeChangeWindowStruct;



#ifndef _NO_PROTO

/* callback and intialize windows */

void initialize_primary_window(Widget, XyiAllDataStruct *);
void quit_xy(Widget w, caddr_t, caddr_t);
void select_output_PR(Widget,caddr_t,XButtonEvent *);
void select_output_motion(Widget, caddr_t, XMotionEvent *);

/* parsing command line options */

void parse_option(XyiAllDataStruct *,unsigned int,char **);

/* Rectanglar Coordinates */

void track_position_rec(Widget, XyiAllDataStruct *, XEvent *);
void send_data_rec(Widget, XyiDataStruct *, XEvent *);
void resize_rec(Widget, caddr_t, caddr_t call_data);
void default_rec(XyiAllDataStruct *);


/* Polar Coordinates */

void track_position_polar(Widget, XyiAllDataStruct *, XEvent *);
void send_data_polar(Widget, XyiDataStruct *, XEvent *);
void resize_polar(Widget, caddr_t, caddr_t call_data);
void default_polar(XyiAllDataStruct *);
void default_polar_in_radian(XyiAllDataStruct *);


/* Spherical Coordinates */ 

void track_position_spherical(Widget, XyiAllDataStruct *, XEvent *);
void send_data_spherical(Widget, XyiDataStruct *, XEvent *);
void resize_spherical(Widget, caddr_t, caddr_t call_data);
void default_spherical(XyiAllDataStruct *);
void default_spherical_in_radian(XyiAllDataStruct *);


/* Help routines */

void initial_help(XyiAllDataStruct *);
void help(Widget, caddr_t, caddr_t);
void display_message(Widget, char **);
XmString xs_str_array_to_xmstr(char **, int);
void quit_dialog(Widget, caddr_t, caddr_t);

/* Pull Down Options */
void pull_down_option(Widget, Widget, caddr_t);

/* configuration file */
void callback_config(WindowStruct *);
void callback_deconfig(WindowStruct *);
void reconfig (Widget, caddr_t, caddr_t);

/* Range Change */
void range_change(Widget, caddr_t, caddr_t);
void range_change_private(Widget, RangeChangeStruct *, int);
void quit_range_change_private(Widget, caddr_t, caddr_t);
void confirm_changes(Widget, caddr_t, caddr_t);


/* axis_grid */
void draw_axis_rec(Widget, ImageDataStruct *);
void draw_axis_polar(Widget, ImageDataStruct *);
void draw_axis_spher(Widget, ImageDataStruct *);
void draw_tick_rec(Widget, ImageDataStruct *);
void draw_tick_polar(Widget,ImageDataStruct *);
void draw_tick_spher(Widget, ImageDataStruct *);
void draw_grid_rec(Widget, ImageDataStruct *);
void draw_grid_polar(Widget, ImageDataStruct *);
void draw_grid_spher(Widget, ImageDataStruct *);
void axis_tickinc_calculate(double, double, double *, double *);
void axistick(Widget, caddr_t, caddr_t);
void axistick_rec(int, ImageDataStruct *);
void axistick_polar(int, ImageDataStruct *);
void axistick_spherical(int, ImageDataStruct *);
void draw_axis_private(Widget, ImageDataStruct *, int, int, int);


/* graphics */
void DPdummy (Widget , ImageDataStruct *, int, int, int);
void DAdummy (Widget , ImageDataStruct *);
void DGdummy (Widget , ImageDataStruct *);
void init_graphics (Widget, ImageDataStruct *);
void default_graphics (Widget, ImageDataStruct *);
void draw_one_point (Widget, ImageDataStruct *, int, int, int);
void draw_square(Widget, ImageDataStruct *, int, int, int);
void draw_circle(Widget, ImageDataStruct *, int, int, int);
void draw_cross(Widget, ImageDataStruct *, int, int, int);
void draw_axis(Widget w, ImageDataStruct *, int, int, int);
void change_point(Widget w, caddr_t, caddr_t);
void redisplay(Widget, caddr_t, caddr_t);
unsigned long change_color (Widget, char *, ImageDataStruct *);

/* colors */
void init_color(WindowStruct *, Widget);
void color_change(Widget, ImageDataStruct *, XColor);

#else 

void initialize_primary_window();
void quit_xy();
void select_output_PR();
void select_output_motion();

void parse_option();

void track_position_rec();
void send_data_rec();
void resize_rec();
void default_rec();

void track_position_polar();
void send_data_polar();
void resize_polar();
void default_polar();
void default_polar_in_radian();

void track_position_spherical();
void send_data_spherical();
void resize_spherical();
void default_spherical();
void default_spherical_in_radian();

void initial_help();
void help();
void display_message();
XmString xs_str_array_to_xmstr();
void quit_dialog();

void pull_down_option();

void callback_config();
void callback_deconfig();
void reconfig ();

void range_change();
void range_change_private();
void quit_range_change_private();
void confirm_changes();

void draw_axis_rec();
void draw_axis_polar();
void draw_axis_spher();
void draw_tick_rec();
void draw_tick_polar();
void draw_tick_spher();
void draw_grid_rec();
void draw_grid_polar();
void draw_grid_spher();
void axis_tickinc_calculate();
void axistick();
void axistick_rec();
void axistick_polar();
void axistick_spherical();
void draw_axis_private();

void DPdummy ();
void DAdummy ();
void DGdummy ();
void init_graphics ();
void default_graphics ();
void draw_one_point ();
void draw_square();
void draw_circle();
void draw_cross();
void draw_axis();
void change_point();
void redisplay();
unsigned long change_color();

void init_color();
void color_change();

#endif


/* External Variables */
/*
WindowStruct windows;
*/







