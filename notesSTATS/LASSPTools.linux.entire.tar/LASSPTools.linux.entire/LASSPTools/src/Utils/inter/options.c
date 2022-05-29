#include "menudef.h"
#include "iodefs.h"
#include "interdef.h"

extern double xmin, xmax, ymin, ymax;
extern int xminset, yminset, xmaxset, ymaxset;
extern int automatic, errorson, ignoreaxis;
extern int moviemode;
extern double autobase, autostep;
extern int moveflag, plotted;
extern int xtickflag, ytickflag;
extern double xtickspace, ytickspace;
extern int xcol;
extern int inputskip,inputspacing;
static char *argptr;
extern double atof();
extern int atoi();

limx() {
    if((argptr = arg()) == NULL) {
	if(plotted) {       /* If a curve has been plotted */
	    fprintf(stderr,"X range is %e to %e\n",xmin, xmax);
	    xmin = dgetd("minimum x", xmin);
	    xmax = dgetd("maximum x", xmax);
	}
	else {
	    xmin = dget("minimum x");
	    xmax = dget("maximum x");
	}
    }
    else {
	xmin = atof(argptr);
	if((argptr = arg()) == NULL) {
	    if(plotted) {
		fprintf(stderr,"X range is %e to %e\n",xmin, xmax);
		xmax = dgetd("maximum x", xmax);
	    }
	    else
		xmax = dget("maximum x");
	}
	else
	    xmax = atof(argptr);
    }

    if(!xtickflag)
	printf("#x %e %e\n",xmin,xmax);
    else
	printf("#x %e %e %e\n",xmin,xmax,xtickspace);

    xminset = xmaxset = 1;
    return 1;
}

limy() {
    if((argptr = arg()) == NULL) {
	if(plotted) {       /* If a curve has been plotted */
	    fprintf(stderr,"Y range is %e to %e\n",ymin, ymax);
	    ymin = dgetd("minimum y", ymin);
	    ymax = dgetd("maximum y", ymax);
	}
	else {
	    ymin = dget("minimum y");
	    ymax = dget("maximum y");
	}
    }
    else {
	ymin = atof(argptr);
	if((argptr = arg()) == NULL) {
	    if(plotted) {
		fprintf(stderr,"Y range is %e to %e\n",ymin, ymax);
		ymax = dgetd("maximum y", ymax);
	    }
	    else
		ymax = dget("maximum y");
	}
	else
	    ymax = atof(argptr);
    }

    if(!ytickflag)
	printf("#y %e %e\n",ymin,ymax);
    else
	printf("#y %e %e %e\n",ymin,ymax,ytickspace);

    yminset = ymaxset = 1;
    return 1;
}

/**********************************************************/

setxmin() {
    if((argptr=arg()) == NULL) {
	if(plotted)
	    xmin = dgetd("minimum x", xmin);
	else
	    xmin = dget("minimum x");
    }
    else
	xmin = atof(argptr);

    xminset = 1;
    if(xmaxset) {
	if(!xtickflag)
	    printf("#x %e %e\n",xmin,xmax);
	else
	    printf("#x %e %e %e\n",xmin,xmax,xtickspace);
    }
    return 1;
}

setxmax() {
    if((argptr=arg()) == NULL) {
	if(plotted)
	    xmax = dgetd("maximum x", xmax);
	else
	    xmax = dget("maximum x");
    }
    else
	xmax = atof(argptr);

    xmaxset = 1;
    if(xminset) {
	if(!xtickflag)
	    printf("#x %e %e\n",xmin,xmax);
	else
	    printf("#x %e %e %e\n",xmin,xmax,xtickspace);
    }
    return 1;
}

setymin() {
    if((argptr=arg()) == NULL) {
	if(plotted)
	    ymin = dgetd("minimum y", ymin);
	else
	    ymin = dget("minimum y");
    }
    else
	ymin = atof(argptr);

    yminset = 1;
    if(ymaxset) {
	if(!ytickflag)
	    printf("#y %e %e\n",ymin,ymax);
	else
	    printf("#y %e %e %e\n",ymin,ymax,ytickspace);
    }
    return 1;
}

