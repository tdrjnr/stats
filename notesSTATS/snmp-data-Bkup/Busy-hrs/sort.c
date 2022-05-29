//======================================================= file = sort.c =====
//=  Program to sort a series X of size N                                   =
//===========================================================================
//=  Notes:                                                                 =
//=    1) Input from input file "in.dat" to stdin (see example below)       =
//=        * Comments are bounded by "&" characters at the beginning and    =
//=          end of the comment block                                       =
//=    2) Output is to stdout                                               =
//=-------------------------------------------------------------------------=
//= Example "in.dat" file:                                                  =
//=                                                                         =
//=    & Sample series of data which can be integers or reals.              =
//=      There are 11 values in this file. &                                =
//=     0.0                                                                 =
//=     2.9                                                                 =
//=     8.9                                                                 =
//=     1.5                                                                 =
//=     0.5                                                                 =
//=     3.1                                                                 =
//=     9.0                                                                 =
//=     4.0                                                                 =
//=    11.5                                                                 =
//=     9.0                                                                 =
//=     0.0                                                                 =
//=-------------------------------------------------------------------------=
//= Example output (for above "in.dat")                                     =
//=                                                                         =
//=   -------------------------------------------------- sort.c -----       =
//=   Sorted 11 double values...                                            =
//=     0.000000                                                            =
//=     0.000000                                                            =
//=     0.500000                                                            =
//=     1.500000                                                            =
//=     2.900000                                                            =
//=     3.100000                                                            =
//=     4.000000                                                            =
//=     8.900000                                                            =
//=     9.000000                                                            =
//=     9.000000                                                            =
//=     11.500000                                                           =
//=   ---------------------------------------------------------------       =
//=-------------------------------------------------------------------------=
//=  Build: gcc sort.c, bcc32 sort.c, cl sort.c                             =
//=-------------------------------------------------------------------------=
//=  Execute: sort < in.dat                                                 =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (05/04/00) - Genesis                                      =
//===========================================================================

//----- Include files -------------------------------------------------------
#include <stdio.h>                 // Needed for printf() and feof()
#include <stdlib.h>                // Needed for exit(), atof(), and qsort()
#include <string.h>                // Needed for strcmp()

//----- Defines -------------------------------------------------------------
#define MAX_SIZE 1000000L          // Maximum size of time series data array

//----- Globals -------------------------------------------------------------
double     X[MAX_SIZE];            // Time series read from "in.dat"
long int   N;                      // Number of values in "in.dat"

//----- Function prototypes -------------------------------------------------
void load_X_array(void);                    // Load X array
int  comp(const void *p, const void *q);    // Compare p and q for qsort()

//===========================================================================
//=  Main program                                                           =
//===========================================================================
void main(void)
{
  long int i, j;                     // Loop counters
  long int count;                    // Frequency counter for this bucket

  // Load the series X
  printf("-------------------------------------------------- sort.c -----\n");
  load_X_array();

  // Sort the series X
  qsort(X, N, sizeof(double), comp);

  // Output the sorted series
  printf("Sorted %d double values... \n", N);
  for (i=0; i<N; i++)
    printf("  %f \n", X[i]);
  printf("---------------------------------------------------------------\n");
}

//===========================================================================
//=  Function to load X array from stdin and determine N                    =
//===========================================================================
void load_X_array(void)
{
  char      temp_string[1024];     // Temporary string variable

  // Read all values into X
  N = 0;
  while(1)
  {
    scanf("%s", temp_string);
    if (feof(stdin)) goto end;

    // This handles a comment bounded by "&" symbols
    while (strcmp(temp_string, "&") == 0)
    {
      do
      {
        scanf("%s", temp_string);
        if (feof(stdin)) goto end;
      } while (strcmp(temp_string, "&") != 0);
      scanf("%s", temp_string);
      if (feof(stdin)) goto end;
    }

    // Enter value in array and increment array index
    X[N] = atof(temp_string);
    N++;

    // Check if MAX_SIZE data values exceeded
    if (N >= MAX_SIZE)
    {
      printf("*** ERROR - greater than %ld data values \n", MAX_SIZE);
      exit(1);
    }
  }

  // End-of-file escape
  end:

  return;
}

//===========================================================================
//=  Function to compare two aruguments in an array of double               =
//=   - Needed by qsort()                                                   =
//===========================================================================
int  comp(const void *p, const void *q)
{
  double *ptr1 = (double *)(p);
  double *ptr2 = (double *)(q);

  if (*ptr1 < *ptr2) return -1;
  else if (*ptr1 == *ptr2) return 0;
  else return 1;
}

