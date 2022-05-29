/*  written by avid Chin, June 1991;  
 *  acknowledgements to Bill Dimm, Bruce Roberts, C.Y. Tan for assistance
 *
 *  This is a C driver for MINPACK routine lmdif1.  The things which need
 *  to be changed for your particular case are (use your editor and 
 *  search for "CHANGE THIS"):
 *	1) number_params -- set to the number of paramaters in the function
 *	   you want to be fitted; the number of initial guesses also
 *	   must be changed (NOTE:  this can't be #defined because I need
 *	   to pass it by reference)
 *	2) the variable lwa -- lwa >= n*(m+5)+m : n = number of parameters,
 *	   m = number of data points (just change the arbitrary literal
 *	   constant that I have there)
 *	3) the statement which looks at stdin for params
 *	4) you need to provide your own theory routine
 *  To interpret the output parameter called info, read the file 
 *  what_is_info.doc, or read the prologue for lmdif and lmdif1.
 *  _NOTE_:  all points are weighted equally when calculating the fit.
 *	     It may be that sometime in the near future, if demand
 *  	     merits it, I will try to put weighting in.  If not, one
 *	     could probably do it by changing fcn.  One may also have
 *	     to diddle with the norm calulating routine, enorm.
 * 1.13 8/14/91 1 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include "math_io.h"

#ifndef _NO_PROTO
#define ARGS(a) a
#else
#define ARGS(a) ()
#endif

#ifndef _NO_PROTO
#define CONST const
#else
#define CONST 
#endif

#ifndef _NO_PROTO
#define VOID void
#else
#define VOID
#endif

#ifdef UNDERSCOREFORTRAN
#define LMDIF1 lmdif1_
#else
#define LMDIF1 lmdif1
#endif

#ifdef UNDERSCOREFORTRAN
#define DPMPAR dpmpar_
#else
#define DPMPAR dpmpar
#endif

#ifdef UNDERSCOREFORTRAN
#define ENORM enorm_
#else
#define ENORM enorm
#endif

#ifdef UNDERSCOREFORTRAN
#define FCN fcn_
#else
#define FCN fcn
#endif

/* Matrix is a type def'ed in math_io.h; it is actually a struct;
 * this has to be defined globally because lmdif expects the data to 
 * be in fcn itself, which it isn't, and fcn needs to access this */
Matrix data_array;

VOID help ARGS(());
VOID plot_data ARGS((int, const char[]));
VOID plot_theory ARGS((int, const char[], const double[]));
VOID print_results ARGS((float, int, int, const double[]));

/* lmdif1 is a minpack routine */
extern double LMDIF1 ARGS((double (*fcn)(int*, int*, int*, const double[], 
					   double[], int*, int*),
			     int *m, int *n, double *x, 
			     double *fvec, double *tol, int *info, int *iw,
			     double *wa, int *lwa));

/* dpmpar is another minpack routine (returns machine constants) */
extern double DPMPAR ARGS((int i));
			     
/* enorm is yet another minpack routine */
extern double ENORM ARGS((int*, const double[]));

/* fcn assigns to fvec the difference between expt and theory */
extern VOID FCN ARGS((int*, int*, int*, const double[], double[],
		       int*, int*));

/*
 * CHANGE THIS
 */
/* theory is, of course, the function whose parameters we want to find */
extern double theory ARGS((double, const double[]));
typedef enum _boolean {false=0, true=1} Boolean; 
Boolean xmgr = false;

#ifndef _NO_PROTO
    main(int argc, char *argv[])
#else
    main(argc, argv)
        int argc;
        char *argv[];
