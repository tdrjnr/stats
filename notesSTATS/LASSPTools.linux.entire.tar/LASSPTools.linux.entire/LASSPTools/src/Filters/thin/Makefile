BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	thin.c \
	whitespace.c

OBJS = \
	thin.o \
	whitespace.o


all: thin

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

thin: $(OBJS)
	$(CC) -o thin $(OBJS) -lm 

clean::
	rm -f *.o thin

install::
	cp -p thin $(BINDIR)
	chmod 775 $(BINDIR)/thin


