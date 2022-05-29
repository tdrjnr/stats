BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	histogram.c

OBJS = \
	histogram.o

all: histogram

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

histogram: $(OBJS)
	$(CC) -o histogram $(OBJS) -lm

clean::
	rm -f *.o histogram

install::
	cp -p histogram $(BINDIR)
	chmod 775 $(BINDIR)/histogram


