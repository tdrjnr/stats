/*---------------------------------------------------------------------------
 * line_io.c
 * This file contains routines for parsing strings for pairs or triples of 
 * doubles and stopping when something non-numeric is reached and creating 
 * strings from lists of pairs or triples of doubles.  These are intended to 
 * be used in conjunction with gets() and puts() to create filters and 
 * programs that use this format (lists of numbers possibly followed by 
 * something starting with a non-numeric character). 
 */

#ifndef _NO_PROTO
#include <stdlib.h>
#else
#include <floatingpoint.h>
#endif

#include <strings.h>
#include <stdio.h>
#include <errno.h>

#include "line_io.h"

/*---------------------------------------------------------------------------
 * isseparator()
 * returns True if the character could be part of the separator
 * between numbers
 */
#ifndef _NO_PROTO
  int isseparator(char c)
#else
  int isseparator(c)
    char c;
#endif
{
  return( c=='\t' || c==' ' || c==',' || c==';' || c==':');
}

/*----------------------------------------------------------------------------
 * get_one_double()
 */
#ifndef _NO_PROTO
  int get_one_double(char *string, double *gotthis, char **remainder)
#else
  int get_one_double(string, gotthis, remainder)
    char *string;
    double *gotthis; 
    char **remainder;
#endif
{
  char *toparse;
  
  toparse = string;
  while (isseparator(*toparse))
    toparse++;
  
  if (strlen(toparse)> 0 && *toparse!='\n') {
    (*gotthis) = strtod(toparse, remainder);
    return(strlen(toparse)!=strlen(*remainder));
  } else {
    (*remainder) =  toparse;
    return(False);
  }
}

/*---------------------------------------------------------------------------
 * expand_array()
 */
#ifndef _NO_PROTO
  char expand_array(double **a, size_t newsize)
#else
  char expand_array(a,newsize)
    double **a;
    size_t newsize;
#endif
{
  size_t resizesize;
  
  resizesize = newsize + (newsize==0);
  
  (*a) = (double *)realloc(*a,resizesize*sizeof(double));
  return(!check_for_mem_error(*a,"line_io library array resizing"));
}


/*---------------------------------------------------------------------------
 * get_two_doubles()
 * This routine gets two doubles from stdin.  If it can only get one, it 
 * leaves it in the remainder string *afterwards and returns False, as it 
 * does when it can get no doubles.
 */
#ifndef _NO_PROTO
  char get_two_doubles(char *thestring, double *x, double *y, 
		       char **afterwards)
#else
  char get_two_doubles(thestring, x, y, afterwards)
    char *thestring; 
    double *x, *y; 
    char **afterwards;
#endif
{
  char gotx, goty, gotalltwo;
  char *parsing, *parsing_result_1, *parsing_result_2;
  
  parsing = thestring;

  gotx = get_one_double(parsing, x, &parsing_result_1);
  goty = get_one_double(parsing_result_1, y, &parsing_result_2);
  
  gotalltwo = (gotx && goty);
  
  if (gotalltwo)
    *afterwards = parsing_result_2;
  else
    *afterwards = parsing;
  
  return(gotalltwo);
}
	
/*---------------------------------------------------------------------------
 * get_three_doubles()
 * This routine gets three doubles from stdin.  If it can only get one or 
 * two, it leaves them in the remainder string *afterwards and returns False,
 * as it does when it can get no doubles.
 */
#ifndef _NO_PROTO
  char get_three_doubles(char *thestring, double *x, double *y, double *z, 
			 char **afterwards)
#else
  char get_three_doubles(thestring, x, y, z, afterwards)
    char *thestring;
    double *x, *y, *z; 
    char **afterwards;
#endif
{
  char gotx, goty, gotz, gotallthree;
  char *parsing, *parsing_result_1, *parsing_result_2, *parsing_result_3;
	
  parsing = thestring;

  gotx = get_one_double(parsing, x, &parsing_result_1);
  goty = get_one_double(parsing_result_1, y, &parsing_result_2);
  gotz = get_one_double(parsing_result_2, z, &parsing_result_3);
  
  gotallthree = (gotx && (goty && gotz));
  
  if (gotallthree)
    *afterwards = parsing_result_3;
  else
    *afterwards = parsing;
  
  return(gotallthree);
}

