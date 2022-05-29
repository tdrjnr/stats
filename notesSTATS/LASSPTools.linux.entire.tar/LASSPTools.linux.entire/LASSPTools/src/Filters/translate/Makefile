BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	translate.c

OBJS = \
	translate.o

all: translate

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

translate: $(OBJS)
	$(CC) -o translate $(OBJS) 

clean::
	rm -f *.o translate

install::
	cp -p translate $(BINDIR)
	chmod 775 $(BINDIR)/translate


