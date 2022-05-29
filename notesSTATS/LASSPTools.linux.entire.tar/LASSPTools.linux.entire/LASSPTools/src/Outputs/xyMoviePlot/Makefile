BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XTLIBS = -lXt -lX11
DEFINES=
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	main.c \
	globals.c \
	initialize.c \
	error.c \
	file.c \
	event.c \
	primitives.c \
	reconfigure.c 

OBJS = \
	main.o \
	globals.o \
	initialize.o \
	error.o \
	file.o \
	event.o \
	primitives.o \
	reconfigure.o 

all: xyMoviePlot

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $(INCDIR) $(SRCS) $(EXTRA)

xyMoviePlot: $(OBJS)
	$(CC) -o xyMoviePlot $(OBJS) $(XTLIBS)  

clean::
	rm -f *.o xyMoviePlot

install::
	cp -p xyMoviePlot $(BINDIR)
	chmod 775 $(BINDIR)/xyMoviePlot

	


