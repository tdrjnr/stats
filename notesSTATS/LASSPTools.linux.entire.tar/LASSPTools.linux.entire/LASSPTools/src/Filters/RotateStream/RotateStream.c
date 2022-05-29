/***************************************************************************
Program RotateStream
Options: 
-b,  -binary            specifies the polydraw binary mode.
-f,  -file_in           reads the input data from a file instead of stdin
-h,  -help              prints help message
-l,  -light             does simulated lighting of polygons.
-p,  -project           removes the z coordinate from the output.
-rc, -remove_comments   removes all lines that begin with text.
-rt, -remove_text       removes all text from the input.
-s,  -sort              sorts the output.
-q,  -quit_eof          quits when it reads EOF in the input data.
-w,  -wait_eof          will not quit until it reads EOF in the input data.
-x,  -axis, -movieaxis  clears the screen for movieaxis at EOF.
----------------------
Implemented in rotate_aid:
     -initial           sets the initial rotation matrix.
-rf, -rotate_file       reads the rotation matrices from a file.
-ro, -rotate            reads the rotation matrices from the rotate program.
-po, -popen             reads the rotation matrices from the popen-ed program. 
-sl, -slider            reads the rotation matrices from the slider program.
***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include "RotateStream.h"


/* these are the functions that main() calls */

#ifndef _NO_PROTO
int get_frame(frame *, char *, char);
void rotate_and_sort_frame(frame, double **);
void lighten_and_output_binary_frame(frame, char *);
void lighten_and_output_frame(frame, char *);
void output_binary_frame(frame, char);
void output_frame(frame, char *);
void fatal(char *);
#else
int get_frame();
void rotate_and_sort_frame();
void lighten_and_output_binary_frame();
void lighten_and_output_frame();
void output_binary_frame();
void output_frame();
void fatal();
#endif

/* these are global variables set by parse options to
 * prevent the function calls from becoming to obscure
 *
 *   variable:          Procedures that use it:
 */
char averaging;      /* rotate_and_sort_frame */
char binary;         /* main */
char blocking;       /* main */
char buffering;      /* main */
char lighting;       /* main */
char long_remains;   /* initialize_frame, get_frame, expand_frame */
char movieaxis_mode; /* get_frame */
char pass_comments;  /* get_frame */
char pass_text;      /* initialize_frame, get_frame */
char quit_eof;       /* main */
int  sorting;        /* rotate_and_sort_frame */
char two_D_out;      /* ouput_frame */
char wait_eof;       /* main */

/*--------------------------------------------------------------------------
 * print_help()
 */
#ifndef _NO_PROTO
void print_help()
#else
void print_help()
#endif
{
  fprintf(stderr,"Usage:\tRotateStream -s z -p < data | PlotAtoms\n\n");
  fprintf(stderr,"Options:\t(none are required)\n");
  fprintf(stderr,"\t\t-b[inary] : the polydraw binary input & output mode.\n");
  fprintf(stderr,"\t\t-f[ile_in] FILE : read the input data from FILE.\n");
  fprintf(stderr,"\t\t-h[elp] : display this help message.\n");
  fprintf(stderr,"\t\t-initial Alpha Beta Gamma : sets the initial rotation matrix.\n");
  fprintf(stderr,"\t\t-l[ight] : simulated lighting of polygons.\n");
  fprintf(stderr,"\t\t-po[pen] \"PROGRAM\" : read the rotation matrices from PROGRAM.\n");
  fprintf(stderr,"\t\t-p[roject] : remove the z coordinate from the output.\n");
  fprintf(stderr,"\t\t-q[uit_eof] : quit at EOF in the input data.\n");
  fprintf(stderr,"\t\t-rc | -remmove_comments : remove lines that begin with text.\n");
  fprintf(stderr,"\t\t-rf | -rotate_file FILE : read the rotation matrices from FILE.\n");
  fprintf(stderr,"\t\t-ro[tate] : read the rotation matrices from rotate.\n");
  fprintf(stderr,"\t\t-rt | -remmove_text : remove all text from the input data.\n");
  fprintf(stderr,"\t\t-sl[ider] : read the rotation matrices from slider.\n");
  fprintf(stderr,"\t\t-s[ort] [a][x |y |z] : sort the output.\n");
  fprintf(stderr,"\t\t-w[ait_eof] : do not quit until EOF in the input data.\n");
  fprintf(stderr,"\t\t-x | -axis | -movieaxis : the movieaxis mode.\n");
  fprintf(stderr,"\nDescription:\tRotateStream is a filter for rotating triples.\n");
}
  

