#include<stdio.h>

#define MAXLINELENGTH 100

#ifndef _NO_PROTO
  main(int argc, char **argv)
#else
  main( argc, argv)
    int argc; 
    char **argv;
#endif
{
  double x,y,z,x0,y0,z0;
  char line[MAXLINELENGTH];
  int n,number;	

  if(argc != 4 ) {
    fprintf(stderr,"%s Usage: translate x y z\n",*argv);
    fflush(stderr);
    exit(-1);
  }
  sscanf(argv[1],"%lf",&x0);
  sscanf(argv[2],"%lf",&y0);
  sscanf(argv[3],"%lf",&z0);

  while(fgets(line,MAXLINELENGTH,stdin)!=NULL) {
      number = sscanf(line,"%lg %lg %lg %i",&x,&y,&z,&n);
      switch(number) {
      case 3:
	fprintf(stdout,"%g %g %g \n",x+x0,y+y0,z+z0);
	break;
      case 4:
	fprintf(stdout,"%g %g %g %i\n",x+x0,y+y0,z+z0,n);
	break;
      default:
	fputs(line,stdout);
	fflush(stdout);
	break;
      }
  }
  return;
}












