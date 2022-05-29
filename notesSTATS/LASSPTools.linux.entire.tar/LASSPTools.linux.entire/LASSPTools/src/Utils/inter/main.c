char version[] = "Version 3.2 + 7 epsilon";
char copyright[] = "Copyright Stephen A. Langer 1988-1993";

#include "menudef.h"
#include "interdef.h"
#include "iodefs.h"
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef sgi
#include <getopt.h>
#endif

FILE *inputfile, *getfilename(), *savefile;
FILE *getnextfile();
int nfiles;
char **filelist;
static char *argptr;   /* pointer to command argument */
int save = 0;   /* Save commands ? */
char *filter = NULL;  /* input filter */

int automatic, plotted, errorson, ignoreaxis;   /* flags */
double autobase = 0.0; /* for automatic abscissas */
double autostep = 1.0; /* for automatic abscissas */
int returnplot;
int moviemode;
int fileplotted;
int moveflag;
int hbins;
double scalefactor, shift, shiftincrement, hist_norm;
int weighthist, wcol;

int inputskip, inputspacing;

int ncols = 0;
int xcol;
double xmin, xmax, ymin, ymax;
int xminset, xmaxset, yminset, ymaxset;
double xtickspace, ytickspace;
int xtickflag, ytickflag;

CURVE **curve = 0;
DATAPT **currentpt = 0;

void *malloc(), *calloc();
char *myfgets(), *dupstr();
FILE *addfilter();

/**************************************************************/

void usage(prog)
char *prog;
{
    fprintf(stderr, "Usage: %s [options] [--] [-] [files]\n", prog);
    fputs("Options:\n",  stderr);
    fputs("       -x          Don't read .interrc file\n", stderr);
    fputs("       -X <file>   Read <file> instead of .interrc\n",  stderr);
    fputs("       -e <command> Execute <command>\n", stderr);
    fputs("       -i          Don't ignore axis commands in input\n", stderr);
    fputs("       -f <filter> Read data files through <filter>\n", stderr);
    fputs("       -a          Read all data files immediately\n", stderr);
    fputs("       -           Read data from standard input\n", stderr);
}

/**************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    extern MENU rootmenu;
    int norcfile = 0;
    char *comfile = NULL;
    char *cmdarg = NULL;
    int autoappend = 0; /* read all data files on command line immediately? */
    int c;
    extern char *optarg;
    extern int optind;

    reset();

    /* parse arguments */
    while((c = getopt(argc, argv, "xX:if:ae:")) != -1) {
	switch(c) {
	    case 'x':                   /* Don't read .interrc */
		norcfile = 1;
		break;
	    case 'X':                   /* Read arg instead of .interrc */
		comfile = dupstr(optarg);
		break;
	    case 'i':
		ignoreaxis = 0;         /* Transmit axis commands */
		break;
	    case 'f':
		filter = dupstr(optarg);
		break;
	    case 'a':
		autoappend = 1;
		break;
	    case 'e':  /* initial commands to be executed */
		cmdarg = dupstr(optarg);
		break;
	    case '?':
		usage(argv[0]);
		exit(1);
	}
    }
 
    messages();

    if ((nfiles = argc - optind) > 0) { /* File name is on command line */
        fprintf(stderr,"\n%d file%s to read\n", nfiles, (nfiles>1 ? "s":""));
        filelist = &argv[optind];
        next_file(0);    /* read file from command line */
        if(autoappend) (void) appendallfiles();
    }

    if(comfile)
       runcomfile(comfile);
    else if(!norcfile)
       runrcfile();

    if(cmdarg) setcommandline(cmdarg);
    
    while(!eoffound)    /* Process commands until EOF found */
        commands(&rootmenu);
}

/**************************************************************/

namefile() {    /* Get a file name, and read the file */
    if((inputfile = getfilename("data","r")) != NULL)
       readfile(0);
    return 1;
}

appendfile() {    /* Get a file name, and read the file */
    if((inputfile = getfilename("data","r")) != NULL)
       readfile(1);
    return 1;
}


