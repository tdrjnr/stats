/*
** Program Fourier
**
** Finds the FFT or inverse FFT of real or complex input data. Along with
** the raw input data the time (or frequency) can be input for each point.
** The output functions handle normalizing the FFT reaults as well as 
** account for time shifting.
** This program uses the core fftpack routines to preform the FFT's. 
** Information can be found in the file Fourier.DOC.
**
** This is called from a UNIX command line as           
** Fourier [-real, -complex] [-?, ? -h, -help]
** 
** flag                abbr     purpose
** ----------------------------------------------------------------------
** -complex_input    : -ci   : use complex input data
** -real_input       : -ri   : use real input data
** -fft              : ***** : calculate the fft
** -inverse          : -inv  : calculate the inverse fft
** -symmetric_norm   : -sn   : Normalizes
** -unnormalize      : -u    : do not normalize the output 
** -center           : -cen  : center the output around the 0 wave vector
** -ordinate_in      : ****  : Input the position with the wave vector
** -ordinate_out     : ****  : Output the position in k space
** -absolute_output  : -ao   : output the absolute value of the results
** -complex_output   : -co   : output the complex results
** -imaginary_output : -io   : output the imaginary parts of the results
** -packed_output    : -pack : used with -ri -inv to to get real output
** -power_output     : -po   : output the absolute value squared of the results
** -real_output      : -ro   : output the real parts of the results
** -no_scale         : ***** : do not scale the results by exp(i*(2*PI*m*x0)/L)
** ?                 : ***** : get the command line help message
** -?                : ***** : get the command line help message
** -h                : ***** : get the command line help message
** -help             : ***** : get the command line help message
** -binary_input     : ***** : input the data using the binary mode
** -binary_output    : ***** : output the data using the binary mode
** -file_input       : ***** : set the default input to InputFile
** -file_output      : ***** : output to OutputFile, not stdout
** -nozero                   : do not output the k=0 point.
**
** Utilizes the double precision fftpack routines
** Version 1.0 August 14, 1991
** Written by Eric Lochstet
** 80% rewritten by Jim Frankenberger 1/93 to support time or frequency scales,
** and break into common functions. COMMENTS also added.
*/

#include "Fourier.h"

#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif

