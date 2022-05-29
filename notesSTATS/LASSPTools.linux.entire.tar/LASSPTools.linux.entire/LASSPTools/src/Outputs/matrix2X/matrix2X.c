
/* Matrix2X: a program for
 *
 *
 *
 *
 */


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "math_io.h"

#define MAX_PIXEL_STRENGTH 65535

int noclear = 0;
int noclick = 0;
int verbose = 0;
int binarymode = 0;
int same_size_rectangles = 0;

Display *display;
int      screen;
int      depth;
Colormap colormap;

double mat_min;
double mat_ratio;

char *help = "\nmatrix2X displays a rectangular matrix on an X terminal.\n\
Options are:\t-geometry\t ..\tset geometry\n\
\t\t-c\t ..\tset color via name or hex\n\
\t\t-rgb\t ..\tset color via next 3 integers\n\
\t\t-sc\t ..\tset seed color via name or hex\n\
\t\t-srgb\t ..\tset seed color via next 3 integers\n\
\t\t-scmin\t ..\tset color range min via name or hex\n\
\t\t-scmax\t ..\tset color range max via name or hex\n\
\t\t-srgbmin ..\tset color range min via next 3 integers\n\
\t\t-srgbmax ..\tset color range max via next 3 integers\n\
\t\t-palette .. \tset a palette of colors via hex\n\
\t\t-scc\t ..\tset number of colors in last range via next integer\n\
\t\t-srgbc\t ..\tidentical to -scc\n\
\t\t-noclear ..\tinhibits window clearing between matrices\n\
\t\t-noclick ..\tinhibits pause for click between matrices\n\
\t\t-b\t ..\tinput matrix in binary mode for speed\n\
\t\t-replicate ..\tmake all elements the same size blocks\n\
\t\t-h\t ..\tgive this message\n\
\t\t-help\t ..\tgive this message\n\
\t\t-?\t ..\tgive this message\n\
\t\t?\t ..\tgive this message\n\n";


/*---------------------------------------------------
**
** find_dimension()
** 
**   This function finds the width or height of a rectangle to draw on the
**   window. It maps an element of a matrix into a region in the window,
**   each time a new element is displayed a new width is calculated, this 
**   allows the matrix to occupy the whole whatever window. Each time
**   a new row is displayed a new height is calculated for all the 
**   rectangles that will be drawn in that row. 
**
**   This function is invoked from display_matrix and is the default when
**   running the program. The -replicate option will disable this function 
**   and draw all the rectangles the same size, this will probably leave 
**   uncolored areas around the window edge if the window size is not a
**   multiple of the matrix size.
**
**   Input:
**      win_size - the number of pixels in the windows width (or height)
**      curr_pos - the starting pixel for this rectangles dimension
**      elements - the number of rows or columns in the matrix
**      curr_element - the element number the rectangle is to represent
**      start - the beginning index (ROWSTART or COLSTART)
**
**   Output:
**      The width or height of a rectangle to be drawn
**
**
**--------------------------------------------------- 
*/
int
#ifndef _NO_PROTO
find_dimension(int win_size, int curr_pos, int elements, int curr_element,
            int start)
#else
find_dimension(win_size, curr_pos, elements, curr_element, start)
int win_size;
int curr_pos;
int elements;
int curr_element;
int start;
#endif
{
     float size;

     int dimension;

     /* Check if we need to do any work */
     if (win_size > elements) {
        
        /* Calculate the ideal size for each rectangle */
        size = (float)((float)win_size / (float)elements);
        if (curr_element == start) {

           /* It is the first element, round down */
           dimension = (int)size;
           
           /* Check if it is the last, an array with 1 row or col */
           if (curr_element == elements)
              /* Make it the window size, and use any extra space */
              dimension = win_size;

        } else {
           /* Check if it is the last row or col */
           if (curr_element == elements)
              /* Set the domension to whatever is left over */
              dimension = win_size - curr_pos;
           else
              /* It is somewhere in the middle, figure out the best size for
              ** this element
              */
              dimension  = (curr_element+1)*size - curr_pos;
	 }
     } else
        /* Data will be chopped off, 1 is the smallest dimension */
        dimension = 1;

     return(dimension);
     
   
}


