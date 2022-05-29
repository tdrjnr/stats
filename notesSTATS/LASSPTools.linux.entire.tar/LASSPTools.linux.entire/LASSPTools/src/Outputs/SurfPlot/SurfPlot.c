/* The convention for axes labelling in this program is:  the vertical axis 
 * of the screen is the y-axis, the axis horizontal to the right is the 
 * x-axis, and the one coming out of the screen is the z-axis, which is the
 * way polydraw has them;  if the numbers seem strange, they are -- they need
 * to be to get the output in the right orientation (polydraw axes are mixed
 * up!).
 * (I got the orientations right by trial and error.  Hence, don't ask me
 *  why I loop over such and such a variable, etc.)
 * Written by David Chin, June 1991
 * 1.13 8/16/91 */
    
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "line_io.h"	/* which includes Xlib.h */
#include "math_io.h"

/* this takes care of the arguments in function declarations */
#ifndef _NO_PROTO
#define ARGS(a) a
#else
#define ARGS(a) ()
#endif

/* this takes care of the const keyword */
#ifndef _NO_PROTO
#define CONST const
#else
#define CONST 
#endif

/* this takes care of the void keyword */
#ifndef _NO_PROTO
#define VOID void
#else
#define VOID
#endif


double matrix_range ARGS((Matrix*));
double matrix_lo ARGS((Matrix*));
double matrix_hi ARGS((Matrix*));
VOID plot_ascii_axes ARGS((VOID));
VOID plot_binary_axes ARGS((VOID));
VOID help ARGS((VOID));


#ifndef _NO_PROTO
     main(int argc, char *argv[])
#else
     main(argc, argv)
         int argc;
         char *argv[];
