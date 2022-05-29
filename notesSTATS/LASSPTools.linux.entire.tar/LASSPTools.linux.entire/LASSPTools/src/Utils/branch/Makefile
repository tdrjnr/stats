BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	branch.c

OBJS = \
	branch.o

all: branch

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

branch: $(OBJS)
	$(CC) -o branch $(OBJS) -lm 

clean::
	rm -f *.o branch

install::
	cp -p branch $(BINDIR)
	chmod 775 $(BINDIR)/branch


