BEGIN_DEF = Linux X86 Makefil
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
XM_LIBS= -L/usr/X11R6/lib/ -lXm -lX11 -lXt -lXext
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	Dial.c \
	grab.c \
	help.c \
	keys.c \
	meter.c \
	parse_argument.c

OBJS = \
	Dial.o \
	grab.o \
	help.o \
	keys.o \
	meter.o \
	parse_argument.o

all: meter

.c.o:
	-$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

meter: $(OBJS)
	-$(CC) -o meter $(OBJS) $(XM_LIBS) -lm 

clean::
	rm -f *.o meter

install::
	-cp -p meter $(BINDIR)
	-chmod 775 $(BINDIR)/meter

	