#endif
{
    CONST double px_min = -1.0;		/* the minimum and maximum  */
    CONST double pz_min = -1.0;		/*			    */
    CONST double px_max = 1.0;		/* x, z allowed by polydraw */
    CONST double pz_max = 1.0;		/*			    */
    CONST double scale_factor = 1.5;	/* scale the size of the plot */
#define NUM_VERTEX 4     		/* number of vertices of a polygon */
    unsigned int binary_mode = False;	/* default for binary mode flag */
    unsigned int no_input_file = True;	/* default for input file flag 	*
					 * (no infile) 			*/
    unsigned int auto_zmax = True;	/* default for input zmax flag	*
					 * (auto zmax is default ) 	*/
    int i, j, k;			/* counters and such */
    Matrix y_mat;			/* matrix of y values */
    int num_rows, num_cols;		/* .rows, .cols respectively */
    int color_tile;			/* color variable for tile */
    FILE *y_fp;				/* pointer to file containing data */
    double *xyz_array;			/* array to contain the 3D data */
    double x, y, z;			/* x, y and z coordinates */ 
    double y_range, y_lo, y_hi;		/* range in y_values, minimum,
					 * and maximum */
    double x_array[NUM_VERTEX], 
           y_array[NUM_VERTEX], 
	   z_array[NUM_VERTEX];		/* x, y, z arrays for binary mode */ 
    XColor surf_color;			/* color of the plot */
    size_t size_of_int = sizeof(int);	/* self descriptive */
    size_t size_of_double = sizeof(double);	/* ditto */	
    
    
    /* if there is stuff other than program name on command line */
    if (argc > 1) {
	/* do command line parsing */
	for (i = 1; i < argc; ++i) {
	    /* want help? */
	    if (strcmp(argv[i], "-h") == 0 || 
		strcmp(argv[i], "-help") == 0) {
		help();
		exit(0);
	    }
	    else
	
	    /* input file? */
	    if (strcmp(argv[i], "-file_in") == 0 ||
		strcmp(argv[i], "-f") == 0) {
		no_input_file = False;
		if ((y_fp = fopen(argv[++i], "r")) == NULL) {
		    fprintf(stderr, "Error in opening file %s\n", argv[i]);
		    exit(1);
		}
		continue;
	    } 
	    else
	
	    /* user zmax? */
	    if (strcmp(argv[i], "-zmax") == 0 )	{
		auto_zmax = False;
		y_hi = atof(argv[++i]); 
		fprintf(stderr, "User-specified zmax: %f\n",y_hi);
		continue;
	    } 
	    else
	    
	    /* binary out? */
	    if (strcmp(argv[i], "-b") == 0 || 
		strcmp(argv[i], "-binary_out") == 0) {
		binary_mode = True;
		continue;
	    }
	    else {
		fprintf(stderr, "Unknown option:  %s\n", argv[i]);
		exit(1);
	    }
	}
    }

    /* initializations */    
    if (no_input_file) {  /* no input file specified */
	y_fp = NULL;
	get_matrix(&y_mat, stdin);  /* default for input is stdin */
    } else {  /* input file specified */
	get_matrix(&y_mat, y_fp);  
    }
    
    y_lo = matrix_lo(&y_mat);	/* smallest element in matrix */

    if(auto_zmax)  y_hi = matrix_hi(&y_mat); 
	    			/* largest element in the matrix */
    
    /* calculate the range of values in matrix */
    if ((y_range = y_hi - y_lo) == 0)
	y_range = 1;	/* default if range is 0 */
    num_rows = y_mat.rows;	/* number of rows in matrix */
    num_cols = y_mat.cols;	/* number of columns in matrix */
        
    /* alloc space for array containing 3D data, then fill it */
    xyz_array = (double *)calloc(3*num_rows*num_cols, sizeof(double));
    for (i = 0; i < num_rows; ++i) {		/* loop over z */
	for (j = 0; j < num_cols; ++j) {	/* loop over x */
	    y = y_mat.el[i][j]/(scale_factor*y_range);
	    x = (double)((px_max - px_min)*j/num_cols - 1)/scale_factor;
	    z = (double)((pz_max - pz_min)*i/num_rows - 1)/scale_factor;
	    xyz_array[3*i*num_cols + 3*j] = x;
	    xyz_array[3*i*num_cols + 3*j + 1] = y;
	    xyz_array[3*i*num_cols + 3*j + 2] = z;
	}
    }
    surf_color.red = surf_color.green = surf_color.blue = 0;

    
    
    /* actual work */
    if (!binary_mode) {
	/* print surface out to stream;	 	*
	 * funny offsets to not plot the edge 	*
	 * polygons joined to the origin 	*/
	for (i = 1; i <= num_rows - 1; ++i) {	    /* loop over z */
	    for (j = 1; j <= num_cols - 2; ++j) {   /* loop over x */
	    
		/* first vertex */ 
		for (k = 0; k < 3; ++k)	
		    printf("%f ", xyz_array[3*i*num_cols + 3*j + k]);
		
		/* second vertex */
		for (k = 0; k < 3; ++k)	
		    printf("%f ", xyz_array[3*i*num_cols + 3*(j+1) + k]);
	    
		/* third vertex */
		for (k = 0; k < 3; ++k)	
		    printf("%f ", xyz_array[3*(i-1)*num_cols + 3*(j+1) + k]);
	    
		/* fourth vertex */
		for (k = 0; k < 3; ++k)	
		    printf("%f ", xyz_array[3*(i-1)*num_cols + 3*j + k]);
	    
		/* rgb values (obviously) */
		color_tile =
		(int)(20000*fabs(y_mat.el[i][j] - y_lo)/y_range) + 45535; 
		if (color_tile > 65535) color_tile = 65535;
		if (color_tile < 45535) color_tile = 45535;
		printf("rgb 0 %d 0\n", color_tile );

/* old		printf("rgb 0 %d 0\n", 
		      (int)(65535*fabs(y_mat.el[i][j] - y_lo)/y_range)); */
	    }	
	}
	plot_ascii_axes();
    } else {	/* binary mode */
	for (i = 1; i <= num_rows - 1; ++i) {		/* loop over z */
	    for (j = 1; j <= num_cols -2; ++j) {	/* loop over x */
	    
		x_array[0] = xyz_array[3*i*num_cols + 3*j];
		x_array[1] = xyz_array[3*i*num_cols + 3*(j+1)];
		x_array[2] = xyz_array[3*(i-1)*num_cols + 3*(j+1)];
		x_array[3] = xyz_array[3*(i-1)*num_cols + 3*j];
		
		y_array[0] = xyz_array[3*i*num_cols + 3*j + 1];
		y_array[1] = xyz_array[3*i*num_cols + 3*(j+1) + 1];
		y_array[2] = xyz_array[3*(i-1)*num_cols + 3*(j+1) + 1];
		y_array[3] = xyz_array[3*(i-1)*num_cols + 3*j + 1];
		
		z_array[0] = xyz_array[3*i*num_cols + 3*j + 2];
		z_array[1] = xyz_array[3*i*num_cols + 3*(j+1) + 2];
		z_array[2] = xyz_array[3*(i-1)*num_cols + 3*(j+1) + 2];
		z_array[3] = xyz_array[3*(i-1)*num_cols + 3*j + 2];
		
		surf_color.green = (int)(65535*fabs(y_mat.el[i][j] - y_lo)/y_range);
				
		write_binary_triples(stdout, x_array, y_array, z_array, NUM_VERTEX);
		write_binary_rgb_color(stdout, &surf_color);
	    }
	}
	plot_binary_axes();
    }

    
    /* housekeeping */
    if (y_fp != NULL) fclose(y_fp);
    if (xyz_array != NULL) free(xyz_array);
    fflush(stdout);
    exit (0);
} /* main */



