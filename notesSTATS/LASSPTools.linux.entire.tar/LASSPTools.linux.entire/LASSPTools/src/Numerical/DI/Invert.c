/*****************************************************************
 Program Invert                                         
 finds the determinant and/or the inverse of matrix A                        
 This is called from a UNIX command line as           
 Invert [-real, -complex] [-general, -symmertric, -hermitian
  -upper_triangular, -lower_triangular] [-condition] [-?, ? -h, -help]
  [-binary_in] [-binary_out] [-file_in InputFile] [-file_out OutputFile]
  [-file_det DeterminantOutputFile]  [-file_inv InverseOutputFile]
  [-file_cond ConditionOutputFile] [-timing]

 flag                abbr     purpose
 ----------------------------------------------------------------------
 -real             : ***** : to set the matrix elements to type real
 -complex          : -comp : to set the matrix elements to type complex
 -general          : -gen  : to set the matrix type to general
 -symmetric        : -sym  : to set the matrix type to symmetric
 -hermitian        : -her  : to set the matrix type to hermitian
 -upper_triangular : -upp  : to set the matrix type to upper triangular
 -lower_triangular : -low  : to set the matrix type to lower triangular
 -condition        : -cond : to calculate the condition of the matrix
 ?                 : ***** : to get the command line help message
 -?                : ***** : to get the command line help message
 -h                : ***** : to get the command line help message
 -help             : ***** : to get the command line help message
 -binary_in        : ***** : to input the matrix using the binary mode
 -binary_out       : ***** : to output the matrix using the binary mode
 -file_in          : ***** : read the matrix from InputFile, not stdin
 -file_out         : ***** : write the output to OutputFile, not stdout
 -file_det         : ***** : write the determinant to DeterminantOutputFile
 -file_inv         : ***** : write the inverse to InverseOutputFile
 -file_cond        : ***** : write the condition to ConditionOutputFile
 -timing           : -tim  : display timing information to stderr

 Utilizes the double precision Linpack routines
 Version 1.0 July 22, 1991
 Written by Eric Lochstet
*******************************************************************/
# include "Invert.h"

char pgmname[255];

#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc, argv)
int argc;
char  **argv;
#endif
{
  Matrix *A;
  ComplexDoubleMatrix *C;
  clock_t  start_time,stop_time; float elapsed_time;
  FILE     *Ain;
  FILE     *detinv_out[2]; /* the first file ptr is to the inv  out file    *
			    * the second file ptr is to the cond out file   */
  DetInvFctn find_detinv;
  GetMatrix  matrix_in;
  PutMatrix  matrix_out;
  int        power;
  int        flag;   /* is 10 for det. 1 for inv. 11 for both     */
  int        type;   /* type is 0 for real matrix, 1 for complex, *
		      *	and 2 for hermitian                       */

  WorkSpace Z;

  strcpy(pgmname,argv[0]);

  get_options(argc, argv, &Ain, detinv_out, &find_detinv,
	      &matrix_in, &matrix_out, &type);

  flag = 1; /* want the inverse only */

  if(type > 0) { /* The section for complex matrices */

    type--; /* NOW type will be 0 for complex and 1 for hermitan matrices */

    /* The initializations for complex matrices */

    C = create_default_complex_double_matrix();

    Z.ipvt = (int *)malloc((size_t)40 * sizeof(int));
    if(Z.ipvt == NULL)
      error("workspace malloc failled");
    else
      Z.Isize = 40;
    Z.work = (double *)malloc((size_t)80 * sizeof(double));
    if(Z.work == NULL)
      error("workspace malloc failled");
    else
      Z.Wsize = 80;

    if(timing) {

      /* The timing loop for complex matrices */

      while(TRUE)
	{
	  start_time=clock();
	  
	  if(feof(Ain) || matrix_in(C,Ain)==EOF) exit(0);
          strcpy(pgmname,argv[0]);
	  
	  find_detinv(C, &Z, detinv_out[1], &flag);
	  
	  stop_time=clock();
	  elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
	  fprintf(stderr,"Invert: time taken was %f seconds\n"
		  , elapsed_time);
	  
	  matrix_out(*C, detinv_out[1]);
	}
    }	  

    else {

      /* The nontiming loop for complex matrices */

      while(TRUE)
	{
	  if(feof(Ain) || matrix_in(C,Ain)==EOF) exit(0);
          strcpy(pgmname,argv[0]);	  	  
	  find_detinv(C, &Z, detinv_out[1], &flag);
	  
	  matrix_out(*C, detinv_out[1]);
	}
    }
  }

  else { /* The section for real matrices */

    /* The initializations for real matrices */

    A = create_default_matrix();

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

      /* The timing loop for real matrices */

      while(TRUE)
	{
	  start_time=clock();
	  
	  if(feof(Ain) || matrix_in(A,Ain)==EOF) exit(0);
          strcpy(pgmname,argv[0]);	  	  
	  find_detinv(A, &Z, detinv_out[1], &flag);
	  
	  stop_time=clock();
	  elapsed_time=((float) (stop_time-start_time))/CLOCKS_PER_SEC;
	  fprintf(stderr,"Invert: time taken was %f seconds\n"
		  , elapsed_time);
	  
	  matrix_out(*A, detinv_out[1]);
	}
    }	  
    else {

      /* The nontiming loop for real matrices */

      while(TRUE)
	{
	  if(feof(Ain) || matrix_in(A,Ain)==EOF) exit(0);
          strcpy(pgmname,argv[0]);
	  	  
	  find_detinv(A, &Z, detinv_out[1], &flag);
	  
	  matrix_out(*A, detinv_out[1]);
	}
    }
  }
}

