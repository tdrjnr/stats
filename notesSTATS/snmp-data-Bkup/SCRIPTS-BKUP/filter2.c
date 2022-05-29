//===================================---============== file = filter2.c =====
//=  Program to filter a time series X of size N                            =
//=   - Filters "in" between minimum and maximum values                     =
//===========================================================================
//=  Notes:                                                                 =
//=    1) Input from input file "in.dat" to stdin (see example below)       =
//=        * Comments are bounded by "&" characters at the beginning and    =
//=          end of the comment block                                       =
//=    2) Output is to stdout                                               =
//=    3) Must manually set MIN_VALUE and MAX_VALUE                         =
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
//=                                                                         =
//=-------------------------------------------------------------------------=
//= Example output (for above "in.dat", MIN_VALUE = 1.0, MAX_VALUE = 9.0)   =
//=                                                                         =
//=  ----------------------------------------------- filter2.c -----        =
//=    Filter in for MIN_VALUE = 1.000000 and MAX_VALUE = 9.000000          =
//=        1 -- 2.900000                                                    =
//=        2 -- 8.900000                                                    =
//=        3 -- 1.500000                                                    =
//=        5 -- 3.100000                                                    =
//=        6 -- 9.000000                                                    =
//=        7 -- 4.000000                                                    =
//=        9 -- 9.000000                                                    =
//=  ---------------------------------------------------------------        =
//=-------------------------------------------------------------------------=
//=  Build: gcc filter2.c, bcc32 filter2.c, cl filter2.c                    =
//=-------------------------------------------------------------------------=
//=  Execute: filter2 < in.dat                                              =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (05/11/00) - Genesis                                      =
//===========================================================================

//----- Include files -------------------------------------------------------
#include <stdio.h>                 // Needed for printf() and feof()
#include <stdlib.h>                // Needed for exit() and atof()
#include <string.h>                // Needed for strcmp()

//----- Defines -------------------------------------------------------------
#define MAX_SIZE      1000000L     // Maximum size of time series data array
#define MIN_VALUE          15400     // Minimum value of notch
#define MAX_VALUE          15400   // Maximum value of notch

//----- Globals -------------------------------------------------------------
double     X[MAX_SIZE];            // Time series read from "in.dat"
long int   N;                      // Number of values in "in.dat"

//----- Function prototypes -------------------------------------------------
void   load_X_array(void);         // Load X array

//===========================================================================
//=  Main program                                                           =
//===========================================================================
void main(void)
{
  long int i;                        // Loop counter

  // Load the series X
  printf("----------------------------------------------- filter2.c -----\n");
  load_X_array();

  // Output MIN_VALUE and MAX_VALUE
  printf("  Filter in for MIN_VALUE = %f and MAX_VALUE = %f \n",
    MIN_VALUE, MAX_VALUE);

  // Do the filtering
  for (i=0; i<N; i++)
    if ((X[i] >= MIN_VALUE) && (X[i] <= MAX_VALUE))
      printf("  %5d -- %f \n", i, X[i]);
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
