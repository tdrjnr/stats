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
 *       File: event.c
 *
 *    Purpose: Central event loop for application.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             04/20/92 - RBP - comments.
 *	       04/25/92 - RBP - updated comments.
 */


#include "globals.h"


void Event_Loop ()
/*
 *  This is the main event loop.  It waits for x-events, and keypresses.
 *  The X-send event command is a kludge so that anyone pressing q in the
 *  xyMoviePlot window will quit immediately (without having to press return
 *  otherwise, it looks at the first character of every line that it sees,
 *  if it is an alphabetic character, then it calls the appropriate routine.
 *  If it is a number, then it reads in the data for the primitive object.
 *  It stops when it finds a 'q' in the data stream.
 *  The only special case is when using Binary mode.  Binary input requires
 *  a data structure  'p' num data, where 'p' is the character p which acts
 *  as a flag that the data is in binary mode, num is the number of data 
 *  values, and data is the data values themselves.
 */
{

	char		text_buff[10];
	KeySym		the_key;
	int		c;
	XEvent		an_event,*new_event;
	XWindowChanges	*new_window;
	XGCValues	pane;

	while (!TERMINATE)
		{
		if (REDRAW)
			{
/*
			XFreeGC (DISPLAY, BACK_GC);
			XFreePixmap (DISPLAY, BACK_PANE);
			Update_Back_Pane ();
*/
			}

                XNextEvent (DISPLAY, &an_event);
                switch (an_event.type)
                        {
                        case MappingNotify:
                                XRefreshKeyboardMapping ((XMappingEvent *) &an_event);
                                break;
                        case KeyPress:
                                if (XLookupString ((XKeyEvent *) &an_event, text_buff,
				    10, &the_key, 0))
                                        {
                                        if ((the_key == 'q') | (the_key == 'Q'))
                                                TERMINATE = TRUE;
                                        }
                                break;
			case ConfigureNotify:
				THE_HINTS.width = ((XConfigureEvent *) &an_event)->width;
				THE_HINTS.height = ((XConfigureEvent *) &an_event)->height;
				SCALE_X = THE_HINTS.width / (MAX_X - MIN_X);
				SCALE_Y = THE_HINTS.height / (MAX_Y - MIN_Y);
				XFreeGC (DISPLAY, THE_GC);
				XFreePixmap (DISPLAY, BACK_PANE);
				pane.foreground = MAIN_FORE.pixel;
				pane.background = MAIN_BACK.pixel;
				BACK_PANE = XCreatePixmap (DISPLAY, THE_WINDOW, THE_HINTS.width,
							   THE_HINTS.height, 
							   DefaultDepth (DISPLAY, THE_SCREEN));
				THE_GC = XCreateGC (DISPLAY, BACK_PANE, GCBackground | 
						    GCForeground, &pane);
        			XSetForeground (DISPLAY, THE_GC, MAIN_BACK.pixel);
        			XFillRectangle (DISPLAY, BACK_PANE, THE_GC, 0, 0, THE_HINTS.width,
						THE_HINTS.height);
				XSetForeground (DISPLAY, THE_GC, MAIN_FORE.pixel);
				New_Frame ();
				REDRAW = TRUE;
				break;
                        default:
                                break;
                        }

		new_event = (XEvent *) malloc (sizeof (XEvent));
		new_event->type = KeyPress;
		XSendEvent (DISPLAY, THE_WINDOW, FALSE, NULL, new_event);

		if (Is_Char ())
			{
			switch (c = Next_Char ())
				{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '-':
					if (!BINARY)
						{
						ungetc (c, stdin);
						Primitive ();
						}
					else
						{
						Soft_Error (NOREGULAR);
						}
					break;
				case 'q':
				case 'Q':
					TERMINATE = TRUE;
					break;
				case 'f':
				case 'F':
					Set_Foreground ();
					break;
				case 'b':
				case 'B':
					Set_Background ();
					break;
				case 's':
				case 'S':
					Set_Scale ();
					break;
				case 't':
				case 'T':
					Set_Title ();
					break;
				case 'h':
				case 'H':
					Set_Horizontal ();
					break;
				case 'v':
				case 'V':
					Set_Vertical ();
					break;
				case 'd':
				case 'D':
					Dump_Screen ();
					break;
				case 'r':
				case 'R':
					Start_Record ();
					break;
				case 'p':
				case 'P':
					if (BINARY)
						{
						Primitive_Binary ();
						}
					else
						{
						Soft_Error (NOBINARY);
						}
					break;
				case '\n':
					New_Frame ();
				default:
					break;
				}
			}
		}
}

