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
 *       File: initialize.c
 *
 *    Purpose: Initialization code for application.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added Prog_Name (string) and Xdefaults (argc, argv).
 *             01/30/92 - MGR - Finished Parse and added Parse_Test.
 *             01/31/92 - MGR - Coded all of Xdefaults.
 *
 */


#include "globals.h"


char *Prog_Name (string)

/*  takes as input the command used to start the program, and strips out
 *  anything that is not part of the program name... (like the path etc.)
 *  This name is then used to name the X-client.
 */

	char	*string;				/* Input string */

{

	char	*modified;				/* Modified string */

	modified = (char *) strrchr (string, '/');	/* Parse the string */
	if (modified)					/* Any characters to remove? */
		{
		return ++modified;			/* YES */
		}
	else
		{
		return string;				/* NO */
		}
}


void XInitialize (argc, argv)

/*  Define CONTEXT, THE_ROOT, THE_SCREEN, DATABASE */

	int	*argc;
	char	**argv;

{

	XtToolkitInitialize ();
	CONTEXT = XtCreateApplicationContext ();
	if ((DISPLAY = XtOpenDisplay (CONTEXT, NULL, PROGRAM, PROGRAM, CL, CL_COUNT, argc, argv)) == NULL)
		{
		Fatal_Error (DISPLAYERR);
		}
	if (*argc != 1)
		{
		Fatal_Error (USAGEERR);
		}
	DATABASE = XtDatabase (DISPLAY);
	THE_ROOT = DefaultRootWindow (DISPLAY);
	THE_SCREEN = DefaultScreen (DISPLAY);
	DEPTH = DefaultDepth (DISPLAY, THE_SCREEN);
	COLORS = DefaultColormap (DISPLAY, THE_SCREEN);

}


void Read_Database ()

/*  The database contains everything from the users .Xdefaults file and the 
 *  command line options.  All we do here is to read the database, and 
 *  put the values into our global resource structure.
 */

{

	GLOBALS.geometry = Database_Access (kRD01);
        GLOBALS.screen = Database_Access (kRD02);
        GLOBALS.binary = Database_Access (kRD03);
	GLOBALS.page = Database_Access (kRD04);
        GLOBALS.record = Database_Access (kRD05);
        GLOBALS.swapxy = Database_Access (kRD06);
	GLOBALS.foreground = Database_Access (kRD30);
	GLOBALS.background = Database_Access (kRD31);
        GLOBALS.Title.foreground = Database_Access (kRD07);
        GLOBALS.Title.background = Database_Access (kRD08);
        GLOBALS.Title.fontlist = Database_Access (kRD09);
        GLOBALS.Title.text = Database_Access (kRD10);
        GLOBALS.Horizontal.foreground = Database_Access (kRD12);
        GLOBALS.Horizontal.background = Database_Access (kRD13);
        GLOBALS.Horizontal.fontlist = Database_Access (kRD14);
        GLOBALS.Horizontal.text = Database_Access (kRD15);
        GLOBALS.Horizontal.minimum = Database_Access (kRD17);
        GLOBALS.Horizontal.maximum = Database_Access (kRD18);
        GLOBALS.Horizontal.LineType.foreground = Database_Access (kRD19);
        GLOBALS.Horizontal.LineType.type = Database_Access (kRD20);
	GLOBALS.Vertical.foreground = Database_Access (kRD21);
	GLOBALS.Vertical.background = Database_Access (kRD22);
	GLOBALS.Vertical.fontlist = Database_Access (kRD23);
	GLOBALS.Vertical.text = Database_Access (kRD24);
	GLOBALS.Vertical.minimum = Database_Access (kRD26);
	GLOBALS.Vertical.maximum = Database_Access (kRD27);
	GLOBALS.Vertical.LineType.foreground = Database_Access (kRD28);
	GLOBALS.Vertical.LineType.type = Database_Access (kRD29);
}


char *Database_Access (string)

/*  This is the code that actually goes in and reads the data base */

	char	*string;

