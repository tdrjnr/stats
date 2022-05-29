#
#         L A S S P T o o l s    M a k e f i l e
#
# 
# This is the toplevel Makefile for the LASSPTools programs,
# To make an individual program you can go to the directory
# and type 'make' or from this directory you can type 
# 'make all' which will build each of the programs.
#
# Each makefile has a set of definitions that may be overridden
# by specifiying the values in the FLAGS variable in this
# makefile. The default Makefiles are for an RS6000.
# The following defintions are in Makefiles that use them:
#   F77=(FC)			/* occasionally */
#   DEFINES=
#   BINDIR=../../../bin
#   LIBDIR=-L../../../lib
#   LIBINSD=../../../lib	/* Library to install LASSP libraries */
#   LALIBS=-L../../../lib
#   INCDIR=-I../../../include
#   INCINSD=../../../include	/* Directory to install LASSP include files */
#   SYSLIBS=-L/usr/ucblib -ltermcap -lucb -lm
#   XM_LIBS=-lXm -lXt -lX11
#   FLIB=-lxlf
#   FFLAGS=-c -O
#   CFLAGS=-c -O -Q -K
#   XLIBS=-lX11
#   XAWLIBS= -L/usr/lib/X11 -lnsl -lXaw -lXmu -lXt -lX11
#   
# In addition the Makefiles will use the default macros defined
# for make. These can be overridden by setting FLAGS to other
# symbols which are passed to the lower level makefiles.
# 
# Questions can be directed to:
#   toolkeeper@msc.cornell.edu
#

#
# Step 1: Setup directories + flags for make commands,
#         the makefiles default to an IBM RS6000.
#

# LINUX on EGWENE
LASSPDIR = /home/sethna/LASSPTools
LASSPTOP = $(LASSPDIR)/src
DEFS=defs.linux


#         For a Sun Sparc comment out the following lines.
#
#LASSPDIR = /usr/local/pub/tools
#LASSPTOP = $(LASSPDIR)/src
#LIB_DIR =  $(LASSPDIR)/lib
#INC_DIR =  $(LASSPDIR)/include
#LIBS = LALIBS=-L$(LIB_DIR)
#DEFS=defs.ibm
# These flags are for the RS6000 (Default)
FLAGS = 

# 
# 
# Uncomment the following line for the Sun Sparc to override the 
# defaults.
# LASSPDIR = /usr/local/pub/tools
# LASSPTOP = $(LASSPDIR)/src
# LIB_DIR =  $(LASSPDIR)/lib
# INC_DIR =  $(LASSPDIR)/include
# LIBS = LALIBS=-L$(LIB_DIR)
# DEFS=defs.sun
# LACFLAGS = "-c -O -DSUN -D_NO_PROTO -DSTRINGS_ALIGNED -DNO_REGEX -DNO_ISDIR -DUSE_RECOMP -DUSE_GETWD -DUNDERSCOREFORTRAN"

# FLAGS = F77=f77 FLIB="-lF77 -lI77 -lU77" CFLAGS=$(LACFLAGS) SP_SRC=dpmpar_sun ARFLAGS=rv

#
#Uncomment the following line for the AT&T 386
# FLAGS = ""

#
# The following lines define the directories where each of
# programs and/or libraries can be found. They are assumed
# all to be at the same level in the directory tree.
#
LIBRARIES = $(LASSPTOP)/Libraries
LIB_MATHIO = $(LIBRARIES)/math_io
LIB_FFTPACK = $(LIBRARIES)/fftpack
LIB_EISPACK = $(LIBRARIES)/eispack
LIB_MINPACK = $(LIBRARIES)/minpack
LIB_LINPACK = $(LIBRARIES)/linpack
LIB_LINEIO = $(LIBRARIES)/line_io
LIB_LIST = $(LIBRARIES)/linked_list
LIB_BLAS = $(LIBRARIES)/blas


#AnalogInputs
ROTATE = $(LASSPTOP)/AnalogInputs/Rotate 
ITEMS = $(LASSPTOP)/AnalogInputs/Items
SLIDER = $(LASSPTOP)/AnalogInputs/Slider
XYINPUT = $(LASSPTOP)/AnalogInputs/XYinput