/*---------------------------------------------------------------------------
 * read_pairs()
 * This routine reads pairs of doubles until it gets stuck and returns a 
 * pointer to what's left of the string.  the memory for the arrays x and y 
 * is allocated in this routine.  It returns True if successful.
 */
#ifndef _NO_PROTO
  char read_pairs(char *string, double **x, double **y, int *numpairs, 
		  char **remainder)
#else 
  char read_pairs(string, x, y, numpairs, remainder)
    char *string;
    double **x, **y; 
    int *numpairs;
    char **remainder;
#endif
{
  char done;
  long currarraysize;
  char *parsefromhere, *whatisleft;
  
  done = False;
  
  parsefromhere = string;

  (*x) = (double *)malloc((size_t)CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*x,
		  "line_io library array allocation (pairs)")) return(False); 

  (*y) = (double *)malloc((size_t)CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*x,
		  "line_io library array allocation (pairs)")) return(False); 

  currarraysize = CHUNKSIZE;

  (*numpairs) = 0;

  while (!done) {
    if (get_two_doubles(parsefromhere, *x + *numpairs, *y + *numpairs, 
			&whatisleft)) 
      {
	parsefromhere = whatisleft;
	(*numpairs)++;
	if ((*numpairs)==currarraysize) {
	  if (!expand_array(x, (size_t)2*currarraysize)) return(False);
	  if (!expand_array(y, (size_t)2*currarraysize)) return(False);
	  currarraysize = 2 * currarraysize;
	}
      } else {
	done = True;
      }
  }
	
  *remainder = string + (whatisleft - string);

  /* shrink the arrays to fit */
  if (!expand_array(x,*numpairs) || !expand_array(y,*numpairs))
    return(False);
  else
    return(True);
}

/*----------------------------------------------------------------------------
 * read_binary_rgb_color()
 */
#ifndef _NO_PROTO
  char read_binary_rgb_color(FILE *thestream, XColor *thecolor)
#else
  char read_binary_rgb_color(thestream, thecolor)
    FILE *thestream;
    XColor *thecolor;
