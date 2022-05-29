#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#define True 1
#define False 0
#define FORMAT "%lf %lf %lf %lf %lf %lf %lf %lf %lf"
#define ROW(a) a[0], a[1], a[2]
#define READ(a) fscanf(stdin, FORMAT, ROW(&a[0]), ROW(&a[1]), ROW(&a[2]))


char slider_mode;

#ifndef _NO_PROTO
void convert_euler(double alpha, double beta, double gamma, double rot[3][3])
#else
void convert_euler(alpha, beta, gamma, rot)
double alpha;
double beta;
double gamma;
double rot[3][3];
#endif
{
  /* Euler angles, using the conventions of aerodynamics */
  /* see H. Goldstein, Classical Mechanics */
  
  rot[0][0] = cos(alpha)*cos(beta);
  rot[0][1] = cos(beta)*sin(alpha);
  rot[0][2] = -sin(beta);

  rot[1][0] = sin(gamma)*sin(beta)*cos(alpha) - sin(alpha)*cos(gamma);
  rot[1][1] = sin(gamma)*sin(beta)*sin(alpha) + cos(alpha)*cos(gamma);
  rot[1][2] = cos(beta)*sin(gamma);

  rot[2][0] = cos(gamma)*sin(beta)*cos(alpha) + sin(gamma)*sin(alpha);
  rot[2][1] = cos(gamma)*sin(beta)*sin(alpha) - sin(gamma)*cos(alpha);
  rot[2][2] = cos(beta)*cos(gamma);
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

  if(fstat(buffer,&sb)==-1) {
    fprintf(stderr, "rotate_aid : fatal error in empty_file");
    exit(1);
  }
  return(sb.st_size <= 0);
}

/*--------------------------------------------------------------------------
 * void open_input_device(int argc, char **argv)
 */