#Filters
3D-2DP = $(LASSPTOP)/Filters/3D-2D
CONVERT_FRAME = $(LASSPTOP)/Filters/ConvertFrames
CONVERT_MATRICES = $(LASSPTOP)/Filters/ConvertMatrices
COMPLEX_FLOAT_MATRICES = $(CONVERT_MATRICES)
COMPLEX_DBL_MATRICES =  $(CONVERT_MATRICES)
CONVERT_VECTOR = $(LASSPTOP)/Filters/ConvertVectors
COMPLEX_FLOAT_VECTORS = $(CONVERT_VECTOR)
COMPLEX_DBL_VECTORS =  $(CONVERT_VECTOR)
ROTATEFILE = $(LASSPTOP)/Filters/RotateFile
ROTATE_STREAM = $(LASSPTOP)/Filters/RotateStream
ROTATE_AID = $(ROTATE_STREAM)
ROTATE_ECHO = $(ROTATE_STREAM)
BLEND = $(LASSPTOP)/Filters/blend
HISTOGRAM = $(LASSPTOP)/Filters/histogram
SLAB = $(LASSPTOP)/Filters/slab
ARBSLAB = $(SLAB)
SORTIT = $(LASSPTOP)/Filters/sortit
THIN = $(LASSPTOP)/Filters/thin
TRANSLATE = $(LASSPTOP)/Filters/translate

# Numerical
DI = $(LASSPTOP)/Numerical/DI 
DET = $(DI)
INVERT = $(DI)
EIGEN = $(LASSPTOP)/Numerical/Eigen
FITIT = $(LASSPTOP)/Numerical/FitIt
FOURIER = $(LASSPTOP)/Numerical/Fourier
MMULT = $(LASSPTOP)/Numerical/MMult
MSOLVE = $(LASSPTOP)/Numerical/MSolve
RANDOM = $(LASSPTOP)/Numerical/Random

# Outputs
DYNAMICLATTICE = $(LASSPTOP)/Outputs/DynamicLattice
LATTICE2PS = $(DYNAMICLATTICE)
PLOTATOMS = $(LASSPTOP)/Outputs/PlotAtoms
SURFPLOT = $(LASSPTOP)/Outputs/SurfPlot
CIRCLEDRAW = $(LASSPTOP)/Outputs/circledraw
MATRIX2X = $(LASSPTOP)/Outputs/matrix2X
METER = $(LASSPTOP)/Outputs/meter
POLYDRAW = $(LASSPTOP)/Outputs/polydraw
POLYDRAW3D = $(LASSPTOP)/Outputs/polydraw3D
# JPS: Removed from distribution XPLOT = $(LASSPTOP)/Outputs/xplot
# JPS: Removed from distribution XYMOVIEPLOT = $(LASSPTOP)/Outputs/xyMoviePlot
PLOTM = $(LASSPTOP)/Outputs/PlotM

# Utils
AXIS = $(LASSPTOP)/Utils/axis
BRANCH = $(LASSPTOP)/Utils/branch
INTER = $(LASSPTOP)/Utils/inter
LASER = $(LASSPTOP)/Utils/laser
PRE_GR = $(LASSPTOP)/Utils/pre_gr
BONDS = $(LASSPTOP)/Utils/bonds
POLY2PS = $(LASSPTOP)/Utils/poly2ps

#
# This list defines the programs that will be built when a make is
# done, if you do not want to build a particular program put a '#'
# as the first character of the line to comment it out. Some programs
# require libraries to be built, the following lists the dependencies
# of programs on libraries:
#     math_io.a =
#     linpack.a =
#     blas.a =
#     minpack.a =
#     eispack.a =
#     fftpack.a =
#     
# If the program(s) you are interested in building do not require the
# libraries you can comment out the lines. If you you are building all
# the programs then all the libraries must be built.
#

PROGRAMS = \
        3D-2D \
	ComplexDoubleMatrices \
	ComplexDoubleVectors \
	ComplexFloatMatrices \
	ComplexFloatVectors \
	ConvertFrames \
	ConvertMatrices \
	ConvertVectors \
	DI \
	Det \
	DynamicLattice \
	Eigen \
	Fourier \
	Invert \
	MMult \
	MSolve \
	PlotAtoms \
	PlotM \
	Random \
	RotateFile \
	RotateStream \
	SurfPlot \
	XYinput \
	arbslab \
	axis \
	blend \
	branch \
	circledraw \
	fit_it \
	histogram \
	inter \
	items \
	laser \
	lattice2ps \
	matrix2X \
	meter \
	movieaxis \
	newaxis \
	poly2ps \
	polydraw \
	polydraw3D \
	pre_xmgr \
	rotate \
	rotate_aid \
	rotate_echo \
	slab \
	slider \
	sortit \
	thin \
	bonds \
	translate 
# JPS: Removed xplot from distribution
#	translate \
#	xplot
# JPS: Removed xyMoviePlot from distribution
#	xplot \
#	xyMoviePlot 

