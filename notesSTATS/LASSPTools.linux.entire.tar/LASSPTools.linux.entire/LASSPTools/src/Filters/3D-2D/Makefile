BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
LIBDIR=-L../../../lib
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	3D-2D.c \
	parse_argument.c

OBJS = \
	3D-2D.o \
	parse_argument.o

all: 3D-2D

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

3D-2D: $(OBJS)
	$(CC) -o 3D-2D $(OBJS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o 3D-2D

install::
	cp -p 3D-2D $(BINDIR)
	chmod 775 $(BINDIR)/3D-2D


