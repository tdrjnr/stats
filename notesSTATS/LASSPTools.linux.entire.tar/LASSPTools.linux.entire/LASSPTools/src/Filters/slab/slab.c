#include<sys/poll.h>
#include<stdio.h>
#include<stdlib.h>

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
  double min, max;
  char inp_line[MAXLINELENGTH], slider_line[MAXLINELENGTH];
  char min_line[MAXLINELENGTH], max_line[MAXLINELENGTH];
  char label_line[MAXLINELENGTH];
  char slider_command[2*MAXLINELENGTH];
  int i,n,number,nslider=0;	
  int parse_list=0;
  FILE *slider;

/* the following is for non-blocking I/O - aarrrgggg */
  int fd;
  int nfds;
  unsigned long nfdsmsgs;
  struct pollfd fd_list[1];

#ifdef DEBUG
  fprintf(stderr,"Argc = %i\n",argc);
  for(i=0;i<argc;i++) {
    fprintf(stderr,"%s\n", argv[i]);
  }
  fflush(stderr);
#endif

  if(argc == 1) help();

  strcpy(min_line,"-min ");
  strcpy(max_line,"-max ");
  strcpy(label_line,"-labels ");

/* This is just a parsing fancy */
  for(i=1;i<argc;i++) {
    if( !strcmp(argv[i],"-min") ) {
      parse_list = (parse_list | MINSET);
      nslider++;
      strcat(min_line,argv[i+1]);
      strcat(min_line," ");
      sscanf(argv[i+1],"%lf",&min);
      strcat(max_line,argv[i+2]);
      strcat(max_line," ");
/*      sscanf(argv[i+2],"%lf",&(xmin[1])); */
      strcat(label_line," min ");
      i += 2;
    }
    if( !strcmp(argv[i],"-max") ) {
      parse_list = (parse_list | MAXSET);
      nslider++;
      strcat(min_line,argv[i+1]);
      strcat(min_line," ");
      sscanf(argv[i+1],"%lf",&max);
      strcat(max_line,argv[i+2]);
      strcat(max_line," ");
/*      sscanf(argv[i+2],"%lf",&(xmax[1]));*/
      strcat(label_line," max ");      
      i += 2;
    }
    if( !strcmp(argv[i],"-h") || !strcmp(argv[i],"-help") ) help();

  } /* ends for(i=1;i<argc... */

#ifdef DEBUG
  fprintf(stderr,"Parse Results:\n");
  fprintf(stderr,"Max: %s\n", max_line);
  fprintf(stderr,"Min: %s\n", min_line);
  fprintf(stderr,"Label: %s\n", label_line);
  fprintf(stderr,"Parse_list = %i\n", parse_list);
  fflush(stderr);
#endif

/* Set up the slider command. */
/*  strcpy(slider_command,"slider "); */
  sprintf(slider_command,"%s %d ","slider ", nslider);
  strcat(slider_command, min_line);
  strcat(slider_command, max_line);
  strcat(slider_command, label_line);

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
  fd_list[0].fd = fd;
  fd_list[0].revents = POLLIN;
  nfdsmsgs = 1;

  while(fgets(inp_line,MAXLINELENGTH,stdin)!=NULL) {

/* for non-blocking I/O */
    nfds = poll(fd_list,nfdsmsgs,0);
    if(nfds!=0 && nfds != -1) {
      if(fgets(slider_line,MAXLINELENGTH,slider)==NULL) break;

#ifdef DEBUG
      fprintf(stderr,"Reading slider line.\n");
      fflush(stderr);
#endif

      sscanf(slider_line,"%lf",&min);
      fgets(slider_line,MAXLINELENGTH,slider);
      sscanf(slider_line,"%lf",&max);
      if(max < min) {
	fprintf(stderr,"%s : WARNING - Min > Max.\n",argv[0]);
	fflush(stdout);
      }

/*      sscanf(slider_line,"%lf",&x0);
      fgets(slider_line,MAXLINELENGTH,slider);
      sscanf(slider_line,"%lf",&y0);
      fgets(slider_line,MAXLINELENGTH,slider);
      sscanf(slider_line,"%lf",&z0); */

    }      

    number = sscanf(inp_line,"%lg %lg %lg %i",&x,&y,&z,&n);
    switch(number) {
    case 3:
      if( z <= max && z >= min) fprintf(stdout,"%g %g %g \n",x,y,z);
      break;
    case 4:
      if( z <= max && z >= min) fprintf(stdout,"%g %g %g %i\n",x,y,z,n);
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
  fprintf(stderr,"slab usage:\n");
  fprintf(stderr,"\t slab -min min1 min2 -max max1 max2\n");
  fprintf(stderr,"\t where min1, min2 are the limits of the lower bound\n");
  fprintf(stderr,"\t and max1 and max2 are the limits of the upper bound\n");
  fprintf(stderr,"\t of the data slice.\n");
  fflush(stderr);
  exit(0);
}