#---------------------------------------------------------------------
#  
# This is where the real work begins, basically we cd into each of
# directories we want to build, pass some flags that will 
# override any in the target makefile.
#
# This will build all programs in the above list
#
#--------------------------------------------------------------------
all: $(PROGRAMS)

world::
	$(CC) up_make.c -o up_make
	make Makefiles	
	make Libraries
	make install_libs
	make

Makefiles::
	cd $(LIB_MATHIO); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_FFTPACK); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_EISPACK); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_MINPACK); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_LINPACK); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_LINEIO); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_LIST); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LIB_BLAS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(ROTATE); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(ITEMS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(SLIDER); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(XYINPUT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(3D-2DP); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(CONVERT_FRAME); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(CONVERT_MATRICES); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(CONVERT_VECTOR); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(ROTATEFILE); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(ROTATE_STREAM); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(BLEND); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(HISTOGRAM); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(SLAB); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(SORTIT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(THIN); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(TRANSLATE); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(DI); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(EIGEN); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(FITIT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(FOURIER); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS) 
	cd $(MMULT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(MSOLVE); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(RANDOM); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(DYNAMICLATTICE); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(PLOTATOMS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(PLOTM); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(SURFPLOT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(CIRCLEDRAW); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(MATRIX2X); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(METER); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(POLYDRAW); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(POLYDRAW3D); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
# JPS: removed from distribution
#	cd $(XPLOT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
# JPS: removed from distribution
#	cd $(XYMOVIEPLOT); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(POLY2PS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(AXIS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(BRANCH); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(BONDS); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(INTER); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(LASER); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
	cd $(PRE_GR); $(LASSPTOP)/up_make Makefile $(LASSPTOP)/$(DEFS)
 
Libraries::
	make math_io
	make linpack
	make minpack
	make eispack
	make fftpack
	make line_io
	make linked_list
	make blas

install_libs::
	cd $(LIB_MATHIO); make install $(FLAGS)
	cd $(LIB_LINPACK); make install $(FLAGS)
	cd $(LIB_MINPACK); make install $(FLAGS)
	cd $(LIB_EISPACK); make install $(FLAGS)
	cd $(LIB_FFTPACK); make install $(FLAGS)
	cd $(LIB_LINEIO); make install $(FLAGS)
	cd $(LIB_LIST); make install $(FLAGS)
	cd $(LIB_BLAS); make install $(FLAGS)

# 
# This builds the math_io library for any other programs that need it
#
math_io::
	cd $(LIB_MATHIO); make $(FLAGS)

linpack::
	cd $(LIB_LINPACK); make $(FLAGS)

minpack:: 
	cd $(LIB_MINPACK); make $(FLAGS)

eispack::
	cd $(LIB_EISPACK); make $(FLAGS)

fftpack::
	cd $(LIB_FFTPACK); make $(FLAGS)

line_io::
	cd $(LIB_LINEIO); make $(FLAGS)

linked_list::
	cd $(LIB_LIST); make $(FLAGS)

blas::
	cd $(LIB_BLAS); make $(FLAGS)

rotate::
	cd $(ROTATE); make $(FLAGS)

items::
	cd $(ITEMS); make $(FLAGS)

slider::
	cd $(SLIDER); make $(FLAGS)

XYinput::
	cd $(XYINPUT); make $(FLAGS)

3D-2D::
	cd $(3D-2DP); make $(FLAGS) $(LIBS) 

ConvertFrames::
	cd $(CONVERT_FRAME); make $(FLAGS) $(LIBS) 

ConvertMatrices::
	cd $(CONVERT_MATRICES); make $(FLAGS) $(LIBS)
	
ComplexDoubleMatrices::
	cd $(COMPLEX_DBL_MATRICES); make $(FLAGS) $(LIBS)

ComplexFloatMatrices::
	cd $(COMPLEX_FLOAT_MATRICES); make $(FLAGS) $(LIBS)

ConvertVectors::
	cd $(CONVERT_VECTOR); make $(FLAGS) $(LIBS) 

ComplexFloatVectors::
	cd $(COMPLEX_FLOAT_VECTORS); make $(FLAGS) $(LIBS) 

ComplexDoubleVectors::
	cd $(COMPLEX_DBL_VECTORS); make $(FLAGS) $(LIBS) 

RotateFile::
	cd $(ROTATEFILE); make $(FLAGS)

RotateStream::
	cd $(ROTATE_STREAM); make RotateStream $(FLAGS) $(LIBS)

rotate_aid::
	cd $(ROTATE_AID); make rotate_aid $(FLAGS);

rotate_echo::
	cd $(ROTATE_ECHO); make rotate_echo $(FLAGS);

blend::
	cd $(BLEND); make $(FLAGS)

histogram::
	cd $(HISTOGRAM); make $(FLAGS)

arbslab::
	cd $(ARBSLAB); make arbslab $(FLAGS)

slab::
	cd $(SLAB); make slab $(FLAGS)

sortit::
	cd $(SORTIT); make $(FLAGS)

thin::
	cd $(THIN); make $(FLAGS)

translate::
	cd $(TRANSLATE); make $(FLAGS)

DI::
	cd $(DI); make DI $(FLAGS) $(LIBS)

Det::
	cd $(DET); make Det $(FLAGS) $(LIBS)

Invert::
	cd $(INVERT); make Invert $(FLAGS) $(LIBS)

Eigen::
	cd $(EIGEN); make $(FLAGS) $(LIBS)

fit_it::
	cd $(FITIT); make $(FLAGS) $(LIBS)

Fourier::
	cd $(FOURIER); make $(FLAGS) $(LIBS)

MMult::
	cd $(MMULT); make $(FLAGS) $(LIBS)

MSolve::
	cd $(MSOLVE); make $(FLAGS) $(LIBS)

Random::
	cd $(RANDOM); make $(FLAGS) $(LIBS)

DynamicLattice::
	cd $(DYNAMICLATTICE); make DynamicLattice $(FLAGS)

lattice2ps::
	cd $(LATTICE2PS); make lattice2ps $(FLAGS)

PlotAtoms::
	cd $(PLOTATOMS); make $(FLAGS)

PlotM::
	cd $(PLOTM); make $(FLAGS)

SurfPlot::
	cd $(SURFPLOT); make $(FLAGS) $(LIBS)

circledraw::
	cd $(CIRCLEDRAW); make $(FLAGS) $(LIBS)

matrix2X::
	cd $(MATRIX2X); make $(FLAGS) $(LIBS)

meter::
	cd $(METER); make $(FLAGS)

polydraw::
	cd $(POLYDRAW); make $(FLAGS) $(LIBS)

polydraw3D::
	cd $(POLYDRAW3D); make $(FLAGS) $(LIBS)

# JPS: removed xplot from distribution
# xplot::
# 	cd $(XPLOT); make $(FLAGS)

# JPS: removed xxyMoviePlot from distribution
# xyMoviePlot::
#	cd $(XYMOVIEPLOT); make $(FLAGS)

poly2ps::
	cd $(POLY2PS); make $(FLAGS) $(LIBS)

axis::
	cd $(AXIS); make axis $(FLAGS)

movieaxis::
	cd $(AXIS); make movieaxis $(FLAGS)

newaxis::
	cd $(AXIS); make newaxis $(FLAGS)

branch::
	cd $(BRANCH); make $(FLAGS)

bonds::
	cd $(BONDS); make $(FLAGS)

inter::
	cd $(INTER); make $(FLAGS)

laser::
	cd $(LASER); make $(FLAGS)

pre_xmgr::
	cd $(PRE_GR); make $(FLAGS)

#------------------------------------------------------------------
#
# This will remove the .o and executable files in each of the
# source directories.
clean::
	cd $(LIB_MATHIO); make clean
	cd $(LIB_FFTPACK); make clean
	cd $(LIB_EISPACK); make clean
	cd $(LIB_MINPACK); make clean
	cd $(LIB_LINPACK); make clean
	cd $(LIB_LINEIO); make clean
	cd $(LIB_LIST); make clean
	cd $(LIB_BLAS); make clean 
	cd $(ROTATE); make clean
	cd $(ITEMS); make clean
	cd $(SLIDER); make clean
	cd $(XYINPUT); make clean
	cd $(3D-2DP); make clean
	cd $(CONVERT_FRAME); make clean
	cd $(CONVERT_MATRICES); make clean
	cd $(CONVERT_VECTOR); make clean
	cd $(ROTATEFILE); make clean
	cd $(ROTATE_STREAM); make clean
	cd $(BLEND); make clean
	cd $(HISTOGRAM); make clean
	cd $(SLAB); make clean
	cd $(SORTIT); make clean
	cd $(THIN); make clean
	cd $(TRANSLATE); make clean
	cd $(DI); make clean
	cd $(EIGEN); make clean
	cd $(FITIT); make clean
	cd $(FOURIER); make clean
	cd $(MMULT); make clean
	cd $(MSOLVE); make clean
	cd $(RANDOM); make clean
	cd $(DYNAMICLATTICE); make clean
	cd $(PLOTATOMS); make clean
	cd $(PLOTM); make clean
	cd $(SURFPLOT); make clean
	cd $(CIRCLEDRAW); make clean
	cd $(MATRIX2X); make clean
	cd $(METER); make clean
	cd $(POLYDRAW); make clean
	cd $(POLYDRAW3D); make clean
# JPS: removed from distribution	cd $(XYMOVIEPLOT); make clean
#	cd $(XPLOT); make clean
# JPS: removed from distribution	cd $(XYMOVIEPLOT); make clean
	cd $(POLY2PS); make clean
	cd $(AXIS); make clean
	cd $(BRANCH); make clean
	cd $(BONDS); make clean
	cd $(INTER); make clean
	cd $(LASER); make clean
	cd $(PRE_GR); make clean

#
# This will move each of the programs built to a common
# directory as defined by BINDIR, the default for BINDIR
# is $(LASSPTOP)/bin
#
install::
	cd $(ROTATE); make install LASSPDIR=$(LASSPDIR)
	cd $(ITEMS); make install LASSPDIR=$(LASSPDIR)
	cd $(SLIDER); make install LASSPDIR=$(LASSPDIR)
	cd $(XYINPUT); make install LASSPDIR=$(LASSPDIR)
	cd $(3D-2DP); make install LASSPDIR=$(LASSPDIR)
	cd $(CONVERT_FRAME); make install LASSPDIR=$(LASSPDIR)
	cd $(CONVERT_MATRICES); make install LASSPDIR=$(LASSPDIR)
	cd $(CONVERT_VECTOR); make install LASSPDIR=$(LASSPDIR)
	cd $(ROTATEFILE); make install LASSPDIR=$(LASSPDIR)
	cd $(ROTATE_STREAM); make install LASSPDIR=$(LASSPDIR)
	cd $(BLEND); make install LASSPDIR=$(LASSPDIR)
	cd $(HISTOGRAM); make install LASSPDIR=$(LASSPDIR)
	cd $(SLAB); make install LASSPDIR=$(LASSPDIR)
	cd $(SORTIT); make install LASSPDIR=$(LASSPDIR)
	cd $(THIN); make install LASSPDIR=$(LASSPDIR)
	cd $(TRANSLATE); make install LASSPDIR=$(LASSPDIR)
	cd $(DI); make install LASSPDIR=$(LASSPDIR)
	cd $(EIGEN); make install LASSPDIR=$(LASSPDIR)
	cd $(FITIT); make install LASSPDIR=$(LASSPDIR)
	cd $(FOURIER); make install LASSPDIR=$(LASSPDIR)
	cd $(MMULT); make install LASSPDIR=$(LASSPDIR)
	cd $(MSOLVE); make install LASSPDIR=$(LASSPDIR)
	cd $(RANDOM); make install LASSPDIR=$(LASSPDIR)
	cd $(DYNAMICLATTICE); make install LASSPDIR=$(LASSPDIR)
	cd $(PLOTATOMS); make install LASSPDIR=$(LASSPDIR)
	cd $(PLOTM); make install LASSPDIR=$(LASSPDIR)
	cd $(SURFPLOT); make install LASSPDIR=$(LASSPDIR)
	cd $(CIRCLEDRAW); make install LASSPDIR=$(LASSPDIR)
	cd $(MATRIX2X); make install LASSPDIR=$(LASSPDIR)
	cd $(METER); make install LASSPDIR=$(LASSPDIR)
	cd $(POLYDRAW); make install LASSPDIR=$(LASSPDIR)
	cd $(POLYDRAW3D); make install LASSPDIR=$(LASSPDIR)
# JPS: removed from distribution
#	cd $(XPLOT); make install LASSPDIR=$(LASSPDIR)
# JPS: removed from distribution
#	cd $(XYMOVIEPLOT); make install LASSPDIR=$(LASSPDIR)
	cd $(POLY2PS); make install LASSPDIR=$(LASSPDIR)
	cd $(AXIS); make install LASSPDIR=$(LASSPDIR)
	cd $(BRANCH); make install LASSPDIR=$(LASSPDIR)
	cd $(BONDS); make install LASSPDIR=$(LASSPDIR)
	cd $(INTER); make install LASSPDIR=$(LASSPDIR)
	cd $(LASER); make install LASSPDIR=$(LASSPDIR)
	cd $(PRE_GR); make install LASSPDIR=$(LASSPDIR)

#---------- End of Makefile -----------------




