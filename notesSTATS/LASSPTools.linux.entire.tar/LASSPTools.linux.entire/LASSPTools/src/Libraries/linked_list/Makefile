BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
LIBINSD=../../../lib
END_DEF = ****************************

SRCS = \
	ListLoad.c \
	DL_List.c

OBJS = \
	ListLoad.o \
	DL_List.o

all: liblist.a

.c.o:
	$(CC) $(CFLAGS) $(SRCS) $(EXTRA)

liblist.a: $(OBJS)
	$(AR) $(ARFLAGS) liblist.a $(OBJS)
	ranlib liblist.a

clean::
	rm -f *.o liblist.a

install::
	cp -p liblist.a $(LIBINSD)/liblist.a
	chmod 664 $(LIBINSD)/liblist.a

