BEGIN_DEF = Linux X86 Makefile
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
CFLAGS_SUN = -c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN
INCDIR = -I../../../include -I/usr/include/X11
LIBDIR=-L../../../lib
BINDIR=../../../bin
END_DEF = ****************************

SRCS = \
	ComplexDoubleVectors.c \
	ComplexFloatVectors.c \
	ConvertVectors.c 

OBJS = \
	ComplexDoubleVectors.o \
	ComplexFloatVectors.o \
	ConvertVectors.o 

# Removed 5-30-95 JPS
# LINE_OBJS = ../line_io/line_io.o

# MATH_OBJS = ../math_io/math_io.o

#all:: line_io math_io ComplexDoubleVectors ComplexFloatVectors ConvertVectors
all:: ComplexDoubleVectors ComplexFloatVectors ConvertVectors

# line_io: $(LINE_OBJS)
# 	cd ../line_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS);

# math_io: $(MATH_OBJS)
# 	cd ../math_io; make CFLAGS="$(CFLAGS)" ARFLAGS=$(ARFLAGS);

.c.o:

$(OBJS): $(SRCS)	
	$(CC) $(CFLAGS) $(INCDIR) $*.c $(EXTRA)

# ConvertVectors: ConvertVectors.o $(LINE_OBJS) $(MATH_OBJS)
ConvertVectors: ConvertVectors.o
	$(CC) -o ConvertVectors ConvertVectors.o $(LIBDIR) -lline_io -lmath_io -lm 

# ComplexDoubleVectors: ComplexDoubleVectors.o $(LINE_OBS) $(MATH_OBJS)
ComplexDoubleVectors: ComplexDoubleVectors.o
	$(CC) -o ComplexDoubleVectors ComplexDoubleVectors.o $(LIBDIR) -lline_io -lmath_io -lm 

# ComplexFloatVectors: ComplexFloatVectors.o $(LINE_OBJS) $(MATH_OBJS)
ComplexFloatVectors: ComplexFloatVectors.o
	$(CC) -o ComplexFloatVectors ComplexFloatVectors.o $(LIBDIR) -lline_io -lmath_io -lm 

clean::
	rm -f *.o ConvertVectors ComplexDoubleVectors ComplexFloatVectors

install::
	cp -p ConvertVectors $(BINDIR)
	cp -p ComplexDoubleVectors $(BINDIR)
	cp -p ComplexFloatVectors $(BINDIR)
	chmod 775 $(BINDIR)/ConvertVectors
	chmod 775 $(BINDIR)/ComplexDoubleVectors
	chmod 775 $(BINDIR)/ComplexFloatVectors
	

