/* This is lassptools slab modified to take the slab in an arbitrary direction
  and to have a generous initial slab thickness */
#include <sys/types.h>
#include<sys/poll.h>
#include<stdio.h>
#ifdef LINUX
#include<stdlib.h>
#endif
#include<sys/stat.h>
#include<math.h>

#define MAXLINELENGTH 150
#define MINSET  (1 << 0)
#define MAXSET  (1 << 1)

void help();

#ifndef _NO_PROTO
  main(int argc, char **argv)
#else
  main( argc, argv)
    int argc; 
    char **argv;
#endif
{
  double x,y,z;
  double min, max, temp;
  char inp_line[MAXLINELENGTH], slider_line[MAXLINELENGTH];
  char slider_command[2*MAXLINELENGTH];
  int i,n,number,nslider=0;	
  FILE *slider , *rotate;
  struct stat sb ;

  int rot=0, project=0 ;
  double rotx, roty, rotz ;

/* the following is for non-blocking I/O - aarrrgggg */
  int fd;
  int nfds;
  unsigned long nfdsmsgs;

  size_t small=1;

#ifdef DEBUG
  fprintf(stderr,"Argc = %i\n",argc);
  for(i=0;i<argc;i++) {
    fprintf(stderr,"%d %s\n",i, argv[i]);
  }
  fflush(stderr);
#endif

/* make stdout have a really small buffer */
   setvbuf(stdout, NULL, _IOFBF, small) ;

  
  if(argc == 1) help();

/* first two arguments are min and max of slab */
    if( (sscanf(argv[1],"%lf",&min) == EOF) || 
        (sscanf(argv[2],"%lf",&max) == EOF)) help() ;
    nslider = 2;

  for(i=3;i<argc;i++) {
    if( !strcmp(argv[i],"-rotate") ) {
      rot = 1; nslider += 3; rotx = 0.0; roty = 0.0 ; rotz = 1.0 ;
    }
    if( !strcmp(argv[i],"-project") ) {
      project = 1; 
    }
    if( !strcmp(argv[i],"-h") || !strcmp(argv[i],"-help") ) help();
  }

/* Set up the slider command. */
  if(rot == 0) 
    sprintf(slider_command,
      "slider %d -title arbslab -min %g %g -max %g %g -initial %g %g -labels min max\n",
         nslider, min, min, max, max, min, max);
  else
    sprintf(slider_command,
      "slider %d -title arbslab -min %g %g -1 -1 -1 -max %g %g 1 1 1 -initial %g %g 0 0 1 -labels min max x y z \n",
         nslider, min, min, max, max, min, max);

#ifdef DEBUG
  fprintf(stderr,"Slider command: %s\n", slider_command);
  fflush(stderr);
#endif
  
  if((slider = popen(slider_command,"r"))==NULL) {
    fprintf(stderr,"%s: Error opening pipe to slider.\n",argv[0]);
    fflush(stderr);
  }

/* set-up for poll subroutine */
  fd = fileno(slider);

  while(fgets(inp_line,MAXLINELENGTH,stdin)!=NULL) {

     if(fstat(fd,&sb) == 1) fprintf(stderr,"error in fstat\n") ;
     else{
       while (sb.st_size > 0){
/* slider has new input, read new min and max until all input is read */
         if(fgets(slider_line,MAXLINELENGTH,slider)==NULL) break;
         sscanf(slider_line,"%lf",&min);
         fgets(slider_line,MAXLINELENGTH,slider);
         sscanf(slider_line,"%lf",&max);
	 if(rot > 0) {  /* get new unit normal */
            fgets(slider_line,MAXLINELENGTH,slider);
            sscanf(slider_line,"%lf",&rotx);
            fgets(slider_line,MAXLINELENGTH,slider);
            sscanf(slider_line,"%lf",&roty);
            fgets(slider_line,MAXLINELENGTH,slider);
            sscanf(slider_line,"%lf",&rotz);
	    temp = 1.0 / sqrt(rotx * rotx + roty * roty + rotz * rotz) ;
	    rotx *= temp ; roty *= temp ; rotz *= temp ;
#ifdef DEBUG
      fprintf(stderr,"rotx roty rotz %g  %g  %g\n",rotx, roty, rotz);
      fflush(stderr);
#endif
	 }

	 if(fstat(fd,&sb) == 1) fprintf(stderr,"error in fstat\n") ;
       }

       if(max < min) {
	  fprintf(stderr,"%s : WARNING - Min > Max.\n",argv[0]);
           fflush(stdout);
       }
     }

    number = sscanf(inp_line,"%lg %lg %lg %i",&x,&y,&z,&n);
    switch(number) {
    case 3:
      if(rot > 0)
         temp = rotx * x + roty * y + rotz * z ;
      else  temp = z ;
      if(project> 0) {
        if ((temp <= max) && (temp >= min))
	   fprintf(stdout,"%g %g \n",x,y);
      } else
        if((temp <= max) && (temp >= min)) 
	   fprintf(stdout,"%g %g %g \n",x,y,z);

      fflush(stdout);
      break;
    case 4:
      if(rot > 0)
         temp = rotx * x + roty * y + rotz * z ;
      else  temp = z ;
      if(project> 0) {
        if ((temp <= max) && (temp >= min))
	   fprintf(stdout,"%g %g %i\n",x,y,n);
      } else
        if ((temp <= max) && (temp >= min))
	  fprintf(stdout,"%g %g %g %i\n",x,y,z,n);

      fflush(stdout);
      break;
    default:
      fputs(inp_line,stdout);
      fflush(stdout);
      break;
    }
  }

  pclose(slider);
  return;

}


void help()
{
  fprintf(stderr,"arbslab usage:\n");
  fprintf(stderr,"\t arbslab min max [-rotate] [-project]\n");
  fflush(stderr);
  exit(0);
}











