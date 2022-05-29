BEGIN_DEF = ***************************
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
XAWLIBS= -L/usr/lib/X11 -lXaw -lXmu -lXt -lX11
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	killer.c \
	xplot.c 

OBJS = \
	killer.o \
	xplot.o

all: xplot

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

xplot: $(OBJS)
	$(CC) -o xplot -L /usr/X11R6/lib -lm $(XAWLIBS) $(OBJS)

clean::
	rm -f *.o xplot

install::
	cp -p xplot $(BINDIR)
	chmod 775 $(BINDIR)/xplot
