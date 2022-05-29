/******************************
*         branch.c            *
*  The exciting new companion *
* to tee!  Now you can enjoy  *
* unprecedented control over  *
* the destination of your     *
* data!                       *
*                             *
* (branch takes a quoted pipe *
* list as its first argument, *
* opens a pipe to the quoted  *
* argument, and echoes stdin  *
* both to the pipe and to     *
* stdout.)                    *
******************************/

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

#ifndef _NO_PROTO
  main(unsigned int argc, char **argv)
#else
  main(argc, argv)
    unsigned int argc; 
    char **argv;
#endif
{
  int  c;
  char *pipe_text;
  FILE *pipe;
  
  if (argc<2) {
    fprintf(stderr,"branch: Usage: branch \"command\" splits stdin into two output streams, \nand sends on of them to stdout and one to \"command\", which branch runs.\n");
    exit(0);
  } 

  pipe_text=strtok(argv[1],"\"");
  
  if (pipe_text[0]=='|') pipe_text++; /* strip leading pipe symbols  */
  
  pipe=popen(pipe_text,"w");
  
  if (pipe==NULL){
    fprintf(stderr,"branch: cannot open named pipe");
    exit(0);
  }
  
/*  fprintf(stderr,"pipe opened.\n"); */
  while ((c=getc(stdin))!=EOF){
    putc((char)c,pipe);
    putc((char)c,stdout);
    fflush(stdout);
    fflush(pipe);
  }
  
  pclose(pipe);
}