/*--------------------------------------------------------------------------
 * char empty_file(int buffer)
 */
#ifndef _NO_PROTO
char empty_file(int buffer)
#else
char empty_file(buffer)
int buffer;
#endif
{
  struct stat sb;

  if(fstat(buffer,&sb)==-1)
    fatal("error in empty_file");
  return(sb.st_size == 0);
}

/*--------------------------------------------------------------------------
 * int read_matrix(int file_no, double matrix[3][3])
 */
#ifndef _NO_PROTO
int read_matrix(int file_no, double matrix[3][3])
#else
int read_matrix(file_no, matrix)
int file_no;
double matrix[3][3];
#endif
{
  double new_matrix[9];
  size_t nbytes;

  nbytes = 9 * sizeof(double);

  if(read(file_no,new_matrix,(unsigned)nbytes) != nbytes)
    return(0);

  memcpy((void *)(&matrix[0][0]), (void *)new_matrix, nbytes);
  return(1);
}
  
/*---------------------------------------------------------------------------
 *void rotate_file_numbers(int argc, char **argv, int *rot_in, int *rot_eof)
 */

#ifndef _NO_PROTO
void rotate_file_numbers(int argc, char **argv, int *rot_in, int *rot_eof)
#else
void rotate_file_numbers(argc, argv, rot_in, rot_eof)
int argc;
char **argv;
int *rot_in;
int *rot_eof;
#endif
{
  int i, pid;
  int pipefd[2], eofpipe[2];
  char begin[6];

  /* if stdin is a tty fork rotate_echo to handle the input */
  if(isatty(0)) {
    if(pipe(pipefd) == -1)
      fatal("error, could not open rotate_echo");
    pid = fork();
    if(pid) {
      if (pid == -1)
	fatal("error, could not open rotate_echo");
      close(pipefd[1]);
      if(close(0) == -1)
	fatal("error, could not set stdin to rotate_echo");
      if(dup(pipefd[0]) == -1)
	fatal("error, could not set stdin to rotate_echo");
    }
    else { /* set pipe and exec rotate_echo */
      close(pipefd[0]); 
      if(close(1) == -1)
	fatal("error, could not open rotate_echo");
      if(dup(pipefd[1]) == -1) /* set stdout for counter */
	fatal("error, could not open rotate_echo");
      close(pipefd[1]);
      execlp("rotate_echo", NULL);
      fatal("error, could not open rotate_echo");
    }
  }
  /* use pipe fork and exec */
  if(pipe(pipefd) == -1)
    fatal("error, could not open rotate");
  if(pipe(eofpipe) == -1)
    fatal("error, could not open rotate");
  pid = fork();
  if(pid) {
    if (pid == -1)
      fatal("error, could not open rotate");
    close(pipefd[1]);
    *rot_in = pipefd[0];
    close(eofpipe[1]);
    *rot_eof = eofpipe[0];
    
    /* get begin flag and look for errors */
    if(read(eofpipe[0],begin,6) < 1) exit(1);
  }
  else { /* set pipes and exec rotate_aid */
    /* set rotate_in */
    close(pipefd[0]); 
    close(eofpipe[0]);
    if(close(1) == -1)
      fatal("error, could not open rotate");
    if(dup(pipefd[1]) == -1) /* set stdout for counter */
      fatal("error, could not open rotate");
    /* set rotate_end_of_file */
    close(pipefd[1]);
    if(eofpipe[1] != 3) {
      if(close(3) == -1 && dup(eofpipe[1]) != 3)
	fatal("error, could not open rotate");
      if(dup(eofpipe[1]) == -1) /* set stdout for counter */
	fatal("error, could not open rotate");
      close(eofpipe[1]);
    }
    execvp("rotate_aid", argv);
    fatal("error, could not open rotate");
  }
}

