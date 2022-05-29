//====================================================== file = ether.c =====
//=  Program to compute utilization for a CSMA/CD Ethernet LAN              =
//===========================================================================
//=  Notes: 1) Implements the "U" formula from W. Stalllings, Data and      =
//=            Computer Communcations, 5th edition, Prentice-Hall, 1997.    =
//=         2) If frame size is less than a slot time (2 * Tpr), a note     =
//=            will be output that the frame size is too small              =
//=-------------------------------------------------------------------------=
//= Example program execution:                                              =
//=                                                                         =
//=    ------------------------------------------ ether.c -----             =
//=    -  Program to compute utilization for Ethernet LAN     -             =
//=    --------------------------------------------------------             =
//=    Length of bus in meters ========================> 10000              =
//=    Data rate of bus in bits per second ============> 16000000           =
//=    Size of frames in bytes ========================> 64                 =
//=    Number of stations =============================> 100                =
//=    --------------------------------------------------------             =
//=    - CSMA/CD Ethernet LAN with legal frame size                         =
//=    -                                                                    =
//=    -   a              = 1.562500                                        =
//=    -   U              = 64.000000 %                                     =
//=    --------------------------------------------------------             =
//=-------------------------------------------------------------------------=
//=  Build: gcc ether.c -lm, bcc32 ether.c, cl ether.c                      =
//=-------------------------------------------------------------------------=
//=  Execute: ether                                                         =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (04/23/99) - Genesis                                      =
//===========================================================================

//----- Include files -------------------------------------------------------
#include <stdio.h>           // Needed for printf() and scanf()
#include <stdlib.h>          // Needed for atoi() and atof()
#include <math.h>            // Needed for pow() and ceil()

//----- Defines -------------------------------------------------------------
#define V     2.0e8          // Copper propagation in meters per second

//===== Main program ========================================================
void main(void)
{
  char     instring[80];     // Temporary input string
  double   d;                // Bus distance in meters
  double   R;                // Bus data rate in bits per second
  long int L;                // Frame length in bits
  long int N;                // Number of stations
  double   A;                // Probability one station xmits on a slot
  double   min_frame;        // Minimum legal frame size ibn bits (= 2 * Tprop)
  double   a;                // Stallings "a" parameter
  double   U;                // Link utilization as a ratio

  // Output banner
  printf("------------------------------------------ ether.c ----- \n");
  printf("-  Program to compute utilization for an Ethernet LAN  - \n");
  printf("-------------------------------------------------------- \n");

  // Prompt for distance of bus in meters
  printf("Length of bus in meters ========================> ");
  scanf("%s", instring);
  d = atof(instring);

  // Prompt for data rate of bus in bits per second
  printf("Data rate of bus in bit per second =============> ");
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

  // Compute "a" value
  a = ((double) R * d) / ((double) V * L);

  // Compute "A" value for Pr[a station xmits in a slot] = 1/N
  A = pow((1 - ((double) 1 / N)), (double) (N - 1));

  // Compute U
  U = (double) 1 / (1 + (2 * a * (1 - A) / A));

  // Check that frame size is greater than or equal to minimum allowed
  min_frame = (d / V) * R;

  // Output results
  printf("-------------------------------------------------------- \n");
  printf("- Ethernet LAN with ");
  if (min_frame <= L)
    printf("legal frame size \n");
  else
  {
    printf("illegal frame size \n");
    printf("-  >>> Minumum legal CSMA/CD frame size is %ld bytes \n",
      (long int) ceil(min_frame / 8));
  }
  printf("- \n");
  printf("-   a              = %f \n", a);
  printf("-   U              = %f %% \n", 100.0 * U);
  printf("-------------------------------------------------------- \n");
}
