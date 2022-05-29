
/* Originally UNIX "graph" code.   					*
*  Modified june 83 to include SLAC UGS whistles and bells.  John	*
*  Richardson and Bob Pearson						*
*  Modified June 84 - new string formats and change options from	*
*  the input data.   Doug Toussaint					*
*  Modified ????  by Dan Sullivan.
*  Modified Dec 89 to allow different tick styles, force solid lines
*  for symbols, make numbers and clipping optional. Steve Langer (SAL)
*  Modified Mar 91 - add rescaling function.  
*  Argument rescale represents the rescaling options 
*   (default to the rescale option is OFF)
*
*
* SUN4's don't seem to like float arguments to functions returning
* structures. I have changed all "float"s to "FLOAT"s, and defined
* "FLOAT" to be "double". The FORMx definitions must be changed
* if FLOAT is changed back to "float".
*                               -- Steve
*
* Modified for movies by JPS.
* Modified Sept. 90 by Mark Ollis
*/

#include <stdio.h>
#include <math.h>

#define ON  (1)
#define OFF (0)

/* To avoid the Sun 4 bug... */
#define FLOAT double
#define FORM1 "%le"
#define FORM2 "%le %le"
#define FORM3 "%le %le %le"

/* coordinate specifications */

struct xy {
   int xlbf;		/* flag:explicit lower bound */
   int xubf;		/* flag:explicit upper bound */
   int xqf;		/* flag:explicit quantum */
   double (*xf)();	/* transform function, e.g. log */
   FLOAT xa, xb;	/* scaling coefficients */
   FLOAT xlb, xub;	/* lower and upper bound */
   FLOAT xquant;	/* quantum */
   FLOAT xoff;		/* screen offset fraction */
   FLOAT xsize;		/* screen fraction */
   int	xbot, xtop;	/* screen coords of border */	
   FLOAT xmult;		/* scaling constant */
   int number;          /* flag: draw numbers  SAL */
   int tickstyle;       /* tick style SAL */
} xd, yd;

/* data specifications */

struct datum {
   FLOAT xv;		/* x coordinate */
   FLOAT yv;		/* y coordinate */
   FLOAT yerr;		/* optional error on y */
   char line_mode;	/* current line mode */
   char err_flag;	/* does data have an error bar */
   char cflag;		/* connect to previous point ? */
   char *lblptr;	/* optional plot symbol or label */
   FLOAT sym_size;	/* size of optional plot symbol or label */
} *xx;

#define LSIZ 1024		/* size of each label buffer */
char *labels;			/* pointer to current label  */
int labelsleft = 0;		/* space left in allocated buffer */

/* axes specifications */

int tick = 50;
int topx = 4000;
int botx = 500;
int leftx = 300;
int topy = 4000;
int boty = 500;
FLOAT absbot;

/* text specifications */

FLOAT global_sym_size = 1.5;		/* default symbol size */
FLOAT lblsize = 1.5;		/* default label character size */
FLOAT titlesize = 2.0;		/* default title character size */
FLOAT xtitlesize = 1.5;		/* default x title character size */
FLOAT ytitlesize = 1.5;		/* default y title character size */
FLOAT titlex = 0.0;
FLOAT titley = 0.0;
FLOAT xtitlex = 0.0;
FLOAT xtitley = 0.0;
FLOAT ytitlex = 0.0;
FLOAT ytitley = 0.0;

int titlexf = OFF;
int titleyf = OFF;
int xtitlexf = OFF;
int xtitleyf = OFF;
int ytitlexf = OFF;
int ytitleyf = OFF;

#define BSIZ 256
char linebuf[BSIZ];
char titlebuf[BSIZ];
char xtitlebuf[BSIZ];
char ytitlebuf[BSIZ];
char *plotsymb;

/* various flags */

int n_points = 0;	/* no. of data points */
int erasf = ON;		/* erase screen ? */
int newframeflag=OFF;	/* another frame for movie? */
int symbf = OFF;	/* default plot symbol in effect ? */
int absf = OFF;		/* supply abscissas ? (can take the value "2") */
int errf = OFF;		/* data has error bars ? */
int transf = OFF;	/* transpose ? */
int lbflag = ON;	/* break after labels ? */
int cflag = ON;		/* connect to previous ? */
int gridst = 1;		/* grid style */
int mode = 1;
FLOAT dx = 1.0;
int clip = ON;           /* Clip?    SAL */
int rescale = OFF;      /* default no-rescaling */ 

/* line styles */

char *modes[] = {
   "solid",
   "solid",
   "dotted",
   "dotdashed",
   "shortdashed",
   "longdashed"
};

/* char *realloc();
   char *malloc();  */
char *copystring();

/* identity function */