/**************************************************************/

include_file() {        /* Get a file name, and copy it verbatim */
    FILE *file;
    if(file = getfilename("axis","r")) transmit(file);
    return 1;
}

include_next() {
    FILE *file;
    if(file = getnextfile()) transmit(file);
    return 1;
}

/**************************************************************/

nextfile() {
    if(!fileplotted) {
       fputs("You haven't plotted anything from the current file!\n", stderr);
       if(!lgetd("Do you really want to go to the next file?", 0)) return 1;
    }
    next_file(0);
    return 1;
}

appendnextfile() {
    next_file(1);
    return 1;
}

appendallfiles() {
    while(nfiles > 0) next_file(1);
    return 1;
}

next_file(appendflag) /* Read the next file named on the command line */
int appendflag;
{
    if(inputfile = getnextfile())
	readfile(appendflag);
}

/**************************************************************/

run_file() {    /* Get a file name, and execute commands from it */
    FILE *frun;
    int saved = save;   /* current save flag */
    save = 0;           /* Don't save commands while running a file */
    if((frun = getfilename("command","r")) != NULL) interrunfile(frun);
    save = saved;               /* Restore save flag */
    return 1;
}

/**************************************************************/

static FILE *getnextfile() {  /* Open the next file on the command line */ 
    char *filename;
    FILE *file;
    if(--nfiles < 0) {
	fputs(" *** No more files to read! ***\n", stderr);
	return NULL;
    }
    filename = *filelist++;
    if(!strcmp(filename, "-")) {
	fputs("Reading standard input...\n", stderr);
	return stdin;
    }
    file = fopen(filename, "r");
    if(!file) {
	fprintf(stderr," @$#^&!! I can't find file %s ***\n", filename);
	return NULL;
    }
    fprintf(stderr, " Reading file %s...\n", filename);
    return file;
}

/**************************************************************/

FILE *getfilename(type,mode)    /* Get the file name, and open the file */
char *type;     /* type of file to prompt for */
char *mode;
{
    char ftype[100];
    FILE *file;
    if((argptr= arg()) == NULL) /* ask explicitly */
        file = opfile(strcat(strcpy(ftype,type)," file name"), mode);
    else {              /* get filename from command line */
        file = fopen(argptr, mode);
        if(file == NULL) {
            fprintf(stderr," @$&^*!! I can't find file %s ***\n",argptr);
            trashline();
        }
    }
    return(file);
}

/**************************************************************/

save_file() {      /* vestigial */

    if(save) save_off();

    /* Start saving commands, if file can be opened */
    if((savefile = getfilename("save","a")) != NULL)
        save = 1;
    return 1;
}

save_off() {    /* Stop saving commands */      /* vestigial */
    if(save) {
        fclose(savefile);
        save = 0;
    }
    return 1;
}

/**************************************************************/

plot() {                /* Plot points */

    int i;

    if(inputfile == NULL) {     /* Is there a current file? */
        fprintf(stderr," No input file!\n");
        trashline();
        return 1;
    }

    if((i = getcol()) == 0) {
        plotcol( igetd("column to plot",(automatic? 1 : 2)) );
        return 1;
    }
    if(i == -1) return 1;  /* Error in getcol */

    do {
        plotcol(i);
    } while ((i = getcol()) > 0);

    return 1;
}

/**************************************************************/

retplot() {               /* Plot y[n] vs. y[n-1] */

    int tempauto = automatic;

    returnplot = 1;
    automatic = 0;

    printf("#m 0\n"); /* Turn off lines */
    plot();

    automatic = tempauto;
    returnplot = 0;
    resetstyle();
    return 1;
}

/**************************************************************/

int oktoplot();