{
  Matrix  *A;
  ComplexDoubleMatrix *C;
  WorkSpace Z;

  FftOut  out_fctn;
  FftIn   in_fctn;
  FILE    *in;
  FILE    *out; 
  clock_t start_time,stop_time; float elapsed_time;
  int     type;   /* type is 0 for real matrix, 1 for complex */
  int     center; /* center is 0 for uncentered output, 1 for output
		   * centered around 0. The default is uncentered */
  int     vector; /* if vector is 1 the position is part of the input
		     if vector is 2 the wave vector is output
                     if vector is 3 position in, wave vector out
		     if vector is 0 no position and no wave vector out
                  */
  int     inverse;/* if inverse is 1 the inverse fft is calculated 
		   * the default is to calculate the fft not the inverse */
  int     normalize; /* if normalize is 1, the default, then the data is 
		      * normalized by dividing it by the square root of N */
  int     N;  /* Changed from N[1], jwf 8-27-92 */
  int     nozero;
  double  norm;
  double  L;
  double  x0;
  int scale = TRUE;
  int first_time = TRUE;
  int binary_out;
  OutputType out_type;

  get_options(argc, argv, &in, &out, &type, &inverse, &normalize, &center,
	      &vector, &out_fctn, &in_fctn, &binary_out, &out_type, &scale,
	      &nozero);

  Z.work = (double *)malloc((size_t)INITIAL_WORK_SIZE*sizeof(double));
  if(Z.work==NULL)
    error("Fourier: unable to allocate work space");
  else {
    Z.Wsize = INITIAL_WORK_SIZE;
    Z.N = 0;
  }

  if(type) { /* complex data */
    C = create_default_complex_double_matrix();

     while(TRUE) {

        if ((!first_time) && (!binary_out))  {
           fprintf(out,"\n");
           fflush(out);
        } else
           first_time = FALSE;

	if(feof(in) || in_fctn(&C,in,vector,&L,&x0)==EOF) exit(0);
	
        
	if(C->rows == 1)
	  N = C->cols;
	else if(C->cols == 1)
	  N = C->rows;
	else error("Fourier: data in wrong format");

	if((4*N+15) > Z.Wsize) {
	  Z.Wsize = 4*N+15;
	  Z.work = (double *)realloc(Z.work,
				      (size_t)(Z.Wsize*sizeof(double)));
	  if(Z.work == NULL)
	    error("Fourier: unable to allocate enough work space");
	}
	if(N != Z.N) {
#ifndef UNDERSCOREFORTRAN
	  cffti(&N, Z.work);
#else
	  cffti_(&N, Z.work);
#endif
	  Z.N = N;
	}
	if(inverse)
#ifndef UNDERSCOREFORTRAN
	  cfftb(&N, C->data, Z.work);
	else
	  cfftf(&N, C->data, Z.work);
#else
	  cfftb_(&N, C->data, Z.work);
	else
	  cfftf_(&N, C->data, Z.work);
#endif
        /*
        **   ordi  sn   inv          norm
        **    0    0     0            1
        **    0    0     1            1
        **    0    1     0            1/sqrt(N)
        **    0    1     1            1/sqrt(N)
        **    1    0     0            N/L
        **    1    0     1            N*2*PI/L
        **    1    1     0            N*sqrt(2*PI)/L
        **    1    1     1            N*sqrt(2*PI)/L
        */

        if (normalize) {
           if (vector & ORDINATE_IN)
              if (inverse)
                 norm = N*sqrt(2*PI)/L;
              else
                 norm = N*sqrt(2*PI)/L;
           else
              if (inverse)
                 norm = sqrt((double)N);
              else
                 norm = sqrt((double)N);
       
        } else {
          if (vector & ORDINATE_IN)
              if (inverse)
                 norm = N ;
              else
                 norm = 1.0;
           else
              if (inverse)
                 norm = N;
              else
                 norm = 1.0;
        }
#ifdef OLD
        if (vector & ORDINATE_IN) 
           if (normalize)
              norm = N*sqrt ((double) 2*PI) / L;
           else
              if (inverse)
                 norm = N*2*PI/L;
              else
                 norm = N / L;
        else
           /* No ordinate on line */
           if (normalize)
              norm = sqrt((double)N);
           else
              norm = 1.0;
#endif

        if (!scale) 
           x0 = 0.0;

	out_fctn(C, N, norm, center, vector, out, L, x0, out_type, nozero);
      }
    }
 
  else { /* real data */
    A = create_default_matrix();

    while(TRUE) {

        if ((!first_time) && (!binary_out))  {
           fprintf(out,"\n");
           fflush(out);
        } else
           first_time = FALSE;

	if(feof(in) || in_fctn(&A,in,vector,&L,&x0)==EOF) exit(0);
	
	if(A->rows == 1)
	  N = A->cols;
	else if(A->cols == 1)
	  N = A->rows;
	else error("Fourier: data in wrong format");

	if(2*N+15 > Z.Wsize) {
	  Z.Wsize = 2*N+15;
	  Z.work = (double *)realloc(Z.work,
				      (size_t)(Z.Wsize)*sizeof(double));
	  if(Z.work == NULL)
	    error("Fourier: unable to allocate enough work space");
	}
	if(N != Z.N) {
#ifndef UNDERSCOREFORTRAN
	  rffti(&N, Z.work);
#else
	  rffti_(&N, Z.work);
#endif
	  Z.N = N;
	}
	if(inverse)
#ifndef UNDERSCOREFORTRAN
	  rfftb(&N, A->data, Z.work);
	else
	  rfftf(&N, A->data, Z.work);
#else
	  rfftb_(&N, A->data, Z.work);
	else
	  rfftf_(&N, A->data, Z.work);
#endif
        /*
        **   ordi  sn   inv          norm
        **    0    0     0            1
        **    0    0     1            1
        **    0    1     0            1/sqrt(N)
        **    0    1     1            1/sqrt(N)
        **    1    0     0            N/L
        **    1    0     1            N*2*PI/L
        **    1    1     0            N*sqrt(2*PI)/L
        **    1    1     1            N*sqrt(2*PI)/L
        */

        if (normalize) {
           if (vector & ORDINATE_IN)
              if (inverse)
                 norm = N*sqrt(2*PI)/L;
              else
                 norm = N*sqrt(2*PI)/L;
           else
              if (inverse)
                 norm = sqrt((double)N);
              else
                 norm = sqrt((double)N);
       
        } else {
          if (vector & ORDINATE_IN)
              if (inverse)
                 norm = N ;
              else
                 norm = 1.0;
           else
              if (inverse)
                 norm = N;
              else
                 norm = 1.0;
        }

#ifdef OLD
        if (vector & ORDINATE_IN) 
           if (normalize)
              norm = N*sqrt ((double) 2*PI) / L;
           else
              if (inverse)
                 norm = N*2*PI/L;
              else
                 norm = N / L;
        else
           /* No ordinate on line */
           if (normalize)
              norm = sqrt((double)N);
           else
              norm = 1.0;
#endif
        if (!scale) 
           x0 = 0.0;
        
	out_fctn(A, N, norm, center, vector, out, L, x0, out_type,nozero);
      }
    }
 
}

