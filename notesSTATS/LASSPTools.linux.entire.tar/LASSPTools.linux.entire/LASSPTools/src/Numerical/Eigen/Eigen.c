/*****************************************************************
 Program Eigen                                        
 finds the eigenvalues and eigenvectors of a matrix
 This is called from a UNIX command line as           
 Eigen [-real, -symmetric -complex -hermitian] [-value] [-vector]
  [-time] [-?, ? -h, -help] [-binary_in] [-binary_out]
  [-file_in InputFile] [-file_out OutputFile] [-file_val ValFile]
  [-file_vec VecFile]

 flag                abbr     purpose
 ----------------------------------------------------------------------
 -real             : -r    : set the matricies to type real
 -complex          : -c    : set the matricies to type complex
 -hermitian        : -her  : set the matricies to type hermitian
 -symmetric        : -s    : set the matricies to type symmetric
 -binary_in        : ***** : input the matrix using the binary mode
 -file_in          : ***** : read the matrix from InputFile, not stdin
 -file_out         : ***** : output to OutputFile, not stdout
 -file_val         : ***** : output eigen values to ValFile, not stdout
 -file_vec         : ***** : output eigen vectors to VecFile, not stdout
 -value            : -val  : calculate the eigen values
 -vector           : -vec  : calculate the eigen vectors
 -time             : -tim  : display timing information to stderr
 ?                 : ***** : get the command line help message
 -?                : ***** : get the command line help message
 -h                : ***** : get the command line help message
 -help             : ***** : get the command line help message

 Version 1.0 August 1, 1991
 Written by Eric Lochstet
*******************************************************************/
#include "eiscalls.h"
#include <time.h>
#include <math.h>
#include "Boolean.h"
#ifdef LINUX
#define TRUE true
#define FALSE false
#endif

int timing = FALSE;

typedef int (*GetMatrix)();
typedef void (*EigenFctn)();

void flag_error();

#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **in, FILE **value_out,
		 FILE **vector_out, int *type, GetMatrix *matrix_in,
		 EigenFctn *eigen_fctn);
#else
void get_options();
#endif

#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif

{
  Matrix     A;  
  ComplexDoubleMatrix C;
  WorkSpace  Z;
  clock_t  start_time,stop_time;
  float elapsed_time;

  FILE     *in;
  FILE     *value_out; 
  FILE     *vector_out; 

  GetMatrix  matrix_in;
  EigenFctn  eigen_fctn;

  int        type;   /* type is 0 for real matrix, 1 for complex */

  get_options(argc, argv, &in, &value_out, &vector_out, &type,
	      &matrix_in, &eigen_fctn);

  Z.work = (double *)malloc((size_t)40*sizeof(double));
  Z.Wsize = 40;
  Z.ipvt = (int *)malloc((size_t)40*sizeof(double));
  Z.Isize = 40;
  if(Z.work == 0 || Z.ipvt == 0)
    error("Eigen: unable to allocate work space");

  if(type) { /* The section for complex matrices */

    C = *create_default_complex_double_matrix();

    if(timing) {

      /* The timing loop for complex matrices */

      while(TRUE)
        {
          start_time=clock();
          
	  if(feof(in) || matrix_in(&C,in)==EOF) exit(0);
	  
	  eigen_fctn(C, &Z, value_out, vector_out);

          stop_time=clock();
          elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
          fprintf(stderr,"Eigen: time taken was %f seconds\n"
                  , elapsed_time);
	}
    }
    else {

      /* The nontiming loop for complex matrices */

      while(TRUE)
	{
	  if(feof(in) || matrix_in(&C,in)==EOF) exit(0);
	  
	  eigen_fctn(C, &Z, value_out, vector_out);
	}
    }
  }
  else { /* The section for real matrices */
    
    A = *create_default_matrix();

    if(timing) {

      /* The timing loop for real matrices */

      while(TRUE)
	{
          start_time=clock();

	  if(feof(in) || matrix_in(&A,in)==EOF) exit(0);
	  
	  eigen_fctn(A, &Z, value_out, vector_out);
	  
          stop_time=clock();
          elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
          fprintf(stderr,"Eigen: time taken was %f seconds\n"
                  , elapsed_time);
	}
    }
    else {

      /* The nontiming loop for real matrices */

      while(TRUE)
	{
	  if(feof(in) || matrix_in(&A,in)==EOF) exit(0);
	  
	  eigen_fctn(A, &Z, value_out, vector_out);
	}    
    }
  }
}