double ident (x)
 double x;
{
   return(x);
}

FLOAT modceil (f, t)
 FLOAT f, t;
{
   t = fabs(t);
   if(f < 0.0 && f+t > 0.0) return 0.0; /* ceil defined differently on sun4 */
   return ( ceil(f/t)*t );              /*    ...   SAL */
}

FLOAT modfloor (f, t)
 FLOAT f, t;
{
   t = fabs(t);
   return ( floor(f/t)*t );
}


main (argc, argv)
 char *argv[];
{

/* initializations */

   space (0, 0, 4096, 4096);
   init (&xd);
   init (&yd);
   xd.xsize = yd.xsize = 1.0;
   xx = (struct datum *) malloc((unsigned) sizeof(struct datum));

/* read options and data */

   setopt (argc, argv);

/* setup options */


   do {
      if(rescale == ON)
         {  xd.xmult=1; /* reset multiplying factors: MDO */
            yd.xmult=1; /* MDO */
         }
      readin();
      transpose ();
      getxlim (&xd,xx);
      getylim (&yd,xx);
      xscale (&xd);
      yscale (&yd);

/* output plot(5) commands */

      if (erasf) erase ();
      plot ();
      axes ();
      title ();

/* finish up */

      move (1, 1);
      fflush(stdout);
      if (erasf) {
	 free(xx);
	 xx = (struct datum *) malloc((unsigned) sizeof(struct datum));
	 n_points = 0;
      }
    }  while (newframeflag--);
   return (0);
}


/* initialize xy structure */

init (p)
 struct xy *p;
{
   p->xlb =  (FLOAT) (HUGE);
   p->xub = -(FLOAT) (HUGE);

   p->xf = ident;
   p->xmult = 1;
   p->number = ON;       /* SAL */
   p->tickstyle = 1;     /* SAL */
}

/* parse arguments in argv */

