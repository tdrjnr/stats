//====================================================== file = token.c =====
//=  Program to compute utilization for Toekn Ring LAN                      =
//===========================================================================
//=  Notes: 1) NTR is Normal Token Release and ETR is Early Token Release   =
//=         2) Implements the "U" formulas from W. Stalllings, Data and     =
//=            Computer Communcations, 5th edition, Prentice-Hall, 1997.    =
//=-------------------------------------------------------------------------=
//= Example program execution:                                              =
//=                                                                         =
//=    ------------------------------------------ token.c -----             =
//=    -  Program to compute utilization for Token Ring LAN   -             =
//=    --------------------------------------------------------             =
//=    Length of ring in meters =======================> 10000              =
//=    Data rate of ring in bits per second ===========> 16000000           =
//=    Size of frames in bytes ========================> 64                 =
//=    Number of stations =============================> 100                =
//=    Token release type (1 = NTR, 2 = ETR) ==========> 1                  =
//=    --------------------------------------------------------             =
//=    - Token Ring LAN with Normal Token Release (NTR)                     =
//=    -                                                                    =
//=    -   a              = 1.562500                                        =
//=    -   U              = 64.000000 %                                     =
//=    --------------------------------------------------------             =
//=-------------------------------------------------------------------------=
//=  Build: gcc token.c -lm, bcc32 token.c, cl token.c                      =
//=-------------------------------------------------------------------------=
//=  Execute: token                                                         =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (04/23/99) - Genesis                                      =
//=           KJC (11/12/01) - Fixed an int/int division problem            =
//===========================================================================

//----- Include files -------------------------------------------------------
#include <stdio.h>           // Needed for printf() and scanf()
#include <stdlib.h>          // Needed for atoi() and atof()
#include <math.h>            // Needed for pow()

//----- Defines -------------------------------------------------------------
#define V     2.0e8          // Copper propagation in meters per second

//===== Main program ========================================================
void main(void)
{
  char     instring[80];     // Temporary input string
  double   d;                // Ring distance in meters
  double   R;                // Ring data rate in bits per second
  long int L;                // Frame length in bits
  long int N;                // Number of stations
  long int release_type;     // Token release type (1 = NTR, 2 = ETR)
  double   a;                // Stallings "a" parameter
  double   U;                // Link utilization as a ratio

  // Output banner
  printf("------------------------------------------ token.c ----- \n");
  printf("-  Program to compute utilization for Token Ring LAN   - \n");
  printf("-------------------------------------------------------- \n");

  // Prompt for distance of ring in meters
  printf("Length of ring in meters =======================> ");
  scanf("%s", instring);
  d = atof(instring);

  // Prompt for data rate of ring in bits per second
  printf("Data rate of ring in bit per second ============> ");
  scanf("%s", instring);
  R = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("Size of frames in bytes ========================> ");
  scanf("%s", instring);
  L = 8 * atoi(instring);

  // Prompt for number of stations
  printf("Number of stations =============================> ");
  scanf("%s", instring);
  N = atoi(instring);

  // Prompt for token release type
  printf("Token release type (1 = NTR, 2 = ETR) ==========> ");
  scanf("%s", instring);
  release_type = atoi(instring);

  // Compute "a" value
  a = (R * d) / ((double) V * L);

  // Compute U for NTR
  if (release_type == 1)
    if (a <= 1.0)
      U = 1.0 / (1.0 + (a / N));
    else
      U = 1.0 / (a * (1.0 + (1.0 / N)));

  // Compute U for ETR
  if (release_type == 2)
    U = 1.0 / (1.0 + (a / N));

  // Output results
  printf("-------------------------------------------------------- \n");
  printf("- Token Ring LAN with ");
  if (release_type == 1)
    printf("Normal Token Release (NTR) \n");
  else
    printf("Early Token Release (ETR) \n");
  printf("- \n");
  printf("-   a              = %f \n", a);
  printf("-   U              = %f %% \n", 100.0 * U);
  printf("-------------------------------------------------------- \n");
}
