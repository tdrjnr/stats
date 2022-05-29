BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
LIBINSD=../../../lib
INCINSD=../../../include
END_DEF = ****************************

SRCS = line_io.c

OBJS = line_io.o

all: libline_io.a

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

libline_io.a: $(OBJS)
	$(AR) $(ARFLAGS) libline_io.a $(OBJS)
	ranlib libline_io.a

clean::
	rm -f *.o libline_io.a

install::
	cp -p libline_io.a $(LIBINSD)/libline_io.a
	chmod 664 $(LIBINSD)/libline_io.a
	cp -p line_io.h $(INCINSD)/line_io.h
	chmod 664 $(INCINSD)/line_io.h