setopt (argc, argv)
 char *argv[];
{
   char *p1, *p2;
   register int i,j;

/* scan the options.  minus signs are optional.
   an options line sets a break! */

   cflag = OFF;

   for (i = 1; i < argc; i++) {

      if (argv[i][0] == '-') argv[i]++;

/* "rescale" option triggers the rescaling each time */

      if (strcmp(argv[i],"rescale") == 0) rescale = ON;

/* "e" and "e0" options toggle error flag */

      else if (strcmp(argv[i],"e") == 0) errf = ON;

      else if (strcmp(argv[i],"e0") == 0) errf = OFF;

/* assorted labels */

/* title string */

      else if (strcmp(argv[i],"lt") == 0) {
         p1 = titlebuf;
         if (++i < argc) {
            p2 = argv[i];
            while (*p1++ = *p2++);
         }
      }

/* x-axis label */

      else if (strcmp(argv[i],"lx") == 0) {
         p1 = xtitlebuf;
         if (++i < argc) {
            p2 = argv[i];
            while (*p1++ = *p2++);
         }
      }

/* y-axis label */

      else if (strcmp(argv[i],"ly") == 0) {
         p1 = ytitlebuf;
         if (++i < argc) {
            p2 = argv[i];
            while (*p1++ = *p2++);
         }
      }

/* title string: x coordinate */

      else if (strcmp(argv[i],"ltx") == 0) {
         if (sscanf(argv[++i],FORM1,&titlex) != 1) badarg();
         titlexf = ON;
      }

/* title string: y coordinate */

      else if (strcmp(argv[i],"lty") == 0) {
         if (sscanf(argv[++i],FORM1,&titley) != 1) badarg();
         titleyf = 1;
      }

/* x-axis label: x coordinate */

      else if (strcmp(argv[i],"lxx") == 0) {
         if (sscanf(argv[++i],FORM1,&xtitlex) != 1) badarg();
         xtitlexf = ON;
      }

/* x-axis label: y coordinate */

      else if (strcmp(argv[i],"lxy") == 0) {
         if (sscanf(argv[++i],FORM1,&xtitley) != 1) badarg();
         xtitleyf = ON;
      }

/* y-axis label: x coordinate */

      else if (strcmp(argv[i],"lyx") == 0) {
         if (sscanf(argv[++i],FORM1,&ytitlex) != 1) badarg();
         ytitlexf = ON;
      }

/* y-axis label: y coordinate */

      else if (strcmp(argv[i],"lyy") == 0) {
         if (sscanf(argv[++i],FORM1,&ytitley) != 1) badarg();
         ytitleyf = ON;
      }

/* title string: size */

      else if (strcmp(argv[i],"lts") == 0) {
         if (sscanf(argv[++i],FORM1,&titlesize) != 1) badarg();
      }

/* x-label: size */

      else if (strcmp(argv[i],"lxs") == 0) {
         if (sscanf(argv[++i],FORM1,&xtitlesize) != 1) badarg();
      }

/* y-label: size */

      else if (strcmp(argv[i],"lys") == 0) {
         if (sscanf(argv[++i],FORM1,&ytitlesize) != 1) badarg();
      }

/* line style */

      else if (strcmp(argv[i],"m") == 0) {
         if (sscanf(argv[++i],"%d",&mode) != 1) badarg();
         if (mode >= sizeof(modes)/sizeof(*modes)) mode = 1;
      }

/* automatic abscissas */

      else if (strcmp(argv[i],"a") == 0) {
         absf = ON;
         dx = 1.0;
         if (i+1 < argc)
             if (sscanf(argv[i+1],FORM1,&dx) != 1) continue;
         i++;
         if (i+1 < argc)
             if (sscanf(argv[i+1],FORM1,&absbot) != 1) continue;
         i++;
         absf = 2;
      }

/* save screen, overlay plot */

      else if (strcmp(argv[i],"s") == 0) erasf = OFF;

/* grid style 0 none, 1 outline, 2 full, 3 arrowed */

      else if (strcmp(argv[i],"g") == 0) {
         if (sscanf(argv[++i],"%d",&gridst) != 1) badarg();
      }

/* tick style 0 none, 1 normal, 2 big only  SAL */

      else if (strcmp(argv[i],"tsx") == 0) {
         if (sscanf(argv[++i],"%d",&xd.tickstyle) != 1) badarg();
      }

      else if (strcmp(argv[i],"tsy") == 0) {
         if (sscanf(argv[++i],"%d",&yd.tickstyle) != 1) badarg();
      }

/* number axes?  SAL */

     else if (strcmp(argv[i], "nx") == 0) xd.number = OFF;
     else if (strcmp(argv[i], "ny") == 0) yd.number = OFF;


/* plain box, no ticks or numbers */

     else if (strcmp(argv[i], "box") == 0) {
        xd.number = yd.number = OFF;
        xd.tickstyle = yd.tickstyle = 0;
        gridst = 1;
     }

/* clip data?  0 = no, 1 = yes */
    
     else if (strcmp(argv[i], "clip") == 0) {
        if(sscanf(argv[++i],"%d",&clip) != 1) badarg();
     }

/* turn off plot symbols */

      else if (strcmp(argv[i],"c0") == 0) symbf = OFF;

/* symbol size */

      else if (strcmp(argv[i],"cs") == 0) {
         if (sscanf(argv[++i],FORM1,&global_sym_size) != 1) badarg();
      }

/* character(s) for plotting */

      else if (strcmp(argv[i],"c") == 0) {
         if (++i >= argc) badarg();
         symbf = ON;
         j = unix_to_ugs(argv[i]);
         plotsymb = copystring(argv[i],j);
      }

/* transpose */

      else if (strcmp(argv[i],"t") == 0) transf = ON;

/* set or unset breaks after labels */

      else if (strcmp(argv[i],"lb1") == 0) lbflag = ON;

      else if (strcmp(argv[i],"lb0") == 0) lbflag = OFF;

/* archaic version of lb0, like "graph" */

      else if (strcmp(argv[i],"b") == 0) lbflag = OFF;

/* x limits */

      else if (strcmp(argv[i],"x") == 0) {
         if (strcmp(argv[i+1],"l") == 0) {
            i++;
            xd.xf = log10;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&xd.xlb) == 1) {
            i++;
            xd.xlbf = ON;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&xd.xub) == 1) {
            i++;
            xd.xubf = ON;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&xd.xquant) == 1) {
            i++;
            xd.xqf = ON;
         }
      }

/* y limits */

      else if (strcmp(argv[i],"y") == 0) {
         if (strcmp(argv[i+1],"l") == 0) {
            i++;
            yd.xf = log10;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&yd.xlb) == 1) {
            i++;
            yd.xlbf = ON;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&yd.xub) == 1) {
            i++;
            yd.xubf = ON;
         }
         if (i+1 < argc && sscanf(argv[i+1],FORM1,&yd.xquant) == 1) {
            i++;
            yd.xqf = ON;
         }
      }

/* fraction of height */

      else if (strcmp(argv[i],"h") == 0) {
         if (sscanf(argv[++i],FORM1,&yd.xsize) != 1) badarg();
      }

/* fraction of width */

      else if (strcmp(argv[i],"w") == 0) {
         if (sscanf(argv[++i],FORM1,&xd.xsize) != 1) badarg();
      }

/* move to right */

      else if (strcmp(argv[i],"r") == 0) {
         if (sscanf(argv[++i],FORM1,&xd.xoff) != 1) badarg();
      }

