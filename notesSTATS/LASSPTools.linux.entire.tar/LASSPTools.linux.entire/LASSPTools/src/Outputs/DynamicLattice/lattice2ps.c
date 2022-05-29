/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 
/*
   lattice2ps:
   program to represent 2d square lattice field in Postscript

   see enclosed man page and online help via "lattice2ps -h"

   Copyright (C) 1990 Christopher R. Myers (with the exception of
   math_io.c, from the LASSPTools distribution).  All rights reserved.
   Feel free to copy, modify and redistribute this program, but do not
   sell it.  Include this notice in any copy.

   written: 10/90; latest revision: 2/93 (v. 3.0)
*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 

#define MAIN

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "math_io.h"

#include "lattice2ps.h"

#define STRCMP0(arg,str) (strcmp(arg,str)==0)
#define even(x)  ((double)x/2  == floor((double)x/2))
#define hexed(x) ((double)x/16 == floor((double)x/16))
#define MIN(x,y) ( (x<y) ? x : y)
#define MAX(x,y) ( (x>y) ? x : y)

#define DEFAULT_NCOLORS 32

double x,y;
char line[128];
Matrix *zmat;

double defaultred[32]   = {.10,.15,.20,.25,.30,.35,.40,.45,.50,.55,.60,.65,.70,
.75,.80,.85,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90};
double defaultgreen[32] = {.10,.15,.20,.25,.30,.35,.40,.45,.50,.55,.60,.65,.70,
.75,.80,.85,.85,.80,.75,.70,.65,.60,.55,.50,.45,.40,.35,.30,.25,.20,.15,.10};
double defaultblue[32]  = {.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,.90,
.90,.90,.90,.85,.80,.75,.70,.65,.60,.55,.50,.45,.40,.35,.30,.25,.20,.15,.10};

main(argc,argv) 
int argc;
char **argv; 
{
    int i, j, counter=0, ishade;
    double z, shade;
    FILE *colormap;

    /* print version information */
    fprintf(stderr, "lattice2ps (version 3.0)\n");

    /* set default values */
    graphics.mode   = 0;
    graphics.gray   = 1;
    graphics.gmin   = 0.; 
    graphics.gmax   = 1.;
    graphics.bgd    = 1.;
    image.size      = 5.;
    image.originx   = 1.75;
    image.originy   = 3.;
    image.rotate    = 0.;
    graphics.gridg  = 0.;
    graphics.border = 0;

    /* provide online help */
    if (argc > 1) 
    {
	if (STRCMP0(argv[1], "-h") || STRCMP0(argv[1], "-help")) help();
    }

    /* read command line arguments */
    parse_argv(argc, argv);

    zmat  = create_matrix(lattice.ny, lattice.nx);
    set.z = (int **) malloc (lattice.ny * sizeof(int *));
    for (j = 0; j < lattice.ny; j++) 
    {
	set.z[j] = (int *) malloc (lattice.nx * sizeof(int));
    }

    switch (input.mode)
    {
	case (0):   /* sites specified individually */

	    zscale.zmin = HUGE_VAL; zscale.zmax = -HUGE_VAL;
	    while (!feof(stdin))
	    {
		/* read in data; compute min and max */
		if (!test_isalpha())
		{
		    scanf ("%d %d %lf\n", &i, &j, &z);
		    zmat->el[j][i] = z;
		    set.z[j][i] = 1;
		    if (z < zscale.zmin) zscale.zmin = z;
		    if (z > zscale.zmax) zscale.zmax = z;
		}
		zscale.zrange = zscale.zmax - zscale.zmin;
	    }
	    break;

	case (2):   /* entire matrix input */

	    if (!test_isalpha()) load_new_matrix(zmat);
	    break;

	}

    /* set scalefactors and translations */
    if (lattice.nx == lattice.ny)	
    { 
	image.sizex = image.sizey = image.size; 
    }
    else if (lattice.nx > lattice.ny)	
    { 
	image.sizex = image.size; 
	image.sizey = (image.size * lattice.ny)/lattice.nx; 
    }
    else
    { 
	image.sizey = image.size; 
	image.sizex = (image.size * lattice.nx)/lattice.ny; 
    }

    if (!set.trans && image.rotate==0.) 
    {	
	image.originx = (lattice.nx/image.sizex)*(8.5-image.sizex)/2.; 
	image.originy = (lattice.ny/image.sizey)*(11.-image.sizey)/2.;
    }
    if (!set.trans && image.rotate==90.) 
    {	
	image.originx = (lattice.nx/image.sizex)*(11.-image.sizex)/2.; 
	image.originy = (lattice.ny/image.sizey)*(8.5-image.sizey)/2.;
    }

    image.scalefx = 72.*(image.sizex/lattice.nx); 
    image.scalefy = 72.*(image.sizey/lattice.ny); 
    image.scalef  = image.scalefx;

    if (!graphics.gray) 
    {
	if ((colormap = fopen(".colormaprc", "r")) == NULL) 
        {
	    use_encoded_colormap_defaults();
	}
	else 
        {
	    fscanf (colormap, "number of levels: %d\n", &graphics.Ncolors);

	    graphics.rcolor = 
	    (double *) malloc (graphics.Ncolors*sizeof(double));
	    graphics.gcolor = 
	    (double *) malloc (graphics.Ncolors*sizeof(double));
	    graphics.bcolor = 
	    (double *) malloc (graphics.Ncolors*sizeof(double));
	    
	    for (i = 0; i < graphics.Ncolors; i++) 
	    {
		fscanf(colormap,"%lf %lf %lf\n", 
		       &graphics.rcolor[i],&graphics.gcolor[i],
		       &graphics.bcolor[i]);
	    }
	}
    }

   /* POSTSCRIPT */
   printf("%s!\n", "%");
   printf("%s: %d %d %d %d\n", "%%BoundingBox", 
	  (int)floor(image.originx*image.scalefx), 
	  (int)floor(image.originy*image.scalefy),
	  (int)floor((image.originx+lattice.nx)*image.scalefx), 
	  (int)floor((image.originy+lattice.ny)*image.scalefy));
   printf("/bo {moveto 0 1 rlineto 1 0 rlineto 0 -1 rlineto closepath} def\n");
   printf("/co {setrgbcolor fill} def\n");
   printf("/vgl {moveto 0 %d rlineto closepath stroke} def\n", lattice.ny);
   printf("/hgl {moveto %d 0 rlineto closepath stroke} def\n", lattice.nx);

   printf("%f %f scale\n", image.scalefx, image.scalefy);
   printf("%f %f translate\n", image.originx, image.originy);
   printf("%f rotate\n", image.rotate);
   if (image.rotate==90.) 
     printf("%f %f translate\n", -image.originx, -image.originy);

   if (graphics.gray) 
   {
	printf("%d %d 8\n", lattice.nx, lattice.ny);
	printf("[1 0 0 -1 0 %d]\n", lattice.ny);
	printf("{<\n");
   }

    for (j = 0; j < lattice.ny; j++) 
    {
	for (i = 0; i < lattice.nx; i++) 
	{
	counter++;
	if (input.mode == 0 && !set.z[j][i])
	{
	    shade = graphics.bgd;
	}
	else
	{
	    z = zmat->el[j][i];
	    shade = ((graphics.gmax-graphics.gmin)*
		     ((z-zscale.zmin)/(zscale.zrange))+graphics.gmin);

	    if (shade < 0.) shade = MIN(graphics.gmin, graphics.gmax);
	    if (shade > 1.) shade = MAX(graphics.gmin, graphics.gmax);
	}

	switch (graphics.gray) 
	{
	    case (1):

		printf("%.2x", (int)floor(255*shade));
		if (even(counter))  printf(" ");
		if (hexed(counter)) printf("\n");

            break;

	    case (0):

		ishade = floor(graphics.Ncolors*shade);
		if (ishade > graphics.Ncolors-1) ishade = graphics.Ncolors-1;
		printf("%d %d bo\n", i, lattice.ny-(j+1));
		printf("%.2f %.2f %.2f co\n", 
		       graphics.rcolor[ishade], graphics.gcolor[ishade], 
		       graphics.bcolor[ishade]);

	    break;
	}
      }
   }

   if (graphics.gray) printf ("\n>}\nimage\n");

   /* draw surrounding box */
   if (graphics.border)
   {
   printf("0. setgray\n");
   printf("%f setlinewidth\n", 1./image.scalef);
   printf("newpath\n");
   printf("0 0 moveto\n"); 
   printf("%d 0 rlineto\n", lattice.nx);
   printf("0 %d rlineto\n", lattice.ny);
   printf("-%d 0 rlineto\n", lattice.nx);
   printf("closepath stroke\n\n");
   }

   if (graphics.grid) grid(lattice.nx,lattice.ny,graphics.gridg);

   draw_map();

   printf("showpage\n");

   return;
}


