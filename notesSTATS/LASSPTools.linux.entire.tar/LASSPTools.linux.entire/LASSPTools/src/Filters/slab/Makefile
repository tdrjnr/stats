BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	slab.c \
	arbslab.c 

OBJS = \
	slab.o \
	arbslab.o

all: slab arbslab

.c.o:
	-$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

slab: $(OBJS)
	-$(CC) -o slab slab.o -lm 

arbslab: $(OBJS)
	-$(CC) -o arbslab arbslab.o -lm 

clean::
	rm -f *.o slab arbslab

install::
	-cp -p slab $(BINDIR)
	-cp -p arbslab $(BINDIR)
	-chmod 775 $(BINDIR)/slab
	-chmod 775 $(BINDIR)/arbslab

	