/*===========================================================================*/

#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **in, FILE **value_out,
		 FILE **vector_out, int *type, GetMatrix *matrix_in,
		 EigenFctn *eigen_fctn)

#else
void get_options(argc, argv, in, value_out, vector_out, type, matrix_in,
		 eigen_fctn)

int argc;
char **argv;
FILE **in;
FILE **value_out;
FILE **vector_out;
int *type;
GetMatrix *matrix_in;
EigenFctn *eigen_fctn;
#endif

/************************
 obtain the options 
 ************************/
{
  char  c;
  char  *in_name=NULL;
  char  *out_name=NULL;
  char  *val_name=NULL;
  char  *vec_name=NULL;
  int   i;
  int   binary_out;
  int   binary_in;
  int   hermitian;
  int   symmetric;
  int   real;
  int   values;
  int   vectors;
  FILE  *out;

  binary_out = FALSE;  /* the default is non binary */
  binary_in = FALSE;  /* the default is non binary */
  *type = FALSE;       /* the default type is real */
  hermitian = FALSE;
  symmetric = FALSE;
  real = FALSE;
  values = FALSE;
  vectors = FALSE;

  for(i=1; i<argc; i++)
    {
      if(strncmp(argv[i],"-file_",6)==0)
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
	  else if(strncmp(argv[i],"val",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else val_name=argv[i];
	    }
	    else flag_error();
	  }
	  else if(strncmp(argv[i],"vec",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else vec_name=argv[i];
	    }
	    else flag_error();
	  }
	  else flag_error();
	}
      else if(strncmp(argv[i],"-binary_",8)==0)
	{
	  argv[i]=argv[i]+8;
	  if(strncmp(argv[i],"out",3)==0) {
	    binary_out = TRUE;
	    flag_error();
	  }
	  else if(strncmp(argv[i],"in",2)==0) {
	    binary_in = TRUE;
	  }
	  else flag_error();
	}
      else if(strncmp(argv[i],"-c",2)==0)
	{
	  if(real)
	    flag_error();
	  else
	    *type = TRUE;
	}
      else if(strncmp(argv[i],"-her",4)==0)
	{
	  if(real)
	    flag_error();
	  else {
	    hermitian = TRUE;
	    *type = TRUE;
	  }
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
      else if(strncmp(argv[i],"-r",2)==0)
	{
	  if(*type)
	    flag_error();
	  else
	    real = TRUE;
	}
      else if(strncmp(argv[i],"-s",2)==0)
	{
	  if(*type)
	    flag_error();
	  else {
	    real = TRUE;
	    symmetric = TRUE;
	  }
	}
      else if(strncmp(argv[i],"-tim",4)==0)
        {
          timing = TRUE;
        }
      else if(strncmp(argv[i],"-val",4)==0)
	{
	  values = TRUE;
	}
      else if(strncmp(argv[i],"-vec",4)==0)
	{
	  vectors = TRUE;
	}
      else
	flag_error();
    }

/*---------------------------------------------------------------------------*/
/* set the function pointers to the correct routines */

  if(*type)
    {
      if(hermitian) {
	if(values) {
	  if(vectors)
	    *eigen_fctn = hermitian_eigen;
	  else
	    *eigen_fctn = hermitian_values;
	}
	else {
	  if(vectors)
	    *eigen_fctn = hermitian_vectors;
	  else
	    *eigen_fctn = hermitian_eigen;
	}
      }
      else {
	if(values) {
	  if(vectors)
	    *eigen_fctn = complex_eigen;
	  else
	    *eigen_fctn = complex_values;
	}
	else {
	  if(vectors)
	    *eigen_fctn = complex_vectors;
	  else
	    *eigen_fctn = complex_eigen;
	}
      }
      if(binary_in)
	*matrix_in = get_complex_double_binary_matrix;
      else
	*matrix_in = get_complex_double_matrix;
/*
      if(binary_out)
	*result_out  = put_complex_double_binary_matrix;
      else {
	*result_out  = put_complex_double_matrix;
      }
 */
    }
  else
    {
      if(symmetric) {
	if(values) {
	  if(vectors)
	    *eigen_fctn = symmetric_eigen;
	  else
	    *eigen_fctn = symmetric_values;
	}
	else {
	  if(vectors)
	    *eigen_fctn = symmetric_vectors;
	  else
	    *eigen_fctn = symmetric_eigen;
	}
      }
      else {
	if(values) {
	  if(vectors)
	    *eigen_fctn = real_eigen;
	  else
	    *eigen_fctn = real_values;
	}
	else {
	  if(vectors)
	    *eigen_fctn = real_vectors;
	  else
	    *eigen_fctn = real_eigen;
	}
      }
      if(binary_in)
	*matrix_in = get_binary_matrix;
      else
	*matrix_in = get_matrix;
/*
      if(binary_out)
	*result_out  = put_binary_matrix;
      else {
	*result_out  = put_matrix;
      }
 */
    }
  
/*---------------------------------------------------------------------------*/
/* obtain the in files */
  
  if(in_name==NULL)
    *in=stdin;
  else {
    *in=fopen(in_name,"r");
    if(*in==NULL)
      error("MatrixMult: the specified input file could not be accessed");
  }

/*---------------------------------------------------------------------------*/
/* obtain the out files */
 
  if(out_name==NULL)
    out=stdout;
  else
    if(strcmp(out_name,"stderr")==0)
      out = stderr;
    else {
      out=fopen(out_name,"w");
      if(out==NULL)
	error("MatrixMult: the specified output file could not be created");
    }
  if(val_name==NULL)
    *value_out=out;
  else
    if(strcmp(val_name,"stderr")==0)
      *value_out = stderr;
    else {
      *value_out=fopen(val_name,"w");
      if(*value_out==NULL)
	error("MatrixMult: the specified output file could not be created");
    }
  if(vec_name==NULL)
    *vector_out=out;
  else
    if(strcmp(vec_name,"stderr")==0)
      *vector_out = stderr;
    else {
      *vector_out=fopen(vec_name,"w");
      if(*vector_out==NULL)
	error("MatrixMult: the specified output file could not be created");
    }
}

