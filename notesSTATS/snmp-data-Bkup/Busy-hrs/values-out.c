//===================================---============== file = filter1.c =====
//=  Program to filter a time series X of size N                            =
//=   - Filters "out" above and below minimum and maximum values            =
//===========================================================================
//=  Notes:                                                                 =
//=    1) Input from input file "in.dat" to stdin (see example below)       =
//=        * Comments are bounded by "&" characters at the beginning and    =
//=          end of the comment block                                       =
//=    2) Output is to stdout                                               =
//=    3) Must manually set MIN_VALUE_THRESHOLD and MAX_VALUE_THRESHOLD                         =
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
//= Example output (for above "in.dat", MIN_VALUE_THRESHOLD = 1.0, MAX_VALUE_THRESHOLD = 9.0)   =
//=                                                                         =
//=   ----------------------------------------------- filter1.c -----       =
//=     Filter out for MIN_VALUE_THRESHOLD = 1.000000 and MAX_VALUE_THRESHOLD = 9.000000        =
//=     MIN -->     0 -- 0.000000                                           =
//=     MIN -->     4 -- 0.500000                                           =
//=     MAX -->     8 -- 11.500000                                          =
//=     MIN -->    10 -- 0.000000                                           =
//=   ---------------------------------------------------------------       =
//=-------------------------------------------------------------------------=
//=  Build: gcc filter1.c, bcc32 filter1.c, cl filter1.c                    =
//=-------------------------------------------------------------------------=
//=  Execute: filter1 < in.dat                                              =
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
#define MAX_SIZE      10000000L     // Maximum size of time series data array
#define MIN_VALUE_THRESHOLD    9800   // Minimum value of notch
#define MAX_VALUE_THRESHOLD    9800  // Maximum value of notch

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
  printf("----------------------------------------------- OUTBOUND Traffic -----\n");
  load_X_array();

  // Output MIN_VALUE_THRESHOLD and MAX_VALUE_THRESHOLD
  printf("  Filter out OUTBOUND Traffic for THRESHOLD = 70 perc and MAX_VALUE_THRESHOLD = 70 perc \n",
    MIN_VALUE_THRESHOLD, MAX_VALUE_THRESHOLD);

  // Do the filtering
  for (i=0; i<N; i++)
  {
    if (X[i] < MIN_VALUE_THRESHOLD)
      printf("  MIN-Below-THRESHOLD --> %5d -- %f Kbits/sec \n", i, X[i]);
    if (X[i] > MAX_VALUE_THRESHOLD)
      printf("  MAX-Above-THRESHOLD --> %5d -- %f Kbits/sec \n", i, X[i]);
  }
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