{

	char		*temp;
	char		*new_type[20];
	XrmValue	new_res;
	XrmDatabase	data;
	int		i;

	if (!(temp = (char *) malloc (sizeof (char) * (strlen(PROGRAM) + strlen(string)))))
		Fatal_Error (MEMERR);
	strcpy (temp, PROGRAM);
	strcat (temp, string);
	if (XrmGetResource (DATABASE, temp, temp, new_type, &new_res))
		{
		free (temp);
		if (!(temp = (char *) malloc (sizeof (char) * (strlen((char *) new_res.addr)))))
			Fatal_Error (MEMERR);
		strcpy (temp, (char *) new_res.addr);
		for (i=0;i++;i<strlen(temp))
			{
			temp[i] = tolower(temp[i]);
			}		
		return temp;
		}
	else
		{
		free (temp);
		return NULL;
		}
}

void Initialize ()

/*  Initialize everything else that we think that we might need.
 *  THE_HINTS generate most of the geometry that we need, where and how
 *  big the xwindow will be.
 *  for everything else, we check to see if the user has defined it on 
 *  the command line or in the .Xdefaults, if the user did define it, then
 *  set our internal global variable to the users preference, otherwise,
 *  set out internal global variable to some normal default.
 */ 

{

        XColor  	temp_color;
	XVisualInfo	vis_info;
	XGCValues	pane;

        BLACK = BlackPixel(DISPLAY,THE_SCREEN);
        WHITE = WhitePixel(DISPLAY,THE_SCREEN);

        THE_HINTS.x = kDefault_X;
        THE_HINTS.y = kDefault_Y;
        THE_HINTS.width = kDefault_Width;
        THE_HINTS.height = kDefault_Height;
        XParseGeometry (GLOBALS.geometry, &THE_HINTS.x, &THE_HINTS.y,
			&THE_HINTS.width, &THE_HINTS.height);
        THE_HINTS.flags = PPosition | PSize;

        THE_WINDOW = XCreateSimpleWindow (DISPLAY, THE_ROOT, THE_HINTS.x, THE_HINTS.y,
                                          THE_HINTS.width, THE_HINTS.height, 0,
                                          WHITE, BLACK);
        XSetStandardProperties (DISPLAY, THE_WINDOW, "xyMoviePlot", "xyMoviePlot",
                                None, NULL, 0, &THE_HINTS);
/*
	XMatchVisualInfo (DISPLAY, THE_SCREEN, DEPTH,???, &vis_info);
	COLORS = XCreateColormap (DISPLAY, THE_WINDOW, vis_info.visual, AllocNone);
*/
	XSetWindowColormap (DISPLAY, THE_WINDOW, COLORS);

        if ((GLOBALS.swapxy) && (GLOBALS.swapxy[0] == 't'))
                SWITCHXY = TRUE;
        if ((GLOBALS.binary) && (GLOBALS.binary[0] == 't'))
                BINARY = TRUE;
        if ((GLOBALS.page) && (GLOBALS.page[0] == 't'))
                PAGE = TRUE;
        if (GLOBALS.screen)
                {
                BACKGROUND = TRUE;
                Load_Image ();
                }
        if (GLOBALS.record)
                {
		if (GLOBALS.record[0] == '-')
			{
			OUTPUT = stdout;
			PIPE = TRUE;
			}
		else
			{
			if ((OUTPUT = fopen(GLOBALS.record, "a")) == NULL)
				Soft_Error (OUTPUTERR);
			PIPE = TRUE;
			}
		}
	if (GLOBALS.Title.fontlist)
		{
		TITLE_FONT = XLoadFont (DISPLAY, GLOBALS.Title.fontlist);
		}
	else
		{
		TITLE_FONT = XLoadFont (DISPLAY, "fixed");
		}
	if (GLOBALS.Horizontal.fontlist)
		{
		HOR_FONT = XLoadFont (DISPLAY, GLOBALS.Horizontal.fontlist);
		}
	else
		{
		HOR_FONT = XLoadFont (DISPLAY, "fixed");
		}
	if (GLOBALS.Vertical.fontlist)
		{
		VERT_FONT = XLoadFont (DISPLAY, GLOBALS.Vertical.fontlist);
		}
	else
		{
		VERT_FONT = XLoadFont (DISPLAY, "fixed");
		}
	if (GLOBALS.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.foreground, &MAIN_FORE,
				  &temp_color);
		}
	else
		{
		XAllocNamedColor (DISPLAY, COLORS, "white", &MAIN_FORE, &temp_color);
		}
	if (GLOBALS.background)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.background, &MAIN_BACK,
				  &temp_color);
		}
	else
		{
		XAllocNamedColor (DISPLAY, COLORS, "black", &MAIN_BACK, &temp_color);
		}
	if (GLOBALS.Title.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Title.foreground,
				  &TITLE_FORE, &temp_color);
		}
	else
		{
		TITLE_FORE = MAIN_FORE;
		}
	if (GLOBALS.Title.background)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Title.background,
				  &TITLE_BACK, &temp_color);
		}
	else
		{
		TITLE_BACK = MAIN_BACK;
		}
	if (GLOBALS.Horizontal.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Horizontal.foreground,
				  &HOR_FORE, &temp_color);
		}
	else
		{
		HOR_FORE = MAIN_FORE;
		}
	if (GLOBALS.Horizontal.background)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Horizontal.background,
				  &HOR_BACK, &temp_color);
		}
	else
		{
		HOR_BACK = MAIN_BACK;
		}
	if (GLOBALS.Vertical.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Vertical.foreground,
				  &VERT_FORE, &temp_color);
		}
	else
		{
		VERT_FORE = MAIN_FORE;
		}
	if (GLOBALS.Vertical.background)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Vertical.background,
				  &VERT_BACK, &temp_color);
		}
	else
		{
		VERT_BACK = MAIN_BACK;
		}
	if (GLOBALS.Horizontal.LineType.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Horizontal.LineType.foreground,
				  &HOR_LINE_COL, &temp_color);
		}
	else
		{
		HOR_LINE_COL = MAIN_FORE;
		}
	if (GLOBALS.Vertical.LineType.foreground)
		{
		XAllocNamedColor (DISPLAY, COLORS, GLOBALS.Vertical.LineType.foreground,
				  &VERT_LINE_COL, &temp_color);
		}
	else
		{
		VERT_LINE_COL = MAIN_FORE;
		}
	if ((GLOBALS.Horizontal.LineType.type) && (GLOBALS.Horizontal.LineType.type[0] == 't'))
		HOR_LINE = TRUE;
	if ((GLOBALS.Vertical.LineType.type) && (GLOBALS.Vertical.LineType.type[0] == 't'))
		VERT_LINE = TRUE;
	if (GLOBALS.Title.text)
		TITLE_TEXT = GLOBALS.Title.text;
	if (GLOBALS.Horizontal.text)
		HOR_TEXT = GLOBALS.Horizontal.text;
	if (GLOBALS.Vertical.text)
		VERT_TEXT = GLOBALS.Vertical.text;
	if (GLOBALS.Horizontal.minimum)
		MIN_X = atof (GLOBALS.Horizontal.minimum);
	if (GLOBALS.Horizontal.maximum)
		MAX_X = atof (GLOBALS.Horizontal.maximum);
	if (GLOBALS.Vertical.minimum)
		MIN_Y = atof (GLOBALS.Vertical.minimum);
	if (GLOBALS.Vertical.maximum)
		MAX_Y = atof (GLOBALS.Vertical.maximum);


	pane.foreground = MAIN_FORE.pixel;
	pane.background = MAIN_BACK.pixel;
        BACK_PANE = XCreatePixmap (DISPLAY, THE_WINDOW, THE_HINTS.width, THE_HINTS.height,
                                   DefaultDepth (DISPLAY, THE_SCREEN));
        THE_GC = XCreateGC (DISPLAY, BACK_PANE, GCBackground | GCForeground, &pane);
	GC_2 = XCreateGC (DISPLAY, THE_ROOT, GCBackground | GCForeground, &pane);
        XSetForeground (DISPLAY, THE_GC, MAIN_BACK.pixel);
        XFillRectangle (DISPLAY, BACK_PANE, THE_GC, 0, 0, THE_HINTS.width, THE_HINTS.height);
	XSelectInput (DISPLAY, THE_WINDOW, KeyPressMask | ExposureMask | StructureNotifyMask);
	XSetBackground (DISPLAY, THE_GC, MAIN_BACK.pixel);
	XSetForeground (DISPLAY, THE_GC, MAIN_FORE.pixel);
	XMapRaised (DISPLAY, THE_WINDOW);

	SCALE_X = THE_HINTS.width / (MAX_X - MIN_X);
	SCALE_Y = THE_HINTS.height / (MAX_Y - MIN_Y);
	REDRAW = FALSE;
/*
	Update_Back_Pane ();
*/
	New_Frame ();
}

