//=================================================== file = summary1.c =====
//=  Program to compute summary statistics for a series X of size N         =
//=   - Computes min, max, sum, mean, v	ar, std dev, and cov                 =
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
//=   ---------------------------------------------- summary1.c -----       =
//=     Total of 11 values                                                  =
//=       Minimum  = 39.000000 (position = 6)                               =
//=       Maximum  = 61.000000 (position = 3)                               =
//=       Sum      = 561.000000                                             =
//=       Mean     = 51.000000                                              =
//=       Variance = 52.545455                                              =
//=       Std Dev  = 7.248824                                               =
//=       CoV      = 0.142134                                               =
//=   ---------------------------------------------------------------       =
//=-------------------------------------------------------------------------=
//=  Build: bcc32 summary1.c                                                =
//=-------------------------------------------------------------------------=
//=  Execute: summary1 < in.dat                                             =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (05/23/00) - Genesis                                      =
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
void   load_X_array(void);         // Load X array

//===========================================================================
//=  Main program                                                           =
//===========================================================================
void main(void)
{
  double   min, max, bandwidth, max_util, min_util; // Minimum and maximum values, min and max util values
  long int minpos, maxpos;           // Positions of min and max
  double   sum;                      // Sum of values
  double   mom1, mom2;               // First and second moments of values
  double   mean, mean_util, range;                     // Computed mean value
  double   var;                      // Computed variance
  double   stddev;                   // Computed standard deviation
  double   cov;                      // Computed coefficient of variation
  double   percent_95;               // 95%-tile value
  double   percent_95_util;          // 95%-tile value utilization
  int      i;                        // Loop counter

  // Load the series X
  printf("-------------------------IN-Bound Statistics------------------------\n");
  load_X_array();

  // Loop to compute min, max, sum, first moment (mean), and second moment
  min = max = X[0];
  minpos = maxpos = 0;
  sum = mom1 = mom2 = 0.0;
  bandwidth = 14000; 		// KbitsPerSec
  for (i=0; i<N; i++)
  {
    if (X[i] <= min)
    {
      min = X[i];
      minpos = i;
    }
    if (X[i] >= max)
    {
      max = X[i];
      maxpos = i;
    }
    sum = sum + X[i];
    mom1 = mom1 + (X[i] / N);
    mom2 = mom2 + (pow(X[i], 2.0) / N);
  }

  // Compute mean, variance, standard deviation, and cov
  mean = mom1;
  var = mom2 - pow(mom1, 2.0);
  stddev = sqrt(var);
  cov = sqrt(var) / mom1;
  percent_95 = mean + (stddev * 2);
  percent_95_util = (percent_95 / bandwidth) * 100;
  max_util = (max / bandwidth) * 100;
  min_util = (min / bandwidth) * 100;
  mean_util = (mean / bandwidth) * 100;
  range = max - min;

  printf("  Total of %ld values \n", N);
  printf("    Minimum  = %f (position = %ld) ", min, minpos); printf(" = %f %\n", min_util);
  printf("    Maximum  = %f (position = %ld) ", max, maxpos); printf(" = %f %\n", max_util);
  printf("    Range    = %f \n", range);
  printf("    Total = %f \n", sum);
  printf("    Mean     = %f ", mean); printf(" = %f %\n", mean_util);
  printf("    Variance = %f \n", var);
  printf("    Std Dev  = %f \n", stddev);
  printf("    CoV      = %f \n", cov);
  printf("    95%% value = %f ", percent_95); printf(" = %f %\n", percent_95_util);
  printf("---------------------------------------------------------------\n");
}

//===========================================================================#
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