plotcol(col)    /* Really plot points */
int col;
{
    int i;
    double x, y;
    DATAPT *xptr, *yptr, *eptr;
    int skipcount = 0;
    int spacecount = 0;

    if(col == 0) return;
    if(col > ncols) {
        fprintf(stderr,"There is no column %d!\n",col);
        return;
    }

    printf("#\n");
    if(moviemode) printf("#axes\n#clear\n");

    xptr = curve[xcol]->firstpt;        /* pointers to first data point */
    yptr = curve[col-1]->firstpt;
    if(errorson) eptr = curve[col]->firstpt;

     /* Skip data lines if desired */
     while(skipcount++ < inputskip && xptr && yptr && (!errorson || eptr)) {
        xptr = xptr->next;
        yptr = yptr->next;
        if(errorson) eptr = eptr->next;
     }

    if(!automatic && !returnplot)                 /* Write x and y */
        while (xptr && yptr && (!errorson || eptr)) {
           if(++spacecount == inputspacing) {
               spacecount = 0;     /* plot this line */
               x = xptr->x;
               y = yptr->x;
               if(oktoplot(x, y)) {
                  printf("%16.9le %16.9le", x, y);      /* x and y */
                  if(errorson) {
                      printf(" %16.9le",eptr->x);  /* error bars */
                  }
                  printf("\n");
                  if(x < xmin) xmin = x;      /* Find limits */
                  if(y < ymin) ymin = y;
                  if(x > xmax) xmax = x;
                  if(y > ymax) ymax = y;
               }
            }
            xptr = xptr->next;
            yptr = yptr->next;
            if(errorson) eptr = eptr->next;
        }
    else if(automatic) {          /* Write one column (automatic absc.) */
        double autox = autobase;
        while(yptr) {
           if(++spacecount == inputspacing) {
               spacecount = 0;
               y = yptr->x;
               if(oktoplot(autox, y)) {
                  printf("%16.9le %16.9le\n",autox, y);
                  if(autox < xmin) xmin = autox;
                  if(autox > xmax) xmax = autox;
                  if(y < ymin) ymin = y;
                  if(y > ymax) ymax = y;
               }
               autox += autostep;
            }
            yptr = yptr->next;
        }
    }
    else {           /* Plot col[n] vs. col[n-1] */
       while(yptr) {
           if(++spacecount == inputspacing) {
               spacecount = 0;
               x = yptr->x;
               yptr = yptr->next;
               if(!yptr) break;
               y = yptr->x;
               if(oktoplot(x, y)) {
                  printf("%16.9le %16.9le\n", x, y);
                  if(x < xmin) xmin = x;      /* Find limits */
                  if(y < ymin) ymin = y;
                  if(x > xmax) xmax = x;
                  if(y > ymax) ymax = y;
               }
           }
           else /* yptr gets moved if pt is plotted too, so 'else' is needed */
              yptr = yptr->next;
       }
    }

    plotted = 1;                /* A curve has been plotted */
    fileplotted = 1;
}

/******************************************************************/

double xrangemin = 0.0;
double xrangemax = 0.0;
double yrangemin = 0.0;
double yrangemax = 0.0;
int xrangeminset = 0;
int yrangeminset = 0;
int xrangemaxset = 0;
int yrangemaxset = 0;

oktoplot(x, y)
double x, y;
{
   if(xrangeminset && x <= xrangemin) return 0;
   if(yrangeminset && y <= yrangemin) return 0;
   if(xrangemaxset && x >= xrangemax) return 0;
   if(yrangemaxset && y >= yrangemax) return 0;
   return 1;
}

/**************************************************************/

char dataline[DATALINE];
char *dataptr;

