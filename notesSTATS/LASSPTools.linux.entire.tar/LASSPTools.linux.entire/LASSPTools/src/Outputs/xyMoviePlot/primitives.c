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
 *       File: primitives.c
 *
 *    Purpose: Handle the dispatch/rendering of all primitives.
 *
 *    History: 03/21/92 - MGR - Initial coding of routines.
 *             04/19/92 - RBP - added comments to primitive routine.
 *             04/24/92 - RBP - Started coding the calls to X drawing routines
 *	       04/25/95 - RBP, MGR - finished coding X-routines and comments
 */


#include "globals.h"


void Primitive ()

/*
 *  This procedure deals with one line of input.  It keep reading values until
 *  it reaches an end-of-line marker.  If it encounters a non-numeric character
 *  or if there are too many point on a line, then it ignores the line.
 *  After it reads a valid object, it calls the routines to draw the object,
 *  switching on the number of data points on the line.  
 *  Our data structure is set up for this purpose, with a each type of 
 *  object having a unique number of data values associated with it.
 *  
*/
{

	Prim_Object	prim_object;
	Parse		temp;
        int     	end_of_line = FALSE;
        int     	data_counter = 0;

        while (!end_of_line)
        	{
		temp = Parse_Input ();
                if (temp.valid)
                        {
                        prim_object.data[data_counter++] = temp.value;
                        if (temp.reason == '\n')
				end_of_line = TRUE;
			if (data_counter == MAX_POINTS)
				{
				Soft_Error (EXCEEDED_POINTS);
				data_counter = 0;
				end_of_line = TRUE;
				while ((Next_Char ()) != '\n')
					{
					}
				}
                        }
                else
                        {
			data_counter = 0;
                        end_of_line = TRUE;
                        }
        	}
        prim_object.num_values = data_counter;
        switch (data_counter)
                {
		case 0:
			break;
		case 1:
			Soft_Error (ONE_POINT);
			break;
                case 2:
                        Draw_Point (prim_object);
			break;
                case 3:
                        Draw_Circle (prim_object);
			break;
                case 4:
                        Draw_Line (prim_object);
			break;
                case 5:
                        Draw_Ellipse (prim_object);
			break;
                default:
			if (data_counter & 1)
				{
				Soft_Error (ODD_POLY);
				break;
				}
                        Draw_Polygon (prim_object);
			break;
                }
}


void Primitive_Binary ()

/* This is the parser for a line of binary input.  This also dispatched
 * the call to the X-drawing routine, since it is easy to switch on 
 * count.  Basically, the binary record structure is set up as follows:
 * the first number is a count of the number of data values to follow,
 * then all the rest of the numbers are data values.
 */

{

	Prim_Object	prim_object;
	int		count;
	int		counter = 0;

	if ((fread (&count, sizeof(int), 1, INPUT)) != 1)
		Soft_Error (BADINPUT);
	if (count >100)
		Soft_Error (EXCEEDED_POINTS);
	while (counter < count)
		{
		if ((fread (&prim_object.data[counter++], sizeof(double), 1, INPUT)) != 1)
			Soft_Error (BADINPUT);
		}
	prim_object.num_values = counter;
	switch (count)
		{
		case 0:
			break;
		case 1:
			Soft_Error (ONE_POINT);
			break;
		case 2:
			Draw_Point (prim_object);
			break;
		case 3:
			Draw_Circle (prim_object);
			break;
		case 4:
			Draw_Line (prim_object);
			break;
		case 5:
			Draw_Ellipse (prim_object);
			break;
		default:
			if (count & 1)
				{
				Soft_Error (ODD_POLY);
				break;
				}
			Draw_Polygon (prim_object);
			break;
		}
} 
	

void Draw_Point (arg)

/*  This actually calls the X-drawPoint routine to put a point into the 
    window.  The If (switchxy) checks to see if the option to switch the 
    x and y axis is on at the moment, and if it is, then switches to x,y
    values in the call to the Xdrawroutine.  The correct_X, correct_y 
    routines change coordinates from the input scale to the window coords
 */

	Prim_Object	arg;

{

	if (SWITCHXY)
		XDrawPoint (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[1]),
			    Correct_Y(arg.data[0]));
	else
		XDrawPoint (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[0]),
			    Correct_Y(arg.data[1]));
}


void Draw_Line (arg)



        Prim_Object  arg;