/*===========================================================================*/
 
#ifndef _NO_PROTO
void get_options(int argc, char **argv, FILE **Ain, FILE **detinv_out,
		 DetInvFctn *det_fctn, GetMatrix *matrix_in,
		 PutMatrix *matrix_out, int *type)
#else
void get_options(argc, argv, Ain, detinv_out, det_fctn, matrix_in,
		 matrix_out, type)
int argc;
char **argv;
FILE **Ain;
FILE **detinv_out;
DetInvFctn *det_fctn;
GetMatrix *matrix_in;
PutMatrix *matrix_out;
int *type;
#endif

/************************
 obtain the options 
 ************************/
{
  char  c;
  char  *Ain_name=NULL,*default_name=NULL;
  char  *cond_name=NULL,*inv_name=NULL;
  int   i;
  int   fact_index;
  int   type_index;
  int   binary_out;
  int   binary_in;
  FILE  *default_out;
      
  DetInvFctn fact_table[2][2];

  fact_table[0][0] = d_gen_di_factor;
  fact_table[0][1] = z_gen_di_factor;
  fact_table[1][0] = d_gen_di_condition;
  fact_table[1][1] = z_gen_di_condition;
  
  binary_out = FALSE; /* the default is non binary  */
  binary_in = FALSE;  /* the default is non binary  */
  type_index = -1; /* -1 means no type flag was set */
  fact_index = 0;  /* the default factorization does not
		    * compute the condition         */
  *type = 0;

  for(i=1; i<argc; i++)
    {
      if(strncmp(argv[i],"-file_",6)==0)
	{ 
	  argv[i]=argv[i]+6;
	  if(strncmp(argv[i],"inv",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else inv_name=argv[i];
	    }
	    else flag_error();
	  }  
	  else if(strncmp(argv[i],"in",2)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else Ain_name=argv[i];
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
	  else if(strncmp(argv[i],"out",3)==0) {
	    i++;
	    if(i<argc) {
	      if(*argv[i]=='-') flag_error();
	      else default_name=argv[i];
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
	  else if(strncmp(argv[i],"in",2)==0) {
	    binary_in = TRUE;
	  }
	  else flag_error();
	}
      else if(strncmp(argv[i],"-comp",5)==0)
	{
	  if(type_index == -1)
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
	  fact_table[0][0] = d_gen_di_factor;
	  fact_table[0][1] = z_gen_di_factor;
	  fact_table[1][0] = d_gen_di_condition;
	  fact_table[1][1] = z_gen_di_condition;
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
	  *type = 1;
	  if(type_index == -1)
	    type_index = 1;
	  else if(type_index == 0)
	    fprintf(stderr,
		    "\n!!WARNNING!! Invert expects real hermitain matrices\n");
	  fact_table[0][0] = d_sym_di_factor;
	  fact_table[0][1] = z_hermitian_di_factor;
	  fact_table[1][0] = d_sym_di_condition;
	  fact_table[1][1] = z_hermitian_di_condition;
	}
      else if(strncmp(argv[i],"-low",4)==0)
	{
	  fact_table[0][0] = d_lower_tri_di_factor;
	  fact_table[0][1] = z_lower_tri_di_factor;
	  fact_table[1][0] = d_lower_tri_di_condition;
	  fact_table[1][1] = z_lower_tri_di_condition;
	}
      else if(strcmp(argv[i],"-real")==0)
	{
	  if(type_index == -1)
	    type_index = 0;
	  else
	    flag_error();
	}
      else if(strncmp(argv[i],"-sym",4)==0)
	{
	  fact_table[0][0] = d_sym_di_factor;
	  fact_table[0][1] = z_sym_di_factor;
	  fact_table[1][0] = d_sym_di_condition;
	  fact_table[1][1] = z_sym_di_condition;
	}
      else if(strncmp(argv[i],"-tim",4)==0)
	{
	  timing = TRUE;
	}
      else if(strncmp(argv[i],"-upp",4)==0)
	{
	  fact_table[0][0] = d_upper_tri_di_factor;
	  fact_table[0][1] = z_upper_tri_di_factor;
	  fact_table[1][0] = d_upper_tri_di_condition;
	  fact_table[1][1] = z_upper_tri_di_condition;
	}
      else flag_error();
    }

/*---------------------------------------------------------------------------*/
/* set the function pointers to the correct routines */

  if(type_index == -1)
    type_index = 0; /* the default type is real */
  *det_fctn = fact_table[fact_index][type_index];

  if(type_index)
    {
      if(binary_in)
	*matrix_in = get_complex_double_binary_matrix;
      else
	*matrix_in = get_complex_double_matrix;
      if(binary_out)
	*matrix_out = put_complex_double_binary_matrix; 
      else
	*matrix_out = put_complex_double_matrix;
    }
  else
    {
      if(binary_in)
	*matrix_in = get_binary_matrix;
      else
	*matrix_in = get_matrix;
      if(binary_out)
	*matrix_out  = put_binary_matrix;
      else
	*matrix_out  = put_matrix;
    }
/*---------------------------------------------------------------------------*/

  *type += type_index;

/*---------------------------------------------------------------------------*/
/* obtain the in files */
  
  if(Ain_name==NULL)
    *Ain=stdin;
  else {
    *Ain=fopen(Ain_name,"r");
    if(*Ain==NULL)
      error("DI: the specified input file could not be accessed");
  }

/*---------------------------------------------------------------------------*/
/* obtain the out files */
 
  if(default_name==NULL)
    default_out=stdout;
  else
    if(strcmp(default_name,"stderr")==0)
      default_out = stderr;
    else {
      default_out=fopen(default_name,"w");
      if(default_out==NULL)
	error("DetterminantInverse: the specified output file could not be created");
  }

  if(inv_name==NULL)
    detinv_out[0]=default_out;
  else
    if(strcmp(inv_name,"stderr")==0)
      detinv_out[0] = stderr;
    else {
      detinv_out[0]=fopen(inv_name,"w");
      if(detinv_out[0]==NULL)
	error("Invert: the specified inverse output file could not be created");
  }

  if(cond_name==NULL)
    detinv_out[1]=default_out;
  else
    if(strcmp(cond_name,"stderr")==0)
      detinv_out[1] = stderr;
    else {
      detinv_out[1]=fopen(cond_name,"w");
      if(detinv_out[1]==NULL)
	error("Invert: the specified condition output file could not be created");
  }
}

/*===========================================================================*/

void flag_error()
{
  fprintf(stderr,"\n");
  fprintf(stderr,"%s",
	  "Invert - a program for calculating determinants or");
  fprintf(stderr,"%s\n"," inverses");
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
  fprintf(stderr,"%s\n"," -binary_in\t\t\t:input the matrices in binary format");
  fprintf(stderr,"%s\n"," -binary_out\t\t\t:output the inverses in binary format");
  fprintf(stderr,"%s\n",
	  " -file_input in_file\t\t:get input from in_file not sdtin");
  fprintf(stderr,"%s\n",
	  " -file_output out_file\t\t:output to out_file not stdout");
  fprintf(stderr,"%s\n",
	  " -file_inverse inv_file\t\t:output the inverses to inv_file");
  fprintf(stderr,"%s\n",
	  " -file_condition cond_file\t:output the conditions to cond_file");
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n","other flags:");
  fprintf(stderr,"%s\n",
	  " -condition\t:estimate the reciprocal conditions of the matrices");
  fprintf(stderr,"%s\n","\t\t     the default is not to estimate them");
  fprintf(stderr,"%s\n"," -tim[e, ing]\t:display timing information");
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
