
/*********************************************************************
 Program MatrixSolve                                         
 finds the solution x of the linear system Ax = B
 This is called from a UNIX command line as
 MatrixSolve [-real, -complex] [-general, -symmertric, -hermitian
  -upper_triangular, -lower_triangular] [-condition] [-?, ? -h, -help]
  [-binary_Ain] [-binary_Bin] [-binary_out] [-file_in InputFile] 
  [-file_Ain AInputFile] [-file_Bin BInputFile] [-file_out OutputFile]
  [-file_Xout XOutputFile] [-file_cond ConditionOutputFile] [-timing]
 
 flag                abbr     purpose
 ----------------------------------------------------------------------
 -real             : ***** : to set the matrix elements to type real
 -complex          : -comp : to set the matrix elements to type complex
 -general          : -gen  : to set the matrix type to general
 -symmetric        : -sym  : to set the matrix type to symmetric
 -hermitian        : -her  : to set the matrix type to hermitian
 -upper_triangular : -upp  : to set the matrix type to upper triangular
 -lower_triangular : -low  : to set the matrix type to lower triangular
 -condition        : -cond : to calculate the condition of A
 ?                 : ***** : to get the command line help message
 -?                : ***** : to get the command line help message
 -h                : ***** : to get the command line help message
 -help             : ***** : to get the command line help message
 -binary_Ain       : ***** : to input matrix A using the binary mode
 -binary_Bin       : ***** : to input matrix B using the binary mode
 -binary_out       : ***** : to output matrix X using the binary mode
 -file_in          : ***** : read A and B from AInputFile, not stdin
 -file_Ain         : ***** : read A from InputFile, not stdin
 -file_Bin         : ***** : read B from AInputFile, not stdin
 -file_out         : ***** : write the output to OutputFile, not stdout
 -file_Xout        : ***** : write X to XOutputFile, not stdout
 -file_cond        : ***** : write the condition to ConditionOutputFile
 -timing           : -tim  : display timing information to stderr

 Utilizes the double precision Linpack routines
 Version 1.0 July 22, 1991
 Written by Eric Lochstet
***********************************************************************/
# include "MSolve.h"

