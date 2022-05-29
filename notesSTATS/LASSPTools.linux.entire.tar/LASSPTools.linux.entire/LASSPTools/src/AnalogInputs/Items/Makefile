BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
INCDIR = -I../../../include -I/usr/include/X11
XM_LIBS= -L/usr/X11R6/lib/ -lXm -lX11 -lXt -lXext
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	items.c

OBJS = \
	items.o

all: items

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(INCDIR) $(EXTRA)

items: $(OBJS)
	$(CC) -o items $(OBJS) $(XM_LIBS) -lm 

clean::
	rm -f *.o items

install::
	cp -p items $(BINDIR)
	chmod 775 $(BINDIR)/items

	
