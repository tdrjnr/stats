/*
 *
 *      Title: xyMoviePlot
 *
 * Written by: Michael G. Reed  (reed@Theory.TC.Cornell.Edu)
 *             Robert B. Pless  (robert@Theory.TC.Cornell.Edu)
 *
 *     Course: ComS 418 (Computer Graphics Practicum)
 *
 * Department: Computer Science and Material Science
 *
 * (c) Copyright 1992, Cornell University
 *
 *       File: error.c
 *
 *    Purpose: Handle all error and shutdown of application.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added Usage () call and Soft_Error ().
 *             03/21/92 - MGR - Updated error coding to handle arrays.
 * 	       04/25/92 - RBP - Added documenation to the code.
 */


#include "globals.h"


void Fatal_Error (code)

/*   as the name implies, a fatal error has occured.  The code is the   
 *   number of the error.  The error message is printed, and Shutdown
 *   is called.  examples of fatal errors are: usage error, memory errors
 *   and the like
 */ 

	int	code;

{

	fprintf (stderr,"%s: Fatal Error /'%s/' has occured.\n",PROGRAM,ERRORS[code]);
	Shutdown (code);

}


void Soft_Error (code)

/*  for less dire errors, a warning message is sent.  the code is the number
 *  of the error.  examples of soft errors are, invalid input in data stream
 *  'p' flag found without the binary option set.
 */

	int	code;

{

	fprintf (stderr,"%s: Soft error \'%s\' has occured.\n",PROGRAM,ERRORS[code]);
}


void Shutdown (code)

/* exit the program for whatever reason. */

	int	code;

{

	if (code == SHUTDOWN)
		{
		code = NOERR;
		XCloseDisplay (DISPLAY);
		}
	exit (code);
}


void Usage ()

/*  standard usage error output.  this shows just the command line options,
 *  and says nothing about the data stream notation.
 */

{
 fprintf (stderr, "USAGE : %s <X Toolkit options> <options>\n",PROGRAM);
 fprintf (stderr, "Where <options> are:\n");
 fprintf (stderr, " -binary              | -bin  : Binary data transfer.\n");
 fprintf (stderr, " -background <>       | -bg   : Background color.\n");
 fprintf (stderr, " -backgroundscreen <> | -bs   : Background pixmap to display data over.\n");
 fprintf (stderr, " -foreground <>       | -fg   : Foreground color.\n");
 fprintf (stderr, " -geometry <>         | -geom : Window geometry.\n");
 fprintf (stderr, " -hline <>            | -hl   : Turn horizontal grid on.\n");
 fprintf (stderr, " -hmaximum <>         | -hmax : Maximum value for horizontal scale.\n");
 fprintf (stderr, " -hminimum <>         | -hmin : Minimum value for horizontal scale.\n");
 fprintf (stderr, " -hmode <>            | -hm   : Type of horizontal grid line.\n");
 fprintf (stderr, " -horizontal <>       | -hor  : Horizontal annotation.\n");
 fprintf (stderr, " -page                | -p    : Turn off page display mode.\n");
 fprintf (stderr, " -record <>           | -rec  : Record input data to file.\n");
 fprintf (stderr, " -swapxy <>           | -sxy  : Record input data to file.\n");
 fprintf (stderr, " -title <>            | -t    : Title of Window and Data.\n");
 fprintf (stderr, " -vertical <>         | -ver  : Vertical annotation.\n");
 fprintf (stderr, " -vline <>            | -vl   : Turn vertical grid on.\n");
 fprintf (stderr, " -vmaximum <>         | -vmax : Maximum value for vertical scale.\n");
 fprintf (stderr, " -vminimum <>         | -vmin : Minimum value for vertical scale.\n");
 fprintf (stderr, " -vmode <>            | -vm   : Type of vertical grid line.\n");
	Shutdown (USAGEERR);
}