/*
** get_options()
**
** This function reads the command line arguments and sets the approiate
** flags.
** 
** argc - argument count
** argv - actual arguments to the program.
** in - Set to File pointer of input file, stdin is the default.
** out - Set to File pointer of output file, stdout is the default.
** type - Flag for binary input
** inverse - Set to true if the inverse FFT is to be done.
** normalize - Set to true of the putput data is to be normalized
** center - Set to true if the output data is to be centered
** vector - Flags set for ordinate_in, ordinate_out, inverse
** out_fctn - Set to the function to call for output
** in_fctn - Set to the function to call to read the input
** binary_out - Set to true if binary output is to be done.
** output_type - Set to what type of output is to be done
** scale - Set to indicate if -ordinate_in is specified and the first
**     position is not zero, default is TRUE. 
** nozero - Do not output the k=0 point if set.
*/
#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **in, FILE **out, int *type,
		 int *inverse, int *normalize, int *center, int *vector,
		 FftOut *out_fctn, FftIn *in_fctn, int *binary_out, OutputType *output_type,
                 int *scale,int *nozero)
#else
void get_options(argc, argv, in, out, type, inverse, normalize, center,
		 vector, out_fctn, in_fctn, binary_out, output_type,scale,nozero )
int argc;
char **argv;
FILE **in;
FILE **out;
int *type;
int *inverse;
int *normalize;
int *center;
int *vector;
FftOut *out_fctn;
FftIn *in_fctn;
int *binary_out;
OutputType *output_type;
int *scale;
int *nozero;
#endif

