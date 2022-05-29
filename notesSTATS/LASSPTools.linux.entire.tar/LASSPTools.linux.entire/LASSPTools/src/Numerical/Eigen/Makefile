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
	Eigen.c \
	eiscalls.c 

OBJS = \
	Eigen.o \
	eiscalls.o 

# all: math_io eispack Eigen
all: Eigen

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)

eispack::
	cd ../eispack; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

.c.o:
	

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

Eigen: $(OBJS) 
	$(CC) -o Eigen $(OBJS) $(LIBDIR) -lmath_io -leispack -lm $(FLIB)

clean::
	rm -f *.o Eigen

install::
	cp -p Eigen $(BINDIR)
	chmod 775 $(BINDIR)/Eigen
	


	