#ifndef _NO_PROTO
char open_input_device(int argc, char **argv)
#else
char open_input_device(argc, argv)
int argc;
char **argv;
#endif
{
  int i, j, old_argc, pid;
  int pipefd[2];
  char null_arg = '\0';
  char *dummy_string = "";
  char **slider_args;
  FILE *shell;

  i = parse_next_argument(argc,argv,0,&dummy_string,"-rf");
  if(i==-1)
    i = parse_next_argument(argc,argv,0,&dummy_string,"-rotate_file");
  if(i!=-1) {
    fprintf(stderr,"rotate_aid : opening rotate_file\n");
    j = open(dummy_string,0);
    if(j==-1) {
      fprintf(stderr, "rotate_aid : fatal error, can not open input file\n");
      exit(1);
    }
    else {
      if(close(0)==-1) { /* close stdin */
	fprintf(stderr, "rotate_aid : fatal error, can not close stdin\n");
	exit(1);
      }
      else {
	if(dup(j)!=0) { /* set stdin to input file */
	  fprintf(stderr, "rotate_aid : fatal error, can not close stdin\n");
	  exit(1);
	}
	else if(close(j)==-1) 
	  fprintf(stderr, "rotate_aid : Warning, unable to close a file descriptor\n");
      }
    }
    return(False);
  }
  
  i = parse_next_argument(argc,argv,0,&dummy_string,"-po");
  if(i==-1)
    i = parse_next_argument(argc,argv,0,&dummy_string,"-popen");
  if(i!=-1) {
    fprintf(stderr,"rotate_aid : opening %s\n", dummy_string);
    shell = popen(dummy_string,"r");
    if(shell == NULL) {
      fprintf(stderr, "rotate_aid : fatal error, can not open %s\n");
      exit(1);
    }
    else {
      j = fileno(shell);
      if(close(0)==-1) { /* close stdin */
	fprintf(stderr, "rotate_aid : fatal error, can not close stdin\n");
	exit(1);
      }
      else {
	if(dup(j)!=0) { /* set stdin to input file */
	  fprintf(stderr, "rotate_aid : fatal error, can not close stdin\n");
	  exit(1);
	}
	else if(close(j)==-1) 
	  fprintf(stderr, "rotate_aid : Warning, unable to close a file descriptor\n");
      }
    }
    return(False);
  }
  
  /* contract the remaining arguments in argv *
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
    */


  i = parse_argument(argc,argv,0,&dummy_string,"-sl");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-slider");
  if (i!=-1) {
    if(pipe(pipefd) == -1) {
      fprintf(stderr,"rotate_aid : fatal error, could not open slider\n");
      exit(1);
    }
    pid = fork();
    if(pid) {
      if (pid == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open slider\n");
	exit(1);
      }
      close(pipefd[1]);
      if(close(0) == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open slider\n");
	exit(1);    
      }
      if(dup(pipefd[0]) == -1) { /* set stdin for rotate_aid */
	fprintf(stderr,"rotate_aid: fatal error, could not open slider\n");
	exit(1);
      }
      close(pipefd[0]);
    }
    else {
      close(pipefd[0]);
      if(close(1) == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open slider\n");
	exit(1);    
      }
      if(dup(pipefd[1]) == -1) { /* set stdin for counter */
	fprintf(stderr,"rotate_aid: fatal error, could not open slider\n");
	exit(1);    
      }
      close(pipefd[1]);
      argc -= i+1;
      slider_args = (char **)malloc((size_t)(14 + argc)*sizeof(char *));
      if(slider_args == NULL) {
	execlp("slider","3","-min","-3.1416","-3.1416","-3.1416",
	      "-max","3.1416","3.1416","3.1416","-labels","alpha","beta",
	      "gamma", NULL);
      }
      else {
	slider_args[0] = argv[0];
	slider_args[1] = "3";
	slider_args[2] = "-min";
	slider_args[3] = "-3.1416";
	slider_args[4] = "-3.1416";
	slider_args[5] = "-3.1416";
	slider_args[6] = "-max";
	slider_args[7] = "3.1416";
	slider_args[8] = "3.1416";
	slider_args[9] = "3.1416";
	slider_args[10] = "-labels";
	slider_args[11] = "alpha";
	slider_args[12] = "beta";
	slider_args[13] = "gamma";
	for(j=14; j<14+(argc); j++)
	  slider_args[j] = argv[++i];
	slider_args[j] = NULL;
	execvp("slider",slider_args);
      }
      fprintf(stderr,"rotate_aid : fatal error, could not open slider\n");
      exit(1);
    }
    return(True);
  }

  i = parse_argument(argc,argv,0,&dummy_string,"-ro");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-rotate");
  if (i!=-1) {
    /* open rotate and make it the stdin for rotate_aid */
    if(pipe(pipefd) == -1) {
      fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
      exit(1);
    }
    pid = fork();
    if(pid) {
      if (pid == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
	exit(1);
      }
      close(pipefd[1]);
      if(close(0) == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
	exit(1);    
      }
      if(dup(pipefd[0]) == -1) { /* set stdin for rotate_aid */
	fprintf(stderr,"rotate_aid: fatal error, could not open rotate\n");
	exit(1);    
      }
      close(pipefd[0]);
    }
    else {
      close(pipefd[0]);
      if(close(1) == -1) {
	fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
	exit(1);    
      }
      if(dup(pipefd[1]) == -1) { /* set stdin for counter */
	fprintf(stderr,"rotate_aid: fatal error, could not open rotate\n");
	exit(1);    
      }
      close(pipefd[1]);

      argv[i] = "rotate";
      execvp("rotate", &(argv[i]));
      fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
      exit(1);
    }
    return(False);
  }
  /* open rotate and make it the stdin for rotate_aid */
  if(pipe(pipefd) == -1) {
    fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
    exit(1);
  }
  pid = fork();
  if(pid) {
    if (pid == -1) {
      fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
      exit(1);
    }
    close(pipefd[1]);
    if(close(0) == -1) {
      fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
      exit(1);    
    }
    if(dup(pipefd[0]) == -1) { /* set stdin for rotate_aid */
      fprintf(stderr,"rotate_aid: fatal error, could not open rotate\n");
      exit(1);    
    }
    close(pipefd[0]);
  }
  else {
    close(pipefd[0]);
    if(close(1) == -1) {
      fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
      exit(1);    
    }
    if(dup(pipefd[1]) == -1) { /* set stdin for counter */
      fprintf(stderr,"rotate_aid: fatal error, could not open rotate\n");
      exit(1);    
    }
    close(pipefd[1]);
    execvp("rotate", argv);
    fprintf(stderr,"rotate_aid : fatal error, could not open rotate\n");
    exit(1);
  }
  return(False);
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
  int i;
  unsigned nbytes;
  double alpha, beta, gamma;
  double matrix[3][3];
  char *dummy_string = "";

  nbytes = 9 * sizeof(double);

  i = parse_argument(argc,argv,0,&dummy_string,"-initial");
  if (i!=-1 &&
      sscanf(argv[++i],"%lf",&alpha) && alpha < 3.2 && alpha > -3.2 &&
      sscanf(argv[++i],"%lf",&beta) && beta < 3.2 && beta > -3.2 &&
      sscanf(argv[++i],"%lf",&gamma) && gamma < 3.2 && gamma > -3.2)
    {
      convert_euler(alpha, beta, gamma, matrix);
      if(write(1,&matrix[0][0],nbytes) != nbytes) {
	fprintf(stderr,"rotate_aid : unsecessful write\n");
	exit(1);
      }
    }

  if(open_input_device(argc, argv)) {
    dummy_string = "begin\n";
    write(3,dummy_string,6);
    while(fscanf(stdin,"%lf %lf %lf", &alpha, &beta, &gamma)==3) {
      convert_euler(alpha, beta, gamma, matrix);
      if(write(1,&matrix[0][0],nbytes) != nbytes) exit(1);
    }
  }
  else {
    dummy_string = "begin\n";
    write(3,dummy_string,6);
    while(READ(matrix) == 9) {
      if(write(1,&matrix[0][0],nbytes) != nbytes) exit(1);
    }
  }
  dummy_string = "end of file\n";
/*  while(!empty_file(0)); */
  write(3,dummy_string,12);
}
