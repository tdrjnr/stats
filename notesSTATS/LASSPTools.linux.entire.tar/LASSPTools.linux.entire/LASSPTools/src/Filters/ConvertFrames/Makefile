BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
INCDIR = -I../../../include -I/usr/include/X11
LIBDIR=-L../../../lib
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	ConvertFrames.c \
	get_frame.c \
	parse_argument.c

OBJS = \
	ConvertFrames.o \
	get_frame.o \
	parse_argument.o

all: ConvertFrames

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

ConvertFrames: $(OBJS)
	$(CC) -o ConvertFrames $(OBJS) $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o ConvertFrames

install::
	cp -p ConvertFrames $(BINDIR)
	chmod 775 $(BINDIR)/ConvertFrames
