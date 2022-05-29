BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
# -lF77 -lI77 -lU77 for Sun
# xlf90 for PowerPC
#FLIB = -lf2c -lF77 -lI77  -lc -lm
FLIB = -lg2c -lc -lm
FFLAGS = -c -O
FC=g77
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
DEFINES=
END_DEF = ****************************

SRCS = \
	com.c \
	generate.c \
	linpack.c \
	random.c \
	ranlib.c \
	griff.f \
	marin.f \
	rcarry.f \
	uni.f 

OBJS = \
	com.o \
	generate.o \
	linpack.o \
	random.o \
	ranlib.o \
	griff.o \
	marin.o \
	rcarry.o \
	uni.o 


# all: math_io Random
all: Random

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $(INCDIR) $*.c $(EXTRA)

.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f

Random: $(OBJS)
	$(CC) -o Random $(OBJS) $(LIBDIR) -lmath_io -lm $(FLIB)

clean::
	rm -f *.o Random

install::
	cp -p Random $(BINDIR)
	chmod 775 $(BINDIR)/Random
	


	
