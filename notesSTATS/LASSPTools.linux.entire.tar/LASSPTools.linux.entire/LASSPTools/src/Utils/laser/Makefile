BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	laser.c

OBJS = \
	laser.o

all: laser

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

laser: $(OBJS)
	$(CC) -o laser $(OBJS) -lm 

clean::
	rm -f *.o laser

install::
	cp -p laser $(BINDIR)
	chmod 775 $(BINDIR)/laser