/*-----------------------------------------------------------------------------
 * fill_colors expects to see the min color in colors[ncolors]
 */

#ifndef _NO_PROTO
int fill_colors(int ncolors, int nfill, XColor *colors, XColor max_color)
#else
int fill_colors(ncolors, nfill, colors, max_color)
int ncolors;
int nfill;
XColor *colors;
XColor max_color;
#endif
{
  int i, skipped;
  XColor min_color;
  double red_del, green_del, blue_del;

  if(nfill < 1) return(0);
  
  if(nfill == 1) {
    colors[ncolors] = max_color;
    if(XAllocColor(display, colormap, (colors + ncolors)))
      return(1);
    else
      return(0);
  }
  
  min_color = colors[ncolors];
  red_del   = (double)max_color.red   - (double)min_color.red;
  green_del = (double)max_color.green - (double)min_color.green;
  blue_del  = (double)max_color.blue  - (double)min_color.blue;
  red_del   = red_del   / (double)(nfill - 1);
  green_del = green_del / (double)(nfill - 1);
  blue_del  = blue_del  / (double)(nfill - 1);
  skipped = 0;
  for(i=0; i < nfill; i++) {
    colors[ncolors].red = min_color.red + (int)(red_del * (double)i);
    colors[ncolors].green = min_color.green + (int)(green_del * (double)i);
    colors[ncolors].blue = min_color.blue + (int)(blue_del * (double)i);
    colors[ncolors].flags = DoRed | DoGreen | DoBlue;
    
    /* Make sure we could allocate the next color and *
     * that it is not the same as the previous color. */
    if(XAllocColor(display, colormap, (colors + ncolors)) && 
       (ncolors == 0 || colors[ncolors].pixel != colors[ncolors -1].pixel))
      ncolors++;
    else
      skipped++;
  }
  return(i - skipped);
}

/*-----------------------------------------------------------------------------
 *
 */
#ifndef _NO_PROTO
int valid_color(XColor color)
#else
int valid_color(color)
XColor color;
#endif
{
  if((color.red   > MAX_PIXEL_STRENGTH) ||
     (color.green > MAX_PIXEL_STRENGTH) ||
     (color.blue > MAX_PIXEL_STRENGTH))
    
    return(0);
  else
    return(1);
}

/*-----------------------------------------------------------------------------
 *
 */
#ifndef _NO_PROTO
int parse_options(int argc, char **argv, XColor *colors, int max_colors,
		  int *x, int *y, int *width, int *height)