#ifndef _NO_PROTO
    double matrix_lo(Matrix *m)
#else
    double matrix_lo(m)
        Matrix *m;
#endif
/* return the lowest valued element of a matrix */
{
    double lo = m->el[0][0];
    double hi = m->el[0][0];
    int i, j;
    
    for (j = 0; j < m->cols; ++j) {
	for (i = 0; i < m->rows; ++i) {
	    if ((m->el[i][j] == lo) || (m->el[i][j] == hi)) {
		continue;
	    } else if (m->el[i][j] > hi) {
		hi = m->el[i][j];
		continue;
	    } else if (m->el[i][j] < lo) {
		lo = m->el[i][j];
		continue;
	    }
	}
    }
    
    return lo;
}

#ifndef _NO_PROTO
    double matrix_hi(Matrix *m)
#else
    double matrix_hi(m)
        Matrix *m;
#endif
/* return the highest valued element of a matrix */
{
    double lo = m->el[0][0];
    double hi = m->el[0][0];
    int i, j;
    
    for (j = 0; j < m->cols; ++j) {
	for (i = 0; i < m->rows; ++i) {
	    if ((m->el[i][j] == lo) || (m->el[i][j] == hi)) {
		continue;
	    } else if (m->el[i][j] > hi) {
		hi = m->el[i][j];
		continue;
	    } else if (m->el[i][j] < lo) {
		lo = m->el[i][j];
		continue;
	    }
	}
    }
    
    return hi;
}


VOID plot_ascii_axes()
/* plot axes in ascii mode */
{
    /* y-axis */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., 0., 1., 0., 0., 0., 0., 0., 0, 0, 0);
    /* arrow head (red) */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n",
	   1., 0., 0., .9, .03, 0., .9, 0., 0., 65535, 0, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n",
	   1., 0., 0., .9, -.03, 0., .9, 0., 0., 65535, 0, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n",
	   1., 0., 0., .9, 0., .03, .9, 0., 0., 65535, 0, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n",
	   1., 0., 0., .9, 0., -.03, .9, 0., 0., 65535, 0, 0);    
    
    /* z-axis */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., 0., 0., 1., 0., 0., 0., 0., 0, 0, 0);
    /* arrow head (yellow) */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 1., 0., .03, .9, 0., 0., .9, 0., 65535, 65535, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 1., 0., -.03, .9, 0., 0., .9, 0., 65535, 65535, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 1., 0., 0., .9, .03, 0., .9, 0., 65535, 65535, 0);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 1., 0., 0., .9, -.03, 0., .9, 0., 65535, 65535, 0);
    
    /* x-axis */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., 0., 0., 0., -1., 0., 0., 0., 0, 0, 0);
    /* arrow head (blue) */
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., -1., .03, 0., -.9, 0., 0., -.9, 0, 0, 65535);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., -1., -.03, 0., -.9, 0., 0., -.9, 0, 0, 65535);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., -1., 0., .03, -.9, 0., 0., -.9, 0, 0, 65535);
    printf("%f %f %f %f %f %f %f %f %f rgb %d %d %d\n", 
	   0., 0., -1., 0., -.03, -.9, 0., 0., -.9, 0, 0, 65535);
}



