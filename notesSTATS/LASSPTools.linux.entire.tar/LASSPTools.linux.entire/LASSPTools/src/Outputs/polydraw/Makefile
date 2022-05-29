BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XLIBS=-L/usr/X11R6/lib -lX11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	polydraw.c \
	parse_argument.c

OBJS = \
	polydraw.o \
	parse_argument.o

all: polydraw

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

polydraw: $(OBJS)
	$(CC) -o polydraw $(OBJS) $(XLIBS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o polydraw

install::
	cp -p polydraw $(BINDIR)
	chmod 775 $(BINDIR)/polydraw

	


