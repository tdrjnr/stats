/*-----------------------------------------------------------------------------
 * line_io.h
 */

#include <stdio.h>
#include <X11/Xlib.h>

/* CHUNKSIZE is the size in number of tuples to initially allocate and double
 * if it's not enough
 */
#define CHUNKSIZE 20

#ifndef _NO_PROTO
  char read_pairs(char *string, double **x, double **y, int *num, 
		  char **remainder);
  char read_triples(char *string, double **x, double **y, double **z, 
		    int *num, char **remainder);
  void write_pairs(char *string, double x[], double y[], int numpairs);
  void write_triples(char *string, double x[], double y[], double z[], 
		     int numpairs);

  char read_binary_pairs(FILE *thestream, double **x, double **y, int *num);
  char read_binary_triples(FILE *, double **, double **, double **, int *);
  char write_binary_pairs(FILE *, double *, double *, int);
  char write_binary_triples(FILE *, double *, double *, double *, 
			    int numpairs);

  char read_binary_rgb_color(FILE *, XColor *);
  char write_binary_rgb_color(FILE *, XColor *);

  char check_for_mem_error(void *, char *);
#else
  char read_pairs();
  char read_triples(); 
  void write_pairs();
  void write_triples();

  char read_binary_pairs();
  char read_binary_triples();
  char write_binary_pairs();
  char write_binary_triples();

  char read_binary_rgb_color();
  char write_binary_rgb_color();

  char check_for_mem_error();
#endif