#else
int parse_options(argc, argv, colors, max_colors,x,y,width,height)
int argc;
char **argv;
XColor *colors;
int max_colors;
int *x, *y, *width, *height;
#endif
{
  int i, j, k;
  int num_fill = 0;
  int num_colors = 0;
  char *dummy_string;
  XColor xcolor, maxcolor, mincolor, scolor;

/*
      else
      fprintf(stderr,"Matrix2X : Warning too many colors; some skipped\n");
*/

  for(i=1; i < argc && num_colors < max_colors; i++) {
    if(strcmp(argv[i], "-c") == 0) {
      if(XParseColor(display,colormap,argv[++i],&(colors[num_colors]))
	 && XAllocColor(display, colormap, &(colors[num_colors])))
	num_colors++;
      else
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
    }
    else if(strcmp(argv[i], "-rgb") == 0) {
      colors[num_colors].red = atoi(argv[++i]);
      colors[num_colors].green = atoi(argv[++i]);
      colors[num_colors].blue = atoi(argv[++i]);
      colors[num_colors].flags = DoRed | DoGreen | DoBlue;
      if(valid_color(colors[num_colors]) &&
	 XAllocColor(display, colormap, &(colors[num_colors])))
	num_colors++;
      else
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
    }
    else if(strcmp(argv[i], "-sc") == 0) {
      if(!XParseColor(display, colormap, argv[++i], &scolor))
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
      else {
	/*  j = index of color count in argv  */
	j = parse_next_argument(argc,argv,i,&dummy_string,"-scc");
	/*  k = index of color count in argv  */
	k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbc");

	if(k > 0 && k < j) /* find the next color count */
	  num_fill = atoi(argv[k]);
      	else if(j > 0)
	  num_fill = atoi(argv[j]);
	else
	  num_fill = max_colors - num_colors;
	if(num_fill >= max_colors - num_colors)
	  num_fill = max_colors - num_colors;

	if(num_fill > 0) {
	  colors[num_colors].red = 0;
	  colors[num_colors].green = 0;
	  colors[num_colors].blue = 0;
	  num_colors += fill_colors(num_colors, num_fill, colors, scolor);
	}
	else
	  fprintf(stderr,"Matrix2X : Warning, colors skipped\n");
      }
    }
    else if(strcmp(argv[i], "-srgb") == 0) {
      scolor.red = atoi(argv[++i]);
      scolor.green = atoi(argv[++i]);
      scolor.blue = atoi(argv[++i]);
      if(!valid_color(scolor))
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
      else {
	j = parse_next_argument(argc,argv,i,&dummy_string,"-scc");
	k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbc");
	
	if(k > 0 && k < j)
	  num_fill = atoi(argv[k]);
	else if(j > 0)
	  num_fill = atoi(argv[j]);
	else
	  num_fill = max_colors - num_colors;
	if(num_fill >= max_colors - num_colors)
	  num_fill = max_colors - num_colors;

	if(num_fill > 0) {
	  colors[num_colors].red = 0;
	  colors[num_colors].green = 0;
	  colors[num_colors].blue = 0;
	  num_colors += fill_colors(num_colors, num_fill, colors, scolor);
	}
	else
	  fprintf(stderr,"Matrix2X : Warning, colors skipped\n");
      }
    }
    else if(strcmp(argv[i], "-scmin") == 0) {
      if(!XParseColor(display, colormap, argv[++i], (colors + num_colors)))
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
      else {
	j = parse_next_argument(argc,argv,i,&dummy_string,"-scc");
	k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbc");
	if(k > 0 && k < j)
	  num_fill = atoi(argv[k]);
      	else if(j > 0)
	  num_fill = atoi(argv[j]);
	else
	  num_fill = max_colors - num_colors;
	if(num_fill >= max_colors - num_colors)
	  num_fill = max_colors - num_colors;

	k = parse_next_argument(argc,argv,i,&dummy_string,"-scmax");
	if(k > 0) {
	  if(XParseColor(display, colormap, argv[k], &scolor))
	    num_colors += fill_colors(num_colors, num_fill, colors, scolor);
	  else
	    fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		    argv[k]);
	}
	else {
	  k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbmax");
	  if(k > 0) {
	    scolor.red = atoi(argv[k]);
	    scolor.green = atoi(argv[++k]);
	    scolor.blue = atoi(argv[++k]);
	    if(valid_color(colors[num_colors]))
	      num_colors += fill_colors(num_colors, num_fill, colors,scolor);
	    else
	      fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		      argv[k]);
	  }
	  else {
	    scolor.red = MAX_PIXEL_STRENGTH;
	    scolor.green = MAX_PIXEL_STRENGTH;
	    scolor.blue = MAX_PIXEL_STRENGTH;
	    num_colors += fill_colors(num_colors, num_fill, colors,scolor);
	  }
	}
      }
    }
    else if(strcmp(argv[i], "-srgbmin") == 0) {
      colors[num_colors].red = atoi(argv[++i]);
      colors[num_colors].green = atoi(argv[++i]);
      colors[num_colors].blue = atoi(argv[++i]);
      if(!valid_color(colors[num_colors]))
	fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		argv[i]);
      else {
	j = parse_next_argument(argc,argv,i,&dummy_string,"-scc");
	k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbc");
	if(k > 0 && k < j)
	  num_fill = atoi(argv[k]);
      	else if(j > 0)
	  num_fill = atoi(argv[j]);
	else
	  num_fill = max_colors - num_colors;
	if(num_fill >= max_colors - num_colors)
	  num_fill = max_colors - num_colors;

	k = parse_next_argument(argc,argv,i,&dummy_string,"-srgbmax");
	if(k > 0) {
	  scolor.red = atoi(argv[k]);
	  scolor.green = atoi(argv[++k]);
	  scolor.blue = atoi(argv[++k]);
	  if(valid_color(colors[num_colors]))
	    num_colors += fill_colors(num_colors, num_fill, colors,scolor);
	  else
	    fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		    argv[k]);
	}
	else {
	  k = parse_next_argument(argc,argv,i,&dummy_string,"-scmax");
	  if(k > 0) {
	    if(XParseColor(display, colormap, argv[k], &scolor))
	      num_colors += fill_colors(num_colors, num_fill, colors, scolor);
	    else
	      fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		      argv[k]);
	  }
	  else {
	    scolor.red = MAX_PIXEL_STRENGTH;
	    scolor.green = MAX_PIXEL_STRENGTH;
	    scolor.blue = MAX_PIXEL_STRENGTH;
	    num_colors += fill_colors(num_colors, num_fill, colors,scolor);
	  }
	}
      }
    }
    else if(strcmp(argv[i], "-palette") == 0) {
      while(++i < argc && *(argv[i]) == '#' && num_colors < max_colors)
	if(XParseColor(display, colormap, argv[i], (colors + num_colors))
	   && XAllocColor(display, colormap, &(colors[num_colors])))
	  num_colors++;
	else
	  fprintf(stderr,"Matrix2X : Warning unable to allocate color %s\n",
		  argv[i]);
    }
    else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0 ||
	    strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "?") == 0)
      {
	fprintf(stdout,"%s",help);
	exit(0);
      }
    else if(strcmp(argv[i], "-noclear") == 0) {
      noclear = 1;
    }
    else if(strcmp(argv[i], "-noclick") == 0) {
      noclick = 1;
    }
    else if(strcmp(argv[i], "-b") == 0) {
      binarymode = 1;
    }
    else if(strcmp(argv[i], "-verbose") == 0) {
      verbose = 1;
    }
    else if(strcmp(argv[i], "-geometry") == 0) {
      XParseGeometry(argv[i+1], x, y, width, height);
    }
    else if (strcmp(argv[i], "-replicate") == 0) {
      same_size_rectangles = 1;      
    }

  }
  
  if(num_colors == 0) { /* try to allocate some colors */
    colors[num_colors].pixel = BlackPixel(display, screen);
    XQueryColor(display, colormap, colors);

    scolor.pixel = WhitePixel(display, screen);
    XQueryColor(display, colormap, &scolor);

    /*  j = index of color count in argv  */
    j = parse_next_argument(argc,argv,0,&dummy_string,"-scc");
    /*  k = index of color count in argv  */
    k = parse_next_argument(argc,argv,0,&dummy_string,"-srgbc");
    
    if(k > 0 && k < j) /* find the next color count */
      num_fill = atoi(argv[k]);
    else if(j > 0)
      num_fill = atoi(argv[j]);
    else
      num_fill = max_colors - num_colors;
    if(num_fill >= max_colors - num_colors)
      num_fill = max_colors - num_colors;
    
    num_colors += fill_colors(num_colors, num_fill, colors, scolor);
  }
  return(num_colors);  
}