/*------------------------------------------------------------------------- */

int load_new_matrix(mat)
Matrix *mat;
{
    int i, j, done;

    if (input.binary) done = get_binary_matrix(mat, stdin);
    else              done = get_matrix(mat, stdin);

    lattice.nx = zmat->cols;
    lattice.ny = zmat->rows;

    if (done == EOF) return done; /* exit at the end of the file */

    /*  Need to find min and max values in matrix.  */

    if (!set.zscale) 
    {
	zscale.zmin = zscale.zmax = zmat->el[0][0];

	for(i = 0; i < zmat->cols; i++)
	{
	    for(j = 0; j < zmat->rows; j++)
	    {
		if (zmat->el[i][j] < zscale.zmin) zscale.zmin = zmat->el[i][j];
		if (zmat->el[i][j] > zscale.zmax) zscale.zmax = zmat->el[i][j];
	    }
	}
	zscale.zrange = zscale.zmax - zscale.zmin;
    }
}

/*------------------------------------------------------------------------- */

grid(nx,ny,gridg)
int nx, ny;
double gridg;
{
   int i,j;

   printf("%f setlinewidth\n", .2/image.scalef);
   printf("%f setgray\n", gridg);

   for (i = 1; i < nx; i++) {
         printf("%d 0 vgl\n", i);
   }
   for (j = 1; j < ny; j++) {
         printf("0 %d hgl\n", j);
   }
}

