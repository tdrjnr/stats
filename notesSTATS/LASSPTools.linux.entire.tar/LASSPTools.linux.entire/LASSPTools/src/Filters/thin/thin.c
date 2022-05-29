#include<stdio.h>

#ifndef _NO_PROTO
#include<stdlib.h>
#endif

#include<string.h>

#define ALLWHITE      1
#define NOTWHITE      0
#define NEWFRAME      1
#define MAXLINELENGTH 256

#ifndef _NO_PROTO
  void help(char *name);
  int whitespace(char xypositions[]); 
#else
  void help();
  int whitespace();
#endif

#ifndef _NO_PROTO
  main(int argc,char *argv[])
#else
  main(argc,argv)
    int argc;
    char *argv[];
#endif
{
  char line[MAXLINELENGTH];
  int thin,count=0;

  if(argc==1) {
    help(argv[0]);
    exit(-1);
  }
  thin = atoi(*++argv);
  if(thin<2) {
    help("thin");
    exit(-1);
  }
  while( (fgets(line,MAXLINELENGTH,stdin))!= NULL) {
          if(whitespace(line)!=NEWFRAME) {
	    if(count==thin) {
	      count=0;
	      fprintf(stdout,"\n");
	      fflush(stdout);
	    }
	    if(count==0) 
	      fprintf(stdout,"%s",line);
	      fflush(stdout);
	  }
	  else
	    ++count;
	}
}

#ifndef _NO_PROTO
  void help(char *name)
#else
  void help(name)
    char *name;
#endif
{
  fprintf(stderr,"%s: Usage: thin n takes every nth block of stdin and writes it to stdout.(n>1)\n",name);
  return;
}