/*-----------------------------------------------------------------------------
**
** display_matrix()
**
** This function displays the matrix as a series of rectangles, each 
** element in the matrix corresponds to a rectangle. Each rectangle is
** colored based on the value of the element. For example, if the matrix
** is to be displayed using 4 colors each element would be drawn as a rectangle
** in one of the 4 colors. The values would be grouped into 4 categories so
** that "low" values were one color, the next group another color etc.
**
** The -replicate option will make all the rectangles the same size.
** This function previously used the same width (or height) for each of
** the rectangles drawn. This would sometimes leave a blank area where
** the window was not a multiple of the the array size. For example, an
** array size of 100x100 in a 550x550 window would leave 50 pixels used
** around the right and bottom edges.
** The default is to interpolate,  filling in the
** entire window with the array, changing the size of the rectangles to
** compensate for a window that does not match the matrix size.
**
*/
#ifndef _NO_PROTO
void display_matrix(Matrix *mat, Window window, XColor *colors,
		    GC gc, int win_width, int win_height)
#else
void display_matrix(mat, window, colors, gc, win_width, win_height)
Matrix *mat;
Window window;
XColor *colors;
GC gc;
int win_width;
int win_height;
#endif
{
  int i, j, color_index;
  int x, y, width, height;

  XEvent dummy_event;

  if(!noclear)
    XClearWindow(display, window);
  
  y = 0;
  width = win_width / mat->cols;
  height = win_height / mat->rows;
  
  if(width < 1) {
    fprintf(stderr,
	    "matrix2X: Warning; window too narrow, data might be clipped.\n");
    width = 1;
  }
  if(height < 1) {
    fprintf(stderr,
	    "matrix2X: Warning; window too short, data might be clipped.\n");
    height = 1;
  }
  for(i = ROWSTART; i < mat->rows + ROWSTART; i++) {
    x = 0;
    
    if (!same_size_rectangles)  
       /* Find the height for each rectangle in this row */
       height = find_dimension(win_height,y,mat->rows,i,ROWSTART);

    for(j = COLSTART; j < mat->cols + COLSTART; j++) {
      
      if (!same_size_rectangles) 
         /* Find the width for each rectangle */
         width  = find_dimension(win_width,x,mat->cols,j,COLSTART);

      color_index = (int)(0.5 + (mat->el[i][j]-mat_min)*mat_ratio);
      XSetForeground(display, gc, colors[color_index].pixel);		     
      XFillRectangle(display, window, gc, x, y, width, height); 
      x += width;
    }
    y += height;
  }

  /* Now remove any expose events to avoid immediate redraw. */
  while(XCheckWindowEvent(display, window, ExposureMask, &dummy_event));
}