/*===========================================================================*/

void flag_error()
{
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "Eigen - a program for finding eigenvalues and eigenvectors");
  fprintf(stderr,"%s\n","\noptions:\t(none are required)");  
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "matrix type flags: the default matrix type is double precision real");
  fprintf(stderr,"%s\n"," -complex\t:use double precision complex matrices");
  fprintf(stderr,"%s\n"," -hermitian\t:use double precision complex hermitian matrices");
  fprintf(stderr,"%s\n"," -real\t\t:use double precision real matrices");
  fprintf(stderr,"%s\n"," -symmetric\t:use double precision real symmetric matrices");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "file flags: the defaults are ascii input from stdin and output to stdout");
  fprintf(stderr,"%s\n"," -binary_in\t\t\t:input the matrices in binary format");
/*  fprintf(stderr,"%s\n"," -binary_out\t\t\t:output the product matricies in binary format"); */
  fprintf(stderr,"%s\n",
	  " -file_input in_file\t\t:get the matrix from in_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_output out_file\t\t:output to out_file not stdout");
  fprintf(stderr,"%s\n",
	  " -file_values val_file\t\t:output the eigenvalues to val_file");
  fprintf(stderr,"%s\n",
	  " -file_vectors vec_file\t\t:output the eigenvectors to vec_file");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","other flags:");
  fprintf(stderr,"%s\n"," -values\t\t:calculate only the eigen values");
  fprintf(stderr,"%s\n"," -vectors\t:calculate only the eigen vectors");
  fprintf(stderr,"%s\n","\t\t     the default is to calculate both");
  fprintf(stderr,"%s\n","\t\t     setting both flags calculates both");
  fprintf(stderr,"%s\n"," -tim[e, ing]\t:display timing information");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","help flags:");
  fprintf(stderr,"%s\n"," ?\t\t:display this message");
  fprintf(stderr,"%s\n"," -?\t\t:display this message");
  fprintf(stderr,"%s\n"," -h\t\t:display this message");
  fprintf(stderr,"%s\n"," -help\t\t:display this message");
  fprintf(stderr,"\n%s\n\n",
	  "some of these flags may be abreviated such as -c for -complex");
  exit(1);
}
/*===========================================================================*/
