BEGIN_DEF = Linux X86 Makefile
FC=g77
FFLAGS = -c -O
LIBINSD=../../../lib
END_DEF = ****************************

SRCS = \
	cfftb.f \
	cfftb1.f \
	cfftf.f \
	cfftf1.f \
	cffti.f \
	cffti1.f \
	passb.f \
	passb2.f \
	passb3.f \
	passb4.f \
	passb5.f \
	passf.f \
	passf2.f \
	passf3.f \
	passf4.f \
	passf5.f \
	pimach.f \
	radb2.f \
	radb3.f \
	radb4.f \
	radb5.f \
	radb5.f \
	radbg.f \
	radf2.f \
	radf3.f \
	radf4.f \
	radf5.f \
	radfg.f \
	rfftb.f \
	rfftb1.f \
	rfftf.f \
	rfftf1.f \
	rffti.f \
	rffti1.f 

OBJS = \
	cfftb.o \
	cfftb1.o \
	cfftf.o \
	cfftf1.o \
	cffti.o \
	cffti1.o \
	passb.o \
	passb2.o \
	passb3.o \
	passb4.o \
	passb5.o \
	passf.o \
	passf2.o \
	passf3.o \
	passf4.o \
	passf5.o \
	pimach.o \
	radb2.o \
	radb3.o \
	radb4.o \
	radb5.o \
	radb5.o \
	radbg.o \
	radf2.o \
	radf3.o \
	radf4.o \
	radf5.o \
	radfg.o \
	rfftb.o \
	rfftb1.o \
	rfftf.o \
	rfftf1.o \
	rffti.o \
	rffti1.o 
 
all:: libfftpack.a
 
.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f 

libfftpack.a: $(OBJS)
	$(AR) $(ARFLAGS) libfftpack.a $(OBJS)
	ranlib libfftpack.a
 
clean::
	rm -f *.o libfftpack.a
 
install::
	cp -p libfftpack.a $(LIBINSD)/libfftpack.a
	chmod 664 $(LIBINSD)/libfftpack.a

 	