#endif
{
    int i;		    	/* counter and such */
        
    /* variables and constants for this specific example */
    int number_params;		/* number of parameters in the theory */
    double *params;		/* array of parameters */
    extern Matrix data_array;	/* matrix of data */
    FILE *data_file;		/* file containing data */
    
    /* here's a list of additional variables (some are actually constants 
     * but for the fact that Fortran passes by reference) which are needed 
     * to use lmdif1 */
    double *fvec;	/* fvec contains difference between
			 * expt and theory */
    double tol; 	/* max allowed error */
    int info;		/* info about why execution stopped */
    int *iw;		/* integer work array */
    int lwa;		/* lwa must be >= n*(m+5)+m for iopt=1 */
    double *wa;		/* another work array */
    float fnorm;  	/* this will contain the final L2 norm of the 
			 * residuals */
	
    /* string constants for point style symbols */
#define PLSTR "pl"
#define CRSTR "cr"
#define DISTR "di"
#define SQSTR "sq"
#define OCSTR "oc"
#define FDSTR "fd"
#define FSSTR "fs"
#define FXSTR "fx"
#define FPSTR "fp"
#define BUSTR "bu"

    /* variables to customize the xplot output */
    int d_style;		/* data plot style */
    int t_style;		/* theory plot style */
    char dp_type[3];		/* symbol for data point plot */
    char tp_type[3];		/* symbol for theory point plot */
				
    strcpy(dp_type, PLSTR);	/* default symbols for 	*/
    strcpy(tp_type, FDSTR);	/* 	point plots 	*/
    
        
    /* need at least the -f flag; but only check if there are no flags */
    if (argc == 1) {
	fprintf(stderr, "\nNeed at least the -f flag\n\n");
	exit(1);
    } else { /* if there is stuff other than program name on command line */
	/* do command line parsing */
	for (i = 1; i < argc; ++i) {
	    /* want help? */
	    if (strcmp(argv[i], "-help") == 0 || 
		strcmp(argv[i], "-h") == 0) {
		help();
		exit(0);
	    }
	    else
	    
	    /* input file? */
	    if (strcmp(argv[i], "-file_in") == 0 ||
		strcmp(argv[i], "-f") == 0) {
		if ((data_file = fopen(argv[++i], "r")) == NULL) {
		    fprintf(stderr, "Error in opening file %s\n", argv[i]);
		    exit(1);
		}
		continue;
	    }

	    else

            /* Check if output should be xmgr compatible */
            if (strcmp(argv[i], "-xmgr") == 0) {
               xmgr = true; 
	    }
            
            else
            	    
	    /* data line style? */
	    if (strcmp(argv[i], "-data_style") == 0 ||
		strcmp(argv[i], "-d") == 0) {
		d_style = atoi(argv[++i]);
		if ((d_style < 0) || (d_style > 5)) {
		    d_style = 0;
		    fprintf(stderr, "\nData line style reverted to default");
		}
		continue;
	    }
	    else
	    
	    /* theory line style? */
	    if (strcmp(argv[i], "-theory_style") == 0 ||
		strcmp(argv[i], "-t") == 0) {
		t_style = atoi(argv[++i]);
		if ((t_style < 0) || (t_style > 5)) {
		    t_style = 1;
		    fprintf(stderr, "\nTheory line style reverted to default");
		}
		continue;
	    }
	    else
	    
	    /* data point plot symbol? */
	    if (strcmp(argv[i], "-data_point") == 0 ||
		strcmp(argv[i], "-dp") == 0) {
		strcpy(dp_type, argv[++i]);
		if ((strcmp(dp_type, PLSTR) != 0) && (strcmp(dp_type, CRSTR) != 0) &&
		    (strcmp(dp_type, DISTR) != 0) && (strcmp(dp_type, SQSTR) != 0) &&
		    (strcmp(dp_type, OCSTR) != 0) && (strcmp(dp_type, FDSTR) != 0) &&
		    (strcmp(dp_type, FSSTR) != 0) && (strcmp(dp_type, FXSTR) != 0) &&
		    (strcmp(dp_type, FPSTR) != 0) && (strcmp(dp_type, BUSTR) != 0)) {
		    strcpy(dp_type, PLSTR);
		    fprintf(stderr, "\nData plot point symbol reverted to default\n");
		}
		continue;
	    }
	    else
	    
	    /* theory point plot symbol? */
	    if (strcmp(argv[i], "theory_point") == 0 ||
		strcmp(argv[i], "-tp") == 0) {
		strcpy(tp_type, argv[++i]);
		if ((strcmp(tp_type, PLSTR) != 0) && (strcmp(tp_type, CRSTR) != 0) &&
		    (strcmp(tp_type, DISTR) != 0) && (strcmp(tp_type, SQSTR) != 0) &&
		    (strcmp(tp_type, OCSTR) != 0) && (strcmp(tp_type, FDSTR) != 0) &&
		    (strcmp(tp_type, FSSTR) != 0) && (strcmp(tp_type, FXSTR) != 0) &&
		    (strcmp(tp_type, FPSTR) != 0) && (strcmp(tp_type, BUSTR) != 0)) {
		    strcpy(tp_type, PLSTR);
		    fprintf(stderr, "\nTheory plot point symbol reverted to default\n");
		}
		continue;
	    }
	    else {
		fprintf(stderr, "Unknown option:  %s\n", argv[i]);
		exit(1);
	    }
	}
    }
    	    	    
    /* initialisations */
    d_style = 0;		/* default style for data is points */
    t_style = 1; 		/* default style for theory is solid line */
    number_params  = 5;		/* CHANGE THIS!! */
    params = (double *)calloc(number_params, sizeof(double));
    get_matrix(&data_array, data_file);	 /* read data_array from data_file */
    fvec = (double *)calloc(data_array.rows, sizeof(double));
    i = 1;			/* this is used in the next line */
    tol = sqrt(DPMPAR(&i));	/* tolerance = sqrt(machine precision) */
    iw = (int *)calloc(number_params, sizeof(int));
    lwa = number_params*(data_array.rows + 5) 
	  + data_array.rows + 25;	 /* 25 is arbitrary; CHANGE THIS
					  * IF NEEDED */
    wa = (double *)calloc(lwa, sizeof(double));
    
 
    /* get initial guess from stdin (usually, output from slider) */
    while(scanf("%lf %lf %lf %lf %lf", &params[0], &params[1], &params[2],
		&params[3], &params[4]) != EOF) {   /* CHANGE THIS!!! */
	/* plot */
	plot_data(d_style, dp_type);
	plot_theory(t_style, tp_type, params);
        if (xmgr) {
           
         fprintf(stdout,"&\n@autoscale\n@redraw\n@sleep 3\n@kill s0\n@kill s1\n");
          
        } else
	   fprintf(stdout, "#Q\n");	/* #Q clears the xplot graph */
	fflush(stdout);			
    }
    
    
    /* when get EOF, use the last set of guessed parameters
     * as initial guesses for least squares fit, and do the fit */
    LMDIF1 (FCN, &data_array.rows, &number_params, params, fvec, 
	    &tol, &info, iw, wa, &lwa);

    /* calculate the euclidean norm of the difference vector */
    fnorm = ENORM (&data_array.rows, fvec);

    /* print results to screen */
    print_results(fnorm, info, number_params, params);

    /* plot the data with the fitted curve */
    plot_data(d_style, dp_type);    
    plot_theory(t_style, tp_type, params);
    if (xmgr)
       fprintf(stdout,"&\n@autoscale\n@redraw\n");	
    fflush(stdout);
    
    /* house-keeping */
    free_matrix(&data_array);
    fclose(data_file);
    free(params);
    free(fvec);
    free(iw);
    free(wa);
    exit(0);
}



