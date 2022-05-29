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
 *       File: globals.c
 *
 *    Purpose: Global variables to all source files.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added TOP_LEVEL, INPUT, OUTPUT
 *             01/31/92 - MGR - Replaced TOP_LEVEL with CONTEXT & DISPLAY.
 *
 */

#define GLOBALS_INC
#include "globals.h"
#undef GLOBALS_INC


/* Application Global Variables */

Globals		 GLOBALS;
char		*PROGRAM = NULL;
FILE		*INPUT = stdin;
FILE		*OUTPUT = NULL;
int		 BINARY = FALSE;
XtAppContext	 CONTEXT;
Display		*DISPLAY;
Colormap	 COLORS;
Window		 THE_WINDOW;
Window		 THE_ROOT;
int		 THE_SCREEN;
XSizeHints	 THE_HINTS;
GC		 THE_GC;
GC		 GC_2;
GC		 BACK_GC;
int		 DEPTH;
XrmDatabase	 DATABASE;
int		 BACKGROUND = FALSE;
int		 TERMINATE = FALSE;
int		 SWITCHXY = FALSE;
double		 SCALE_X = 1;
double		 SCALE_Y = 1;
double		 MIN_X = 0;
double		 MIN_Y = 0;
double		 MAX_X = 100;
double		 MAX_Y = 100;
int		 PAGE = FALSE;
int		 PIPE = FALSE;
Font		 TITLE_FONT;
Font		 HOR_FONT;
Font		 VERT_FONT;
XColor		 MAIN_BACK;
XColor		 MAIN_FORE;
XColor		 TITLE_BACK;
XColor		 TITLE_FORE;
XColor		 HOR_BACK;
XColor		 HOR_FORE;
XColor		 VERT_BACK;
XColor		 VERT_FORE;
XColor		 VERT_LINE_COL;
XColor		 HOR_LINE_COL;
int		 VERT_LINE = FALSE;
int		 HOR_LINE = FALSE;
int		 REDRAW = TRUE;
char		*TITLE_TEXT = NULL;
char		*HOR_TEXT = NULL;
char		*VERT_TEXT = NULL;
unsigned long	 BLACK;
unsigned long	 WHITE;
Pixmap		 BACK_PANE;
int		 CL_COUNT = 31;
XrmOptionDescRec CL[] ={{kCom01,kRes01,XrmoptionNoArg,"TRUE"},
                        {kCom02,kRes01,XrmoptionNoArg,"TRUE"},
                        {kCom03,kRes02,XrmoptionNoArg,"TRUE"},
                        {kCom04,kRes02,XrmoptionNoArg,"TRUE"},
                        {kCom05,kRes02,XrmoptionNoArg,"TRUE"},
                        {kCom06,kRes03,XrmoptionSepArg,NULL},
                        {kCom07,kRes03,XrmoptionSepArg,NULL},
                        {kCom08,kRes04,XrmoptionSepArg,NULL},
                        {kCom09,kRes04,XrmoptionSepArg,NULL},
                        {kCom10,kRes05,XrmoptionSepArg,NULL},
                        {kCom11,kRes05,XrmoptionSepArg,NULL},
                        {kCom12,kRes06,XrmoptionSepArg,NULL},
                        {kCom13,kRes06,XrmoptionSepArg,NULL},
                        {kCom14,kRes07,XrmoptionSepArg,NULL},
                        {kCom15,kRes07,XrmoptionSepArg,NULL},
                        {kCom16,kRes08,XrmoptionSepArg,NULL},
                        {kCom17,kRes08,XrmoptionSepArg,NULL},
                        {kCom18,kRes09,XrmoptionSepArg,NULL},
                        {kCom19,kRes09,XrmoptionSepArg,NULL},
                        {kCom20,kRes10,XrmoptionSepArg,NULL},
                        {kCom21,kRes10,XrmoptionSepArg,NULL},
                        {kCom22,kRes11,XrmoptionSepArg,NULL},
                        {kCom23,kRes11,XrmoptionSepArg,NULL},
                        {kCom24,kRes12,XrmoptionSepArg,NULL},
                        {kCom25,kRes12,XrmoptionSepArg,NULL},
                        {kCom26,kRes13,XrmoptionNoArg,"TRUE"},
                        {kCom27,kRes13,XrmoptionNoArg,"TRUE"},
                        {kCom28,kRes14,XrmoptionSepArg,NULL},
                        {kCom29,kRes14,XrmoptionSepArg,NULL},
                        {kCom30,kRes15,XrmoptionSepArg,NULL},
                        {kCom31,kRes15,XrmoptionSepArg,NULL}};
char		*ERRORS[]={"No Error Occured",
                          "Unknown Fatal Error",
                          "Illegal Usage Error",
                          "Output to pipe Error",
                          "Could not open Xdisplay Error",
                          "Out of Memory Error",
			  "Bad line of input on stdin Error",
			  "Exceeded the maximum number of points/primitive Error",
			  "Only one point on last line Error",
			  "Binary data sent without binary mode enabled Error",
			  "Regular data sent with binary mode enabled Error",
			  "Odd number of data units for a polygon Error",
                          " "};