#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char  **argv;
#endif
{
  ComplexDoubleMatrix *AC;
  ComplexDoubleMatrix *BC;
  Matrix     *A;
  Matrix     *B;
  clock_t  start_time,stop_time; float elapsed_time;
  FILE     *Ain;
  FILE     *Bin;
  FILE     *Xout[2]; /* the first file ptr is to the result out file *
		      * the second file ptr is to the cond out file  */
  WorkSpace  Z;
  SolveFctn  solve;
  GetMatrix  matrix_Ain, matrix_Bin;
  PutMatrix  data_out;
  int        type;   /* type is 0 for real matrix, 1 for complex */

  get_options(argc, argv, &Ain, &Bin, Xout, &solve, &type,
	      &matrix_Ain, &matrix_Bin, &data_out);

  if(type) {
    AC = create_default_complex_double_matrix();
    BC = create_default_complex_double_matrix();
    Z.ipvt = (int *)malloc((size_t)40 * sizeof(int));
    if(Z.ipvt == NULL)
      error("MatrixSolve: workspace malloc failled");
    else
      Z.Isize = 40;
    Z.work = (double *)malloc((size_t)80 * sizeof(double));
    if(Z.work == NULL)
      error("workspace malloc failled");
    else
      Z.Wsize = 80;

    if(timing) {
      while(TRUE)
	{
	  start_time=clock();
	  
	  if(feof(Ain) || matrix_Ain(AC,Ain)==EOF) exit(0);
	  if(feof(Bin) || matrix_Bin(BC,Bin)==EOF) exit(0);
	  
	  solve(AC, BC, &Z, Xout[1]);
	  
	  stop_time=clock();
	  elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
	  fprintf(stderr,"MatrixSolve: time taken was %f seconds\n", elapsed_time);

	  data_out(*BC, Xout[0]);
	}
    }
    else {
      while(TRUE)
	{
	  if(feof(Ain) || matrix_Ain(AC,Ain)==EOF) exit(0);
	  if(feof(Bin) || matrix_Bin(BC,Bin)==EOF) exit(0);
	  
	  solve(AC, BC, &Z, Xout[1]);
	  
	  data_out(*BC, Xout[0]);
	}
    }
  }
  else {
    A = create_default_matrix();
    B = create_default_matrix();
    Z.ipvt = (int *)malloc((size_t)40 * sizeof(int));
    if(Z.ipvt == NULL)
      error("workspace malloc failled");
    else
      Z.Isize = 40;
    Z.work = (double *)malloc((size_t)40 * sizeof(double));
    if(Z.work == NULL)
      error("workspace malloc failled");
    else
      Z.Wsize = 40;
    if(timing) {
      while(TRUE)
	{
	  start_time=clock();
	  
	  if(feof(Ain) || matrix_Ain(A,Ain)==EOF) exit(0);
	  if(feof(Bin) || matrix_Bin(B,Bin)==EOF) exit(0);
	
	  solve(A, B, &Z, Xout[1]);

	  stop_time=clock();
	  elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
	  fprintf(stderr,"MatrixSolve: time taken was %f seconds\n", elapsed_time);

	  data_out(*B, Xout[0]);
	}
    }
    else {
      while(TRUE)
	{
	  if(feof(Ain) || matrix_Ain(A,Ain)==EOF) exit(0);
	  if(feof(Bin) || matrix_Bin(B,Bin)==EOF) exit(0);
	  
	  solve(A, B, &Z, Xout[1]);
	  
	  data_out(*B, Xout[0]);
	}
    }
  }
}

/*===========================================================================*/
 
#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **Ain, FILE **Bin, FILE **Xout,
		 SolveFctn *solve_fctn, int *type, GetMatrix *matrix_Ain,
		 GetMatrix *matrix_Bin, PutMatrix *data_out)
#else
void get_options(argc, argv, Ain, Bin, Xout, solve_fctn, type, matrix_Ain,
		 matrix_Bin, data_out)
int argc;
char **argv;
FILE **Ain;
FILE **Bin;
FILE **Xout;
SolveFctn *solve_fctn;
int *type;
GetMatrix *matrix_Ain;
GetMatrix *matrix_Bin;
PutMatrix *data_out;
#endif

