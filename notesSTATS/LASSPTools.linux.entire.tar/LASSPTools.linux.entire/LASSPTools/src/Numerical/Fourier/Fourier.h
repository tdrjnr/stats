/*
** Fourier.h
**
** This file contains type definitions and external definitions for the Fourier
** program.
**
*/

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include "math_io.h"

#define TRUE 1
#define FALSE 0
#define PI 3.14159265358979

#define INITIAL_WORK_SIZE 2000

/* Define bit positions which are used as flags in the vector variable. Setting each
** corresponds to turning on the flag. For example '3' = 011 (binary) = ordinate_in
** and ordinate_out are set. 111 (7) indicates all flags are set.
**
*/
#define NO_ORDINATE 0
#define ORDINATE_IN 1
#define ORDINATE_OUT 2
#define INVERSE 4

/* These are two convience macros for determining if an integer is even
** or odd.
*/
#define IsEven(n) (! (n & 1))
#define IsOdd(n) (n&1)

/* This defines the possible types of input */
typedef enum in_t {Complex_In, Real_in, ComplexBin_In, RealBin_In} InputType;

/* This defines the possible types of output */
typedef enum out_t {Complex_Out, Real_Out, Imag_Out, Power_Out, Absolute_Out, Pack_Out} OutputType;


/* This defines what the FFT routines are expecting for data format. */
typedef struct {
  int     Wsize;
  int     N;
  double  *work;

} WorkSpace;

typedef void (*FftOut)();
typedef int (*FftIn)();

void flag_error();
void info();
int complex_fft_in_fctn();
int complex_fft_binary_in();
void complex_fft_binary_out();
void complex_fft_out();

int real_fft_in_fctn();
int real_fft_binary_in();
void real_fft_binary_out();
void real_fft_packed_out();
void real_fft_out();


#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **in, FILE **out, int *type,
		 int *inverse, int *normalize, int *center, int *vector,
		 FftOut *out_fctn, FftIn *in_fctn, int *binary_out, 
                 OutputType *otype, int *scale, int *nozero);

#else
void get_options();
#endif

#ifndef UNDERSCOREFORTRAN
void cffti();
void cfftb();
void cfftf();

void rffti();
void rfftb();
void rfftf();

#else
void cffti_();
void cfftb_();
void cfftf_();

void rffti_();
void rfftb_();
void rfftf_();
#endif