/************************
 obtain the options 
 ************************/
{
  char  c;
  char  *in_name=NULL;
  char  *out_name=NULL;
  int   i, out_flag;
  int   binary_in, packed_out;

  FftOut real_out_fctn = real_fft_out;
  FftOut complex_out_fctn = complex_fft_out;

  *type = TRUE;       /* the default type is complex */
  *center = FALSE;    /* the default is not to center */
  *vector = NO_ORDINATE;  /* the default is not to print the wave vector */
  *inverse = FALSE;   /* the default is the fft, not the inverse fft */
  *normalize = FALSE;  /* the default is not to normalize the results by
		       * dividing them by the square root of N, where
		       * N is the number of data points. */
  *scale = TRUE; 
  *nozero = FALSE;    /* default is to output the zero point */
  out_flag = FALSE;   /* can only have one output option */
 
  *output_type = Complex_Out;
  
  binary_in = FALSE;
  *binary_out = FALSE;
  packed_out = FALSE;

  for(i=1; i<argc; i++)
    {
      if(strncmp(argv[i],"-binary_",8)==0)
	{ 
	  argv[i]=argv[i]+8;
	  if(strncmp(argv[i],"in",2)==0) {
	    binary_in = TRUE;
	  }
	  else if(strncmp(argv[i],"out",3)==0) {
	    if(out_flag && !(packed_out))
	      error("Fourier: conflicting output options");
	    else {
	      out_flag = TRUE;
	      *binary_out = TRUE;
	    }
	  }
	}
      else if(strncmp(argv[i],"-file_",6)==0)
	{ 
	  argv[i]=argv[i]+6;
	  if(strncmp(argv[i],"in",2)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else in_name=argv[i];
	    }
	    else flag_error();
	  }
	  else if(strncmp(argv[i],"out",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else out_name=argv[i];
	    }
	    else flag_error();
	  }
	}
      else if(strcmp(argv[i],"-ao")==0 || 
	      strncmp(argv[i],"-absolute_out",4)==0)
	{
	  if(out_flag)
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Absolute_Out;
	    out_flag = TRUE;
 	  }
	}
      else if (strcmp(argv[i],"-ordinate_in") == 0) 
              *vector |= ORDINATE_IN;
      else if (strcmp(argv[i],"-ordinate_out") == 0)
              *vector |= ORDINATE_OUT;
      else if (strcmp(argv[i],"-no_scale") == 0)
              *scale = FALSE;
      else if (strcmp(argv[i],"-nozero") == 0)
              *nozero = TRUE;
      else if(strncmp(argv[i],"-cen",4)==0) 
	{
	  *center = TRUE;
	}
      else if(strcmp(argv[i],"-ci")==0 || 
	      strncmp(argv[i],"-complex_in",11)==0)
	{
	  *type = TRUE;
	}
      else if(strcmp(argv[i],"-co")==0 || 
	      strncmp(argv[i],"-complex_out",12)==0)
	{
	  if(out_flag)
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Complex_Out;
	    out_flag = TRUE;
	  }
	}
      else if(strncmp(argv[i],"-fft",4)==0)
	{
	  *inverse = FALSE;
	}
      else if(strcmp(argv[i],"?")==0)
	{
	  flag_error();
	}
      else if(strcmp(argv[i],"-?")==0)
	{
	  flag_error();
	}
      else if(strcmp(argv[i],"-h")==0)
	{
	  flag_error();
	}
      else if(strcmp(argv[i],"-help")==0)
	{
	  flag_error();
	}
      else if(strcmp(argv[i],"-info")==0)
	{
	  info();
	}
      else if(strncmp(argv[i],"-inv",4)==0)
	{
	  *inverse = TRUE;
          *vector |= INVERSE;
	}
      else if(strcmp(argv[i],"-io")==0 || 
	      strncmp(argv[i],"-imaginary_out",5)==0)
	{
	  if(out_flag)
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Imag_Out;
	    out_flag = TRUE;
	  }
	}
      else if (strcmp(argv[i],"-normalize")==0)
	{
	  *normalize = TRUE;
	}
      else if ((strcmp(argv[i],"-symmetric_normalize")==0) ||
               (strcmp(argv[i],"-sn") ==0))
	{
	  *normalize = TRUE;
	}
      else if(strncmp(argv[i],"-packed_out",5)==0)
	{
	  if(out_flag && !(*binary_out))
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Pack_Out;
	    out_flag = TRUE;

	  }
	}
      else if(strcmp(argv[i],"-po")==0 || 
	      strncmp(argv[i],"-power_out",6)==0)
	{
	  if(out_flag)
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Power_Out;
	    out_flag = TRUE;
	  }
	}
      else if(strcmp(argv[i],"-ri")==0 ||
	      strncmp(argv[i],"-real_in",8)==0)
	{
	  if(*type)
	    *type = FALSE;
	  else
	    flag_error();
	}
      else if(strcmp(argv[i],"-ro")==0 ||
	      strncmp(argv[i],"-real_out",9)==0)
	{
	  if(out_flag)
	    error("Fourier: conflicting output options");
	  else {
            *output_type = Real_Out;
	    out_flag = TRUE;

	  }
	}
     
      else if(strncmp(argv[i],"-u",2)==0)
	{
	  *normalize = FALSE;
	}
      
      else flag_error();
    }


  /* Check if Complex input */
  if (*type)
    {
      if(*binary_out)
	*out_fctn = complex_fft_binary_out;
      else
	*out_fctn = complex_out_fctn;

      if(binary_in)
	*in_fctn = complex_fft_binary_in;
      else
	*in_fctn = complex_fft_in_fctn;

      if(packed_out)
	error("Fourier: packed output only works with real input");
    }
  else {
      /* The input is real */
      if(*binary_out)
	*out_fctn = real_fft_binary_out;
      else
	*out_fctn = real_fft_out;

      if (*output_type == Pack_Out)
         *out_fctn = real_fft_packed_out;

      if(binary_in)
	*in_fctn = real_fft_binary_in;
      else
	*in_fctn = real_fft_in_fctn;
    }

  /* Set up the file to  read the data points from */
  
  if(in_name==NULL)
    *in=stdin;
  else {
    *in=fopen(in_name,"r");
    if(*in==NULL)
      error("MatrixMult: the specified input file could not be accessed");
  }

  /* Open the file where the output is to go */
 
  if(out_name==NULL)
    *out=stdout;
  else
    if(strcmp(out_name,"stderr")==0)
      *out = stderr;
    else {
      *out=fopen(out_name,"w");
      if(*out==NULL)
	error("MatrixMult: the specified output file could not be created");
    }
}