/*---------------------------------------------------------------------------
 *parse_options(int argc, char **argv)
 */

#ifndef _NO_PROTO
void parse_options(int argc, char **argv)
#else
void parse_options(argc, argv)
int argc;
char **argv;
#endif
{
  int i,j, old_argc;
  char *dummy_string = "";
  char null_arg = '\0';

  i = parse_argument(argc,argv,0,&dummy_string,"-h");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-help");
  if (i!=-1) {
    print_help(); /* this causes the program to exit */
    exit(0);
  }
  averaging = True;
  sorting = NOSORT;
  i = parse_argument(argc,argv,0,&dummy_string,"-s");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-sort");
  if (i!=-1) {
    argv[i] = &null_arg;
    i++;
    if(i<argc && argv[i][0] != '-') {
      if(argv[i][0] == 'a' || argv[i][0] == 'A')
	(argv[i])++;
      else
	averaging = False;
      if((argv[i][0]=='z' || argv[i][0]=='Z') && argv[i][1]=='\0')
	sorting = Z;
      else if((argv[i][0]=='y' || argv[i][0]=='Y') && argv[i][1]=='\0')
	sorting = Y;
      else if((argv[i][0]=='x' || argv[i][0]=='X') && argv[i][1]=='\0')
	sorting = X;
      else {
	sorting = Z; /* assume flag following -s is spurious */
	averaging = True; /* restore the averaging flag */
	i--; /* make sure the next argv is not deleted */
      }
      argv[i] = &null_arg;
    }
    else sorting = Z;
  }
  
  i = parse_argument(argc,argv,0,&dummy_string,"-rc");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-remove_comments");
  if (i!=-1) {
    pass_comments = False;
    argv[i] = &null_arg;
  }
  else
    pass_comments = True;

  i = parse_argument(argc,argv,0,&dummy_string,"-rt");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-remove_text");
  if (i!=-1) {
    pass_text = False;
    pass_comments = False;
    argv[i] = &null_arg;
  }
  else
    pass_text = True;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-p");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-project");
  if (i!=-1) {
    two_D_out = True;
    argv[i] = &null_arg;
  }
  else
    two_D_out = False;

  i = parse_argument(argc,argv,0,&dummy_string,"-b");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-binary");
  if (i!=-1) {
    binary = True;
    argv[i] = &null_arg;
  }
  else
    binary = False;

  i = parse_argument(argc,argv,0,&dummy_string,"-buf");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-buffer");
  if (i!=-1) {
    buffering = True;
    argv[i] = &null_arg;
  }
  else
    buffering = False;

  i = parse_argument(argc,argv,0,&dummy_string,"-long");
  if (i!=-1) {
    long_remains = True;
    argv[i] = &null_arg;
  }
  else
    long_remains = False;

  i = parse_argument(argc,argv,0,&dummy_string,"-x");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-axis");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-movieaxis");
  if (i!=-1) {
    movieaxis_mode = True;
    argv[i] = &null_arg;
  }
  else
    movieaxis_mode = False;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-l");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-light");
  if (i!=-1) {
    if(pass_text)
      lighting = True;
    else if(!binary) {
      fprintf(stderr,"the -light option does not work with -remove_text\n");
      exit(1);
    }
    argv[i] = &null_arg;
  }
  else
    lighting = False;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-q");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-quit_eof");
  if (i!=-1) {
    quit_eof = True;
    argv[i] = &null_arg;
  }
  else
    quit_eof = False;
  
  i = parse_argument(argc,argv,0,&dummy_string,"-w");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-wait_eof");
  if (i!=-1) {
    wait_eof = True;
    argv[i] = &null_arg;
  }
  else {
    i = parse_argument(argc,argv,0,&dummy_string,"-rf");
    if (i==-1)
      i = parse_argument(argc,argv,0,&dummy_string,"-rotate_file");
    if (i!=-1)
      wait_eof = True;
    else
      wait_eof = False;
  }

  i = parse_next_argument(argc,argv,0,&dummy_string,"-f");
  if(i==-1)
    i = parse_next_argument(argc,argv,0,&dummy_string,"-file_in");
  if(i!=-1) {
    argv[i--] = &null_arg;
    argv[i] = &null_arg;
    j = open(dummy_string,0);
    if(j==-1)
      fatal("error, can not open input file");
    else {
      if(close(0)==-1) /* close stdin */
	fatal("error, can not close stdin");
      else {
	if(dup(j)!=0) /* set stdin to input file */
	  fatal("error, can not close stdin");
	else if(close(j)==-1) 
	  fprintf(stderr, "RotateStream : Warning, unable to close a file descriptor\n");
      }
    }
  }

  /* contract the remaining arguments in argv */
  old_argc = argc;
  j = 0;
  i = 1;
  while(i<argc) {
    if(*(argv[i])=='\0') {
      if(j<i) j = i;
      while((++j)<old_argc && *(argv[j])=='\0');
      if(j<old_argc) {
	argv[i] = argv[j];
	argv[j] = &null_arg;
      }
      else
	break;
    }
    i++;
  }
  argc = i;
  argv[argc]=NULL;
}

