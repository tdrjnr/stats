BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XLIBS=-L/usr/X11R6/lib -lX11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	polydraw3D.c \
	parseargument.c \
	processdata.c \
	tracking.c 

OBJS = \
	polydraw3D.o \
	parseargument.o \
	processdata.o \
	tracking.o 

all: polydraw3D

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

polydraw3D: $(OBJS)
	$(CC) -o polydraw3D $(OBJS) $(XLIBS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o polydraw3D

install::
	cp -p polydraw3D $(BINDIR)
	chmod 775 $(BINDIR)/polydraw3D

	


	