int readfile(appendflag)
/* read columns of data into data structures */
{
    int firstline = 1;
    int icol;
    double datum;
    DATAPT *dpt;
    int oldcols;  /* number of columns retained from previous reads */
    int oldnpts;  /* length of columns retained */

    fileplotted = 0;
    if(!appendflag) freespace(0); /* delete previously read file from memory */
    if(filter) inputfile = addfilter(filter, inputfile, "r");
    if(!inputfile) return;
    oldcols = ncols; /* ncols is reset by freespace() */

    /* read lines from file */
    while(myfgets(dataline,DATALINE,inputfile) != NULL) {

        dataptr = dataline;                     /* start of line */
        while(isspace(*dataptr)) dataptr++;     /* skip spaces */

        if(*dataptr == '\0') continue;  /* skip blank lines */

        if(*dataptr == '#') {           /* transmit axis commands verbatim */
            if(!ignoreaxis) printf("%s\n",dataline);
            continue;
        }

        /* Ignore comments (lines not beginning with numbers) */
        if(!(isdigit(*dataptr) || (*dataptr == '.') || (*dataptr == '-')) )
            continue;

        /* Read numbers from data line */

        if(firstline) { /* first line of data, set up curves */
            while (readnumber(&datum)) ncols++; /* count number of columns */
            dataptr = dataline;                 /* reset ptr to reread line */
            while(isspace(*dataptr)) dataptr++; /* skip spaces */
            if(!appendflag)
               curve = (CURVE **) calloc((unsigned) ncols, sizeof(CURVE*));
            else {
               curve = (CURVE **) realloc((void *) curve, ncols*sizeof(CURVE*));
	       oldnpts = curve[oldcols-1]->npts;
	    }
            currentpt = (DATAPT **) calloc((unsigned) ncols, sizeof(DATAPT*));

            icol = oldcols;
            while(readnumber(&datum) && icol<ncols) {
                curve[icol] = (CURVE *) malloc( sizeof(CURVE) );
                curve[icol]->npts = 1;
                dpt = (DATAPT *) malloc( sizeof(DATAPT) );
                curve[icol]->firstpt = dpt;
                dpt->x = datum;
                dpt->next = (DATAPT *) NULL;
                currentpt[icol] = dpt;
                icol++;
            }
            firstline = 0;
            ncols = icol; /* is this line needed? */
        }
        else {          /* Not first line of data */
            icol = oldcols;
            while(readnumber(&datum) && icol < ncols) {
                curve[icol]->npts++;
                dpt = (DATAPT *) malloc( sizeof(DATAPT) );
                currentpt[icol]->next = dpt;
                dpt->x = datum;
                dpt->next = (DATAPT *) NULL;
                currentpt[icol] = dpt;
                icol++;
            }
            if(icol < ncols) ncols = icol;
        }
    }

    if(ncols == oldcols) {
        fprintf(stderr," Nothing in this file!\n");
        fileplotted = 1;
    }
    else {
        fprintf(stderr, " file has %d lines and %d column%s of data\n",
            curve[oldcols]->npts,ncols-oldcols,(ncols-oldcols==1?"":"s"));
        if(appendflag) {
            if(ncols==oldcols+1)
                fprintf(stderr, " New data is in column %d\n", ncols);
            else
                fprintf(stderr, " New data is in columns %d through %d\n", 
                         oldcols+1, ncols);
	    if(oldnpts != curve[oldcols]->npts) fputs(
	       " *** Warning! new and old data files have different lengths!\n",
	       stderr);
        }
    }
}

int readnumber(number)
double *number;
{
    extern double atof();
    char c, *ptr;

    while(isspace(*dataptr)) dataptr++; /* Skip spaces */
    if(!isdigit(c= *dataptr) && c!='-' && c!='.')
        return(0);              /* Next item is not a number */
    ptr = dataptr;                              /* start of number */
    while((c= *ptr)!='\0' && !isspace(c)) {
        if(c=='d' || c=='D') {  /* Correct for real*8 format */
            *ptr = 'e';
            break;
        }
        ptr++;
    }
    *number = (double) atof(dataptr);   /* read number */
    while((c= *dataptr)!='\0' && !isspace(c)) dataptr++; /* skip number */
    return(1);
}

/**************************************************************/

/* duplicate a column of data.
 * This routine is here, instead of in options.c, because it
 * uses the same allocation procedures as readfile().
 */
static void dupcol();

int dupcolumn() {
    int col;
    if(ncols == 0) {
	fputs("No data to duplicate!\n", stderr);
	trashline();
	return 1;
    }
    if((col = getcol()) == 0) {
	dupcol(igetd("column to duplicate", ncols));
    }
    else if(col > 0) {
	do {
	    dupcol(col);
	} while((col = getcol()) > 0);
    }
    return 1;
}

