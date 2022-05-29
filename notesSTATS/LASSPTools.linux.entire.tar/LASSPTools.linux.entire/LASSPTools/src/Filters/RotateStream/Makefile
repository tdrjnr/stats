BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
BINDIR=../../../bin
INCDIR = -I../../../include -I/usr/include/X11
LIBDIR=-L../../../lib
END_DEF = ****************************

SRCS = \
	RotateStream.c \
	get_frame.c \
	parse_argument.c \
	rotate_aid.c \
	rotate_echo.c \
	rotate_frame.c 

OBJS = \
	RotateStream.o \
	get_frame.o \
	parse_argument.o \
	rotate_aid.o \
	rotate_echo.o \
	rotate_frame.o 


all: rotate_echo rotate_aid RotateStream

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

rotate_echo: $(OBJS)
	$(CC) -o rotate_echo rotate_echo.o -lm 

rotate_aid: $(OBJS)
	$(CC) -o rotate_aid rotate_aid.o parse_argument.o -lm 

RotateStream: $(OBJS)
	$(CC) -o RotateStream RotateStream.o rotate_frame.o parse_argument.o get_frame.o $(LIBDIR) -lline_io -lm 

clean::
	rm -f *.o rotate_aid rotate_echo RotateStream

install::
	cp -p rotate_aid $(BINDIR)
	cp -p rotate_echo $(BINDIR)
	cp -p RotateStream $(BINDIR)
	chmod 775 $(BINDIR)/rotate_aid
	chmod 775 $(BINDIR)/rotate_echo
	chmod 775 $(BINDIR)/RotateStream
	


	
