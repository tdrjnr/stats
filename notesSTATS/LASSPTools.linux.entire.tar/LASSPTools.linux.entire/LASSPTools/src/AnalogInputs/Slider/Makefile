BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
# XM_LIBS= /usr/X11R6/lib/libXm.a -lX11 -lXt -lXext 
XM_LIBS= -L/usr/X11R6/lib/ -lXm -lX11 -lXt -lXext
# XM_LIBS= /usr/X11R6/lib/libXm.a /usr/X11R6/lib/libXt.a -lXext /usr/X11R6/lib/libX11.a
# INCDIR = -I../../../include -I/usr/include/X11
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	slider.c

OBJS = \
	slider.o

all: slider

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

slider: $(OBJS)
	$(CC) -o slider $(OBJS) $(XM_LIBS) -lm 

clean::
	rm -f *.o slider

install::
	cp -p slider $(BINDIR)
	chmod 775 $(BINDIR)/slider

