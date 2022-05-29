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
 *       File: main.c
 *
 *    Purpose: Main program dispatch code.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added call to Prog_Name.
 *	       04/25/92 - RBP - Added comments.
 */


#include "globals.h"


void main (argc, argv)

	int	argc;
	char	*argv[];

/* 	this is the structure of the program:  Xtinitialize checks the 
	X defaults and the command line, Read_DataBase pulls the relevant
	information from the database, and Initialize, sets all the internal
	variables to their starting values and puts the X-window onto the 
	screen.  Event_Loop does everything else, (cycling through the data
	drawing it onto the screen) until it quits out, at which point 
	Shutdown closes the application down.
 */
 
{

	PROGRAM = Prog_Name (argv[0]);		/* Get root program name */
	XInitialize(&argc, argv);		/* Check Xdefaults and X command line */
	Read_Database ();			/* Get information out of X Database */
	Initialize ();				/* Initialize application */
	Event_Loop ();				/* Start event loop */
	Shutdown (SHUTDOWN);			/* Close the application down */
}
