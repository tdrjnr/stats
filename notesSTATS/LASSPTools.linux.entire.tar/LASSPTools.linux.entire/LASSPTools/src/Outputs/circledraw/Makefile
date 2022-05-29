BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XLIBS=-L/usr/X11R6/lib -lX11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	circledraw.c \
	parse_argument.c 

OBJS = \
	circledraw.o \
	parse_argument.o 


all: circledraw

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

circledraw: $(OBJS)
	$(CC) -o circledraw $(OBJS) $(XLIBS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o circledraw

install::
	cp -p circledraw $(BINDIR)
	chmod 775 $(BINDIR)/circledraw

	


