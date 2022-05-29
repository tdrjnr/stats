BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XM_LIBS= -L/usr/X11R6/lib/ -lXm -lX11 -lXt -lXext
BINDIR=../../../bin
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	axistick.c \
	color.c \
	config.c \
	graphics.c \
	help.c \
	parse.c \
	polar.c \
	primaryshell.c \
	pulldown.c \
	rangechange.c \
	rectangle.c \
	spherical.c \
	xy_plot.c 		

OBJS = \
	axistick.o \
	color.o \
	config.o \
	graphics.o \
	help.o \
	parse.o \
	polar.o \
	primaryshell.o \
	pulldown.o \
	rangechange.o \
	rectangle.o \
	spherical.o \
	xy_plot.o 		


all: XYinput

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

XYinput: $(OBJS)
	$(CC) -o XYinput $(OBJS) $(XM_LIBS) -lm 

clean::
	rm -f *.o XYinput

install::
	cp -p XYinput $(BINDIR)
	chmod 775 $(BINDIR)/XYinput

