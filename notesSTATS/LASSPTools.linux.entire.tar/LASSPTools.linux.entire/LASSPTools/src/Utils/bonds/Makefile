BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	bonds.c

OBJS = \
	bonds.o

all: bonds

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

bonds: $(OBJS)
	$(CC) -o bonds $(OBJS) -lm 

clean::
	rm -f *.o bonds

install::
	cp -p bonds $(BINDIR)
	chmod 775 $(BINDIR)/bonds


