BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
# -lF77 -lI77 -lU77 for Sun
# xlf90 for PowerPC
#FLIB = -lf2c -lF77 -lI77  -lc -lm
FLIB = -lg2c -lc -lm
FFLAGS = -c -O
BINDIR=../../../bin
LIBDIR=-L../../../lib
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	MSolve.c \
	linMS.c  

OBJS = \
	MSolve.o \
	linMS.o  

# all: math_io blas linpack MSolve
all: MSolve

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)

blas::
	cd ../blas; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

linpack::
	cd ../linpack; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

.c.o:

$(OBJS): $(SRCS)	
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

MSolve: $(OBJS)
	$(CC) -o MSolve $(OBJS) $(LIBDIR) -lmath_io -llinpack -lblas -lm $(FLIB)

clean::
	rm -f *.o MSolve

install::
	cp -p MSolve $(BINDIR)
	chmod 775 $(BINDIR)/MSolve
	


	
