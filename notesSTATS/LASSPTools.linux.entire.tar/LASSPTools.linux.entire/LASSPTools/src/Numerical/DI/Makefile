BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
FFLAGS = -c -O
# -lF77 -lI77 -lU77 for Sun
# xlf90 for PowerPC
#FLIB = -lf2c -lF77 -lI77  -lc -lm
FLIB = -lg2c -lc -lm
BINDIR=../../../bin
LIBDIR=-L../../../lib
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	DI.c \
	Det.c \
	Invert.c \
	linDI.c 

OBJS = \
	linDI.o 


# all: linpack math_io blas DI Det Invert
all: DI Det Invert

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS) 

linpack::
	cd ../linpack; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

blas::
	cd ../blas; make FFLAGS="$(FFLAGS)" F77=$(FC) ARFLAGS=$(ARFLAGS)

.c.o:    
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

DI: $(OBJS) DI.o
	$(CC) -o DI DI.o $(OBJS) $(LIBDIR) -llinpack -lmath_io -lblas -lm $(FLIB)

Det: $(OBJS) Det.o
	$(CC) -o Det Det.o $(OBJS) $(LIBDIR) -llinpack -lmath_io -lblas -lm $(FLIB)

Invert: $(OBJS) Invert.o
	$(CC) -o Invert Invert.o $(OBJS) $(LIBDIR) -llinpack -lmath_io -lblas -lm $(FLIB)

clean::
	rm -f *.o DI Det Invert

install::
	cp -p DI $(BINDIR)
	cp -p Det $(BINDIR)
	cp -p Invert $(BINDIR)
	chmod 775 $(BINDIR)/DI
	chmod 775 $(BINDIR)/Det
	chmod 775 $(BINDIR)/Invert
	


	
