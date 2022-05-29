BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	sortit.c \
	whitespace.c

OBJS = \
	sortit.o \
	whitespace.o


all: sortit

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

sortit: $(OBJS)
	$(CC) -o sortit $(OBJS) -lm 

clean::
	rm -f *.o sortit

install::
	cp -p sortit $(BINDIR)
	chmod 775 $(BINDIR)/sortit