/*------------------------------------------------------------------------- */

draw_map()
{
   int i, ishade, counter=0;
   double g, shade;

   printf("%f %f scale\n", 72*image.sizex/32/image.scalefx,
			   144*image.sizex/32/image.scalefx);
   printf("0 -2. translate\n");

   if (graphics.gray) 
   {
	printf("32 1 8\n", lattice.nx, lattice.ny);
	printf("[1 0 0 -1 0 1]\n", 1);
	printf("{<\n");
   }

   for (i = 0; i < 32; i++)
   {
	g = graphics.gmin + (i/31.)*(graphics.gmax-graphics.gmin);
        counter++;

	shade = ((graphics.gmax-graphics.gmin)*
		 ((g-graphics.gmin)/(graphics.gmax-graphics.gmin))
		 +graphics.gmin);

	switch (graphics.gray) 
	{
	    case (1):

		printf("%.2x", (int)floor(255*shade));
		if (even(counter))  printf(" ");
		if (hexed(counter)) printf("\n");

            break;

	    case (0):

		ishade = floor(graphics.Ncolors*shade);
		if (ishade > graphics.Ncolors-1) ishade = graphics.Ncolors-1;
		printf("%d %d bo\n", i, 0);
		printf("%.2f %.2f %.2f co\n", 
		       graphics.rcolor[ishade], graphics.gcolor[ishade], 
		       graphics.bcolor[ishade]);

	    break;
	}
   }

   if (graphics.gray) printf (">}\nimage\n");

   /* draw surrounding box */
   printf("0. setgray\n");
   printf("0.01 setlinewidth\n");
   printf("newpath\n");
   printf("0 0 moveto\n"); 
   printf("32 0 rlineto\n");
   printf("0 1 rlineto\n");
   printf("-32 0 rlineto\n");
   printf("closepath stroke\n\n");


}

/*------------------------------------------------------------------------- */

int test_isalpha()
{
    char nc;

    if (isalpha(nc=getc(stdin)))
    {
        ungetc(nc,stdin);
	gets(line);
        return (1);
    }
    else
    {
        ungetc(nc,stdin);
        return (0);
    }
}

/*------------------------------------------------------------------------- */