/* move up */

      else if (strcmp(argv[i],"u") == 0) {
         if (sscanf(argv[++i],FORM1,&yd.xoff) != 1) badarg();
      }

      else badarg();
   }
}

readin ()
{
   register char *t, *tt;
   char *index(), *argv[32];
   struct datum *temp;
   int i, qf, argc; 

   if (absf == ON) {
      if (xd.xlbf)
         absbot = xd.xlb;
      else if (xd.xf == log10)
         absbot = 1.0;
   }

   for ( ; ; ) {

/* End of file or garbage? */

      if (getline (linebuf) == -1) return;

/* find first nonwhite character */

      t = linebuf;
      while (*t == ' ' || *t == '\t') t++;

/* End this input, and output a frame of a movie? */

      if ( (*t == '#')&&(*(t+1) =='Q') ) {
	newframeflag = ON;
	return;
      }

/* Control line: make fake argc and argv, call setopt() again */

      if (*t == '#') {

         i = 0; qf = OFF; argc = 1;

/* scan line */

         for (t++; *t != '\0'; t++) {

/* replace white space by nulls except in quotes. i=0 means
   next nonwhite is new*/

            switch (*t) {

            case ' ':

            case '\t':
               if (!qf) {
                  *t = '\0';
                   i = 0;
               }
               break;

 /* start or finish a quotation */

            case '"':
               if (!qf)
                  qf = ON;
               else {
                  *t = '\0';
                  qf = OFF;
               }
               break;

            default:
               if (i == 0) argv[argc++] = t;
               if (argc > 32) break;
               i = 1;   /* i=1 means in middle of arg. */
               break;
            }
         }
         setopt(argc,argv);
      }

      else {

/* make a new structure for next value */

         temp = (struct datum *) realloc((char*)xx,
         (unsigned) (n_points+1)*sizeof(struct datum));
         if (temp == NULL) return;
         xx = temp;

/* read in the value */

         if (absf) {
            xx[n_points].xv = n_points*dx + absbot;

            if (errf) {
               if (sscanf (linebuf, FORM2, &xx[n_points].yv,
                      &xx[n_points].yerr) != 2) continue;
            }
            else {
               if (sscanf (linebuf, FORM1, &xx[n_points].yv) != 1) continue;
            }
         }
         else {
            if (errf) {
               if (sscanf (linebuf, FORM3, &xx[n_points].xv,
                &xx[n_points].yv, &xx[n_points].yerr) != 3) continue;
            }
            else {
               if (sscanf (linebuf, FORM2, &xx[n_points].xv,
                        &xx[n_points].yv) != 2) continue;
            }
         }

/* label ? */

         if ((t = index (linebuf, '"')) != NULL) {

            if ((tt = index (++t, '"')) != NULL) *tt = '\0';
            i = unix_to_ugs (t);
            xx[n_points].lblptr = copystring (t,i);
            xx[n_points].sym_size = global_sym_size;
         }
         else if (symbf) {
            xx[n_points].lblptr = plotsymb;
            xx[n_points].sym_size = global_sym_size;
         }
         else {
            xx[n_points].lblptr = NULL;
         }

/* decide whether point should be connected to previous */

         xx[n_points].cflag = cflag;
         cflag = ON;

/* if a label break is set and there is a symbol or
   an error bar, set break for next point */

         if (lbflag  && (xx[n_points].lblptr != NULL || errf)) cflag = OFF;

         xx[n_points].line_mode = mode;
         xx[n_points].err_flag = errf;
         n_points++;
      }
   }
}

/* get a line from stdin, put it in string p 
   return length of string, -1 if EOF
*/

getline (p)
 char *p;
{
   register int i, c;

   i = -1;
   while ((c = getchar()) != EOF) {
      if (c == '\n') {
         p[++i] = '\0';
         return (i);
      }
      else
         p[++i] = c;
   }

   if (i >= 0) p[++i] = '\0';

   return (i);
}

/* transpose x and y axes */

transpose ()
{
   register i;
   FLOAT f;
   struct xy t;

   if (!transf) return;

   t = xd; xd = yd; yd = t;

   for (i = 0; i < n_points; i++) {
      if (xx[i].err_flag) {
         fprintf (stderr, "axis: cannot transpose data with error bars.\n");
         exit (1);
      }
      f = xx[i].xv; xx[i].xv = xx[i].yv; xx[i].yv = f;
   }
}

/* copy a string into the label list */

char* copystring (p, length)
 char *p;
{
   register char *temp;
   register i;

   length++;      /* length of string plus null char */

   if (length > labelsleft) {      /* need more space */
      labels = (char *) malloc(LSIZ);
      labelsleft = LSIZ;
      if (labels == NULL) exit (-1);
   }
   temp = labels;
   for (i = 0; i < length; i++) labels[i] = p[i];
   labels += length;
   labelsleft -= length;
   return (temp);
}

