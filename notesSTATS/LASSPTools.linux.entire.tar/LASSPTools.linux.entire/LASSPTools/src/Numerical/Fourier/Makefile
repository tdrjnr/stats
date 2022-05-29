BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
# -lF77 -lI77 -lU77 for Sun
# xlf90 for PowerPC
#FLIB = -lf2c -lF77 -lI77  -lc -lm
# FLIB = -lf2c -lc -lm
FLIB = -lg2c -lc -lm
FFLAGS = -c -O
BINDIR=../../../bin
LIBDIR=-L../../../lib
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	Fourier.c \
	FourierIn.c \
	FourierOut.c 

OBJS = \
	Fourier.o \
	FourierIn.o \
	FourierOut.o 

# all: math_io fftpack Fourier
all: Fourier

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)

fftpack::
	cd ../fftpack; make FFLAGS="$(FFLAGS)" FC=$(FC) ARFLAGS=$(ARFLAGS)

.c.o:
	

$(OBJS): $(SRCS) 
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

Fourier: $(OBJS)
	$(CC) -o Fourier $(OBJS) $(LIBDIR) -lmath_io -lfftpack -lm $(FLIB)

clean::
	rm -f *.o Fourier

install::
	cp -p Fourier $(BINDIR)
	chmod 775 $(BINDIR)/Fourier
	


	
