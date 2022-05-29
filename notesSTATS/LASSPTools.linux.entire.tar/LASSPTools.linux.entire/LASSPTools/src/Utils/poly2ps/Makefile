BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	poly2ps.c 

OBJS = \
	poly2ps.o

all: poly2ps

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

poly2ps: $(OBJS)
	$(CC) -o poly2ps $(OBJS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o poly2ps

install::
	cp -p poly2ps $(BINDIR)
	chmod 775 $(BINDIR)/poly2ps


	