/*-----------------------------------------------------------------------------
 *
 */
#ifndef _NO_PROTO
int load_new_matrix(Matrix *mat, int num_colors)
#else
int load_new_matrix(mat, num_colors)
Matrix *mat;
int num_colors;
#endif
{
  int i, j, done;

  if(verbose) fprintf(stderr, "Loading Matrix....  ");
  if (binarymode)
    done = get_binary_matrix(mat, stdin);
  else
    done = get_matrix(mat, stdin);
  if(verbose){ 
      fprintf(stderr, "Matrix Loaded\n");
      fprintf(stderr, "\nRows=%d, Cols=%d\n",mat->rows, mat->cols);
    }
  if(done == EOF) return done; /* exit at the end of the file */

  /*  Need to find min and max values in matrix.  */
  
  mat_min = mat_ratio = mat->el[ROWSTART][COLSTART];
  
  for(i = ROWSTART; i < mat->rows + ROWSTART; i++)
    for(j = COLSTART; j < mat->cols + COLSTART; j++)
      if(mat->el[i][j] > mat_ratio)
	mat_ratio = mat->el[i][j];
      else if(mat->el[i][j] < mat_min)
	mat_min = mat->el[i][j];

  if(mat_ratio != mat_min) {
    mat_ratio = mat_ratio - mat_min;
    mat_ratio = (double)(--num_colors) / mat_ratio;
  }
  else mat_ratio = 0.0;
}

/*-----------------------------------------------------------------------------
 *
 */
