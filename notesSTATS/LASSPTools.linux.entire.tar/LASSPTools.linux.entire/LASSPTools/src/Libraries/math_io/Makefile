BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
INCINSD=../../../include
LIBINSD=../../../lib
END_DEF = ****************************

SRCS = math_io.c

OBJS = math_io.o

all: libmath_io.a

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

libmath_io.a: $(OBJS)
	$(AR) $(ARFLAGS) libmath_io.a $(OBJS)
	ranlib libmath_io.a

clean::
	rm -f *.o libmath_io.a

install::
	cp -p libmath_io.a $(LIBINSD)/libmath_io.a
	chmod 664 $(LIBINSD)/libmath_io.a
	cp -p math_io.h $(INCINSD)/math_io.h
	chmod 664 $(INCINSD)/math_io.h
