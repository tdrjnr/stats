BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XM_LIBS= -L/usr/X11R6/lib/ -lXm -lX11 -lXt -lXext
BINDIR=../../../bin
INCDIR = -I../../../include -I/usr/include/X11
END_DEF = ****************************

SRCS = \
	main.c \
	info.c \
	info2.c \
	tracker.c \
	depth.c \
	show.c 

OBJS = \
	main.o \
	info.o \
	info2.o \
	tracker.o \
	depth.o \
	show.o 

all: PlotM

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

PlotM: $(OBJS)
	$(CC) -o PlotM $(OBJS) $(XM_LIBS) -lm 

clean::
	rm -f *.o PlotM

install::
	cp -p PlotM $(BINDIR)
	chmod 775 $(BINDIR)/PlotM

	


	