#ifndef _NO_PROTO
int main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif
{
  XEvent  myevent;
  Window  window;
  XColor *color_array;
  int     number_of_colors;
  int     num_colors = 0;
  GC      gc;
  char   *display_name;
  char   *window_name;

  int done;
  int x, y, width, height, bd_width;
  XColor bg, bd;
  XSizeHints xsize_hints;
  

  Matrix *mat;

  mat = create_default_matrix();
  
  /* Parse Options 
   *   get standard X defaults
   */

  /* These hard coded defaults will be selectable via options in the future */
  
  display_name = NULL;
  if((display = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr,"%s: cannot connect to X server %s\n",
	    argv[0], XDisplayName(display_name));
    exit(1);
  }
  screen = DefaultScreen(display);
  depth = DefaultDepth(display, screen);
  colormap = DefaultColormap(display, screen);

  window_name = argv[0]; /* Which should be "matrix2X" */
  x = 200;
  y = 300;
  width = 350;
  height = 250;
  bd_width = 5;
  bg.pixel = BlackPixel(display, screen);
  bd.pixel = BlackPixel(display, screen);

  /* Allocate the colors.  The maximum number of colors available is
   * 2 to the n. 
   *   get color array
   */
  color_array = NULL;
  if(depth > 0) { 
    number_of_colors = 1;
    for(done = 0; done < depth; done++)
      number_of_colors *= 2;
    color_array = (XColor *)malloc((size_t)number_of_colors*sizeof(XColor));
  }
  if(color_array == NULL) {
    fprintf(stderr,"Matrix2X : Error allocating color array\n");
    exit(1);
  }

  num_colors = parse_options(argc, argv, color_array, number_of_colors,
			     &x,&y,&width,&height);
  if(verbose) fprintf(stderr, "Color allocation complete\n");

  if(num_colors == 0)
    fprintf(stderr,"ERROR:  No colors allocated!!\n");
  
 
  /* Create And Raise Graphics Window
   */
  
  window = XCreateSimpleWindow(display, RootWindow(display,screen), x, y,
			       width, height, bd_width, bd.pixel, bg.pixel);

  /* This makes sure that the window's colormap is the same as the colormap
   * that we allocate colors for when we parse the command line */
  XSetWindowColormap(display, window, colormap);
  

  /* Create Icon Pixmap
   */  
  
  

  /* Initialize XSizeHints Structure
   */  
  
  xsize_hints.flags = USPosition | USSize;
  xsize_hints.x = x;
  xsize_hints.y = y;
  xsize_hints.width = width;
  xsize_hints.height = height;
  

  /* Set Standard Properties For Window Manager
   */  
  XSetStandardProperties(display, window, window_name, window_name, NULL,
			 argv ,argc, &xsize_hints);



  /* Select Event Types
   */  
  XSelectInput(display, window, (ButtonPressMask | KeyPressMask |
				 ExposureMask | StructureNotifyMask));
  

  /* Create Graphics Context
   */  
  gc = XCreateGC(display, window, 0, 0);

  /* Display The Window
   */
  XMapWindow(display, window);
  XFlush(display);

  /* Read and Graph Matrices
   */

  
  if(noclick == 0) {
    for(done = load_new_matrix(mat, num_colors); done != EOF;
	XSync(display,False)) {
      XNextEvent(display, &myevent);
      switch(myevent.type) {
      case Expose:
	if (myevent.xexpose.count == 0)
	  display_matrix(mat, window, color_array, gc, width, height);
	break;
	
      case ButtonPress:
	if((done = load_new_matrix(mat, num_colors)) != EOF)
	  display_matrix(mat, window, color_array, gc, width, height);
	break;
	
      case KeyPress:
	done = EOF;
	break;
	
      case ConfigureNotify:
	x = myevent.xconfigure.x;
	y = myevent.xconfigure.y;
	width = myevent.xconfigure.width;
	height = myevent.xconfigure.height;
	bd_width = myevent.xconfigure.border_width;
	break;
	
      default:
	break;
      }
    }
  }
  else {
    while(load_new_matrix(mat, num_colors) != EOF && done != EOF) {
      display_matrix(mat, window, color_array, gc, width, height);
      XFlush(display);
      while(XEventsQueued(display, QueuedAlready) != 0 && done != EOF) {
	XNextEvent(display, &myevent);
	switch(myevent.type) { 
	case Expose:
	  if(myevent.xexpose.count == 0)
	    display_matrix(mat, window, color_array, gc, width, height);
	  break;
	  
	case ButtonPress:
	  done = EOF;
	  break;

	case KeyPress:
	  done = EOF;
	  break;
	  
	case ConfigureNotify:
	  x = myevent.xconfigure.x;
	  y = myevent.xconfigure.y;
	  width = myevent.xconfigure.width;
	  height = myevent.xconfigure.height;
	  bd_width = myevent.xconfigure.border_width;
	  break;
	  
	default:
	  break;
	}
      }
    }
  }
  
  /* Clean Up Before Exit
   */
  free_matrix(mat);
  XFreeGC(display, gc);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  exit(0);
}









