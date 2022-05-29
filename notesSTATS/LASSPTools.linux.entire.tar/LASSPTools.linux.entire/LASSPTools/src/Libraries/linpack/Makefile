BEGIN_DEF = Linux X86 Makefile
FC=g77
FFLAGS = -c -O
LIBINSD=../../../lib
INCINSD=../../../include
END_DEF = ****************************

SRCS = \
	dgeco.f \
	dgedi.f \
	dgefa.f \
	dgesl.f \
	dgtsl.f \
	dsico.f \
	dsidi.f \
	dsifa.f \
	dsisl.f \
	dtrco.f \
	dtrdi.f \
	dtrsl.f \
	zgeco.f \
	zgedi.f \
	zgefa.f \
	zgesl.f \
	zgtsl.f \
	zhico.f \
	zhidi.f \
	zhifa.f \
	zhisl.f \
	zsico.f \
	zsidi.f \
	zsifa.f \
	zsisl.f \
	ztrco.f \
	ztrdi.f \
	ztrsl.f 

OBJS = \
	dgeco.o \
	dgedi.o \
	dgefa.o \
	dgesl.o \
	dgtsl.o \
	dsico.o \
	dsidi.o \
	dsifa.o \
	dsisl.o \
	dtrco.o \
	dtrdi.o \
	dtrsl.o \
	zgeco.o \
	zgedi.o \
	zgefa.o \
	zgesl.o \
	zgtsl.o \
	zhico.o \
	zhidi.o \
	zhifa.o \
	zhisl.o \
	zsico.o \
	zsidi.o \
	zsifa.o \
	zsisl.o \
	ztrco.o \
	ztrdi.o \
	ztrsl.o 
 
all:: liblinpack.a
 
.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f 

liblinpack.a: $(OBJS)
	$(AR) $(ARFLAGS) liblinpack.a $(OBJS)
	ranlib liblinpack.a
 
clean::
	rm -f *.o liblinpack.a
 
install::
	cp -p liblinpack.a $(LIBINSD)/liblinpack.a
	chmod 664 $(LIBINSD)/liblinpack.a
	cp -p lincalls.h $(INCINSD)/lincalls.h
	chmod 664 $(INCINSD)/lincalls.h

