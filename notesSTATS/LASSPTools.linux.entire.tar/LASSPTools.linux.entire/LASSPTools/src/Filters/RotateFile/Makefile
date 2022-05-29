BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
INCDIR = -I../../../include -I/usr/include/X11
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	RotateFile.c

OBJS = \
	RotateFile.o

all: RotateFile

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

RotateFile: $(OBJS)
	$(CC) -o RotateFile $(OBJS) 

clean::
	rm -f *.o RotateFile

install::
	cp -p RotateFile $(BINDIR)
	chmod 775 $(BINDIR)/RotateFile