VOID plot_binary_axes()
/* plot axes in binary mode */
{
#define NUM_CORNERS 3		/* three corners in a triangle */
    int i;			/* counter */
    double x_arr[NUM_CORNERS];
    double y_arr[NUM_CORNERS];
    double z_arr[NUM_CORNERS];
    XColor acolor;

    
    acolor.red = acolor.green = acolor.blue = 0.;
        
    /* y-axis */
    x_arr[0] = 0.;  x_arr[1] = 1.;  x_arr[2] = 0.;
    y_arr[0] = y_arr[1] = y_arr[2] = 0.;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    /* arrow head (red) */
    acolor.red = 65535;  acolor.green = acolor.blue = 0.;
    x_arr[0] = 1.;  x_arr[1] = x_arr[2] = .9;
    y_arr[0] = 0.;  y_arr[1] = .03;  y_arr[2] = 0.;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    y_arr[0] = 0.;  y_arr[1] = -.03;  y_arr[2] = 0.;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    y_arr[0] = y_arr[1] = y_arr[2] = 0.;
    z_arr[0] = 0.;  z_arr[1] =.03;  z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    z_arr[0] = 0.;  z_arr[1] = -.03;  z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);    
    
    acolor.red = acolor.green = acolor.blue = 0.;    
    
    /* z-axis */
    x_arr[0] = x_arr[1] = x_arr[2] = 0.;
    y_arr[0] = 0.;  y_arr[1] = 1.;  y_arr[2] = 0.;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    /* arrow head (yellow) */
    acolor.red = acolor.green = 65535;
    x_arr[0] = 0.;  x_arr[1] = .03;  x_arr[2] = 0.;
    y_arr[0] = 1.;  y_arr[1] = y_arr[2] = .9;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    x_arr[0] = 0.;  x_arr[1] = -.03;  x_arr[2] = 0.;
    z_arr[0] = z_arr[1] = z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    x_arr[0] = x_arr[1] = x_arr[2] = 0.;
    z_arr[0] = 0.;  z_arr[1] = .03;  z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    z_arr[0] = 0.;  z_arr[1] = -.03;  z_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    acolor.red = acolor.green = acolor.blue = 0.;    
    
    /* x-axis */
    x_arr[0] = x_arr[1] = x_arr[2] = 0.;
    y_arr[0] = y_arr[1] = y_arr[2] = 0.;
    z_arr[0] = z_arr[2] = 0.;  z_arr[1] = 1.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    /* arrow head (blue) */
    acolor.blue = 65535;
    x_arr[0] = 0.;  x_arr[1] = .03;  x_arr[2] = 0.;
    y_arr[0] = y_arr[1] = y_arr[2] = 0.;
    z_arr[0] = 1.;  z_arr[1] = z_arr[2] = .9;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    x_arr[0] = 0.;  x_arr[1] = -.03;  x_arr[2] = 0.;
    y_arr[0] = y_arr[1] = y_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    x_arr[0] = x_arr[1] = x_arr[2] = 0.;
    y_arr[0] = 0.;  y_arr[1] = .03;  y_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
    
    y_arr[0] = 0.;  y_arr[1] = -.03;  y_arr[2] = 0.;
    write_binary_triples(stdout, x_arr, y_arr, z_arr, NUM_CORNERS);
    write_binary_rgb_color(stdout, &acolor);
}


VOID help()
/* print out long help */
{
    printf("\nUsage:	Plot3D [-f[ile_in] datafile] [-b[inary_out]] [-h[elp]]");
    printf("\n\nOptions:  (none required)");  
    printf("\n          -f[ile_in] specifies input file.  Default is standard input.");
    printf("\n          -b[inary_out] produces binary output.  Default is ascii output.");
    printf("\n          -zmax d   scales plot so d as max height\n");
    printf("\n          -h[elp] brings up this long help\n");
    printf("\nDescription:  Outputs numbers which, when piped to the appropriate");
    printf("\n              programs, produces a surface plot a la Mathematica.\n");
}