/* determine limits on x axis */

getxlim (p, v)
 register struct xy *p;
 register struct datum *v;
{
   register i;

   if (n_points == 0) {
      if ((p->xf) == log10) {
         if (!p->xlbf) p->xlb = 1.0;
         if (!p->xubf) p->xub = 10.0;
      }
      else {
         if (!p->xlbf) p->xlb = 0.0;
         if (!p->xubf) p->xub = 1.0;
      }
   }
   else
      for (i = 0; i < n_points; i++) {
         if (!p->xlbf && p->xlb > v[i].xv) p->xlb = v[i].xv;
         if (!p->xubf && p->xub < v[i].xv) p->xub = v[i].xv;
      }
}

/* determine limits on y axis */

getylim (p, v)
 register struct xy *p;
 register struct datum *v;
{
   register i;
   FLOAT t;

   if (n_points == 0) {
      if (p->xf == log10) {
         if (!p->xlbf) p->xlb = 1.0;
         if (!p->xubf) p->xub = 10.0;
      }
      else {
         if (!p->xlbf) p->xlb = 0.0;
         if (!p->xubf) p->xub = 1.0;
      }
   }
   else
      for (i = 0; i < n_points; i++) {
         if (v[i].err_flag && p->xf == log10) {

            if (!p->xlbf && p->xlb > (v[i].yv * (t = exp(
                    -v[i].yerr/v[i].yv)))) p->xlb = v[i].yv*t;

            if (!p->xubf && p->xub < v[i].yv/t) p->xub = v[i].yv/t;
         }
         else if (v[i].err_flag) {

            if (!p->xlbf && p->xlb > (v[i].yv - (t = v[i].yerr)))
               p->xlb = v[i].yv - t;

            if (!p->xubf && p->xub < (v[i].yv + t)) p->xub = v[i].yv + t;
         }
         else {

            if (!p->xlbf && p->xlb > v[i].yv) p->xlb = v[i].yv;

            if (!p->xubf && p->xub < v[i].yv) p->xub = v[i].yv;
         }
      }
}


struct z {
   FLOAT lb, ub, mult, quant;
} setloglim (), setlinlim ();


setlim (p)
 register struct xy *p;
{
   FLOAT t, delta, sign;
   struct z z;
   int lmark[512], mark[512];
   FLOAT lb, ub;
   int lbf, ubf;

   lb = p->xlb;
   ub = p->xub;
   delta = ub-lb;

/* user supplied quant */

   if (p->xqf) {
      if (delta*p->xquant <= 0 ) badarg();
      return;
   }

   sign = 1;
   lbf = p->xlbf;   /* is user supplied lower bound */
   ubf = p->xubf;   /* is user supplied upper bound */
   if (delta < 0) {
      sign = -1;
      t = lb;
      lb = ub;
      ub = t;
      t = lbf;
      lbf = ubf;
      ubf = t;
   }
   else if (delta == 0) {
      if (ub > 0) {
         ub = 2*ub;
         lb = 0;
      } 
      else
         if (lb < 0) {
            lb = 2*lb;
            ub = 0;
         } 
         else {
            ub = 1;
            lb = -1;
         }
   }

   if ((p->xf) == log10 && lb > 0 && ub > lb) {
      z = setloglim (lbf, ubf, lb, ub);
      p->xlb = z.lb;
      p->xub = z.ub;
      p->xmult *= z.mult;
      p->xquant = z.quant;
      if (setmark (mark, lmark, p) < 2) {
         p->xqf = lbf = ubf = ON;
         lb = z.lb; ub = z.ub;
      }
      else
         return;
   }

   z = setlinlim (lbf, ubf, lb, ub);
   if (sign > 0) {
      p->xlb = z.lb;
      p->xub = z.ub;
   }
   else {
      p->xlb = z.ub;
      p->xub = z.lb;
   }

   p->xmult *= z.mult;
   p->xquant = sign*z.quant;
}

struct z setloglim (lbf, ubf, lb, ub)
 FLOAT lb, ub;
{
   FLOAT r, s, t;
   struct z z;

   for (s = 1.0; lb*s < 1.0; s *= 10.0) ;

   lb *= s;
   ub *= s;

   for (r = 1.0; 10*r <= lb; r *= 10.0) ;
   for (t = 1.0; t < ub; t *= 10.0) ;

   z.lb = !lbf ? r : lb;
   z.ub = !ubf ? t : ub;

   if (ub/lb < 100) {
      if (!lbf) {
         if (lb >= 5*z.lb)
            z.lb *= 5;
         else if (lb >= 2*z.lb)
            z.lb *= 2;
      }
      if (!ubf) {
         if (ub*5 <= z.ub)
            z.ub /= 5;
         else if (ub*2 <= z.ub)
            z.ub /= 2;
      }
   }

   z.mult = s;
   z.quant = r;

   return (z);
}

