BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
INCDIR = -I../../../include -I/usr/include/X11
LIBDIR=-L../../../lib
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	ComplexDoubleMatrices.c \
	ComplexFloatMatrices.c \
	ConvertMatrices.c

OBJS = \
	ComplexDoubleMatrices.o \
	ComplexFloatMatrices.o \
	ConvertMatrices.o

# Removed JPS 5-30-95
# LINE_OBJS = ../line_io/line_io.o

# MATH_OBJS = ../math_io/math_io.o

# all:: line_io math_io ComplexDoubleMatrices ComplexFloatMatrices ConvertMatrices
all:: ComplexDoubleMatrices ComplexFloatMatrices ConvertMatrices

# line_io: $(LINE_OBJS)
# 	cd ../line_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS);
# 
# math_io: $(MATH_OBJS)
#	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS);

.c.o:
	
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

# ConvertMatrices: ConvertMatrices.o $(LINE_OBJS) $(MATH_OBJS)
ConvertMatrices: ConvertMatrices.o
	$(CC) -o ConvertMatrices ConvertMatrices.o $(LIBDIR) -lline_io -lmath_io -lm 

# ComplexDoubleMatrices: ComplexDoubleMatrices.o $(LINE_OBJS) $(MATH_OBJS)
ComplexDoubleMatrices: ComplexDoubleMatrices.o
	$(CC) -o ComplexDoubleMatrices ComplexDoubleMatrices.o $(LIBDIR) -lline_io -lmath_io -lm 

# ComplexFloatMatrices: ComplexFloatMatrices.o $(LINE_OBJS) $(MATH_OBJS)
ComplexFloatMatrices: ComplexFloatMatrices.o
	$(CC) -o ComplexFloatMatrices ComplexFloatMatrices.o $(LIBDIR) -lline_io -lmath_io -lm 

clean::
	rm -f *.o ConvertMatrices ComplexDoubleMatrices ComplexFloatMatrices

install::
	cp -p ConvertMatrices $(BINDIR)
	cp -p ComplexDoubleMatrices $(BINDIR)
	cp -p ComplexFloatMatrices $(BINDIR)
	chmod 775 $(BINDIR)/ConvertMatrices
	chmod 775 $(BINDIR)/ComplexDoubleMatrices
	chmod 775 $(BINDIR)/ComplexFloatMatrices
