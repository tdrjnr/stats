#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef ATTArchitecture
#include <sys/ioctl.h>
#else
#include <bsd/sgtty.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#ifdef ATTArchitecture
#include <X11/Label.h>
#else
#include <X11/Xaw/Label.h>
#endif

#define LINE 512
#define WAITINTERVAL 50000
char buf[LINE];
char cptr[LINE];
char *prog;
/* char *index(); */

Display *dpy;
Window widgetwin;
Pixmap pixmap;
GC gc;
XGCValues gcv;
XtInputId inid;
Pixel fg, bg;
XFontStruct *font;
int rwidth = 500, rheight = 500;

short xmin, ymax;
double xfactor, yfactor;

#define MAPX(x) (int) (((x) - xmin) * xfactor)
#define MAPY(y) (int) ((ymax - (y)) * yfactor)

int spaceset = 0;
short curpt[] = {0,0};

FILE *instrm = stdin;
int len = 0;
int endinp;
char *curp;

Arg wargs[20];
Widget toplevel;
Widget child;

main(argc,argv)
int argc;
char **argv;
{
	struct sgttyb gp;
	short old;
	int fd;
	int argf;

	prog = argv[0];
	fd = fileno(instrm);

/*	Commented out by Mark Newman	(JPS)
	gtty(fd,&gp);
	old = gp.sg_flags;
	gp.sg_flags |= (CBREAK);
	stty(fd,&gp); */


	fcntl(fd,F_GETFL,argf);
	fcntl(fd,F_SETFL, argf|O_NDELAY);

	openpl(argc,argv);

	fcntl(fd,F_SETFL,argf);
/*	Commented out by Mark Newman: now works under Linux (JPS)
	gp.sg_flags = old;
	stty(fd,&gp); */

}

doplot(client_data,fds,id)
caddr_t client_data;
int *fds; /* None of these arguments are actually used... */
int *id;
{
	char c;
	short x[6];
	void buttonhandler();

	while(readit(instrm)){
	    while((len > 0) && (!endinp)){
		len--;
		c = *curp++;
/*		printf("%c",c); */
		switch(c){
			case 'm': /* Move */
				if (getcoords(x,2,instrm))
					move(x[0],x[1]);
				break;
			case 'n': /* Continue */
				if (getcoords(x,2,instrm))
					cont(x[0],x[1]);
				break;
			case 'p': /* Plot */
				if (getcoords(x,2,instrm))
					point(x[0],x[1]);
				break;
			case 'l': /* Draw Line */
				if (getcoords(x,4,instrm))
					line(x[0],x[1],x[2],x[3]);
				break;
			case 't': /* Label */
				if (getstring(buf,instrm))
					label(buf);
				break;
			case 'a': /* Arc */
				if (getcoords(x,6,instrm))
					arc(x[0],x[1],x[2],x[3],x[4],x[5]);
				break;
			case 'c': /* Circle */
				if (getcoords(x,3,instrm))
					circle(x[0],x[1],x[2]);
				break;
			case 'e': /* Erase */
				erase();
				break;
			case 'f': /* Linemod */
				if (getstring(buf,instrm))
					linemod(buf);
				break;
			case 's': /* Space */
				if (getcoords(x,4,instrm))
					space(x[0],x[1],x[2],x[3]);
				break;
			default:
				printf("Error?: character = %d\n",c);
				break;
		}
	    }
	}
	if (errno != EWOULDBLOCK){
		if (fork()) exit(0);
		XtRemoveInput(inid);
		XtAddEventHandler(toplevel,ButtonPressMask,False,
						buttonhandler, NULL);
	} 
	XClearArea(dpy,widgetwin,0,0,0,0,True);
}

void buttonhandler(w,client_data,event)
Widget w;
caddr_t client_data;
XEvent *event;
{
	closepl();
	exit(0);
}

drawellipse(x,y,r1,r2)
int x,y,r1,r2;
{
/*	XPoint *points;
	int i,npoints;
	double delta;

	npoints = 2 + abs(r1) + abs(r2);
	points = (XPoint *) (malloc(npoints * sizeof(XPoint)));
	delta = 2 * M_PI / (npoints - 1);
	for (i=0;i<npoints-1;i++){
		points[i].x = x + r1 * cos(i * delta);
		points[i].y = y + r2 * sin(i * delta);
	}
	points[npoints-1].x = x + r1;
	points[npoints-1].y = y ;
	XDrawLines(dpy,pixmap,gc,points,npoints,CoordModeOrigin);
	*/
	XDrawArc(dpy,pixmap,gc, x - r1, y - r2, 2 * r1, 2 * r2, 0,
					64 * 360 - 1);
}

