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
 *       File: globals.h
 *
 *    Purpose: Header for global variables to all source files.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added Error Codes, external definitions.
 *
 */


/* System Includes */

#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifdef AIXV3
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>


/* Application Defines */

#define VERSION 0.7.5D
#define MAX_POINTS 100
#define SHUTDOWN -1				/* ExitToShell with no error code */
#define NOERR 0					/* ExitToShell with error 0 */
#define FATALERR 1				/* ExitToShell with error 1 */
#define USAGEERR 2				/* ExitToShell with error 2 */
#define OUTPUTERR 3				/* SoftError code 3 */
#define DISPLAYERR 4				/* ExitToShell with error 4 */
#define MEMERR 5				/* ExitToShell with error 5 */
#define BADINPUT 6				/* SoftError code 6 */
#define EXCEEDED_POINTS 7			/* SoftError code 7 */
#define ONE_POINT 8				/* SoftError code 8 */
#define NOBINARY 9				/* SoftError code 9 */
#define NOREGULAR 10				/* SoftError code 10 */
#define ODD_POLY 11				/* SoftError code 11 */


/* Application Typedefs */

typedef struct _prim_object
{
        int     num_values;
        double  data[MAX_POINTS];           /* 50 would be arbitrary polygon max */
} Prim_Object;

typedef struct _parse
{
	double	value;
	int	valid;
	int	reason;
} Parse;

typedef struct _line
{
	char	*foreground;
	char	*type;
} NewLine;

typedef struct _grid
{
	char	*foreground;
	char	*background;
	char	*fontlist;
	char	*text;
	char	*position;
	char	*minimum;
	char	*maximum;
	NewLine	LineType;
} Grid;

typedef struct _globals
{
	char	*geometry;
	char	*background;
	char	*foreground;
	char	*screen;
	char	*binary;
	char	*page;
	char	*record;
	char	*swapxy;
	Grid	Horizontal;
	Grid	Vertical;
	Grid	Title;
} Globals;	


/* Application Constants */

#define kCom01 "-page"
#define kCom02 "-p"
#define kCom03 "-binary"
#define kCom04 "-bin"
#define kCom05 "-b"
#define kCom06 "-backgroundscreen"
#define kCom07 "-bs"
#define kCom08 "-hmaximum"
#define kCom09 "-hmax"
#define kCom10 "-hminimum"
#define kCom11 "-hmin"
#define kCom12 "-hmode"
#define kCom13 "-hm"
#define kCom14 "-horizontal"
#define kCom15 "-hor"
#define kCom16 "-record"
#define kCom17 "-rec"
#define kCom18 "-vertical"
#define kCom19 "-vert"
#define kCom20 "-vmaximum"
#define kCom21 "-vmax"
#define kCom22 "-vminimum"
#define kCom23 "-vmin"
#define kCom24 "-vmode"
#define kCom25 "-vm"
#define kCom26 "-swapxy"
#define kCom27 "-sxy"
#define kCom28 "-hline"
#define kCom29 "-hl"
#define kCom30 "-vline"
#define kCom31 "-vl"
#define kRes01 ".page"
#define kRes02 ".binary"
#define kRes03 ".screen"
#define kRes04 ".Grid.Horizontal.max"
#define kRes05 ".Grid.Horizontal.min"
#define kRes06 ".Grid.Horizontal.LineType.type"
#define kRes07 ".Grid.Horizontal.text"
#define kRes08 ".record"
#define kRes09 ".Grid.Vertical.text"
#define kRes10 ".Grid.Vertical.max"
#define kRes11 ".Grid.Vertical.min"
#define kRes12 ".Grid.Vertical.LineType.type"
#define kRes13 ".swapxy"
#define kRes14 ".Grid.Horizontal.LineType.lines"
#define kRes15 ".Grid.Vertical.LineType.lines"
#define kRD01 ".geometry"
#define kRD02 ".screen"
#define kRD03 ".binary"
#define kRD04 ".page"
#define kRD05 ".record"
#define kRD06 ".swapxy"
#define kRD07 ".Grid.Title.foreground"
#define kRD08 ".Grid.Title.background"
#define kRD09 ".Grid.Title.fontlist"
#define kRD10 ".Grid.Title.text"
#define kRD11 ".Grid.Title.position"
#define kRD12 ".Grid.Horizontal.foreground"
#define kRD13 ".Grid.Horizontal.background"
#define kRD14 ".Grid.Horizontal.fontlist"
#define kRD15 ".Grid.Horizontal.text"
#define kRD16 ".Grid.Horizontal.position"
#define kRD17 ".Grid.Horizontal.min"
#define kRD18 ".Grid.Horizontal.max"
#define kRD19 ".Grid.Horizontal.LineType.foreground"
#define kRD20 ".Grid.Horizontal.LineType.type"
#define kRD21 ".Grid.Vertical.foreground"
#define kRD22 ".Grid.Vertical.background"
#define kRD23 ".Grid.Vertical.fontlist"
#define kRD24 ".Grid.Vertical.text"
#define kRD25 ".Grid.Vertical.position"
#define kRD26 ".Grid.Vertical.min"
#define kRD27 ".Grid.Vertical.max"
#define kRD28 ".Grid.Vertical.LineType.foreground"
#define kRD29 ".Grid.Vertical.LineType.type"
#define kRD30 ".foreground"
#define kRD31 ".background"
#define kDefault_X 100;
#define kDefault_Y 100;
#define kDefault_Width 500;
#define kDefault_Height 500;


