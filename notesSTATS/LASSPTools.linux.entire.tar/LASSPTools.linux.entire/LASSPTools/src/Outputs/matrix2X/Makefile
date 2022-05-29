BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
XLIBS=-L/usr/X11R6/lib -lX11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	matrix2X.c \
	parse_argument.c 

OBJS = \
	matrix2X.o \
	parse_argument.o 

# all: math_io matrix2X
all: matrix2X

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS);

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

matrix2X: $(OBJS)
	$(CC) -o matrix2X $(OBJS) $(XLIBS) $(LIBDIR) -lmath_io -lm 

clean::
	rm -f *.o matrix2X

install::
	cp -p matrix2X $(BINDIR)
	chmod 775 $(BINDIR)/matrix2X

	