/************************
 obtain the options 
 ************************/
{
  char  c;
  char  *Ain_name=NULL,*default_in_name=NULL,*Bin_name=NULL;
  char  *Xout_name=NULL,*cond_name=NULL,*default_out_name=NULL;
  int   i;
  int   fact_index;
  int   type_index;
  int   hermitian;
  int   binary_out;
  int   binary_Ain;
  int   binary_Bin;
  FILE  *default_in;
  FILE  *default_out;
      
  SolveFctn  fact_table[2][2];

  fact_table[0][0] = d_gen_ms_factor;
  fact_table[0][1] = z_gen_ms_factor;
  fact_table[1][0] = d_gen_ms_condition;
  fact_table[1][1] = z_gen_ms_condition;
  
  binary_out = FALSE; /* the default is non binary */
  binary_Ain = FALSE;  /* the default is non binary */
  binary_Bin = FALSE;  /* the default is non binary */
  type_index = 0; /* the default type is real */
  hermitian = FALSE; /* the default matrix is general not hermitian */
  fact_index = 0; /* the default factorization does not
		   * compute the condition */

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
	  else if(strncmp(argv[i],"cond",4)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else cond_name=argv[i];
	    }
	    else flag_error();
	    fact_index = 1;
	  }
	  else if(strncmp(argv[i],"in",2)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else default_in_name=argv[i];
	    }
	    else flag_error();
	  }
	  else if(strncmp(argv[i],"out",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else default_out_name=argv[i];
	    }
	    else flag_error();
	  }
	  else if(strncmp(argv[i],"Xout",4)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else Xout_name=argv[i];
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
	  }
	  else if(strncmp(argv[i],"Ain",3)==0) {
	    binary_Ain = TRUE;
	  }
	  else if(strncmp(argv[i],"Bin",3)==0) {
	    binary_Bin = TRUE;
	  }
	  else flag_error();
	}
      else if(strncmp(argv[i],"-comp",5)==0)
	{
	  if(!type_index)
	    type_index = 1;
	  else
	    flag_error();
	}
      else if(strncmp(argv[i],"-cond",5)==0)
	{
	  fact_index = 1;
	}
      else if(strncmp(argv[i],"-gen",4)==0)
	{
	  fact_table[0][0] = d_gen_ms_factor;
	  fact_table[0][1] = z_gen_ms_factor;
	  fact_table[1][0] = d_gen_ms_condition;
	  fact_table[1][1] = z_gen_ms_condition;
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
      else if(strncmp(argv[i],"-her",4)==0)
	{
	  type_index = 1;
	  hermitian =TRUE;
	  fact_table[0][0] = d_sym_ms_factor;
	  fact_table[0][1] = z_hermitian_ms_factor;
	  fact_table[1][0] = d_sym_ms_condition;
	  fact_table[1][1] = z_hermitian_ms_condition;
	}
      else if(strncmp(argv[i],"-low",4)==0)
	{
	  fact_table[0][0] = d_lower_tri_ms_factor;
	  fact_table[0][1] = z_lower_tri_ms_factor;
	  fact_table[1][0] = d_lower_tri_ms_condition;
	  fact_table[1][1] = z_lower_tri_ms_condition;
	}
      else if(strcmp(argv[i],"-real")==0)
	{
	  if(type_index)
	    flag_error();
	}
      else if(strncmp(argv[i],"-sym",4)==0)
	{
	  fact_table[0][0] = d_sym_ms_factor;
	  fact_table[0][1] = z_sym_ms_factor;
	  fact_table[1][0] = d_sym_ms_condition;
	  fact_table[1][1] = z_sym_ms_condition;
	}
      else if(strcmp(argv[i],"-time")==0)
	{
	  timing = TRUE;
	  fprintf(stderr,"timing is true\n");
	}
      else if(strncmp(argv[i],"-upp",4)==0)
	{
	  fact_table[0][0] = d_upper_tri_ms_factor;
	  fact_table[0][1] = z_upper_tri_ms_factor;
	  fact_table[1][0] = d_upper_tri_ms_condition;
	  fact_table[1][1] = z_upper_tri_ms_condition;
	}
      else flag_error();
    }

/*---------------------------------------------------------------------------*/
/* set the function pointers to the correct routines */
  *solve_fctn = fact_table[fact_index][type_index];

  if(type_index)
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
	*data_out  = put_complex_double_binary_matrix;
      else 
	*data_out  = put_complex_double_matrix;
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
	*data_out  = put_binary_matrix;
      else
	*data_out  = put_matrix;
    }      
/*---------------------------------------------------------------------------*/
/* set the type
 * if *type == 0 then the matrix is real                  *
 * if *flag == 1 then the matrix is complex               */

  *type = type_index;

/*---------------------------------------------------------------------------*/
/* obtain the in files */
  
  if(default_in_name==NULL)
    default_in=stdin;
  else {
    default_in=fopen(default_in_name,"r");
    if(default_in==NULL)
      error("MatrixSolve: the specified input file could not be created");
  }

  if(Ain_name==NULL)
    *Ain=default_in;
  else {
    *Ain=fopen(Ain_name,"r");
    if(*Ain==NULL)
      error("MatrixSolve: the specified A input file could not be accessed");
  }

  if(Bin_name==NULL)
    *Bin=default_in;
  else {
    *Bin=fopen(Bin_name,"r");
    if(*Bin==NULL)
      error("MatrixSolve: the specified B input file could not be accessed");
  }

