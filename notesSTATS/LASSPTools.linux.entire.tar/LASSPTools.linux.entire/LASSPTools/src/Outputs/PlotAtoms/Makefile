BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XLIBS=-L/usr/X11R6/lib -lX11
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	main.c \
	whitespace.c \
	help.c \
	set_up.c \
	parser.c \
	image.c \
	image_bw.c 

OBJS = \
	main.o \
	whitespace.o \
	help.o \
	set_up.o \
	parser.o \
	image.o \
	image_bw.o 

all: PlotAtoms

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

PlotAtoms: $(OBJS)
	$(CC) -o PlotAtoms $(OBJS) $(XLIBS) -lm 

clean::
	rm -f *.o PlotAtoms

install::
	cp -p PlotAtoms $(BINDIR)
	chmod 775 $(BINDIR)/PlotAtoms

	