drawarc(x,y,x0,y0,x1,y1)
int x,y,x0,y0,x1,y1;
{
	int a0,b0,a1,b1;
	int a02,b02,a12,b12;
	double ar,br,ar2,br2;
	double theta0,theta1;

	a0 = x0 - x;
	a02 = a0*a0;
	a1 = x1 - x;
	a12 = a1*a1;
	b0 = y0 - y;
	b02 = b0*b0;
	b1 = y1 - y;
	b12 = b1*b1;
	if (b12 == b02) return 0;
	ar2 = (a02*b12 - a12*b02)/(b12 - b02);
	if (ar2 < 0) return 0;
	ar = sqrt(ar2);
	br2 = (b02*a12 - b12*a02)/(a12 - a02);
	if (br2 < 0) return 0;
	br = sqrt(br2);
	theta0 = -atan2(b0/br,a0/ar);
	theta1 = -atan2(b1/br,a1/ar);
	if (theta0 > theta1) theta1 += 2 * M_PI;
	if (theta0 < 0){
		theta0 += 2 * M_PI;
		theta1 += 2 * M_PI;
	}
	XDrawArc(dpy,pixmap,gc,(int) (x - ar), (int) (y - br),
		  (int)(2 * ar), (int) (2 * br), (int) (64* theta0
		  * 180/M_PI), (int) (64 * (theta1 - theta0) * 180/M_PI));
}

/* Openpl is supposed to set up the X window stuff, by creating a
   widget, in which the image is to be put in a background pixmap. */

openpl(argc,argv)
int argc;
char **argv;
{
	int i, dimset = 0, narg;
	char *ptr;

	for (i=0;i<argc;i++){
		if (strcmp(argv[i],"-geometry"))
			continue;
		else
		{
			dimset = 1;
			rwidth = atoi(argv[i+1]);
			rheight = atoi(1+index(argv[i+1],'x'));
		}
	}
	toplevel = XtInitialize(argv[0], "xplot",NULL, 0, &argc, argv); 
	dpy = XtDisplay(toplevel);

	if (!dimset)
		if (ptr = XGetDefault(dpy,"xplot","geometry")){
			dimset = 1;
			rwidth = atoi(ptr);
			rheight = atoi(1 + index(ptr,'x'));
		}

	pixmap = XCreatePixmap(dpy,DefaultRootWindow(dpy), rwidth, rheight,
		DefaultDepth(dpy, DefaultScreen(dpy)));

	narg = 0;
	XtSetArg(wargs[narg], XtNbitmap, pixmap); narg++;
	XtSetArg(wargs[narg], XtNinternalWidth, 0); narg++;
	XtSetArg(wargs[narg], XtNinternalHeight, 0); narg++;
	if (dimset == 0) {
		XtSetArg(wargs[narg], XtNheight, rheight); narg++;
		XtSetArg(wargs[narg], XtNwidth, rwidth); narg++;
	} 

	child = XtCreateManagedWidget(prog, labelWidgetClass,
						toplevel, wargs, narg);
	XtRealizeWidget(toplevel);
	widgetwin = XtWindow(child);

/*      Causes errors: added for some reason in '95
	bg = DefaultGC(dpy,DefaultScreen(dpy))->values.background;
        fg = DefaultGC(dpy,DefaultScreen(dpy))->values.foreground; */

/*	Mark Newman's fix JPS */
        bg = 1;
        fg = 0;

	narg = 0;
	XtSetArg(wargs[narg], XtNbackground, &bg); narg++;
	XtSetArg(wargs[narg], XtNforeground, &fg); narg++;
	XtSetArg(wargs[narg], XtNfont, &font); narg++;
	XtGetValues(child, wargs, narg);

	gcv.foreground = fg;
	gcv.background = bg;
	gcv.font = font->fid;
	gc = XCreateGC(dpy, pixmap, GCForeground | GCBackground | GCFont,
			 &gcv);
	erase();

	inid = XtAddInput(fileno(instrm), XtInputReadMask, doplot, NULL);
	XtMainLoop();
}

/* Erase should wait for the current page to be absorbed, before creating
  a new one (presumably by erasing the pixmap and then redrawing the widget.*/
erase()
{
/* Wait for button event */
/* Clear pixmap */
	XSetForeground(dpy, gc, bg);
	XFillRectangle(dpy,pixmap,gc,0,0,rwidth,rheight);
	XSetForeground(dpy, gc, fg);
}

