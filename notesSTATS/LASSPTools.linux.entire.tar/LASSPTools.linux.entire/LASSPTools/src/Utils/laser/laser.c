/*
 *   plot(5) to Postscript
 */

#include <stdio.h>
#include <pwd.h>
#include <strings.h>

#define TRUE	(1)
#define FALSE	(0)
#define P	fprintf
#define DEFAULT_PRINTER ("ap")

/* system functions */

   extern void exit();
   extern char *ctime(), *getenv();
   extern long time();
/*  extern FILE *popen(), *pclose(); */
/*  removed for IBM */

   char *progname;
   FILE *fin;
   int xmin,xmax,ymin,ymax;
   int Texmode = FALSE;


#ifndef _NO_PROTO
  main(int argc, char **argv)
#else
  main (argc, argv)
   int argc;
   char *argv[];
#endif
{
   register char *arg;
   long clock;
   struct passwd *pswd;
   char hostname[256];
   char *printer, *ps = "lpr -P";
   int manualfeed = FALSE;
   int NoSpool = FALSE;
   int Landscape = TRUE;
   int thickness = 0;

/* default printer */

   if ((printer = getenv("PRINTER")) == NULL)
      printer = DEFAULT_PRINTER;

   progname = *argv;

/* scan arguments for valid options */

   while ( (argc > 1) && (argv[1][0] == '-') ) {
      argc--;
      arg = *++argv;
      switch (arg[1]) {

      case 'T':      /* Tex mode - psfig */
	 Texmode = TRUE;
	 Landscape = 1 - Landscape;
	 NoSpool = TRUE;
	 break;

      case 't':      /* line thickness */
         if (arg[2]) {
            if (sscanf (&arg[2], "%d", &thickness) != 1) 
               BadArg("Thickness must be an integer");
         }
         else if (argc > 1) {
            argc--;
            if (sscanf (*++argv, "%d", &thickness) != 1) 
               BadArg("Thickness must be an integer");
         }
         else {
            BadArg("t?");
         }

         if (thickness < 0)
            BadArg("t?");
         break;

      case 'P':      /* printer name */
         if (arg[2])
            printer = &arg[2];
         else if (argc > 1) {
            argc--;
            printer = *++argv;
         }
         break;

      case 'm':      /* manual paper feed */
         manualfeed = TRUE;
         break;

      case 'f':      /* send output to stdio */
         NoSpool = TRUE;
         break;

      case 'R':      /* Inhibit Landscape mode (allow in Tex mode) */
         Landscape = 1 - Landscape;
         break;

      case 'h':      /* help: Fall through...*/

      default:       /* unrecognized option */
         help();
         break;

      }
   }

   if (NoSpool == FALSE) {
      strcat(ps, printer);

/* open pipe to output routine */

      if ((fin = popen(ps, "w")) == NULL) {
         fprintf (stderr, "%s: cannot run %s\n", progname, ps);
         exit(3);
      }
   }
   else {
      fin = stdout;
   }

/* Get the space command, and hope it's near the beginning */
   if (Texmode == TRUE)
	if (CopyFile(TRUE) == FALSE) {
		fprintf(stderr,"Error!! No plot(5) size command in input\n");
		exit(-1);
	}

/* output prologue */

    pswd = getpwuid((int) getuid());
    (void) gethostname(hostname, sizeof hostname);

P (fin, "%%!\n");
P (fin, "%%%%BoundingBox: %d %d %d %d\n",xmin,ymin,xmax,ymax);
P (fin, "%%%%Creator: %s:%s(%s)\n",hostname,pswd->pw_name, pswd->pw_gecos);
P (fin, "%%%%Title: Unix plot file\n");
P (fin, "%%%%CreationDate: %s",((void) time(&clock),ctime(&clock)));
P (fin, "%%%%DocumentFonts: Courier\n");
P (fin, "%%%%EndComments\n");

    if (manualfeed) P (fin, "statusdict /manualfeed true put\n");

P (fin, "save 50 dict begin /psplot exch def\n");

P (fin, "/StartPSPlot\n");
P (fin, "   {newpath 0 0 moveto %d setlinewidth ",thickness);
P (fin, "            0 setgray 1 setlinecap\n");
P (fin, "   /imtx matrix currentmatrix def ");
P (fin, "   /dmtx matrix defaultmatrix def\n");
P (fin, "   /fnt /Courier findfont def /smtx matrix def\n");
P (fin, "    fnt 8 scalefont setfont}def\n");

/* These definitions should include some thickness dependence, maybe? */
P (fin, "/solid {{}0}def\n");
P (fin, "/dotted {[%d nail %d nail ] 0}def\n", 1, 2);
P (fin, "/longdashed {[%d nail] 0}def\n", 10);
P (fin, "/shortdashed {[%d nail] 0}def\n", 6);
P (fin, "/dotdashed {[%d nail %d nail %d nail %d nail] 0}def\n",
	2, 6, 10, 6);

P (fin, "/min {2 copy lt{pop}{exch pop}ifelse}def\n");
P (fin, "/max {2 copy lt{exch pop}{pop}ifelse}def\n");
P (fin, "/len {dup mul exch dup mul add sqrt}def\n");
P (fin, "/nail {0 imtx dtransform len 0 idtransform len}def\n");
P (fin, "/Landscape {90 rotate 0 -15840 translate} def\n");

P (fin, "/m {stroke moveto}def\n");
P (fin, "/n {lineto}def\n");
P (fin, "/p {stroke moveto gsave 1 setlinecap solid setdash\n");
P (fin, "    dmtx setmatrix .4 nail setlinewidth\n");
P (fin, "    .05 0 idtransform rlineto stroke grestore}def\n");
P (fin, "/l {stroke moveto lineto}def\n");
P (fin, "/t {stroke smtx currentmatrix pop imtx setmatrix ");
P (fin, "     show smtx setmatrix}def\n");
P (fin, "/a {stroke gsave /y2 exch def /x2 exch def\n");
P (fin, "    /y1 exch def /x1 exch def /yc exch def /xc exch def\n");
P (fin, "    /r x1 xc sub dup mul y1 yc sub dup mul add sqrt\n");
P (fin, "    x2 xc sub dup mul y2 yc sub dup mul add sqrt add 2 div def\n");
P (fin, "    /ang1 y1 yc sub x1 xc sub atan def\n");
P (fin, "    /ang2 y2 yc sub x2 xc sub atan def\n");
P (fin, "    xc yc r ang1 ang2 arc stroke grestore}def\n");
P (fin, "/c {stroke gsave 0 360 arc stroke grestore}def\n");
P (fin, "/e {stroke gsave showpage grestore 0 0 moveto}def\n");
P (fin, "/f {stroke load exec setdash}def\n");
P (fin, "/s {/ury exch def /urx exch def /lly exch def /llx exch def\n");
P (fin, "    imtx setmatrix newpath clippath pathbbox newpath\n");
P (fin, "    /dury exch def /durx exch def /dlly exch def /dllx exch def\n");
P (fin, "    /md durx dllx sub dury dlly sub min def\n");
P (fin, "    /Mu urx llx sub ury lly sub max def\n");
P (fin, "    dllx dlly translate md Mu div dup scale llx\n");
P (fin, "         neg lly neg translate}def\n");

P (fin, "/EndPSPlot {stroke clear psplot end restore}def\n");
P (fin, "%% end fixed prolog\n");

   if (Landscape == TRUE) P (fin, "Landscape\n");

P (fin, "StartPSPlot\n");
P (fin, "%%%%EndProlog\n");

/* convert stdio to Postscript */

    CopyFile(FALSE);

P (fin, "%%%%Trailer\n");
P (fin, "EndPSPlot\n");

    if (NoSpool == FALSE) pclose(fin);
}