/*---------------------------------------------------------------------------
 *initialize_frame(frame F)
 */

#ifndef _NO_PROTO
frame initialize_frame(frame F)
#else
frame initialize_frame(F)
frame F;
#endif
{
  size_t i;

  i = (size_t)INITIALPOLY * sizeof(int);
  F.num = (int *)malloc(i);
  F.ordering = (int *)malloc(i);
  i = (size_t)INITIALPOLY * sizeof(double *);
  F.Act[X] = (double **)malloc(i);
  F.Act[Y] = (double **)malloc(i);
  F.Act[Z] = (double **)malloc(i);
  F.Rot[X] = (double **)malloc(i);
  F.Rot[Y] = (double **)malloc(i);
  F.Rot[Z] = (double **)malloc(i);
  if(binary) {
    F.remains = NULL;
    F.color = (XColor *)malloc((size_t)INITIALPOLY * sizeof(XColor));
  }
  else {
    F.remains = (char **)malloc((size_t)INITIALPOLY * sizeof(char *));
    F.color = NULL;
  }
  if(F.num == NULL || F.ordering == NULL ||
     F.Act[X] == NULL || F.Act[Y] == NULL || F.Act[Z] == NULL ||
     F.Rot[X] == NULL || F.Rot[Y] == NULL || F.Rot[Z] == NULL ||
     (binary && F.color == NULL) || (!binary && F.remains == NULL))
    {
      fatal("memory error in initialize_frame");
    }
 /* now set x and y and z to NULL so that read_triples knows that they are
   * initially unallocated
   */
  for (i=0; i<INITIALPOLY; i++) {
    F.num[i] = 0;
    F.Act[X][i]  = NULL;  F.Act[Y][i] = NULL;  F.Act[Z][i] = NULL;
    F.Rot[X][i] = NULL; F.Rot[Y][i] = NULL; F.Rot[Z][i] = NULL;
  }
  if(!binary) {
    if(long_remains)
      for (i=0; i<INITIALPOLY; i++)
	F.remains[i] = NULL;
    else {
      i = sizeof(char) * (size_t)(INITIALTEXT * INITIALPOLY);
      F.remains[0]=(char *)malloc(i);
      if(F.remains[0] == NULL) fatal("memory error in initialize_frame");
      for(i=1; i<INITIALPOLY; i++)
	F.remains[i] = F.remains[i-1] + 30;
    }
  }
  F.maxpoly = INITIALPOLY;
  F.numpoly = 0; /* This assignment is very important for   *
		  * the initial iteration of the main loop. */
  return(F);
}

