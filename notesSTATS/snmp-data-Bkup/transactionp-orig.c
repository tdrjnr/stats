//====================================================== file = ether.c =====
//=  Program to compute Utilisation for a CSMA/CD Ethernet LAN              =
//===========================================================================
//=  Notes: 1) Implements the "U" formula from W. Stalllings, Data and      =
//=            Computer Communcations, 5th edition, Prentice-Hall, 1997.    =
//=         2) If frame size is less than a slot time (2 * Tpr), a note     =
//=            will be output that the frame size is too small              =
//=-------------------------------------------------------------------------=
//= Example program execution:                                              =
//=                                                                         =
//=    ------------------------------------------ ether.c -----             =
//=    -  Program to compute Utilisation for Ethernet LAN     -             =
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
#define LOCATION    Seun

//===== Main program ========================================================
void main(void)
{
  char     instring[80];     // Temporary input string
  double   Trans_type;                // Bus distance in meters
  long int Size_Trans;                // Frame length in bits
  double   Num_Users;                // Bus distance in meters
  double   Peak_Num_Trans_hr;                // Bus data rate in bits per second
  long int Bandwidth;                // Number of stations
  double   Util_value;
  double   Total_Trans_hr;                // Probability one station xmits on a slot
  double   Total_Trans_sec;
  double   Total_Data_Kbytes_hr;                // Probability one station xmits on a slot
  double   Total_Data_Kbits_hr;                // robability one station xmits on a slot
  double   Total_Data_Kbits_sec;                // Probability one station xmits on a slot
  double   Total_plus_overhead;                // Probability one station xmits on a slot
  double   Utilisation;                // Probability one station xmits on a slot
  double   Utilisation_plus_curr_util;
  double   Growth_perc;                // Probability one station xmits on a slot
  double   Growth_Total_Trans_hr;
  double   Growth_Total_Trans_sec;
  double   Growth_Total_Data_Kbytes_hr;                // Probability one station xmits on a slot
  double   Growth_Total_Data_Kbits_hr;
  double   Growth_Total_Data_Kbits_sec;
  double   Growth_Total_plus_overhead;
  double   Growth_Utilisation;



  // Output banner
  printf("------------------------------------------ ether.c ----- \n");
  printf("-  Program to compute CAPACITY requirements for TRANSACTION   - \n");
  printf("-------------------------------------------------------- \n");

  // Prompt for distance of bus in meters
  printf("Number of users/workers ========================> ");
  scanf("%s", instring);
  Num_Users = atof(instring);

  // Prompt for data rate of bus in bits per second
  printf("Number of transactions at (PEAK time) =============> ");
  scanf("%s", instring);
  Peak_Num_Trans_hr = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("Bandwidth of SITE in bits/sec ========================> ");
  scanf("%s", instring);
  Bandwidth = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("Percentile/Max/Average/Median Utilisation in KBits/sec ========================> ");
  scanf("%s", instring);
  Util_value = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("MAX Size of transactions in bytes ========================> ");
  scanf("%s", instring);
  Size_Trans = atof(instring);

  // Prompt for size of frames in bytes (converted to bits for L)
  printf("Transaction projected growth in percentage(%) ========================> ");
  scanf("%s", instring);
  Growth_perc = atof(instring);


  // Compute  "a" value
  Total_Trans_hr = Num_Users * Peak_Num_Trans_hr;

  // Compute  "a" value
  Total_Trans_sec = Total_Trans_hr / 3600;

 // Compute value
  Total_Data_Kbytes_hr = (Total_Trans_sec * Size_Trans) / 1000;
 
 // Compute value
  Total_Data_Kbits_hr = (Total_Trans_hr * Size_Trans) * 8 / 1000;

 // Compute value
  Total_Data_Kbits_sec = Total_Data_Kbits_hr / 3600;

 // Compute value
  Total_plus_overhead = Total_Data_Kbits_sec * 1.2 * 1.1;

 // Compute value
  Utilisation = (Total_plus_overhead * 1000) / Bandwidth * 100;

 // Compute value
  Utilisation_plus_curr_util = Utilisation + (Util_value * 1000 / Bandwidth * 100);

 // Compute value
  Utilisation_plus_curr_util = Utilisation + (Util_value * 1000 / Bandwidth * 100);

 // Compute value
  Growth_Total_Trans_hr = (Growth_perc * Total_Trans_hr / 100) + Total_Trans_hr;

 // Compute value
  Growth_Total_Data_Kbits_hr = (Growth_Total_Data_Kbytes_hr * Total_Trans_hr / 100) + Total_Trans_hr;


  // Output results
  printf("-------------------------------------------------------- \n");
  printf("- CURRENT Transaction profile RESULTS \n" );
  printf("- \n");
  printf("-    Peak Total transactions per/hr  = %f \n", Total_Trans_hr);
  printf("-    Peak Total transactions per/sec  = %f \n", Total_Trans_sec);
  printf("-    Total data transmitted at peak hr (Kbytes/hr)  = %f \n", Total_Data_Kbytes_hr);
  printf("-    Total data transmitted at peak hr (Kbits/hr)  = %f \n", Total_Data_Kbits_hr);
  printf("-    Total data transmitted at peak (Kbits/sec)  = %f \n", Total_Data_Kbits_sec );
  printf("-    Total data transmitted at peak plus overheads (Kbits/sec)  = %f \n", Total_plus_overhead );
  printf("-    Added Utilisation  = %f %\n", Utilisation);
  printf("-    Total Utilisation on LINK  = %f %\n", Utilisation_plus_curr_util);
  printf("-------------------------------------------------------- \n");

// Output results
  printf("-------------------------------------------------------- \n");
  printf("- GROWTH Transaction profile RESULTS \n" );
  printf("- \n");
  printf("-    Peak Total transactions per/hr  = %f \n", Total_Trans_hr);
  printf("-    Total data transmitted at peak hr (Kbytes/hr)  = %f \n", Total_Data_Kbytes_hr);
  printf("-    Total data transmitted at peak hr (Kbits/hr)  = %f \n", Total_Data_Kbits_hr);
  printf("-    Total data transmitted at peak (Kbits/sec)  = %f \n", Total_Data_Kbits_sec );
  printf("-    Total data transmitted at peak plus overheads (Kbits/sec)  = %f \n", Total_plus_overhead );
  printf("-    Added Utilisation  = %f %\n", Utilisation);
  printf("-------------------------------------------------------- \n");
}