/* Don't know how I'm going to do this ! */

label(s)
char *s;
{
	XDrawString(dpy,pixmap,gc,MAPX(curpt[0]),MAPY(curpt[1]),s,
			strlen(s) - 1);
#ifdef WideLabel
	curpt[0] += (strlen(s)*font->max_bounds.width)/xfactor;
#endif
}

line(x1,y1,x2,y2)
short x1,y1,x2,y2;
{
	curpt[0] = x2;
	curpt[1] = y2;
	if (spaceset == 0) return;
	XDrawLine(dpy,pixmap,gc, MAPX(x1), MAPY(y1), MAPX(x2), MAPY(y2));
}

circle(x,y,r)
short x,y,r;
{

	if (spaceset == 0) return;
	drawellipse(MAPX(x), MAPY(y), (int) (r * xfactor), (int) (r*yfactor));
}


arc(x,y,x0,y0,x1,y1)
short x,y,x0,y0,x1,y1;
{
	if (spaceset == 0) return;
	drawarc(MAPX(x), MAPY(y), MAPX(x0), MAPY(y0), MAPX(x1), MAPY(y1));
}

move(x,y)
short x,y;
{
	curpt[0] = x;
	curpt[1] = y;
}

cont(x,y)
short x,y;
{
	line(curpt[0],curpt[1],x,y);
}

point(x,y)
short x,y;
{
	curpt[0] = x;
	curpt[1] = y;
	if (spaceset == 0) return;
	XDrawPoint(dpy,pixmap,gc, MAPX(x), MAPY(y));
}

linemod(s)
char *s;
{
	int line_style;

	line_style = gcv.line_style;

	if (!strcmp(s,"solid\n"))
		line_style = LineSolid;
	else if ((!strcmp(s,"dotted\n"))||(!strcmp(s,"shortdashed\n")))
		line_style = LineOnOffDash;
	else if ((!strcmp(s, "longdashed\n"))||(!strcmp(s,"dotdashed\n")))
		line_style = LineDoubleDash;
	gcv.line_style = line_style;
	XChangeGC(dpy, gc, GCLineStyle, &gcv);
}

space(x0,y0,x1,y1)
short x0,y0,x1,y1;
{
	spaceset = 1;
	xmin = x0;
	ymax = y1;
	xfactor = ((double) rwidth)/(x1 + 1 - x0);
	yfactor = ((double) rheight)/(y1 + 1 - y0);
#ifdef KeepScale
	if (xfactor < yfactor) {
		yfactor = xfactor;
		ymax = rheight/(2*xfactor) + (y0 + y1)/2;
	} else if (yfactor < xfactor){
		xfactor = yfactor;
		xmin = (x0 + x1)/2 - rwidth/(2*yfactor);
	}
#endif
}

closepl()
{
	XFreeGC(dpy,gc);
	XFreePixmap(dpy,pixmap);
	XtDestroyWidget(toplevel);
}

int getcoords(x,n,strm)
short *x;
int n;
FILE *strm;
{
	unsigned char c,d;
	int i;

	while (len < 2*n) {
		len++;
		curp--;
		if (!readit(strm))
			return 0;
		len--;
		curp++;
	}
	len -= 2 * n;
	for (i=0;i< n;i++){
		c = *curp++;
		d = *curp++;
		x[i] = (short) ((d << 8) | c);
/*		printf(" %d",x[i]); */
	}
/*	printf("\n"); */
	return 1;
}

int getstring(ptr,strm)
char *ptr;
FILE *strm;
{
	char *cp;
	int lenp;

	while ((cp = index(curp,'\n')) == NULL){
		len++;
		curp--;
		if (!readit(strm))
			return 0;
		len--;
		curp++;
	}
	lenp = cp - curp + 1;
	bcopy(curp,ptr,lenp);
	ptr[lenp] = 0;
	len -= lenp;
	curp += lenp;
/*	printf("%s",ptr); */
	return lenp;
}

/* Have to do my own buffering: */

int readit(strm)
FILE *strm;
{
	int lenp;

	endinp = 0;
	if (len > 0)
		bcopy(curp, cptr, len);
	if ((lenp = fread(cptr+len, 1, LINE - len, strm)) == 0){
		usleep(WAITINTERVAL); /* Wait in case stuff is on the way */
		if ((lenp = fread(cptr+len, 1, LINE - len, strm)) == 0){
			endinp = 1;
			return 0;
		}
	}
	len += lenp;
	curp = cptr;
	return 1;
}