struct z setlinlim (lbf, ubf, xlb, xub)
 int lbf, ubf;
 FLOAT xlb, xub;
{
   struct z z;
   FLOAT r, s, delta;
   FLOAT ub, lb;

loop:

   ub = xub;
   lb = xlb;
   delta = ub - lb;

/* scale up by s, a power of 10, so range (delta) exceeds 1
   find power of 10 quantum, r, such that delta/10 <= r < delta */

   r = s = 1;

   while (delta*s < 10) s *= 10;
   delta *= s;
   while (10*r < delta) r *= 10;

   lb *= s;   ub *= s;

/* set r=(1,2,5)*10**n so that 3-5 quanta cover range */

   if (r >= delta/2)
      r /= 2;
   else if (r < delta/5)
      r *= 2;

   z.ub = ubf ? ub : modceil (ub, r);
   z.lb = lbf ? lb : modfloor (lb, r);

   if (!lbf && z.lb <= r && z.lb > 0) {
      xlb = 0;
      goto loop;
   }
   else if (!ubf && z.ub >= -r && z.ub < 0) {
      xub = 0;
      goto loop;
   }
   z.quant = r;
   z.mult = s;

   return (z);
}

xscale (p)
 register struct xy *p;
{
   FLOAT edge;

   setlim (p);
   edge = topx-botx;
   p->xa = p->xsize*edge/((*p->xf)(p->xub) - (*p->xf)(p->xlb));
   p->xbot = botx + edge*p->xoff;
   p->xtop = p->xbot + edge*p->xsize;
   p->xb = p->xbot - (*p->xf)(p->xlb)*p->xa + .5;
}

yscale (p)
 register struct xy *p;
{
   FLOAT edge;

   setlim (p);
   edge = topy-boty;
   p->xa = p->xsize*edge/((*p->xf)(p->xub) - (*p->xf)(p->xlb));
   p->xbot = boty + edge*p->xoff;
   p->xtop = p->xbot + edge*p->xsize;
   p->xb = p->xbot - (*p->xf)(p->xlb)*p->xa + .5;
}

/* plot axes */

axes ()
{
   register i;
   int lmark[512], mark[512];
   int xn, yn;

   if (mode != 1) {
      linemod (modes[1]);
      mode = 1;
   }
         
   switch (gridst) {

   case (0) :
      return;
      break;

   case (3) :
      line (xd.xbot, yd.xtop, xd.xbot, yd.xbot);
      cont (xd.xtop, yd.xbot);
      break;

   default :
      line (xd.xbot, yd.xbot, xd.xtop, yd.xbot);
      cont (xd.xtop, yd.xtop);
      cont (xd.xbot, yd.xtop);
      cont (xd.xbot, yd.xbot);
      break;
   }

   xn = setmark (mark, lmark, &xd);
   for (i = 0; i < xn; i++) {
      if (lmark[i] == 3*tick && gridst == 2)
         line (mark[i], yd.xbot, mark[i], yd.xtop);
      else if (xd.tickstyle != 0) {    /* SAL */
         if(xd.tickstyle != 2 || lmark[i] == 3*tick) {
            line (mark[i], yd.xbot, mark[i], yd.xbot+lmark[i]);
            if (gridst != 3)
               line (mark[i], yd.xtop-lmark[i], mark[i], yd.xtop);
         }
      }
   }

   yn = setmark (mark, lmark, &yd);
   for (i = 0; i < yn; i++) {
      if (lmark[i] == 3*tick && gridst == 2)
         line (xd.xbot, mark[i], xd.xtop, mark[i]);
      else if(yd.tickstyle != 0) {   /* SAL */
         if(yd.tickstyle != 2 || lmark[i] == 3*tick) {    /* SAL */
            line (xd.xbot, mark[i], xd.xbot+lmark[i], mark[i]);
            if (gridst != 3)
               line (xd.xtop-lmark[i], mark[i], xd.xtop, mark[i]);
         }
      }
   }
}

