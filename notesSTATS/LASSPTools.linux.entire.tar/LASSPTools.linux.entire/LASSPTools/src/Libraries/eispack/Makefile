BEGIN_DEF = Linux X86 Makefile
FC=g77
FFLAGS = -c -O
LIBINSD=../../../lib
INCINSD=../../../include
END_DEF = ****************************

SRCS = \
	balanc.f \
	balbak.k \
	cbabk2.f \
	cbal.f \
	cdiv.f \
	comqr.f \
	comqr2.f \
	corth.f \
	csroot.f \
	elmhes.f \
	eltran.f \
	epslon.f \
	hqr.f \
	hqr2.f \
	htribk.f \
	htridi.f \
	pythag.f \
	tql2.f \
	tqlrat.f \
	tred1.f \
	tred2.f	

OBJS = \
	balanc.o \
	balbak.o \
	cbabk2.o \
	cbal.o \
	cdiv.o \
	comqr.o \
	comqr2.o \
	corth.o \
	csroot.o \
	elmhes.o \
	eltran.o \
	epslon.o \
	hqr.o \
	hqr2.o \
	htribk.o \
	htridi.o \
	pythag.o \
	tql2.o \
	tqlrat.o \
	tred1.o \
	tred2.o	

 
all:: libeispack.a
 
.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f 

libeispack.a: $(OBJS)
	$(AR) $(ARFLAGS) libeispack.a $(OBJS)
	ranlib libeispack.a
 
clean::
	rm -f *.o libeispack.a
 
install::
	cp -p libeispack.a $(LIBINSD)/libeispack.a
	cp -p eiscalls.h $(INCINSD)/eiscalls.h
	chmod 664 $(LIBINSD)/libeispack.a
	chmod 664 $(INCINSD)/eiscalls.h


