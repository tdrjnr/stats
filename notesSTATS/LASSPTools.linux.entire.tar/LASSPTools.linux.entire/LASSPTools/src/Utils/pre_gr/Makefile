BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	pre_gr.c

OBJS = \
	pre_gr.o

all: pre_gr

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

pre_gr: $(OBJS)
	$(CC) -o pre_gr $(OBJS) -lm 

clean::
	rm -f *.o pre_gr

install::
	cp -p pre_gr $(BINDIR)
	chmod 775 $(BINDIR)/pre_gr