setmark (xmark, lmark, p)   /* Set tick and number positions */
 int *lmark, *xmark;
 register struct xy *p;
{
   int i, xn = 0;
   FLOAT x, xl, xu;
   FLOAT q;

   if (p->xf == log10 && !p->xqf) {
      for (x = p->xquant; x < p->xub; x *= 10) {
         lmark[xn] = 3*tick;
         submark (xmark, &xn, x, p);
         for (i = 2; i < 10; i++){
            lmark[xn] = tick;
            submark (xmark, &xn, i*x, p);
         }
      }
   }
   else {
      xn = 0;
      q = p->xquant;
      if (q > 0) {
         xl = modfloor (p->xlb, q);
         xu = modfloor (p->xub-q/10, q/5)+q/10;
      }
      else {
         xl = modceil (p->xub, q);
         xu = modfloor (p->xlb+q/10, q/5)-q/10;
      }
      for (i = 0, x = xl; x <= xu; i++, x += fabs(p->xquant)/5.0){
         if (x <= p->xlb || x >= p->xub) continue;
           xmark[xn] = (*p->xf)(x)*p->xa + p->xb;
         if (i % 5)
            lmark[xn++] = tick; 
         else
            lmark[xn++] = 3*tick;
      }
   }

   return (xn);
}

submark (xmark, pxn, x, p)
 int *xmark;
 int *pxn;
 FLOAT x;
 struct xy *p;
{
   if (1.001*p->xlb < x && .999*p->xub > x) {
      xmark[(*pxn)++] = log10(x)*p->xa + p->xb;
      return (1);
   }

   return (0);
}

plot ()
{
   int ix, iy, iy1, iy2;
   int jx, jy, jy1, jy2;
   int ix1, ix2;
   int bar, dbar, delta;
   int i;
   int conn;
   FLOAT t;

   bar = 3*tick*(n_points+10)/(3*n_points+10);
   conn = OFF;

   if (mode != 0) linemod (modes[mode]);

   for (i = 0; i < n_points; i++) {

/* check for break */

      conn &= xx[i].cflag;

/* reset line style */

      if (xx[i].line_mode != mode) {
         mode = xx[i].line_mode;
         linemod (modes[mode]);
      }

/* plot error bar */

      if (xx[i].err_flag) {
         if(mode != 1) linemod(modes[1]);  /* error bars are solid   SAL */
         jx = conv (xx[i].xv, &xd, &ix);
         jy = conv (xx[i].yv, &yd, &iy);
         if (yd.xf == log10) {
            jy1 = conv (xx[i].yv*(t=exp(xx[i].yerr/xx[i].yv)), &yd, &iy1);
            jy2 = conv (xx[i].yv/t, &yd, &iy2);
          }
         else {
            jy1 = conv (xx[i].yv+xx[i].yerr, &yd, &iy1);
            jy2 = conv (xx[i].yv-xx[i].yerr, &yd, &iy2);
         }
         if (jx == 0) continue;
         if (jy == 0) continue;

/* connect center of bar */

         if (conn) cont (ix, iy);

         if (jy1 == 0) iy1 = yd.xtop;
         if (jy2 == 0) iy2 = yd.xbot;
         delta = iy1-iy2;
         delta = delta/5;
         dbar = (bar < delta) ? bar : delta;
         ix1 = (xd.xbot < ix-dbar) ? ix-dbar : xd.xbot;
         ix2 = (xd.xtop > ix+dbar) ? ix+dbar : xd.xtop;
         if (iy1) {
            move (ix1, iy1);
            cont (ix2, iy1);
         }
         if (iy2) {
            move (ix1, iy2);
            cont (ix2, iy2);
         }
         move (ix, iy1);
         cont (ix, iy2);

         symbol (ix, iy, xx[i].lblptr, xx[i].sym_size);

         conn = ON;
         if(mode != 1) linemod(modes[mode]);  /* error bars are solid   SAL */
         continue;
      }

/* no error bar */

      else {

/* point out of bounds? */

         if (!conv (xx[i].xv, &xd, &ix) || !conv (xx[i].yv, &yd, &iy)) {
            conn = OFF;
            continue;
         }

         if (mode != 0) {
            if (conn)
               cont (ix, iy);
            else
               move (ix, iy);
            conn = ON;
         }
         symbol (ix, iy, xx[i].lblptr, xx[i].sym_size);
      }
   }

   linemod (modes[1]);
}

/* convert x/y value to scaled version, and
   check whether it is in bounds */

conv (xv, p, ip)
 FLOAT xv;
 register struct xy *p;
 int *ip;
{
   long ix;

   ix = (p->xa) * (*p->xf)(xv * (p->xmult)) + (p->xb);

   if (clip && (ix < (p->xbot) || ix > (p->xtop))) return (OFF);

   *ip = ix;
   return (ON);
}

/* plot label */

symbol (ix, iy, ptr, size)
 char *ptr;
 FLOAT size;
{
   if (ptr == NULL) {
      if (mode == 0) point(ix,iy);
   }
   else {
      if (mode != 1) linemod (modes[1]);/*SAL print symbols with solid lines */
      ascii (ptr, ix, iy, size);
      move (ix, iy);
      if (mode != 1) linemod (modes[mode]);  /* SAL */
   }
}

