/*****************************************************************
 Program MatrixMult                                        
 finds the product X of two matrices A and B
 This is called from a UNIX command line as           
 MatrixMult [-real, -complex] [-?, ? -h, -help]
  [-binary_Ain] [-binary_Bin] [-binary_out] [-file_in InputFile] 
  [-file_Ain AInputFile] [-file_Bin BInputFile] [-file_out OutputFile]

 flag                abbr     purpose
 ----------------------------------------------------------------------
 -real             : ***** : to set the matrix elements to type real
 -complex          : -comp : to set the matrix elements to type complex
 ?                 : ***** : to get the command line help message
 -?                : ***** : to get the command line help message
 -h                : ***** : to get the command line help message
 -help             : ***** : to get the command line help message
 -binary_Ain       : ***** : to input matrix A using the binary mode
 -binary_Bin       : ***** : to input matrix B using the binary mode
 -binary_out       : ***** : to output matrix X using the binary mode
 -file_in          : ***** : set the default input to InputFile
 -file_Ain         : ***** : read A from AInputFile, not stdin
 -file_Bin         : ***** : read B from BInputFile, not stdin
 -file_out         : ***** : output X to OutputFile, not stdout

 Version 1.0 July 22, 1991
 Written by Eric Lochstet
*******************************************************************/
# include "MMult.h"


#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif

{
  Matrix     A;
  Matrix     B;
  Matrix     *Result;
  
  ComplexDoubleMatrix AC;
  ComplexDoubleMatrix BC;
  ComplexDoubleMatrix *ResultC;

  FILE     *Ain, *Bin;
  FILE     *out; 

  GetMatrix  matrix_Ain, matrix_Bin;
  ResultOut  result_out;
  int        type;   /* type is 0 for real matrix, 1 for complex */

  get_options(argc, argv, &Ain, &Bin, &out, &type, &matrix_Ain,
	      &matrix_Bin, &result_out);

  if(type) {
    AC = *create_default_complex_double_matrix();
    BC = *create_default_complex_double_matrix();
    ResultC = create_default_complex_double_matrix();

    while(TRUE)
      {
	if(feof(Ain) || matrix_Ain(&AC,Ain)==EOF) exit(0);
	if(feof(Bin) || matrix_Bin(&BC,Bin)==EOF) exit(0);
	
	complex_matrix_mult(AC, BC, ResultC);
	
	result_out(*ResultC, out);
      }
  }
  else {
    A = *create_default_matrix();
    B = *create_default_matrix();
    Result = create_default_matrix();

    while(TRUE)
      {
	if(feof(Ain) || matrix_Ain(&A,Ain)==EOF) exit(0);
	if(feof(Bin) || matrix_Bin(&B,Bin)==EOF) exit(0);
	
	real_matrix_mult(A, B, Result);
	
	result_out(*Result, out);
      }    
  }
}


/*===========================================================================*/
 
#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **Ain, FILE **Bin, FILE **out,
		 int *type, GetMatrix *matrix_Ain, GetMatrix *matrix_Bin,
		 ResultOut *result_out)
#else
void get_options(argc, argv, Ain, Bin, out, type, matrix_Ain, matrix_Bin,
		 result_out)
int argc;
char **argv;
FILE **Ain;
FILE **Bin;
FILE **out;
int *type;
GetMatrix *matrix_Ain;
GetMatrix *matrix_Bin;
ResultOut *result_out;
#endif

