//======================================================== file = dlc.c =====
//=  Program to compute utilization for DLC protocols                       =
//===========================================================================
//=  Notes: 1) SAW is Stop-and-Wait and SW is sliding window.  ARQ is       =
//=            SR for Selective Repeat and GBN for Go-Back-N.               =
//=         2) Implements the "U" formulas from Appenidx 7a of              =
//=            W. Stalllings, Data and Computer Communcations, 6th edition, =
//=            Prentice-Hall, 2000.                                         =
//=-------------------------------------------------------------------------=
//= Example program execution:                                              =
//=                                                                         =
//=    -------------------------------------------- dlc.c -----             =
//=    -  Program to compute utilization for DLC protocols    -             =
//=    --------------------------------------------------------             =
//=    Length of link in meters =======================> 100000             =
//=    Data rate of link in bits per second ===========> 64000              =
//=    Size of frames in bytes ========================> 250                =
//=    Pr[bit error] (0.0 to 1.0) =====================> 0.00001            =
//=    Protocol type (1 = SAW, 2 = SW) ================> 2                  =
//=    Window size ====================================> 4                  =
//=    ARQ type (1 = SR, 2 = GBN) =====================> 2                  =
//=    --------------------------------------------------------             =
//=    - Protocol is Sliding Window with Go-Back-N ARQ                      =
//=    -                                                                    =
//=    -   a              = 0.016000                                        =
//=    -   Pr[frame loss] = 0.019801                                        =
//=    -   U              = 97.957787 %                                     =
//=    --------------------------------------------------------             =
//=-------------------------------------------------------------------------=
//=  Build: gcc dlc.c -lm, bcc32 dlc.c, cl dlc.c                            =
//=-------------------------------------------------------------------------=
//=  Execute: dlc                                                           =
//=-------------------------------------------------------------------------=
//=  Author: Kenneth J. Christensen                                         =
//=          University of South Florida                                    =
//=          WWW: http://www.csee.usf.edu/~christen                         =
//=          Email: christen@csee.usf.edu                                   =
//=-------------------------------------------------------------------------=
//=  History: KJC (05/25/99) - Genesis                                      =
//=           KJC (05/15/02) - Updated reference from 5th to 6th edition    =
//=           KJC (05/15/02) - Corrected example program execution          =
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
  double   d;                // Link distance in meters
  double   R;                // Link data rate in bits per second
  long int L;                // Frame length in bits
  double   pr_bit;           // Probability of a bit error
  long int protocol_type;    // Protocol type (1 = SAW, 2 = SW)
  long int ARQ_type;         // ARQ type for SW (1 = SR, 2 = GBN)
  long int N;                // Window size for SW
  double   P;                // Probability of a frame error
  double   a;                // Stallings "a" parameter
  double   U;                // Link utilization as a ratio

  // Output banner
  printf("-------------------------------------------- dlc.c ----- \n");
  printf("-  Program to compute utilization for DLC protocols    - \n");
  printf("-------------------------------------------------------- \n");

  // Prompt for distance of link in meters
  printf("Length of link in meters =======================> ");
  scanf("%s", instring);
  d = atof(instring);

  // Prompt for data rate of link in bits per second
  printf("Data rate of link in bits per second ===========> ");
  scanf("%s", instring);
  R = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("Size of frames in bytes ========================> ");
  scanf("%s", instring);
  L = 8 * atoi(instring);

  // Prompt for error rate (BER)
  printf("Pr[bit error] (0.0 to 1.0) =====================> ");
  scanf("%s", instring);
  pr_bit = atof(instring);

  // Prompt for protocol (SAW or SW)
  printf("Protocol type (1 = SAW, 2 = SW) ================> ");
  scanf("%s", instring);
  protocol_type = atoi(instring);

  // If SW then prompt for other parameters
  if (protocol_type == 2)
  {
    // Prompt for window size
    printf("Window size ====================================> ");
    scanf("%s", instring);
    N = atoi(instring);

    // Prompt for error control (GBN or SR)
    printf("ARQ type (1 = SR, 2 = GBN) =====================> ");
    scanf("%s", instring);
    ARQ_type = atoi(instring);
  }

  // Compute P (probability of frame error)
  P = 1 - pow((1 - pr_bit), (double) L);

  // Compute "a" value
  a = ((double) R * d) / ((double) V * L);

  // Compute U for SAW
  if (protocol_type == 1)
    U = (1 - P) / (1 + (2 * a));

  // Compute U for SW and SR
  if ((protocol_type == 2) && (ARQ_type == 1))
    if (N >= ((2 * a) + 1))
      U = (1 - P);
    else
      U = (N * (1 - P)) / ((2 * a) + 1);

  // Compute U for SW and GBN
  if ((protocol_type == 2) && (ARQ_type == 2))
    if (N >= ((2 * a) + 1))
      U = (1 - P) / (1 + (2 * a * P));
    else
      U = (N * (1 - P)) / (((2 * a) + 1) * (1 - P + N * P));

  // Output results
  printf("-------------------------------------------------------- \n");
  printf("- Protocol is ");
  if (protocol_type == 1)
    printf("Stop-and-Wait \n");
  else
  {
    printf("Sliding Window with ");
    if (ARQ_type == 1)
      printf("Selective Reject ARQ \n");
    else
      printf("Go-Back-N ARQ \n");
  }
  printf("- \n");
  printf("-   a              = %f \n", a);
  printf("-   Pr[frame loss] = %f \n", P);
  printf("-   U              = %f %% \n", 100.0 * U);
  printf("-------------------------------------------------------- \n");
}
