BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -g -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
XLIBS=-lX11
BINDIR=../../../bin
INCDIR = -I../../../include -I/usr/include/X11
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	dlat.c \
	xinit.c \
	gx2.c 

PS_SRCS = \
	lattice2ps.c 
 
OBJS = \
	dlat.o \
	xinit.o \
	gx2.o 

PS_OBJS = \
	lattice2ps.o 

# all: math_io DynamicLattice lattice2ps
all: DynamicLattice lattice2ps

math_io::
	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS)
 
.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

DynamicLattice: $(OBJS)
	$(CC) -o DynamicLattice dlat.o xinit.o gx2.o $(LIBDIR) -lmath_io $(XLIBS) -lm 

lattice2ps: $(PS_OBJS)
	$(CC) -o lattice2ps lattice2ps.o $(LIBDIR) -lmath_io $(XLIBS) -lm 

clean::
	rm -f *.o DynamicLattice lattice2ps

install::
	cp -p DynamicLattice $(BINDIR)
	cp -p lattice2ps $(BINDIR)
	chmod 775 $(BINDIR)/DynamicLattice
	chmod 775 $(BINDIR)/lattice2ps
	


	