/*--------------------------------------------------------------------------
 * int main(int argc, char **argv)
 */
#ifndef _NO_PROTO
int main(int argc, char **argv)
#else
int main(argc, argv)
int argc;
char **argv;
#endif
{
  /********************* Declare Variables *********************************/
  int i,j;
  size_t index;
  double rot[3][3]; /* holds the rotation matrix */
  char input[BUFSIZ +1]; /* a buffer to hold the line currently being read *
			  * get_frame returns the line of text immediately *
			  * following new frame in input.                  */
  int rotate_eof, rotate_in;
  frame F;
  
  /********************* Initialize Variables ******************************/
  parse_options(argc, argv);
  F = initialize_frame(F);
  
  blocking = (quit_eof || wait_eof);
  
  /* open the rotate controller */
  rotate_file_numbers(argc, argv, &rotate_in, &rotate_eof);
  
  /* give them an unrated view to look at while they go for the mouse
   * to play with the rotation controller
   */
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      if (i==j)
	rot[i][j]=1.0;
      else
	rot[i][j]=0.0;

  /********************* Enter The Main Loop *******************************/
  if(!binary) {
    i = 0;
    /* write an exit conditon function */
    while((!quit_eof || i != EOF) && (wait_eof || empty_file(rotate_eof))) {
      
      /* get the next frame if there is one.  get_frame is non-blocking */
      i = get_frame(&F, input, blocking);
      if(i == EOF) wait_eof = False;
      
      /* get the next rotation matrix if there is one */
      if(!empty_file(rotate_in))
	j = read_matrix(rotate_in, rot);
      else
	j = 0;
      
      /* output if anything has changed */
      if((j || i > 0) && F.numpoly) {
	rotate_and_sort_frame(F,rot);
	if(lighting)
	  lighten_and_output_frame(F, input);
	else
	  output_frame(F, input);
      }
    }  
    if(buffering) {
      input[0]='\0';
      if(lighting)
	lighten_and_output_frame(F, input);
      else
	output_frame(F, input);
    }
  }
  else {
    i = 0;
    /* write an exit conditon function */
    while((!quit_eof || i != EOF) && (wait_eof || empty_file(rotate_eof))) {
      
      /* get the next frame if there is one.  get_frame is non-blocking */
      i = get_binary_frame(&F, blocking);
      if(i == EOF) wait_eof = False;
      
      /* get the next rotation matrix if there is one */
      if(!empty_file(rotate_in))
	j = read_matrix(rotate_in, rot);
      else
	j = 0;
      
      /* output if anything has changed */
      if((j || i > 0) && F.numpoly) {
	rotate_and_sort_frame(F,rot);
	if(lighting)
	  lighten_and_output_binary_frame(F, True);
	else
	  output_binary_frame(F, True);
      }
    }
    if(buffering) {
      if(lighting)
	lighten_and_output_binary_frame(F, False);
      else
	output_binary_frame(F, False);
    }
  }
  /********************* Clean Up Before Quiting ***************************/
  for(index = 0; index < F.numpoly; index++) {
    free(F.Act[X][index]);
    free(F.Act[Y][index]);
    free(F.Act[Z][index]);
  }
  free(F.Rot[X][0]);
  free(F.Rot[Y][0]);
  free(F.Rot[Z][0]);
  if(binary)
    free(F.color);
  else
    if(long_remains)
      for(index = 0; index < F.numpoly; index++)
	free(F.remains[index]);
    else
      for(index = 0; index < F.maxpoly; index++)
	free(F.remains[index]);
  free(F.num);
  free(F.ordering);
  close(rotate_in);
  exit(0);
}