#endif
{
  size_t size_of_unsigned_short;

  size_of_unsigned_short = sizeof(unsigned short);

  if ( fread( (void *)&(thecolor->red), 
	     size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);

  if ( fread( (void *)&(thecolor->green), 
	     size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);

  if ( fread( (void *)&(thecolor->blue), 
	     size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);
  
  return(True);
}

/*---------------------------------------------------------------------------
 * write_binary_rgb_color()
 */
#ifndef _NO_PROTO
  char write_binary_rgb_color(FILE *thestream, XColor *thecolor)
#else
  char write_binary_rgb_color(thestream, thecolor)
    FILE *thestream; 
    XColor *thecolor;
#endif
{
  size_t size_of_unsigned_short;

  size_of_unsigned_short = sizeof(unsigned short);
  
  if ( fwrite( (void *)&(thecolor->red), 
	      size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);

  if ( fwrite( (void *)&(thecolor->green), 
	      size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);
  
  if ( fwrite( (void *)&(thecolor->blue), 
	      size_of_unsigned_short, 1, thestream ) != 1 )
    return(False);
  
  return(True);
}

/*----------------------------------------------------------------------------
 * read_binary_pairs()
 */
#ifndef _NO_PROTO
  char read_binary_pairs(FILE *thestream, double **x, double **y, 
			 int *numpairs)
#else
  char read_binary_pairs(thestream, x, y,numpairs)  
    FILE *thestream; 
    double **x, **y; 
    int *numpairs;
#endif
{
  size_t num_chunks, size_of_int, size_of_double;

  size_of_int = sizeof(int);

  if (fread((void *)numpairs, size_of_int, 1, thestream) != 1) {
    *numpairs = 0;
    return(False);
  }
  
  if ((*numpairs)==0)
    return(True);
  
  /* Now allocate memory as necessary.  It is always allocated in multiples 
   * of a certain chunksize since this should be most efficient
   */
  
  num_chunks = (size_t)( (double)(*numpairs)/CHUNKSIZE + 1.0 );
  (*x) = (double *)malloc(num_chunks * CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*x,
	"line_io library array allocation (binary pairs)")) return(False); 
  
  (*y) = (double *)malloc(num_chunks * CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*y,
	"line_io library array allocation (binary pairs)")) return(False); 
  
  /* read all the x's then all the y's */
  size_of_double = sizeof(double);

  if (fread((void *)*x, size_of_double, (size_t)(*numpairs), 
	    thestream) != (*numpairs)) 
    {
      (*numpairs)=0;
      return(False);
    }

  if (fread((void *)*y, size_of_double, (size_t)(*numpairs), 
	    thestream) != (*numpairs)) 
    {
      (*numpairs)=0;
      return(False);
    }
  
  /* shrink the arrays to fit */
  if (!expand_array(x,*numpairs) || !expand_array(y,*numpairs))
    return(False);
  else
    return(True);
}

/*---------------------------------------------------------------------------
 * read_binary_triples()
 */
#ifndef _NO_PROTO
  char read_binary_triples(FILE *thestream, double **x, double **y, 
			   double **z, int *numtriples)
#else 
  char read_binary_triples(thestream, x, y, z, numtriples)
    FILE *thestream; 
    double **x, **y, **z;
    int *numtriples;
#endif
{
  size_t size_of_int, size_of_double;	
  size_t num_chunks;
  
  size_of_int = sizeof(int);
  
  if (fread((void *)numtriples, size_of_int, 1, thestream) != 1) {
    *numtriples = 0;
    return(False);
  }

  if ((*numtriples)==0)
    return(True);
  
  /* Now allocate memory as necessary.  It is always allocated in multiples 
   * of a certain chunksize since this should be most efficient.
   */
  num_chunks = (size_t)( (double)(*numtriples)/CHUNKSIZE + 1.0 );
  
  (*x) = (double *)malloc(num_chunks * CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*x,
	"line_io library array allocation(binary triples)")) return(False); 
  
  (*y) = (double *)malloc(num_chunks * CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*y,
	"line_io library array allocation(binary triples)")) return(False); 

  (*z) = (double *)malloc(num_chunks * CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*z,
	"line_io library array allocation(binary triples)")) return(False); 

  /* read all the x's then all the y's then all the z's */
  size_of_double = sizeof(double);
  
  if (fread((void *)*x, size_of_double, (size_t)(*numtriples), 
	    thestream) != (*numtriples)) 
    {
      (*numtriples)=0;
      return(False);
    }

  if (fread((void *)*y, size_of_double, (size_t)(*numtriples), 
	    thestream) != (*numtriples)) 
    {
      (*numtriples)=0;
      return(False);
    }
  
  if (fread((void *)*z, size_of_double, (size_t)(*numtriples), 
	    thestream) != (*numtriples)) 
    {
      (*numtriples)=0;
      return(False);
    }

  /* shrink the arrays to fit */
  if (!expand_array(x,*numtriples) || !expand_array(y,*numtriples) || 
      !expand_array(z,*numtriples))
    return(False);
  else
    return(True);
}

/*---------------------------------------------------------------------------
 * read_triples()
 * This routine reads triples of doubles until it gets stuck and returns a 
 * pointer to what's left of the string.  The memory for the arrays x, y, 
 * and z is allocated in this routine.  It returns True if successful.
 */
#ifndef _NO_PROTO
  char read_triples(char *string, double **x, double **y, double **z, 
		    int *numtriples, char **remainder)
#else
  char read_triples(string, x, y, z, numtriples, remainder)
    char *string;
    double **x, **y, **z; 
    int *numtriples; 
    char **remainder;
#endif
{
  char done;
  long currarraysize;
  char *parsefromhere, *whatisleft;
  
  done = False;
  parsefromhere = string;

  (*x) = (double *)malloc((size_t)CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*x,
		"line_io library array allocation(triples)")) return(False); 

  (*y) = (double *)malloc((size_t)CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*y,
		"line_io library array allocation(triples)")) return(False); 

  (*z) = (double *)malloc((size_t)CHUNKSIZE * sizeof(double));
  if (check_for_mem_error( (void *)*z,
		"line_io library array allocation(triples)")) return(False); 

  currarraysize = CHUNKSIZE;
  (*numtriples) = 0;

  while (!done) {
    if (get_three_doubles(parsefromhere, *x + *numtriples, 
	*y + *numtriples, *z + *numtriples, &whatisleft)) 
      {
	parsefromhere = whatisleft;
	(*numtriples)++;
	if ((*numtriples)==currarraysize) {
	  if (!expand_array(x, (size_t)2*currarraysize)) return(False);
	  if (!expand_array(y, (size_t)2*currarraysize)) return(False);
	  if (!expand_array(z, (size_t)2*currarraysize)) return(False);
	  currarraysize = 2 * currarraysize;
	}
      } else {
	done = True;
      }
  }
	
  *remainder = string + (whatisleft - string);
  
  /* shrink the arrays to fit */
  if (!expand_array(x,*numtriples) || !expand_array(y,*numtriples) || 
      !expand_array(z,*numtriples) )
    return(False);
  else
    return(True);
}

