//=================================================== file = summary2.c =====
//=  Program to compute summary statistics for a series X of size N         =
//=   - Computes median, 1%, 2%, 5%, 95%, 98%, and 99%                      =
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
//=    50                                                                   =
//=    42                                                                   =
//=    48                                                                   =
//=    61                                                                   =
//=    60                                                                   =
//=    53                                                                   =
//=    39                                                                   =
//=    54                                                                   =
//=    42                                                                   =
//=    59                                                                   =
//=    53                                                                   =
//=-------------------------------------------------------------------------=
//= Example output (for above "in.dat"):                                    =
//=                                                                         =
//=   ---------------------------------------------- summary2.c -----       =
//=     Total of 11 values                                                  =
//=       Median    = 53.000000                                             =
//=        1% value = 39.000000                                             =
//=        2% value = 39.000000                                             =
//=        5% value = 39.000000                                             =
//=       95% value = 61.000000                                             =
//=       98% value = 61.000000                                             =
//=       99% value = 61.000000                                             =
//=   ---------------------------------------------------------------       =
//=-------------------------------------------------------------------------=
//=  Build: bcc32 summary2.c                                                =
//=-------------------------------------------------------------------------=
//=  Execute: summary2 < in.dat                                             =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (05/23/00) - Genesis                                      =
//=           KJC (10/02/00) - Fixed cast problem with array indexes        =
//=           KJC (06/06/02) - Changed MAX_SIZE to 2 million                =
//=           KJC (08/26/03) - Median is computed correctly for odd even    =
//===========================================================================

//----- Include files -------------------------------------------------------
#include <stdio.h>                 // Needed for printf() and feof()
#include <stdlib.h>                // Needed for exit() and atof()
#include <string.h>                // Needed for strcmp()
#include <math.h>                  // Needed for pow()

//----- Defines -------------------------------------------------------------
#define MAX_SIZE 2000000           // Maximum size of time series data array

//----- Globals -------------------------------------------------------------
double     X[MAX_SIZE];            // Time series read from "in.dat"
int        N;                      // Number of values in "in.dat"

//----- Function prototypes -------------------------------------------------
void load_X_array(void);                    // Load X array
int  comp(const void *p, const void *q);    // Compare p and q for qsort()

//===========================================================================
//=  Main program                                                           =
//===========================================================================
void main(void)
{
  double   median, bandwidth;                   // Median and Bandwidth value
  double   percent_1;                // 1%-tile value
  double   percent_2;                // 2%-tile value
  double   percent_5;                // 5%-tile value
  double   percent_95;               // 95%-tile value
  double   percent_98;               // 98%-tile value
  double   percent_99;               // 99%-tile value
  double   median_util;                   // Median value utilization
  double   percent_1_util;                // 1%-tile value utilization
  double   percent_2_util;                // 2%-tile value utilization
  double   percent_5_util;                // 5%-tile value utilization
  double   percent_95_util;               // 95%-tile value utilization
  double   percent_98_util;               // 98%-tile value utilization
  double   percent_99_util;               // 99%-tile value utilization
  int      i;                        // Loop counter

  // Load the series X
  printf("-------------------------OUT-Bound Statistics PERCENTILE------------------------\n");
  load_X_array();
  bandwidth = 14000; 		// KbitsPerSec

  // Sort the series X
  qsort(X, N, sizeof(double), comp);

  // Computes median precisely
  if ((N % 2) == 1)
    median = X[N / 2];
  else
    median = 0.5 * (X[(N - 1) / 2] + X[(N + 1) / 2]);

  // Compute percentiles with a round-down of index
  percent_1 = X[(int) (0.01 * N)];
  percent_2 = X[(int) (0.02 * N)];
  percent_5 = X[(int) (0.05 * N)];
  percent_95 = X[(int) (0.95 * N)];
  percent_98 = X[(int) (0.98 * N)];
  percent_99 = X[(int) (0.99 * N)];
  median_util = (median / bandwidth) * 100;
  percent_1_util = (percent_1 / bandwidth) * 100;
  percent_2_util = (percent_2 / bandwidth) * 100;
  percent_5_util = (percent_5 / bandwidth) * 100;
  percent_95_util = (percent_95 / bandwidth) * 100;
  percent_98_util = (percent_98 / bandwidth) * 100;
  percent_99_util = (percent_99 / bandwidth) * 100;


  printf("  Total of %ld values \n", N);
  printf("    Median    = %f ", median); printf(" = %f %\n", median_util);
  printf("    1%% value = %f ", percent_1); printf(" = %f %\n", percent_1_util);
  printf("    2%% value = %f ", percent_2); printf(" = %f %\n", percent_2_util);
  printf("    5%% value = %f ", percent_5); printf(" = %f %\n", percent_5_util);
  printf("    95%% value = %f ", percent_95); printf(" = %f %\n", percent_95_util);
  printf("    98%% value = %f ", percent_98); printf(" = %f %\n", percent_98_util);
  printf("    99%% value = %f ", percent_99); printf(" = %f %\n", percent_99_util);
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

  if (*ptr1 < *ptr2)
    return -1;
  else if (*ptr1 == *ptr2)
    return 0;
  else
    return 1;
}