/************************
 obtain the options 
 ************************/
{
  char  c;
  char  *Ain_name=NULL,*Bin_name=NULL;
  char  *out_name=NULL;
  int   i;
  int   binary_out;
  int   binary_Ain;
  int   binary_Bin;
  int   short_form;

  binary_out = FALSE;  /* the default is non binary */
  binary_Ain = FALSE;  /* the default is non binary */
  binary_Bin = FALSE;  /* the default is non binary */
  short_form = FALSE;
  *type = FALSE;       /* the default type is real */

  for(i=1; i<argc; i++)
    {
      if(strncmp(argv[i],"-file_",6)==0)
	{ 
	  argv[i]=argv[i]+6;
	  if(strncmp(argv[i],"Ain",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else Ain_name=argv[i];
	    }
	    else flag_error();
	  }
	  else if(strncmp(argv[i],"Bin",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else Bin_name=argv[i];
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
      else if(strncmp(argv[i],"-binary_",8)==0)
	{
	  argv[i]=argv[i]+8;
	  if(strncmp(argv[i],"out",3)==0) {
	    binary_out = TRUE;
	  }
	  else if(strncmp(argv[i],"Ain",3)==0) {
	    binary_Ain = TRUE;
	  }
	  else if(strncmp(argv[i],"Bin",3)==0) {
	    binary_Bin = TRUE;
	  }
	}
      else if(strncmp(argv[i],"-comp",5)==0)
	{
	  if(!(*type))
	    *type = TRUE;
	  else
	    flag_error();
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
      else if(strcmp(argv[i],"-real")==0)
	{
	  if(*type)
	    flag_error();
	}
      else if(strcmp(argv[i],"-short")==0)
	{
	  short_form = TRUE;
	}
    }

/*---------------------------------------------------------------------------*/
/* set the function pointers to the correct routines */

  if(*type)
    {
      if(binary_Ain)
	*matrix_Ain = get_complex_double_binary_matrix;
      else
	*matrix_Ain = get_complex_double_matrix;

      if(binary_Bin)
	*matrix_Bin = get_complex_double_binary_matrix;
      else
	*matrix_Bin = get_complex_double_matrix;

      if(binary_out)
	*result_out  = put_complex_double_binary_matrix;
      else {
	if(short_form)
	  *result_out = put_short_complex_matrix;
	else
	  *result_out  = put_complex_double_matrix;
      }
    }
  else
    {
      if(binary_Ain)
	*matrix_Ain = get_binary_matrix;
      else
	*matrix_Ain = get_matrix;

      if(binary_Bin)
	*matrix_Bin = get_binary_matrix;
      else
	*matrix_Bin = get_matrix;

      if(binary_out)
	*result_out  = put_binary_matrix;
      else {
	if(short_form)
	  *result_out = put_short_matrix;
	else
	  *result_out  = put_matrix;
      }
    }
  
/*---------------------------------------------------------------------------*/
/* obtain the in files */
  
  if(Ain_name==NULL)
    *Ain=stdin;
  else {
    *Ain=fopen(Ain_name,"r");
    if(*Ain==NULL)
      error("MatrixMult: the specified input file could not be accessed");
  }

  if(Bin_name==NULL)
    *Bin=stdin;
  else {
    *Bin=fopen(Bin_name,"r");
    if(*Bin==NULL)
      error("MatrixMult: the specified input file could not be accessed");
  }

/*---------------------------------------------------------------------------*/
/* obtain the out files */
 
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

/*===========================================================================*/

void flag_error()
{
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "MultMatrix - a program for multiplying matricies");
  fprintf(stderr,"%s\n","\noptions:\t(none are required)");  
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "data type flags: the default data type is double precision real");
  fprintf(stderr,"%s\n"," -complex\t:use double precision complex matrices");
  fprintf(stderr,"%s\n"," -real\t\t:use double precision real matrices");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "file flags: the defaults are ascii input from stdin and output to stdout");
  fprintf(stderr,"%s\n"," -binary_Ain\t\t\t:input the A matrices in binary format");
  fprintf(stderr,"%s\n"," -binary_Bin\t\t\t:input the B matrices in binary format");
  fprintf(stderr,"%s\n"," -binary_out\t\t\t:output the product matricies in binary format");
  fprintf(stderr,"%s\n",
	  " -file_Ainput Ain_file\t\t:get A from Ain_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_Binput Bin_file\t\t:get B from Bin_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_output out_file\t\t:output to out_file not stdout");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","help flags:");
  fprintf(stderr,"%s\n"," ?\t\t:display this message");
  fprintf(stderr,"%s\n"," -?\t\t:display this message");
  fprintf(stderr,"%s\n"," -h\t\t:display this message");
  fprintf(stderr,"%s\n"," -help\t\t:display this message");
  fprintf(stderr,"\n%s\n\n",
	  "some of these flags may be abreviated such as -comp for -complex");
  exit(1);
}
/*===========================================================================*/

#ifndef _NO_PROTO
void complex_matrix_mult(ComplexDoubleMatrix A, ComplexDoubleMatrix B,
			 ComplexDoubleMatrix *result)
					  
#else
void complex_matrix_mult(A, B, result)
ComplexDoubleMatrix A;
ComplexDoubleMatrix B;
ComplexDoubleMatrix *result;
#endif
{
  char transa[1], transb[1];
  int m[1], n[1], k[1], lda[1], ldb[1], ldc[1];
  double alpha[2], beta[2];
  int row_count;

  if(A.cols != B.rows)
    error("MatrixMult: the matricies must have compatible dimensions");
	
  *transa = 'n';
  *transb = 'n';
  *m = B.cols;
  *n = A.rows;
  *k = B.rows;
  alpha[0] = 1;
  alpha[1] = 0;
  beta[0] = 0;
  beta[1] = 0;
  *lda = B.cols;
  *ldb = A.cols;
  *ldc = B.cols;

  if((A.rows * B.cols)<result->data_reserved) {
    result->data_reserved = A.rows * B.cols;
    result->data = (ComplexDouble *) realloc((char*) result->data, 
		(unsigned) result->data_reserved*sizeof(ComplexDouble));
    if (!result->data) 
      error("MatrixMult: realloc of result space failed in complex_matrix_mult");
  }
  result->cols = B.cols;
  result->rows = A.rows;

  if (result->rows>=result->pointers_reserved) {
    result->el = (ComplexDouble **)
      realloc((char*) result->el,
	      (unsigned) result->rows*sizeof(ComplexDouble*));
    if (!result->el) 
      error("MatrixMult: realloc of el pointers failed in complex_matrix_mult");
    result->pointers_reserved = result->rows;
  }
  for(row_count=0; row_count<result->rows; row_count++)
    result->el[row_count+ROWSTART] = 
      result->data + result->cols * row_count - COLSTART;
  
#ifndef UNDERSCOREFORTRAN
  zgemm(transa, transb, m, n, k, alpha, B.data, lda,
	A.data, ldb, beta, result->data, ldc);
#else
  zgemm_(transa, transb, m, n, k, alpha, B.data, lda,
	A.data, ldb, beta, result->data, ldc);
#endif

}
/*===========================================================================*/

#ifndef _NO_PROTO
void real_matrix_mult(Matrix A, Matrix B, Matrix *result)
#else
void real_matrix_mult(A, B, result)
Matrix A;
Matrix B;
Matrix *result;
#endif
{
  char transa[1], transb[1];
  int m[1], n[1], k[1], lda[1], ldb[1], ldc[1];
  double alpha[1], beta[1];
  int row_count;

  if(A.cols != B.rows)
    error("MatrixMult: the matricies must have compatible dimensions");
	
  *transa = 'n';
  *transb = 'n';
  *m = B.cols;
  *n = A.rows;
  *k = B.rows;
  *alpha = 1;
  *beta = 0;
  *lda = B.cols;
  *ldb = A.cols;
  *ldc = B.cols;

  if((A.rows * B.cols)<result->data_reserved) {
    result->data_reserved = A.rows * B.cols;
    result->data = (double *) realloc((char*) result->data, 
		(unsigned) result->data_reserved*sizeof(double));
    if (!result->data) 
      error("MatrixMult: realloc of result space failed in complex_matrix_mult");
  }
  result->cols = B.cols;
  result->rows = A.rows;

  if (result->rows>=result->pointers_reserved) {
    result->el = (double **)
      realloc((char*) result->el,
	      (unsigned) result->rows*sizeof(double*));
    if (!result->el) 
      error("MatrixMult: realloc of el pointers failed in complex_matrix_mult");
    result->pointers_reserved = result->rows;
  }
  for(row_count=0; row_count<result->rows; row_count++)
    result->el[row_count+ROWSTART] = 
      result->data + result->cols * row_count - COLSTART;
  
#ifndef UNDERSCOREFORTRAN
  dgemm(transa, transb, m, n, k, alpha, B.data, lda,
	A.data, ldb, beta, result->data, ldc);
#else
  dgemm_(transa, transb, m, n, k, alpha, B.data, lda,
	A.data, ldb, beta, result->data, ldc);
#endif

}
/*===========================================================================*/

#ifndef _NO_PROTO
void put_short_matrix(Matrix M, FILE *out)
#else
void put_short_matrix(M, out)
Matrix M;
FILE *out;
#endif

{
    int  i,j;

    for(i=0; i<M.rows; i++){
        for(j=0; j<M.cols; j++)
            fprintf(out, "%5.2f ", M.el[i+ROWSTART][j+COLSTART]);
        if (i<M.rows-1) fprintf(out,"\n");
    }
    fprintf(out,"\n\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_short_complex_matrix(Matrix A, FILE *out)
#else
void put_short_complex_matrix(A, out)
Matrix A;
FILE *out;
#endif
     /* displays A on the given file */
     
# define SEPARATOR  ' '
# define OUT_FORMAT  "(%5.2f %5.2f) "
     
{
  int  i,j;
  for(i=ROWSTART; i<A.rows+ROWSTART; i++){
    for(j=COLSTART; j<(2*A.cols)+COLSTART; j=j+2)
      fprintf(out, OUT_FORMAT, A.el[i][j], A.el[i][j+1],SEPARATOR);
    fprintf(out,"\n");
  }
  fprintf(out,"\n");
}
# undef SEPARATOR
# undef OUT_FORMAT

/*===========================================================================*/