setymax() {
    if((argptr=arg()) == NULL) {
	if(plotted)
	    ymax = dgetd("maximum y", ymax);
	else
	    ymax = dget("maximum y");
    }
    else
	ymax = atof(argptr);

    ymaxset = 1;
    if(yminset) {
	if(!ytickflag)
	    printf("#y %e %e\n",ymin,ymax);
	else
	    printf("#y %e %e %e\n",ymin,ymax,ytickspace);
    }
    return 1;
}

/**********************************************************/

extern double xrangemin, xrangemax, yrangemin, yrangemax;
extern int xrangeminset, yrangeminset, xrangemaxset, yrangemaxset;

setxrangemin() {
    if((argptr = arg()) == NULL)
	xrangemin = dgetd("minimum x to plot", xrangemin);
    else
	xrangemin = atof(argptr);
    xrangeminset = 1;
    return 1;
}

setxrangemax() {
    if((argptr = arg()) == NULL)
	xrangemax = dgetd("maximum x to plot", xrangemax);
    else
	xrangemax = atof(argptr);
    xrangemaxset = 1;
    return 1;
}

setyrangemin() {
    if((argptr = arg()) == NULL)
	yrangemin = dgetd("minimum y to plot", yrangemin);
    else
	yrangemin = atof(argptr);
    yrangeminset = 1;
    return 1;
}

setyrangemax() {
    if((argptr = arg()) == NULL)
	yrangemax = dgetd("maximum y to plot", yrangemax);
    else
	yrangemax = atof(argptr);
    yrangemaxset = 1;
    return 1;
}

unsetxrange() {
    xrangeminset = xrangemaxset = 0;
    return 1;
}

unsetyrange() {
    yrangeminset = yrangemaxset = 0;
    return 1;
}

/**********************************************************/

xtickmark() {
    fprintf(stderr,"Note: Limits must be set manually to set tick spacing.\n");
    if((argptr=arg()) == NULL)
	xtickspace = dget("distance between x tickmarks");
    else
	xtickspace = atof(argptr);
    xtickflag = (xtickspace != 0.0);
    return 1;
}

ytickmark() {
    fprintf(stderr,"Note: Limits must be set manually to set tick spacing.\n");
    if((argptr=arg()) == NULL)
	ytickspace = dget("distance between y tickmarks");
    else
	ytickspace = atof(argptr);
    ytickflag = (ytickspace != 0.0);
    return 1;
}

xtickfull() {
    puts("#tsx 1");
    return 1;
}

ytickfull() {
    puts("#tsy 1");
    return 1;
}

xticksemi() {
    puts("#tsx 2");
    return 1;
}

yticksemi() {
    puts("#tsy 2");
    return 1;
}

xticknone() {
    puts("#tsx 0");
    return 1;
}

yticknone() {
    puts("#tsy 0");
    return 1;
}

/**************************************************************/

nonumberx() {
    puts("#nx");
    return 1;
}

nonumbery() {
    puts("#ny");
    return 1;
}

clip_on() {
    puts("#clip 1");
    return 1;
}

clip_off() {
    puts("#clip 0");
    return 1;
}

/**************************************************************/

autoset() {             /* automatic abscissas? */
    automatic = 1;
    if(argptr = arg()) autobase = atof(argptr);
    if(argptr = arg()) autostep = atof(argptr);
    return 1;
}

transpose() {
    printf("#t\n");
    return 1;
}

errorbars() {
    if(errorson) {
	printf("#e0\n");
	errorson = 0;
	fprintf(stderr,"Error bars are turned off.\n");
    }
    else {
	printf("#e\n");
	errorson = 1;
	fprintf(stderr,"Error bars are turned on.\n");
    }
    return 1;
}

