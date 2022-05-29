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
 *       File: reconfigure.c
 *
 *    Purpose: Control all of the X characteristics and look-and-feel of the display window.
 *
 *    History: 03-27-92 - MGR - Created the initial prototypes.
 *
 * The procedures that follow are all more or less self explanatory.  Each will
 * modify one of the characteristics of the window in which the xyMoviePlot is
 * being displayed.  Since each of these commands is global in nature, they 
 * are only allowed at the beginning of a frame.  
 */


#include "globals.h"


void Set_Scale ()

/*  takes minx,miny maxX, maxY, and recomputes the scale factors */

{

	Parse	temp;

	if (BINARY)
		{
		if ((fread (&MIN_X, sizeof (float), 1, INPUT)) != 1)
			Soft_Error (BADINPUT);
		if ((fread (&MAX_X, sizeof (float), 1, INPUT)) != 1)
			Soft_Error (BADINPUT);
		if ((fread (&MIN_Y, sizeof (float), 1, INPUT)) != 1)
			Soft_Error (BADINPUT);
		if ((fread (&MAX_Y, sizeof (float), 1, INPUT)) != 1)
			Soft_Error (BADINPUT);
		SCALE_X = THE_HINTS.width / (MAX_X - MIN_X);
		SCALE_Y = THE_HINTS.height / (MAX_Y - MIN_Y);
		REDRAW = TRUE;
		}
	else
		{
		Non_Space ();
		temp = Parse_Input ();
		if (temp.valid)
			MIN_X = temp.value;
		if (temp.reason == '\n')
			{
			Soft_Error (BADINPUT);
			return;
			}
		temp = Parse_Input ();
		if (temp.valid)
			MAX_X = temp.value;
		if (temp.reason == '\n')
			{
			Soft_Error (BADINPUT);
			return;
			}
		SCALE_X = THE_HINTS.width / (MAX_X - MIN_X);
		REDRAW = TRUE;
		temp = Parse_Input ();
		if (temp.valid)
			MIN_Y = temp.value;
		if (temp.reason == '\n')
			{
			Soft_Error (BADINPUT);
			return;
			}
		temp = Parse_Input ();
		if (temp.valid)
			MAX_Y = temp.value;
		if (temp.reason != '\n')
			Soft_Error (BADINPUT);
		SCALE_Y = THE_HINTS.height / (MAX_Y - MIN_Y);
		}
}

void Set_Vertical ()

/* Reads in the new vertical annotation */

{
	VERT_TEXT = Read_EOL ();
	REDRAW = TRUE;
}

void Set_Background ()

/* Change the Background color */
{

	XColor	temp_color,temp_color2;	

	if (XAllocNamedColor (DISPLAY, COLORS, Read_EOL (), &temp_color, &temp_color2) != 0)
		{
		MAIN_BACK = temp_color;
		XSetWindowBackground (DISPLAY, THE_WINDOW, MAIN_BACK.pixel);
		}
}

void Set_Foreground ()

/*  Change the foreground color */
{

	XColor	temp_color,temp_color2;

	if (XAllocNamedColor (DISPLAY, COLORS, Read_EOL (), &temp_color, &temp_color2) != 0)
		{
		MAIN_FORE = temp_color;
		XSetForeground (DISPLAY, THE_GC, MAIN_FORE.pixel);
		}
}

void Set_Horizontal ()

/*  Read in a new horizontal annotation */

{
	HOR_TEXT = Read_EOL ();
	REDRAW = TRUE;
}

void Set_Title ()

/*  Read in new Title */

{
	TITLE_TEXT = Read_EOL ();
	REDRAW = TRUE;
}

void New_Frame ()

/*  We are done with a frame, kill the old GC and wipe the screen */
{

	XCopyArea (DISPLAY, BACK_PANE, THE_WINDOW, GC_2, 0, 0, THE_HINTS.width,
		   THE_HINTS.height, 0, 0);
	XSetForeground (DISPLAY, THE_GC, MAIN_BACK.pixel);
	XFillRectangle (DISPLAY, BACK_PANE, THE_GC, 0, 0, THE_HINTS.width, THE_HINTS.height);
	XSetForeground (DISPLAY, THE_GC, MAIN_FORE.pixel);
}

void Update_Back_Pane ()
{


	BACK_PANE = XCreatePixmap (DISPLAY, THE_WINDOW, THE_HINTS.width, THE_HINTS.height,
				   DefaultDepth (DISPLAY, THE_SCREEN));
	BACK_GC = XCreateGC (DISPLAY, BACK_PANE, 0, 0);
	XSetForeground (DISPLAY, BACK_GC, MAIN_FORE.pixel);
	XDrawLine (DISPLAY, BACK_PANE, BACK_GC, 0, 0, 100, 100);
/*
	XSetWindowBackgroundPixmap (DISPLAY, THE_WINDOW, BACK_PANE);
*/
}


