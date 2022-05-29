BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	blend.c

OBJS = \
	blend.o

all: blend

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

blend: $(OBJS)
	$(CC) -o blend $(OBJS) -lm

clean::
	rm -f *.o blend

install::
	cp -p blend $(BINDIR)
	chmod 775 $(BINDIR)/blend


