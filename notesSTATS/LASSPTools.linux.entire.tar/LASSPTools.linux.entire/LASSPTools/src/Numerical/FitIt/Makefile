BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
# -lF77 -lI77 -lU77 for Sun
# xlf90 for PowerPC
#FLIB = -lf2c -lF77 -lI77  -lc -lm
FLIB = -lg2c -lc -lm
FFLAGS = -c -O
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	fcn_fit_it.c \
	fit_it.c \
	theory_fecht.c 

OBJS = \
	fcn_fit_it.o \
	fit_it.o \
	theory_fecht.o 

# all: math_io minpack fit_it
all: fit_it

minpack::
	cd ../minpack; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS);

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS) ARFLAGS=$(ARFLAGS);

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

fit_it: $(OBJS)
	$(CC) -o fit_it $(OBJS) $(LIBDIR) -lmath_io -lminpack -lm $(FLIB)

clean::
	rm -f *.o fit_it

install::
	cp -p fit_it $(BINDIR)
	chmod 775 $(BINDIR)/fit_it
	


	
