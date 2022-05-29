#
# The Following defintions are for the IBM RS600 running AIX
#
BEGIN_DEF = Linux X86 Makefile
FC=g77
FFLAGS = -c -O
LIBINSD=../../../lib
END_DEF = *****************************

SRCS = \
	dasum.f \
	daxpy.f \
	dcabs1.f \
	dcopy.f \
	ddot.f \
	dgemm.f \
	dscal.f \
	dswap.f \
	dzasum.f \
	idamax.f \
	izamax.f \
	lsame.f \
	xerbla.f \
	zaxpy.f \
	zcopy.f \
	zdotc.f \
	zdotu.f \
	zdscal.f \
	zgemm.f \
	zscal.f \
	zswap.f

OBJS = \
	dasum.o \
	daxpy.o \
	dcabs1.o \
	dcopy.o \
	ddot.o \
	dgemm.o \
	dscal.o \
	dswap.o \
	dzasum.o \
	idamax.o \
	izamax.o \
	lsame.o \
	xerbla.o \
	zaxpy.o \
	zcopy.o \
	zdotc.o \
	zdotu.o \
	zdscal.o \
	zgemm.o \
	zscal.o \
	zswap.o
 
all:: libblas.a
 
.f.o:
	$(FC) $(FFLAGS) $(EXTRA) $*.f

libblas.a: $(OBJS)
	$(AR) $(ARFLAGS) libblas.a $(OBJS)
	ranlib libblas.a
 
clean::
	rm -f *.o libblas.a
 
install::
	cp -p libblas.a $(LIBINSD)/libblas.a
	chmod 664 $(LIBINSD)/libblas.a