short int getint ()
{
   register int b1, b2;
   b1 = getchar() & 0377;
   b2 = getchar();
   return (b1 + (((b2 > 128) ? (b2-256) : b2) << 8) );
}

#ifndef _NO_PROTO
  int CopyFile(int flag)
#else
  int CopyFile (flag)
    int flag;
#endif
{
   int c, d;
   short int x, y, x0, y0, x1, y1, r;
   int curpage = 1;
   int donegraphics = 0;	/* bug fix: D.S 22 July 87 */

   if (flag == FALSE) P (fin, "%%%%Page: %d %d\n", curpage, curpage);

   while ((c = getchar()) != EOF) {
      if ((flag == TRUE) && (c!='s'))
	     fprintf(stderr, "Warning!! Initial plot(5) commands ignored\n");
      switch (c) {
         case 'm': /* move */
         case 'n': /* cont */
         case 'p': /* point */
            x = getint();
            y = getint();
            if (flag == FALSE) P (fin, "%d %d %c\n", x, y, c);
            donegraphics = 1;
            break;

         case 'l': /* line */
            x0 = getint();
            y0 = getint();
            x1 = getint();
            y1 = getint();
            if (flag == FALSE) P (fin, "%d %d %d %d l\n", x1, y1, x0, y0);
            donegraphics = 1;
            break;

         case 't': /* label */
            putc ('(', fin);
            for (d = getchar(); d != '\n'; d = getchar()) {
	     if (flag == FALSE){
               if ((d == ')') || (d == '(') || (d == '\\'))
                  putc ('\\', fin);
               if ((d > 0176) || (d <040)) {
                  putc ('\\', fin);
                  putc (((d >> 6) & 07)+'0', fin);
                  putc (((d >> 3) & 07)+'0', fin);
                  putc ((d & 07)+'0', fin);
               }
               else putc (d, fin);
	      }
            }
            if (flag == FALSE) P (fin, ")t\n");
            donegraphics = 1;
            break;

         case 'a': /* arc */
            x = getint();
            y = getint();
            x0 = getint();
            y0 = getint();
            x1 = getint();
            y1 = getint();
            if (flag == FALSE) P (fin, "%d %d %d %d %d %d a\n",
			x, y, x0, y0, x1, y1);
            donegraphics = 1;
            break;

         case 'c': /* circle */
            x = getint();
            y = getint();
            r = getint();
            if (flag == FALSE) P (fin, "%d %d %d c\n", x, y, r);
            donegraphics = 1;
            break;

         case 'e': /* erase */
	    if (flag == FALSE){
              if (donegraphics){
               donegraphics = 0;
               curpage++;
               P (fin, "e\n");
               P (fin, "%%%%Page: %d %d\n", curpage, curpage);
              }
	    }
            break;

         case 'f': /* linemod */
            for (d = '/'; d != '\n'; d = getchar())
		if (flag == FALSE) putc (d, fin);
            if (flag == FALSE) P (fin, " f\n");
            break;

         case 's': /* space */
            x0 = getint();
            y0 = getint();
            x1 = getint();
            y1 = getint();
            if ((flag == FALSE)&&(Texmode == FALSE))
			P (fin, "%d %d %d %d s\n", x0, y0, x1, y1);
	    if (flag == TRUE) {
		xmin = x0;
		xmax = x1;
		ymin = y0;
		ymax = y1;
		return TRUE;
	    }
            break;

         default:
            fprintf (stderr, "%s: unknown plot(5) command %03o\n",
                   progname, c);
            exit(3);
      }
   }
   if (flag == TRUE) return FALSE;
   if (donegraphics) P (fin, "e\n");
}

BadArg ()
{
   fprintf (stderr, "Oops");
}

help()
{
   fprintf (stderr, "\nArguments: \n");
   fprintf (stderr, "    -T [Texmode - for psfig]\n");
   fprintf (stderr, "    -P (printer) [default PRINTER or ap] \n");
   fprintf (stderr, "    -t (line thickness) [default 0] \n");
   fprintf (stderr, "    -m [manualfeed] \n");
   fprintf (stderr, "    -f [output goes to standard I/O] \n");
   fprintf (stderr, "    -R [No Landscape] \n");
   fprintf (stderr, "\n\n");

   exit(-1);
}
