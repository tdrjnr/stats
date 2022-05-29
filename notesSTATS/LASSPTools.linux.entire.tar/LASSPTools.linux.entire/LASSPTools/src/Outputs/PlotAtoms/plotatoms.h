#include<stdio.h>
#include<string.h>
#include<math.h>

#ifndef SUN
#include<stdlib.h>
#endif

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xresource.h>

#include"macros.h"

#define MAXLINELENGTH 100          /* Input line length */
#define ALLWHITE      1            /* for compatibility with whitespace.c */
#define NEWFRAME      ALLWHITE     /* Switch to new frame */
#define FACTOR        .25          /* Internal "border" width */
#define SET           1
#define UNSET         0
#define COLOR         8            /* 8 planes */
#define MONO          1            /* 1 plane */
#define NCOLORS       256          /* color only */
#define PI            3.14159265

#define PAEntire       1L<<0
#define PAPosDefaults  1L<<1
#define PARadius       1L<<2
#define PAScale        1L<<3
#define PAXMinSet      1L<<4
#define PAXMaxSet      1L<<5
#define PAYMinSet      1L<<6
#define PAYMaxSet      1L<<7
#define PAShade        1L<<8

typedef struct {
  int width;
  int height;
  int xpos;
  int ypos;
} Geometry;

typedef struct {
  double xmin;
  double xmax;
  double ymin;
  double ymax;
} Limits;

typedef struct {
  int radius;
  char *color;
  float aspect_ratio;
  int angle;
} Sphere;                /* Atom is a defined X data type */

/* options */
typedef struct {
  Geometry geometry;
  Limits limits;
  int natoms;
  Sphere default_atom;
  Sphere *atoms;
  double border;
  char *foreground;
  char *background;
} Options;

/* function declarations */

#ifndef _NO_PROTO
int whitespace(char xypositions[]);
void shutdown(Display *display, Window window1);
void option_error(char option[]);
XrmDatabase parse(int *argc, char **argv);
void set_options(XrmDatabase *options_db, unsigned long *option_mask, 
		 Options *options);
void create_atoms(Display *display, int screen, int depth, Options *options,
		   unsigned long option_mask, unsigned long *foreground,
		   unsigned long *background);
void create_atoms_bw(Display *display, int screen, int depth, Options *options,
		   unsigned long option_mask, unsigned long *foreground,
		   unsigned long *background);
void debug_options(Options *options);
#else
int whitespace();
void shutdown();
void option_error();
XrmDatabase parse();
void set_options();
void create_atoms();
void creat_atoms_bw();
void debug_options();
#endif

/* external variables */
int carg;
char **varg;
char name[30];
Pixmap *atom_shapes;
Pixmap *atom_masks;

/* plotatoms.h */
/* last modified 3-29-91 */