/*---------------------------------------------------------------------------
 * check_for_mem_error()
 */
#ifndef _NO_PROTO
  char check_for_mem_error(void *checkee, char *string)
#else
  char check_for_mem_error(checkee, string)
    void *checkee; 
    char *string;
#endif
{
  char toreturn = False;

  if (checkee == NULL) {
    fprintf(stderr,"Fatal memory allocation error in %s\n",string);
    toreturn = True;
  }
  return(toreturn);
}


/*---------------------------------------------------------------------------
 * write_pairs()
 */
#ifndef _NO_PROTO
  void write_pairs(char *string, double x[], double y[], int numpairs)
#else
  void write_pairs(string, x, y, numpairs)
    char *string;
    double x[], y[]; 
    int numpairs;
#endif
{
  int i;
  char tocat[256];
  string[0] = '\0';
  
  for (i=0; i<numpairs; i++) {
    sprintf (tocat, "%g %g ",x[i],y[i]);
    strcat(string, tocat);
  }
}

/*---------------------------------------------------------------------------
 * write_binary_pairs()
 */
#ifndef _NO_PROTO
  char write_binary_pairs(FILE *thestream, double *x, double *y, 
			  int numpairs)
#else
  char write_binary_pairs(thestream, x, y, numpairs)
    FILE *thestream;
    double x[], y[]; 
    int numpairs;
#endif
{
  size_t size_of_double, size_of_int;

  size_of_int = sizeof(int);

  if (fwrite((void *)&numpairs, size_of_int, 1, thestream) != 1)
    return(False);
  
  if (numpairs==0)
    return(True);
  
  /* first write all the x's then all the y's */
  size_of_double = sizeof(double);
  
  if (fwrite((void *)x, size_of_double, (size_t)numpairs, 
	     thestream) != numpairs)
    return(False);
  
  if (fwrite((void *)y, size_of_double, (size_t)numpairs, 
	     thestream) != numpairs)
    return(False);
  
  return(True);
}


/*---------------------------------------------------------------------------
 * write_triples()
 */
#ifndef _NO_PROTO
  void write_triples(char *string, double x[], double y[], double z[], 
		     int numtriples)
#else
  void write_triples(string, x, y, z, numtriples)
    char *string;
    double x[], y[], z[]; 
    int numtriples;
#endif
{
  int i;
  char tocat[256];
  string[0] = '\0';
  
  for (i=0; i<numtriples; i++) {
    sprintf (tocat, "%g %g %g ",x[i],y[i],z[i]);
    strcat(string, tocat);
  }
}


/*---------------------------------------------------------------------------
 * write_binary_triples()
 */
#ifndef _NO_PROTO
  char write_binary_triples(FILE *thestream, double *x, double *y, 
			    double *z, int numtriples)
#else
  char write_binary_triples(thestream, x, y, z, numtriples)
    FILE *thestream; 
    double x[], y[], z[];
    int numtriples;
#endif
{
  size_t size_of_int, size_of_double;
  
  size_of_int = sizeof(int);
  
  if (fwrite((void *)&numtriples, size_of_int, 1, thestream) != 1)
    return(False);
  
  if (numtriples==0)
    return(True);
  
  /* first write all the x's then all the y's then all the z's */
  size_of_double = sizeof(double);
  
  if (fwrite((void *)x, size_of_double, (size_t)numtriples, 
	     thestream) != numtriples)
    return(False);
  
  if (fwrite((void *)y, size_of_double, (size_t)numtriples, 
	     thestream) != numtriples)
    return(False);

  if (fwrite((void *)z, size_of_double, (size_t)numtriples, 
	     thestream) != numtriples)
    return(False);
  
  return(True);
}