/* plot strings, and axes */

title ()
{
   int ix, iy;

/* print title */

   if (*titlebuf) {
      unix_to_ugs (titlebuf);
      ix = titlexf ?
         titlex*(xd.xtop-xd.xbot)+xd.xbot :
         (xd.xtop+xd.xbot)/2.;
      iy = titleyf ?
         titley*(yd.xtop-yd.xbot)+yd.xbot :
         yd.xtop-4*tick-(int)(titlesize*40);
      cascii (titlebuf, ix, iy, titlesize);
      move (xd.xbot, yd.xbot-100);
   }
   leftx = xd.xbot;

/* plot axes */

   if (gridst) {
      axlab ('x', &xd);
      axlab ('y', &yd);
   }

/* print x-axis label */

   if (*xtitlebuf) {
      unix_to_ugs (xtitlebuf);
      ix = xtitlexf ?
         xtitlex*(xd.xtop-xd.xbot)+xd.xbot :
         (xd.xtop+xd.xbot)/2.;
      iy = xtitleyf ?
         xtitley*(yd.xtop-yd.xbot)+yd.xbot :
         yd.xbot - (int)(lblsize*160 + xtitlesize*80);
      cascii (xtitlebuf, ix, iy, xtitlesize);
   }

/* print y-axis label */

   if (*ytitlebuf) {
      unix_to_ugs (ytitlebuf);
      ix = ytitlexf ?
         ytitlex*(xd.xtop-xd.xbot)+xd.xbot :
         leftx - (int)(80*ytitlesize);
      iy = ytitleyf ?
         ytitley*(yd.xtop-yd.xbot)+yd.xbot :
         (yd.xtop+yd.xbot)/2;
      cvascii (ytitlebuf, ix, iy, ytitlesize);
   }
}

/* number axes */

axlab (c, p)
 char c;
 register struct xy *p;
{
   int mark;
   int width, digits;
   char buf[128], *style;
   FLOAT x, xl, xu;
   FLOAT q;


   if(!p->number) return; /* SAL */

/* log axes and no user-supplied quantum? */

   if ((p->xf) == log10 && !(p->xqf)) {

      style = ((fabs(log10(p->xlb)) >= 4.0) || (fabs(log10(p->xub)) >= 4.0)) ?
              "%.0e" : "%g";

      for (x = (p->xquant); x <= (p->xub); x *= 10) {

/* x is between the two bounds */

         if ((p->xlb) <= x && (p->xub) >= x) {

            sprintf (buf, style, x / (p->xmult));
            mark = log10(x) * (p->xa) + (p->xb);
            num_lbl (c, buf, mark);
         }

/* bounds are too close in magnitude */

         if ((p->xub)/(p->xlb) <= 100) {

            if ((p->xlb) <= 2*x && (p->xub) >= 2*x) {
               sprintf (buf, style, 2.0*x / (p->xmult));
               mark = log10(2.0*x) * (p->xa) + (p->xb);
               num_lbl (c, buf, mark);
            }

            if ((p->xlb) <= 5*x && (p->xub) >= 5*x) {
               sprintf (buf, style, 5.0*x / (p->xmult));
               mark = log10(5.0*x) * (p->xa) + (p->xb);
               num_lbl (c, buf, mark);
            }
         }
      }
   }
   else {

      q = (p->xquant);
      if (q > 0) {
         xl = modceil ((p->xlb)-q/6, q);
         xu = modfloor ((p->xub)+q/6, q);
      }
      else {
         xl = modceil ((p->xub)+q/6, q);
         xu = modfloor ((p->xlb)-q/6, q);
      }

      digit (xl/(p->xmult), xu/(p->xmult), (int)((xu-xl)/q + 1.5),
         &width, &digits);

      for (x = xl; x <= xu; x += fabs(p->xquant)) {
         sprintf (buf, "%*.*f\n", width, digits, x/(p->xmult));
         mark = (*p->xf)(x) * (p->xa) + (p->xb);
         num_lbl (c, buf, mark);
        }
   }
}

/* print vertical/horizontal label */

num_lbl (c, buf, mark)
 char c, *buf;
{
   char pri[BSIZ];
   int i;

   g_to_ugs (buf, pri);

   switch (c) {

   case 'x':
      cascii (pri, mark, yd.xbot - (int)(120*lblsize), lblsize);
      break;

   case 'y':
      i = xd.xbot - 50*lblsize - 60*strlen(pri)*lblsize;
      leftx = (leftx > i) ? i : leftx;

      rascii (pri, xd.xbot-(int)(50*lblsize), mark, lblsize);
      break;
   }
}

/* error handling for unknown argument/command */

badarg ()
{
   fprintf (stderr, "axis: error in arguments\n");
   exit (-1);
}