parse_argv(argc,argv)
int argc;
char **argv;
{
    int i;

    /* read arguments and compute parameters */
    for(i = 1; i < argc; i++){
	/* specify number of x points */
	if (STRCMP0(argv[i],"-nx"))
	{
	    set.nx = 1;
	    if (sscanf(argv[++i],"%d",&lattice.nx) != 1)
	      fprintf(stderr,"bad argument %s\n",argv[i]);
	}
	/* specify number of y points */
	if (STRCMP0(argv[i],"-ny"))
	{
	    set.ny = 1;
	    if (sscanf(argv[++i],"%d",&lattice.ny) != 1)
	      fprintf(stderr,"bad argument %s\n",argv[i]);
	}
	/* specify background shade of gray */
	if (STRCMP0(argv[i],"-bg"))
	{
	    if (sscanf(argv[++i],"%lf",&graphics.bgd) != 1)
	      fprintf(stderr,"bad argument %s\n",argv[i]);
	}
        /* set overriding colormap file */
        if (STRCMP0(argv[i],"-cmap"))
        {
            set.map = 1;
            strcpy(graphics.mapf, argv[++i]);
	}
	/* specify grid if desired */
	if (STRCMP0(argv[i],"-grid"))
	{
		graphics.grid = 1;
		if (sscanf(argv[++i],"%lf",&graphics.gridg) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        continue;
	}
	/* rotate image */
	if (STRCMP0(argv[i],"-r"))
	{
	    image.rotate = 90.;
	    image.size   = 8.;
	}
	/* specify output size (largest direction) */
	if (STRCMP0(argv[i],"-size"))
	{
		if (sscanf(argv[++i],"%lf",&image.size) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        continue;
	}
	/* specify output translation */
	if (STRCMP0(argv[i],"-tr"))
	{   
		if (sscanf(argv[++i],"%lf",&image.originx) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
		if (sscanf(argv[++i],"%lf",&image.originy) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        set.trans = 1;
	        continue;
	}
	/* replace gmin and gmax if desired */
	if (STRCMP0(argv[i],"-gray"))
	{
		if (sscanf(argv[++i],"%lf",&graphics.gmin) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
		if (sscanf(argv[++i],"%lf",&graphics.gmax) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        continue;
	}
	if (STRCMP0(argv[i],"-matrix"))
	{
	    input.mode = 2;
	}
	if (STRCMP0(argv[i],"-border"))
	{
	    graphics.border = 1;
	}
	/* replace zmin and zmax if desired */
	if (STRCMP0(argv[i],"-z"))
	{
		set.zscale = 1;
		if (sscanf(argv[++i],"%lf",&zscale.zmin) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
		if (sscanf(argv[++i],"%lf",&zscale.zmax) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
		zscale.zrange = zscale.zmax - zscale.zmin;
	        continue;
	}
        if (STRCMP0(argv[i], "-a"))
	{
	        graphics.mode = 1; 
	        continue;
	}
        if (STRCMP0(argv[i], "-color"))
	{
	        graphics.gray = 0; 
	        continue;
	}
    }
}

use_encoded_colormap_defaults()
{
   int i;

   fprintf(stderr, "Using default colormap\n");
   graphics.Ncolors = DEFAULT_NCOLORS;

   graphics.rcolor = (double *) malloc (graphics.Ncolors*sizeof(double));
   graphics.gcolor = (double *) malloc (graphics.Ncolors*sizeof(double));
   graphics.bcolor = (double *) malloc (graphics.Ncolors*sizeof(double));
	    
   for (i = 0; i < graphics.Ncolors; i++) 
   {
	graphics.rcolor[i] = defaultred[i];
	graphics.gcolor[i] = defaultgreen[i];
	graphics.bcolor[i] = defaultblue[i];
   }
}

help()
{
   fprintf(stderr, "\nlattice2ps: shade values on a lattice by value, writing in postscript format\n\n");

   fprintf(stderr, "\t input:  rows of matrix separated by carriage returns \n");
   fprintf(stderr, "\t         or stream of numbers from stdin (filling rows first)\n");
   fprintf(stderr, "\t output: postscript file\n");
   fprintf(stderr, "\nAvailable options are:\n");
   fprintf(stderr, "-nx ncols \t sets number of columns (default = sqrt(no. of entries))\n");
   fprintf(stderr, "-ny nrows \t sets number of rows (default = sqrt(no. of entries))\n");
   fprintf(stderr, "-grid graylevel  turns on grid of specified graylevel\n");
   fprintf(stderr, "-border \t draws black border around image\n");
   fprintf(stderr, "-tr trx try \t translates postscript origin to (trx,try) in inches\n");
   fprintf(stderr, "\t\t (default = centered)\n");
   fprintf(stderr, "-size size \t scales largest axis to size inches (default = 4)\n");
   fprintf(stderr, "-gray gmin gmax \t sets range of grayscales from gmin to gmax \n");
   fprintf(stderr, "-matrix \t sets input mode to LASSPTools matrix format (see man DynamicLattice) \n");
   fprintf(stderr, "\t\t (default = 0.->1. (black->white))\n");
   fprintf(stderr, "-z zmin zmax \t sets range of z values from zmin to zmax \n");
   fprintf(stderr, "\t\t (default = zmin,zmax of data)\n");
   fprintf(stderr, "-color \t\t shades sites with colors according to colormap\n");
   fprintf(stderr, "\t\t (default blue->red, override in .colormaprc)\n");

   exit(-1);
}

/*------------------------------------------------------------------------- */