static void dupcol(col)
int col;
{
    DATAPT *oldpt, *newpt;
    CURVE *newcurve, *oldcurve;
    int ncol = ncols;
    
    if(col <= 0 || col > ncols) return;
    ncols++;
    curve = (CURVE **) realloc((void *) curve, ncols*sizeof(CURVE*));
    newcurve = curve[ncols-1] = (CURVE *) malloc(sizeof(CURVE));
    oldcurve = curve[col-1];
    newcurve->npts = oldcurve->npts;
    oldpt = oldcurve->firstpt;
    if(!oldpt) return;
    newpt = newcurve->firstpt = (DATAPT *) malloc(sizeof(DATAPT));
    newpt->x = oldpt->x;
    newpt->next = (DATAPT *) 0;
    while(oldpt = oldpt->next) {
	newpt->next = (DATAPT *) malloc(sizeof(DATAPT));
	newpt = newpt->next;
	newpt->x = oldpt->x;
	newpt->next = (DATAPT *) 0;
    }
    fprintf(stderr, " New data is in column %d\n", ncols);
}

/**************************************************************/

transmit(file)     /* Transmit file verbatim */
FILE *file;
{
    if(filter) file = addfilter(filter, file, "r");
    if(!file) return;
    while(myfgets(dataline, DATALINE, file) != NULL)
        printf("%s\n",dataline);
}

/**************************************************************/

done() {
    if(!fileplotted) {
       fputs(" You haven't plotted anything from the current file!\n", stderr);
       if(!lgetd("Are you really done?", 0))
           return 1;
    }
    fflush(stdout);
    exit(0);
    /*NOTREACHED*/
}

/**************************************************************/

freespace(flag)   /* Free allocated space for curves */
int flag;
{
    int i;
    DATAPT *dpt, *temp;

    if(flag) {
        for(i=0; i<ncols; i++) {
            dpt = curve[i]->firstpt;
            free((char *) curve[i]);
            while (dpt != NULL) {
                temp = dpt;
                dpt = dpt->next;
                free((char *) temp);
            }
        }
        if(curve) free((char *) curve);
        if(currentpt) free((char *) currentpt);
    }
    
    curve = 0;
    currentpt = 0;
    ncols = 0;
}

forgetdata() {
    freespace(1);
    return 1;
}

/**************************************************************/

reset() {
    freespace(1);
    xcol = 0;                           /* which column is x? */
    xmin = ymin = HUGE;                 /* limits on x */
    xmax = ymax = -HUGE;                /* limits on y */
    xminset = xmaxset = yminset = ymaxset = 0; /* have limits been set ? */
    automatic = 0;                      /* automatic abscissae? */
    returnplot = 0;                     /* plot y[n] vs. y[n-1] */
    moviemode = 0;                      /* Clear screen before each curve? */
    plotted = 0;                        /* has a curve been plotted? */
    fileplotted = 1;                /* has curve been plotted from this file?*/
    filter = NULL;
    errorson = 0;                       /* are there error bars? */
    ncols = 0;                  /* number of columns that have been read */
    inputfile = NULL;                   /* pointer to input file */
    ignoreaxis = 1;     /* Should axis commands be ignored in input?*/
    moveflag = 0;                       /* Has the plot been moved? */
    hbins = 100;                        /* number of histogram bins */
    hist_norm = 1.;                     /* histogram normalization */
    weighthist = 0;                     /* make weighted histogram? */
    scalefactor = 1.;                   /* scale factor applied by 'adjust' */
    shift = 0.;                         /* shift applied by 'adjust' */
    shiftincrement = 0.;                /* amount to increase shift */
    save = 0;                           /* save commands ? */
    inputspacing = 1;                   /* spacing of lines in input file */
    inputskip = 0;                      /* Initial lines to skip */
    xrangeminset = xrangemaxset = yrangeminset = yrangemaxset = 0;
                                        /* Have ranges been set? */
    return 1;
}

/**************************************************************/

dummyfn() {     /* Null response */
    return 1;
}
