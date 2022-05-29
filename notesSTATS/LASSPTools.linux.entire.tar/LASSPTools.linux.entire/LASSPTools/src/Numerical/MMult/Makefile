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
	MMult.c 

OBJS = \
	MMult.o 

# all: math_io blas MMult
all: MMult

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)

blas::
	cd ../blas; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

.c.o:

$(OBJS): $(SRCS)	
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

MMult: $(OBJS)
	$(CC) -o MMult $(OBJS) $(LIBDIR) -lmath_io -lblas -lm $(FLIB)

clean::
	rm -f *.o MMult

install::
	cp -p MMult $(BINDIR)
	chmod 775 $(BINDIR)/MMult
	


	