{

	if (SWITCHXY)
		XDrawLine (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[1]),
			   Correct_Y(arg.data[0]), Correct_X(arg.data[3]),
			   Correct_Y(arg.data[2]));
	else
		XDrawLine (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[0]),
			   Correct_Y(arg.data[1]), Correct_X(arg.data[2]),
			   Correct_Y(arg.data[3]));
}


void Draw_Circle (arg)

/*  this uses the xdrawarc routine to draw a circle.  NOTE!! this 
 *  means a circle in the input coordinate system, if the scale is not 
 *  the same on both axes, or the xwindow is not square, this will not
 *  be an actual circle on the screen.
 */

        Prim_Object  arg;
{
	if (SWITCHXY)
		XDrawArc (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[1] - arg.data[2]),
			  Correct_Y(arg.data[0] - arg.data[2]), 2 * Correct_XS(arg.data[2]),
			  2 * Correct_YS(arg.data[2]), 0, 23040);
	else
		XDrawArc (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[0] - arg.data[2]),
			  Correct_Y(arg.data[1] + arg.data[2]), 2 * Correct_XS(arg.data[2]),
			  2 * Correct_YS(arg.data[2]), 0, 23040);
}


void Draw_Ellipse (arg)

/*  As of this release, we cannot draw skewed ellipses.  This can only
 *  accept ellipses oriented along the x-axis or the y-axis.  the input    
 *  format is  x, y, x-axis_radius, y-axis_radius, ignored_data_value.
 *  This procedure then calls the XDrawArc routine to actually draw the 
 *  object.
 */

        Prim_Object  arg;

{
        if (SWITCHXY)
                XDrawArc (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[1] - arg.data[3]),
                          Correct_Y(arg.data[0] + arg.data[2]), 2 * Correct_XS(arg.data[3]),
                          2 * Correct_YS(arg.data[2]), 0, 23040);
        else
                XDrawArc (DISPLAY, BACK_PANE, THE_GC, Correct_X(arg.data[0] - arg.data[2]),
                          Correct_Y(arg.data[1] + arg.data[3]), 2 * Correct_XS(arg.data[2]),
                          2 * Correct_YS(arg.data[3]), 0, 23040);
}


void Draw_Polygon (arg)

/*  IMPORTANT: polygon is a misnomer.. this is really a set of connected 
 *  lines, but does not draw a line from the 1st point to the last.  This
 *  takes the input, and generates an array of XPoints, the X internal type
 *  required by the XdrawLines routine.
 */

        Prim_Object	arg;

{

	XPoint	*points = (XPoint *) malloc(arg.num_values * sizeof(XPoint));
	int	counter = 0;

	if (!SWITCHXY)
		{
		while (counter < arg.num_values)
			{
			points[(int)counter/2].x = Correct_X(arg.data[counter]);
			points[(int)counter/2].y = Correct_Y(arg.data[counter+1]);
			counter = counter + 2;
			}
		}
	else
		{
		while (counter < arg.num_values)
			{
			points[(int)counter/2].x = Correct_X(arg.data[counter+1]);
			points[(int)counter/2].y = Correct_Y(arg.data[counter]);
			counter = counter + 2;
			}
		}
	XDrawLines (DISPLAY, BACK_PANE, THE_GC, points, arg.num_values/2, CoordModeOrigin);
	free (points);
}


int Correct_X (value)

/*  Takes an X value in the coord system of the input and translates it to 
 *  the coord system of the window.
 */

	double	value;

{

	int	temp;

	temp = (double) ((value - MIN_X) * SCALE_X);
	return (temp);
}


int Correct_Y (value)

/*  Takes an X value in the coord system of the input and translates it to
 *  the coord system of the window.
 */

	double	value;

{

	int	temp;

	temp = (double) ((value - MIN_Y) * SCALE_Y);
	temp = THE_HINTS.height - temp;
	return (temp);
}


int Correct_XS (value)

/* this takes an x distance in the coord system of the input and translates
 * to a distance on the window coord system.  used to squash circles when 
 * the scale or window has different x, y  axis scale factors.
 */
 
	double	value;

{

	int	temp;

	temp = (double) (value * SCALE_X);
	return (temp);
}


int Correct_YS (value)

/* this takes an y distance in the coord system of the input and translates
 * to a distance on the window coord system.  used to squash circles when
 * the scale or window has different x, y  axis scale factors.
 */


	double	value;

{

	int	temp;

	temp = (double) (value * SCALE_Y);
	return (temp);
}