/*---------------------------------------------------------------------------*/
/* obtain the out files */
 
  if(default_out_name==NULL)
    default_out=stdout;
  else
    if(strcmp(default_out_name,"stderr")==0)
      default_out = stderr;
    else {
      default_out=fopen(default_out_name,"w");
      if(default_out==NULL)
	error("MatrixSolve: the specified inverse output file could not be created");
  }

  if(Xout_name==NULL)
    Xout[0]=default_out;
  else
    if(strcmp(Xout_name,"stderr")==0)
      Xout[0] = stderr;
    else {
      Xout[0]=fopen(Xout_name,"w");
      if(Xout[1]==NULL)
	error("MatrixSolve: the specified inverse output file could not be created");
    }

  if(cond_name==NULL)
    Xout[1]=default_out;
  else
    if(strcmp(cond_name,"stderr")==0)
      Xout[1] = stderr;
    else {
      Xout[1]=fopen(cond_name,"w");
      if(Xout[1]==NULL)
	error("MatrixSolve: the specified condition output file could not be created");
    }
}

/*===========================================================================*/

void flag_error()
{
  fprintf(stderr,"\n");
  fprintf(stderr,"%s",
	   "MatrixSolve - a program for solving linear systems of the form Ax = B");
  fprintf(stderr,"%s\n"," ");
  fprintf(stderr,"%s\n","\noptions:\t(none are required)");  
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "data type flags: the default data type is double precision real");
  fprintf(stderr,"%s\n"," -complex\t:use double precision complex matrices");
  fprintf(stderr,"%s\n"," -real\t\t:use double precision real matrices");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "matrix type flags: the default matrix type is general");
  fprintf(stderr,"%s\n"," -general\t\t:works with any type of matrix");
  fprintf(stderr,"%s\n"," -hermitian\t\t:optimized for hermitian matricies");
  fprintf(stderr,"%s\n"," -lower_triangle\t:optimized for upper triangular matricies");
  fprintf(stderr,"%s\n"," -symmetric\t\t:optimized for symmetric matricies");
  fprintf(stderr,"%s\n"," -upper_triangle\t:optimized for lower triangular matricies");
  fprintf(stderr,"%s\n"," -triadiagonal\t\t:not implemented yet");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",
	  "file flags: the defaults are ascii input from stdin and output to stdout");
  fprintf(stderr,"%s\n"," -binary_Ain\t\t\t:input the matrices in binary format");
  fprintf(stderr,"%s\n"," -binary_Bin\t\t\t:input the matrices in binary format");
  fprintf(stderr,"%s\n"," -binary_out\t\t\t:output the inverses in binary format");
  fprintf(stderr,"%s\n",
	  " -file_Ainput in_Afile\t\t:get input from in_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_Binput in_Bfile\t\t:get input from in_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_output out_file\t\t:output to out_file not stdout");
  fprintf(stderr,"%s\n",
	  " -file_condition cond_file\t:output the conditions to cond_file");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","other flags:");
  fprintf(stderr,"%s\n",
	  " -condition\t:estimate the reciprocal conditions of the matrices");
  fprintf(stderr,"%s\n","\t\t     the default is not to estimate them");
  fprintf(stderr,"%s\n"," -time\t\t:display timing information");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","help flags:");
  fprintf(stderr,"%s\n"," ?\t\t:display this message");
  fprintf(stderr,"%s\n"," -?\t\t:display this message");
  fprintf(stderr,"%s\n"," -h\t\t:display this message");
  fprintf(stderr,"%s\n"," -help\t\t:display this message");
  fprintf(stderr,"\n%s\n\n",
	  "some of these flags may be abreviated such as -det for -determinant");
  exit(1);
}
/*===========================================================================*/