/* Application Global Variables */

extern Globals GLOBALS;
extern char *PROGRAM;
extern FILE *INPUT;
extern FILE *OUTPUT;
extern int BINARY;
extern XtAppContext CONTEXT;
extern Display *DISPLAY;
extern Colormap COLORS;
extern int DEPTH;
extern Window THE_WINDOW;
extern Window THE_ROOT;
extern int THE_SCREEN;
extern XSizeHints THE_HINTS;
extern GC THE_GC;
extern GC GC_2;
extern GC BACK_GC;
extern XrmDatabase DATABASE;
extern char *ERRORS[];
extern int BACKGROUND;
extern int TERMINATE;
extern int SWITCHXY;
extern int SCALE;
extern int PAGE;
extern double SCALE_X;
extern double SCALE_Y;
extern double MIN_X;
extern double MIN_Y;
extern double MAX_X;
extern double MAX_Y;
extern int PIPE;
extern Font TITLE_FONT;
extern Font HOR_FONT;
extern Font VERT_FONT;
extern XColor MAIN_BACK;
extern XColor MAIN_FORE;
extern XColor TITLE_BACK;
extern XColor TITLE_FORE;
extern XColor HOR_BACK;
extern XColor HOR_FORE;
extern XColor VERT_BACK;
extern XColor VERT_FORE;
extern XColor VERT_LINE_COL;
extern XColor HOR_LINE_COL;
extern int VERT_LINE;
extern int HOR_LINE;
extern int REDRAW;
extern char *TITLE_TEXT;
extern char *HOR_TEXT;
extern char *VERT_TEXT;
extern unsigned long BLACK;
extern unsigned long WHITE;
extern Pixmap BACK_PANE;
extern int CL_COUNT;
#ifndef GLOBALS_INC
extern XrmOptionDescRec CL[];
#endif


/* Application Prototypes */

extern void main ();
extern int Correct_X ();
extern int Correct_Y ();
extern int Correct_XS ();
extern int Correct_YS ();
extern char *Database_Access ();
extern void Draw_Circle ();
extern void Draw_Ellipse ();
extern void Draw_Line ();
extern void Draw_Point ();
extern void Draw_Polygon ();
extern void Dump_Screen ();
extern void Event_Loop ();
extern void Fatal_Error ();
extern void Initialize ();
extern int Is_Char ();
extern void Load_Image ();
extern void New_Frame ();
extern int Next_Char ();
extern void Non_Space ();
extern Parse Parse_Input ();
extern void Primitive ();
extern void Primitive_Binary ();
extern char *Prog_Name ();
extern void Read_Database ();
extern char *Read_EOL ();
extern void Set_Background ();
extern void Set_Foreground ();
extern void Set_Horizontal ();
extern void Set_Scale ();
extern void Set_Title ();
extern void Set_Vertical ();
extern void Shutdown ();
extern void Soft_Error ();
extern void Start_Record ();
extern void Update_Back_Pane ();
extern void Usage ();
extern void Write_Char ();
extern void XInitialize ();

