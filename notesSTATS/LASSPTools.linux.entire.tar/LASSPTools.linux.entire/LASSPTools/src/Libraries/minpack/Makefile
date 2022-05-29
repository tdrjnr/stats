BEGIN_DEF = Linux X86 Makefile
FC=g77
FFLAGS = -c -O
LIBINSD=../../../lib
# change to dpmpar_sun for Sun
# change to dpmpar_linux for X86 chips
# JPS: David Chin changed ints to doubles in weird place ... probably
# shouldn't trust dpmpar.  I've done Linux, in c, probably correctly.
SP_SRC = dpmpar_linux
# Linux dpmpar is written in c.  Probably all should be translated,
# if this works.  Will compile dpmpar_linux on other platforms, but
# won't use it.
SP_SRCC=dpmpar_linux
END_DEF = ****************************

C_SRCS = $(SP_SRCC).c

SRCS = \
	chkder.f \
	$(SP_SRC).f \
	enorm.f \
	fdjac2.f \
	lmdif.f \
	lmdif1.f \
	lmpar.f \
	qrfac.f \
	qrsolv.f \
	r1updt.f

OBJS = \
	chkder.o \
	$(SP_SRC).o \
	enorm.o \
	fdjac2.o \
	lmdif.o \
	lmdif1.o \
	lmpar.o \
	qrfac.o \
	qrsolv.o \
	r1updt.o
 
all:: libminpack.a
 
.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f 

.c.o:
	$(CC) -c $(CFLAGS) $*.c

libminpack.a: $(OBJS)
	$(AR) $(ARFLAGS) libminpack.a $(OBJS)
	ranlib libminpack.a
 
clean::
	rm -f *.o libminpack.a
 
install::
	cp -p libminpack.a $(LIBINSD)/libminpack.a
	chmod 664 $(LIBINSD)/libminpack.a

