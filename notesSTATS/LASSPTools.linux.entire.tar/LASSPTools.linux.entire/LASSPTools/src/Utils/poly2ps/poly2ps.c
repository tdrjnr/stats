/*  This program reads in data made for input into Steve Townsend's
    polydraw program and produces postscript output with shading.
    It is a basterdized version of Chris Myer's lattice2ps program.
    Plagiarism by Joel Shore.
    4-21-91.								*/
#include <stdio.h>
#include <math.h>

#define MAXLENGTH 2000
#define GREYMAX 65535.0

char input[MAXLENGTH];
char *remains;

int status, colorf=0, trf=0, numsides, joelf = 0, noframef = 0, nofillf = 0;
int linesf = 0;
double  scalefx, scalefy, scalef,
 size = 7., sizex, sizey, originx = 2.25, originy = 3.5, borderf,
 xmax = 1.0, ymax = 1.0, xmin = -1.0, ymin = -1.0, gmin = 0.2, gmax = 0.9,
 gave = 0.55, color_constant = 1. / 65535., max_width, frame_width = 1;

main(argc,argv) 
int argc;
char **argv; 
{
   int i, red, green, blue, first_red = 1, first_green = 1, first_blue = 1;
   double shade, *x, *y, xtranslate, ytranslate, total_color;

    /* provide online help */
    if (argc > 1) {
       if (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-help") == 0) help();
    }

    /* read command line arguments */
    parse_argv(argc, argv);

    gave = 0.5 * (gmax + gmin);

    /* set scalefactors and translations */
    xtranslate = -xmin;
    ytranslate = -ymin;
    sizex = sizey = size;
    if (fabs((ymax-ymin)-(xmax-xmin))/(xmax-xmin) > 0.0000001) {
	fprintf(stderr,"\n***WARNING: (xmax-xmin) != (ymax-ymin): ");
	fprintf(stderr,"Figure will be distorted!***\n\n");
    }
    if (ymax-ymin > xmax-xmin) {
        max_width = ymax - ymin;
    } else {
        max_width = xmax - xmin;
    }

    if (!trf) {	
	originx = ((xmax-xmin)/sizex)*(8.5-sizex)/2.; 
	originy = ((ymax-ymin)/sizey)*(11.-sizey)/2.;
    }

    scalefx = 72.*(sizex/(xmax-xmin)); scalefy = 72.*(sizey/(ymax-ymin)); 
	scalef = scalefx;

   /* POSTSCRIPT Header */
   printf("%s!\n", "%");
   printf("%s: %d %d %d %d\n", "%%BoundingBox", 
	  (int)floor(originx*scalefx), 
	  (int)floor(originy*scalefy),
	  (int)floor((originx+ xmax - xmin)*scalefx), 
	  (int)floor((originy+ ymax - ymin)*scalefy));
   printf("/co  {setrgbcolor fill} def\n");
   printf("/sf  {setgray fill} def\n");
   printf("/bo  {closepath gsave .0 setgray ");
   printf("%e setlinewidth stroke grestore} def\n", 
		max_width * frame_width/500.);
   printf("/bon {gsave .0 setgray ");
   printf("%e setlinewidth stroke grestore} def\n", 
		max_width * frame_width/500.);
   printf("/cl  {closepath} def\n");
   printf("/mt  {moveto} def\n");
   printf("/lt  {lineto} def\n");
   printf("/rt  {rlineto} def\n");
   printf("/np  {newpath} def\n");

   printf("%f %f scale\n", scalefx, scalefy);
   printf("%f %f translate\n", originx, originy);

   /* Read in polygons and print out POSTSCRIPT */
   while (fgets(input,MAXLENGTH,stdin) != NULL) {
       if (read_pairs(input, &x, &y, &numsides, &remains)) {
          if (numsides != 0) {
              status = getRGB(remains, &red, &green, &blue);
/* allow polygon not to be skipped if it is not going to be filled anyway */
	      if (!status && !linesf && !nofillf) fprintf(stderr, "Skipped Polygon\n");

	      printf("np\n%.4e %.4e mt ", x[0]+xtranslate, y[0] + ytranslate);

	      if (joelf & numsides == 4) {
	          if (green == 0 && blue == 0) {
		      if (first_red) {
   			  printf( "\n/rhr  {%.4e %.4e rt %.4e %.4e rt ",
				x[1]-x[0], y[1]-y[0], x[2]-x[1], y[2]-y[1]);
			  printf("%.4e %.4e rt } def\n", x[3]-x[2], y[3]-y[2]);
			  first_red = 0;
		      }
		      printf("rhr ");
	          } else if (green == 0 && red == 0) {
		      if (first_blue) {
   			  printf( "\n/rhb  {%.4e %.4e rt %.4e %.4e rt ",
				x[1]-x[0], y[1]-y[0], x[2]-x[1], y[2]-y[1]);
			  printf("%.4e %.4e rt } def\n", x[3]-x[2], y[3]-y[2]);
			  first_blue = 0;
		      }
		      printf("rhb ");
	          } else if (blue == 0 && red == 0) {
		      if (first_green) {
   			  printf( "\n/rhg  {%.4e %.4e rt %.4e %.4e rt ",
				x[1]-x[0], y[1]-y[0], x[2]-x[1], y[2]-y[1]);
			  printf("%.4e %.4e rt } def\n", x[3]-x[2], y[3]-y[2]);
			  first_green = 0;
		      }
		      printf("rhg ");
	          } else { 
		      fprintf(stderr,
			      "Error---Can't use -joel option---Stopping\n");
		      exit(1);
		  }
	      } else {
	          for (i = 1; i < numsides; ++i) {
	              printf("%.4e %.4e lt ", x[i]+xtranslate, 
			     y[i] + ytranslate);
	          }
	      }
/* cl means to close the polygon...
   bo also draws it, in black....
   bon draws the path in black without closing it.   */

	      if (noframef && !linesf ) {
	          printf("cl ");
	      } else if(linesf) {
	          printf("bon ");
	      } else {
	          printf("bo ");
	      }

	      if (colorf && !linesf && !nofillf) {

                  printf("%.2f %.2f %.2f co\n", color_constant * red, 
			color_constant * green, color_constant * blue);

	      } else if(!linesf && !nofillf) {
	                        /* Users may need to modify this part of code
				   so their color scheme is nicely converted
				   to black and white shading.		     */

	          if (green == 0 && blue == 0) {
		      shade = gmin;
	          } else if (green == 0 && red == 0) {
		      shade = 0.5*(gmin+gmax);
	          } else if (blue == 0 && red == 0) {
		      shade = gmax;
	          } else if (blue == red && red == green) {
		      shade = (double) blue/ GREYMAX;
	          } else { 
	              total_color = (double) (blue + green + red);
		      shade = (gmin * red + gave * blue + gmax * green) / 
				total_color;
	          }
	          printf("%.2f sf\n", shade); 

	      }

          }
       }
   }
   printf("showpage\n");

   return;
}


parse_argv(argc,argv)
int argc;
char **argv;
{
    int i;

    /* read arguments and compute parameters */
    for(i = 1; i < argc; i++){
	/* specify no framing of polygons */
	if(strcmp(argv[i],"-noframe") == 0){
		noframef = 1;
	        continue;
	}
	/* specify no filling of polygons */
	if(strcmp(argv[i],"-nofill") == 0){
		nofillf = 1;
	        continue;
	}
	/* specify lines only, and don't connect last side */
	if(strcmp(argv[i],"-lines") == 0){
		linesf = 1;
	        continue;
	}
	/* specify this is for output from Joel's tiling model ----
	   There are special macros to put postscript in a more compact form. */
	if(strcmp(argv[i],"-joel") == 0){
		joelf = 1;
	        continue;
	}
	/* specify output size (largest direction) */
	else if(strcmp(argv[i],"-size") == 0){
		if(sscanf(argv[++i],"%lf",&size) != 1 || size <= 0.0)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        continue;
	}
	/* specify frame width for polygons */
	else if(strcmp(argv[i],"-framewidth") == 0){
		if(sscanf(argv[++i],"%lf",&frame_width) != 1 || frame_width < 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        continue;
	}
	/* specify output translation */
	else if(strcmp(argv[i],"-tr") == 0){
		if(sscanf(argv[++i],"%lf",&originx) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
		if(sscanf(argv[++i],"%lf",&originy) != 1)
			fprintf(stderr,"bad argument %s\n",argv[i]);
	        trf=1;
	        continue;
	}
        else if(strcmp(argv[i], "-color") == 0) {
	        colorf = 1; 
	        continue;
	}
        /* replace gmin and gmax if desired */
        else if(strcmp(argv[i],"-g") == 0){
                if(sscanf(argv[++i],"%lf",&gmin) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                if(sscanf(argv[++i],"%lf",&gmax) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        /* replace xmin and xmax if desired */
        else if(strcmp(argv[i],"-x") == 0){
                if(sscanf(argv[++i],"%lf",&xmin) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                if(sscanf(argv[++i],"%lf",&xmax) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        else if(strcmp(argv[i],"-xmin") == 0){
                if(sscanf(argv[++i],"%lf",&xmin) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        else if(strcmp(argv[i],"-xmax") == 0){
                if(sscanf(argv[++i],"%lf",&xmax) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        /* replace ymin and ymax if desired */
        else if(strcmp(argv[i],"-y") == 0){
                if(sscanf(argv[++i],"%lf",&ymin) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                if(sscanf(argv[++i],"%lf",&ymax) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        else if(strcmp(argv[i],"-ymin") == 0){
                if(sscanf(argv[++i],"%lf",&ymin) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
        else if(strcmp(argv[i],"-ymax") == 0){
                if(sscanf(argv[++i],"%lf",&ymax) != 1)
                        fprintf(stderr,"bad argument %s\n",argv[i]);
                continue;
        }
	/* polydraw options not supported by poly2ps */
        else if(strcmp(argv[i], "-noheal") == 0
             || strcmp(argv[i], "-buffer") == 0 
             || strcmp(argv[i], "-b") == 0 
             || strcmp(argv[i], "-i") == 0 
             || strcmp(argv[i], "-interval") == 0 
             || strcmp(argv[i], "-geometry") == 0 
             || strcmp(argv[i], "-title") == 0 ) {
 	        fprintf(stderr,"%s is a polydraw argument---Ignored\n",
			argv[i]);
	        continue;
	}
	/* garbage */
 	else fprintf(stderr,"bad argument %s\n",argv[i]);
    }
}

help()
{
   fprintf(stderr, "\npoly2ps: takes input (ASCII) for polydraw and writes it in postscript format\n");
   fprintf(stderr, "\n[See polydraw program for details on input format.]\n");
   fprintf(stderr, "The conversion from colors to b/w shading will probably only\n");
   fprintf(stderr, "work well for the case of red, blue, and green polygons.\n");
   fprintf(stderr, "Users may need to modify code to convert their specific colors.\n");
   fprintf(stderr, "\nAvailable options are:\n");
   fprintf(stderr, "-tr trx try \t translates postscript origin to (trx,try) in inches\n");
   fprintf(stderr, "\t\t (default = centered)\n");
   fprintf(stderr, "-g gmin gmax \t sets range of grayscales from gmin to gmax\n");
   fprintf(stderr, "\t\t (default = 0.2->0.9 (dark->light))\n");
   fprintf(stderr, "-x xmin xmax \t sets range of x-values\n");
   fprintf(stderr, "\t\t (default = -1.0 1.0)\n");
   fprintf(stderr, "-y ymin ymax \t sets range of y-values\n");
   fprintf(stderr, "\t\t (default = -1.0 1.0)\n");
   fprintf(stderr, "-xmin xmin \t alternate way to set xmin (Compatible with polydraw)\n");
   fprintf(stderr, "-xmax xmax \t alternate way to set xmax (Compatible with polydraw)\n");
   fprintf(stderr, "-ymin ymin \t alternate way to set ymin (Compatible with polydraw)\n");
   fprintf(stderr, "-ymax ymax \t alternate way to set ymax (Compatible with polydraw)\n");
   fprintf(stderr, "-size size \t scales largest axis to this many inches (default = 7.0)\n");
   fprintf(stderr, "-framewidth fw \t sets width of frame around each polygon\n");
   fprintf(stderr, "\t\t [a number greater than 0] (default = 1)\n");
   fprintf(stderr, "-color \t\t shades sites with the specified colors\n");
   fprintf(stderr, "\t\t (otherwise it will convert to b/w shading)\n");
   fprintf(stderr, "-noframe \t omits the frame around each polygon\n");
   fprintf(stderr, "-nofill \t does not fill polygons\n");
   fprintf(stderr, "-lines \t\t treats input as lines, not polygons\n");
   fprintf(stderr, "-joel \t\t uses macros specialized to Joel's tiling model input\n");
   fprintf(stderr, "\t\t in order to produce more compact postscript for this case\n");

   exit(-1);
}

/*-----------------------------------------------------------------------------
 * getRGB()
 * this routine parses a string of the form "rgb 65535 0 0" into red, green,
 * and blue components which are stored in the corresponding fields of the
 * passed XColor.
 */

  int getRGB(thestring, red, green, blue)
  int *red, *green, *blue;
  char *thestring;
{
        int numgot;
        char dummy[255];

        numgot = sscanf(thestring, "%s %d %d %d", dummy, red, green, blue);
/*	fprintf(stderr,"%d %d %d\n", *red, *green, *blue);	*/

        if (numgot!=4) {
                return(0);
        } else {
                return(1);
        }
}

