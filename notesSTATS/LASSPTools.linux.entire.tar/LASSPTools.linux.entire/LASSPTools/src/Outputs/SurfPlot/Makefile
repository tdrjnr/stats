BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
XLIBS=-L/usr/X11R6/lib -lX11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	SurfPlot.c 

OBJS = \
	SurfPlot.o 

# all: math_io line_io SurfPlot
all: SurfPlot

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS) 

line_io::
	cd ../line_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS) 

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

SurfPlot: $(OBJS)
	$(CC) -o SurfPlot $(OBJS) $(XLIBS) $(LIBDIR) -lmath_io -lline_io -lm 

clean::
	rm -f *.o SurfPlot

install::
	cp -p SurfPlot $(BINDIR)
	chmod 775 $(BINDIR)/SurfPlot

	