/*
** flag_error()
**
** This function dumps out some error messages if the command line arguments
** were not specified correctly.
**
**
*/
void flag_error()
{
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "Fourier - a program for fast fourier transform");
  fprintf(stderr,"%s\n","\noptions:\t(none are required)");  
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "data type flags: the default data type is double precision complex");
  fprintf(stderr,"%s\n"," -complex_input\t:use double precision complex data");
  fprintf(stderr,"%s\n"," -real_input\t:use double precision real data");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "output mode flags:\tthe default output mode is double precision complex");
  fprintf(stderr,"%s\n","\t\t\tselect only one of the following modes at a time");
  fprintf(stderr,"%s\n"," -absolute_output\t:output the absolute value of the results");
  fprintf(stderr,"%s\n"," -complex_output\t:output the complex results; the default mode");
  fprintf(stderr,"%s\n"," -imaginary_output\t:output the imaginary part of the results");
  fprintf(stderr,"%s\n"," -packed_output\t\t:output the packed results from -real_input");
  fprintf(stderr,"%s\n"," -power_output\t\t:output the power spectrum");
  fprintf(stderr,"%s\n"," -real_output\t\t:output the real part of the results");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "file flags: the defaults are ascii input from stdin and output to stdout");
  fprintf(stderr,"%s\n",
	  " -binary_input\t\t\t:input the data in binary format");
  fprintf(stderr,"%s\n",
	  " -binary_output\t\t\t:output the results in binary format");
  fprintf(stderr,"%s\n",
	  " -file_input in_file\t\t:get data from in_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_output out_file\t\t:output to out_file not stdout");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "other flags: the defaults are ascii input from stdin and output to stdout");
  fprintf(stderr,"%s\n"," -center\t:center the output around the 0 wave vector");
  fprintf(stderr,"%s\n"," -fft\t\t:calculate the fft; the default");
  fprintf(stderr,"%s\n"," -inverse\t:calculate the inverse fft");
  fprintf(stderr,"%s\n"," -normalize\t:normalize the results; the default");
  fprintf(stderr,"%s\n"," -unnormalize\t:do not normalize the results");
  fprintf(stderr,"%s\n"," -nozero\t:do not output the k=0 point");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","help flags:");
  fprintf(stderr,"%s\n"," ?\t\t:display this message");
  fprintf(stderr,"%s\n"," -?\t\t:display this message");
  fprintf(stderr,"%s\n"," -h\t\t:display this message");
  fprintf(stderr,"%s\n"," -help\t\t:display this message");
  fprintf(stderr,"%s\n"," -info\t\t:display information about the fft algorithm");
  fprintf(stderr,"\n%s\n\n",
	  "some of these flags may be abreviated such as -inv for -inverse\nor -ri for -real_input");
  exit(2);
}

/*
** info()
**
** This function dumps out the details of how the actual FFT is done,
** for more deatils see the file Fourier.Doc.
**
*/
void info()
{
  char c;

  fprintf(stderr, "%s\n\n","output: for complex data fft");
  fprintf(stderr, "%s\n\n","c\tfor j=1,...,n");
  fprintf(stderr, "%s\n\n","\t    c(j)=the sum from k=1,...,n of");
  fprintf(stderr, "%s\n\n","\t\t  c(k)*exp(-i*(j-1)*(k-1)*2*pi/n)/sqrt(n)");
  fprintf(stderr, "%s\n\n","\t\t\twhere i=sqrt(-1)");

  fprintf(stderr, "\n%s\n\n","output: for real data fft");
  fprintf(stderr, "%s\n\n","r\tr(1) = the sum from i=1 to i=n of r(i)/sqrt(n)");
  fprintf(stderr, "%s\n\n","\tif n is even set l =n/2   , if n is odd set l = (n+1)/2");
  fprintf(stderr, "%s\n\n","\t  then for k = 2,...,l");
  fprintf(stderr, "%s\n\n","\t     r(2*k-2) = the sum from i = 1 to i = n of");
  fprintf(stderr, "%s\n\n","\t\t  r(i)*cos((k-1)*(i-1)*2*pi/n)/sqrt(n)");
  fprintf(stderr, "%s\n\n","\t     r(2*k-1) = the sum from i = 1 to i = n of");
  fprintf(stderr, "%s\n\n","\t\t  -r(i)*sin((k-1)*(i-1)*2*pi/n)/sqrt(n)");
  fprintf(stderr, "%s\n\n","\tif n is even");
  fprintf(stderr, "%s\n\n","\t     r(n) = the sum from i = 1 to i = n of");
  fprintf(stderr, "%s\n\n","\t\t  (-1)**(i-1)*r(i)/sqrt(n)");

  exit(1);
}