VOID help()
{
    printf("\nUsage:	fit_it -f[ile_in] filename [-d[ata_style] dataStyle]");
    printf("\n          [[-data_point] [-dp] dataPointSymbol]");
    printf("\n          [-t[heory_style] theoryStyle]");
    printf("\n          [[-theory_point] [-tp] theoryPointSymbol] [-h[elp]]");
    printf("\nWhere:	filename = file containing sorted data");
    printf("\n\tdataStyle = line style for data plot ");
    printf("\n\tdataPointSymbol = symbol to use if data is point plotted");
    printf("\n\ttheoryStyle = line style for theory plot");
    printf("\n\ttheoryPointSymbol = symbol to use if theory is point plotted");
    printf("\n	-h[elp] brings up this long help");
    printf("\nAvailable line styles:");
    printf("\n\t0 = point plot (default for dataStyle),");
    printf("\n\t1 = solid line (default for theoryStyle),");
    printf("\n\t2 = dotted line,");
    printf("\n\t3 = short dashes,");
    printf("\n\t4 = long dashes,");
    printf("\n\t5 = dot-dash.");
    printf("\nAvailable point symbols:");
    printf("\n\tpl = plus sign (default),");
    printf("\n\tcr = cross,");
    printf("\n\tdi = diamond,");
    printf("\n\tsq = square,");
    printf("\n\toc = octagon,");
    printf("\n\tfd = fancy diamond,");
    printf("\n\tfs = fancy square,");
    printf("\n\tfx = fancy cross,");
    printf("\n\tfp = fancy plus,");
    printf("\n\tbu = burst.");
    printf("\n  -xmgr Output is compatible with xmgr graph program.");
    printf("\n");
}

#ifndef _NO_PROTO
    void plot_data(int d_style, const char dp_type[])
#else
    plot_data(d_style, dp_type)
        int d_style;
        char dp_type[];
#endif
{
    extern Matrix data_array;
    int i;
    
       if (!xmgr) {
          fprintf(stdout, "#-m %d\n", d_style);		
          if (d_style == 0) {					
	      fprintf(stdout, "#-cs 0.5\n");			
	      fprintf(stdout, "#-c \"\\%s\"\n", dp_type);	
          }
       }
    for (i = 0; i < data_array.rows; ++i) {
	fprintf(stdout, "%f %f\n", data_array.el[i][0], 
		data_array.el[i][1]);
    }
}

#ifndef _NO_PROTO
    void plot_theory(int t_style, const char tp_type[], const double params[])
#else
    plot_theory(t_style, tp_type, params)
        int t_style;
        char tp_type;
        double params[];
#endif
{
    extern Matrix data_array;
    double theoretical_value;
    int i;
    
       if (xmgr) {
          fprintf(stdout,"&\n");
       } else {
          fprintf(stdout, "#-m %d\n", t_style);
          fprintf(stdout, "#-c0\n");		
          if (t_style == 0) {
	     fprintf(stdout, "#-cs 0.5\n");
	     fprintf(stdout, "#-c \"\\%s\"\n", tp_type);
          }
       }	
    for (i = 0; i < data_array.rows; ++i) {
	theoretical_value = theory(data_array.el[i][0], params);
	fprintf(stdout, "%f %f\n", data_array.el[i][0], 
		theoretical_value);
    }
}

#ifndef _NO_PROTO
    void print_results(float fnorm, int info, int num_params, 
		       const double params[])
#else
    print_results(fnorm, info, num_params, params)
        float fnorm;
        int info;
        int num_params;
        double params[];
#endif
{
    int i;
    
    fprintf(stderr, "\nFinal L2 norm of the residuals:	    %f", 
	    fnorm);
    fprintf(stderr, "\nInfo:	");
    fprintf(stderr, "%d", info);
    fprintf(stderr, "\n\nValues of parameters:");
    for (i = 0; i < num_params; ++i)
	fprintf(stderr, "\nParameter[%d] = %f", i, params[i]);
    fprintf(stderr, "\nEnd\n");
}
