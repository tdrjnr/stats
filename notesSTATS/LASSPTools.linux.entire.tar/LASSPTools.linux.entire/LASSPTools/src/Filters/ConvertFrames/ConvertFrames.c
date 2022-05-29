#include <sys/types.h>
#include <sys/stat.h>
#include "ConvertFrames.h"

/* ouput_frame */
char two_D_out = True;

/* main */
char binary = True;

/* get_frame */
char long_remains = False;
char movieaxis_mode = False;
char pass_comments = False;
char pass_text = True;


/*-----------------------------------------------------------------------------
 * getRGB()
 * this routine parses a string of the form "rgb 65535 0 0" into red, green,
 * and blue components which are stored in the corresponding fields of the
 * passed XColor.
 */
#ifndef _NO_PROTO
int getRGB(char *thestring, XColor *thecolor)
#else
int getRGB(thestring, thecolor)
char *thestring;
XColor *thecolor;
#endif
{
  int red, green, blue;
  int numgot;
  char dummy[255];
  
  numgot = sscanf(thestring, "%s %d %d %d", dummy, &red, &green, &blue);
  
  if (numgot!=4) {
    return(False);
  } else {
    thecolor->red = (unsigned short)red;
    thecolor->green = (unsigned short)green;
    thecolor->blue = (unsigned short)blue;
    return(True);
  }
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
  int i;

  F.num = (int *)malloc((size_t)INITIALPOLY * sizeof(int));
  F.ordering = (int *)malloc((size_t)INITIALPOLY * sizeof(int));
  F.Act[X] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.Act[Y] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.Act[Z] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.Rot[X] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.Rot[Y] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.Rot[Z] = (double **)malloc((size_t)INITIALPOLY * sizeof(double *));
  F.color = (XColor *)malloc((size_t)INITIALPOLY * sizeof(XColor));
  F.remains = (char **)malloc((size_t)INITIALPOLY * sizeof(char *));
  
  if(F.num == NULL || F.ordering == NULL ||
     F.Act[X] == NULL || F.Act[Y] == NULL || F.Act[Z] == NULL ||
     F.Rot[X] == NULL || F.Rot[Y] == NULL || F.Rot[Z] == NULL ||
     F.color == NULL || F.remains == NULL)
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
    F.remains[i] = NULL;
  }
  if(!long_remains)
    for(i=0; i<INITIALPOLY; i++) {
      F.remains[i] = (char *)malloc(INITIALTEXT * sizeof(char));
      if(F.remains[i] == NULL)
	fatal("memory error in initialize_frame");
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
  int i, j;
  char blocking;
  char input[BUFSIZ +1];
  char *dummy_string = "";
  frame F;

  F = initialize_frame(F);

  i = parse_argument(argc,argv,0,&dummy_string,"-p");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-project");
  if (i!=-1)
    two_D_out = True;
  else
    two_D_out = False;

  i = parse_argument(argc,argv,0,&dummy_string,"-b");
  if (i==-1)
    i = parse_argument(argc,argv,0,&dummy_string,"-binary");
  if (i!=-1)
    binary = True;
  else
    binary = False;
  
  blocking = True;
  i = 0;
  if(binary) 
    while((i = get_binary_frame(&F, blocking)) != EOF) {
      if(i)
	for(j=0; j<F.numpoly; j++) {
	  if(two_D_out)
	    write_pairs(input,F.Act[X][j],F.Act[Y][j],F.num[j]);
	  else
	    write_triples(input,F.Act[X][j],F.Act[Y][j],F.Act[Z][j],F.num[j]);
	  fputs(input,stdout);
	  fprintf(stdout, " rgb %d %d %d\n",F.color[j].red,F.color[j].green,
		  F.color[j].blue);
	}
      fprintf(stdout,"\n");
    }
  else
    while((i = get_frame(&F, input, blocking)) != EOF) {
      if(i)
	for(j=0; j<F.numpoly; j++) {
	  getRGB(F.remains[j],&(F.color[j]));
	  if(two_D_out)
	    write_binary_pairs(stdout,F.Act[X][j],F.Act[Y][j],F.num[j]);
	  else
	    write_binary_triples(stdout,F.Act[X][j],F.Act[Y][j],
				 F.Act[Z][j],F.num[j]);
	  write_binary_rgb_color(stdout,&(F.color[j]));
	}
      j = 0;
      fwrite((void *)&j, sizeof(int), (size_t)1, stdout);
    }
}