movie_mode() {
    moviemode ^= 1;
    fprintf(stderr, "%ssing movie mode.\n", moviemode? "U" : "Not u");
    return 1;
}

/**************************************************************/

logx() {
    printf("#x l\n");
    return 1;
}

logy() {
    printf("#y l\n");
    return 1;
}

logxy() {
    logx();
    logy();
    return 1;
}

/**************************************************************/

/* Line styles */
int line_solid();
static int (*currentstyle)() = line_solid;   /* Last style function called */

line_solid() {
    printf("#m 1\n");
    currentstyle = line_solid;
    return 1;
}

line_dotted() {
    printf("#m 2\n");
    currentstyle = line_dotted;
    return 1;
}

line_dotdashed() {
    printf("#m 3\n");
    currentstyle = line_dotdashed;
    return 1;
}

line_dashed() {
    printf("#m 4\n");
    currentstyle = line_dashed;
    return 1;
}

line_longdashed() {
    printf("#m 5\n");
    currentstyle = line_longdashed;
    return 1;
}

line_none() {
    printf("#m 0\n");
    currentstyle = line_none;
    return 1;
}

line_style() {
    static int style = 1;

    if((argptr=arg()) == NULL)
	style = igetd("line style (0-5)",style);
    else
	style = atof(argptr);

    printf("#m %d\n", style);
    return 1;
}

resetstyle() {
    (*currentstyle)();
}

/**************************************************************/

/* grid styles */

grid_none() {
    puts("#g 0");
    return 1;
}

grid_frame() {
    puts("#g 1");
    return 1;
}

grid_full() {
    puts("#g 2");
    return 1;
}

grid_arrow() {
    puts("#g 3");
    return 1;
}

grid_box() {
    puts("#box");
    return 1;
}

/**************************************************************/

move_up() {
    double distance;

    if((argptr=arg()) == NULL)
	distance = dgetd("distance to move up",0.0);
    else
	distance = atof(argptr);

    printf("#u %e\n",distance);
    return 1;
}

move_right() {
    double distance;

    if((argptr=arg()) == NULL)
	distance = dgetd("distance to move right",0.0);
    else
	distance = atof(argptr);

    printf("#r %e\n",distance);
    moveflag = (distance != 0.0);
    return 1;
}

height() {
    double distance;

    if((argptr=arg()) == NULL)
	distance = dgetd("height of plot",1.0);
    else
	distance = atof(argptr);

    printf("#h %e\n",distance);
    return 1;
}

width() {
    double distance;

    if((argptr=arg()) == NULL)
	distance = dgetd("width of plot",1.0);
    else
	distance = atof(argptr);

    printf("#w %e\n",distance);
    return 1;
}

noclear() {
    printf("#s\n");
    return 1;
}

/**************************************************************/

ignore() {
    if(ignoreaxis) {
	ignoreaxis = 0;
	fprintf(stderr,"AXIS commands in input will be transmitted.\n");
    }
    else {
	ignoreaxis = 1;
	fprintf(stderr,"AXIS commands in input will be ignored.\n");
    }
    return 1;
}

/**************************************************************/

xcolumn() {
    int col;
    if((col = getcol()) == -1)
	xcol = igetd("column in which to find x",1) - 1;
    else
	xcol = col - 1;
    automatic = 0;
    resetgetcol();
    return 1;
}

/**************************************************************/

set_inputspacing() {
    if((argptr=arg()) == NULL)
	inputspacing = igetd("spacing of input lines in data file", inputspacing);
    else
	inputspacing = atoi(argptr);
    return 1;
}

set_inputskip() {
    if((argptr=arg()) == NULL)
	inputskip = igetd("number of lines to skip at start of file",
	    inputskip);
    else
	inputskip = atoi(argptr);
    return 1;
}

/****************************************************************/

aspect() {
    static double asp = 1.0;

    if((argptr=arg()) == NULL)
	asp = dgetd("aspect ratio",asp);
    else
	asp = atof(argptr);

    printf("#aspect %e\n",asp);
    return 1;
}
