/* PlotMolecule -
**    An X-windows and Motif program to display ball and stick
**    representations of molecules along with interactive
**    control for scaling and rotating.
**
** Feel free to copy, modify and redistribute this program, but do not
** sell it. Include this notice in any copy.
**
** Author: Jim Frankenberger 1993
**         Laboratory of Atomic and Solid State Physics
**         Cornell University
**
*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <math.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/MainW.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/BulletinB.h>
#include <Xm/ArrowB.h>
#include <Xm/List.h>

#include <Xm/Label.h>
Widget x_label;
Widget y_label;
Widget z_label;
Widget info;
Widget x_text;
Widget y_text;
Widget z_text;
Widget top;
Widget bottom;
Widget left;
Widget right;
Widget front;
Widget back;
Widget box;
Widget quit;
Widget options;
Widget scale_x;
Widget info_x;
Widget bogus;
Widget xmin_label;
Widget xmax_label;
Widget info_y;
Widget scale_y;
Widget name_text;
Widget name_label;
Widget ymax_label;
Widget ymin_label;
Widget up_scale;
Widget down_scale;
Widget bogus2;
Widget size_label;
Widget help;
Widget up_shade;
Widget down_shade;
Widget shade_label;
Widget up_radius;
Widget down_radius;
Widget radius_label;
Widget arr_sep;
Widget save_ps;
Widget atom_select;
Widget center;

#include "show.h"

OBJECT bounds;
float figxmin,figymin,figzmin,figxmax,figymax,figzmax;
float world_xmax,world_ymax,world_zmax,world_xmin,world_ymin,world_zmin;
int bounds_set;

extern void top_view();
extern void bot_view();
extern void left_view();
extern void right_view();
extern void front_view();
extern void back_view();
extern void quit_app();
extern void test_app();
extern void bound_box();
extern void x_change();
extern void y_change();
extern void do_up_scale();
extern void do_down_scale();
extern void do_up_shade();
extern void do_down_shade();
extern void do_up_radius();
extern void do_down_radius();
extern void save_postscript();
extern void do_help();
extern void ok_callback();
extern void options_win();
extern void done_opt_back();
extern void none_opt_back();
extern void all_opt_back();
extern void help_opt_back();
extern void browse_opt_back();
extern void center_pic();
extern void atom_change();

/*
** This is for on the help window popup 
*/
char *help_str[] = { 
"General",
"--------",
"The figure can be rotated by moving to the rotate window and holding down",
"the left mouse and dragging axis to a new angle.",
"Clicking on a sphere in the window will display its X,Y,Z coordinates in",
"the top section of the window. See the man page for PlotMolecule for",
"input formats and command line options.",
" ",
"Arrow Buttons",
"-------------",
"Size - clicking on the up/down arrows increase/decrease the size of the figure.",
"Radius - clicking on the up/down arrows increase/decrease the radius spheres.",
"Shade - clicking on the up/down arrows increase/decrease the shading.",
" ",
"Scrollbars",
"----------",
"If the figure is too large to fit in the window the scrollbars can be used",
"to change what part of the object is being seen.",
"\0"
};

/*
** create_info()
**
** This function creates all the small widgets that are used for interactive
** control.
*/
#ifndef _NO_PROTO
create_info(long pix)
#else
create_info(pix)
long pix;
#endif
{
    Widget dialog_opt;
    XmString str;
    int n;
    Arg args[20];

    /* The area at the top of the application */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNbottomWidget,frame_canvas); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    info = XtCreateManagedWidget("frame", xmFormWidgetClass, board, args, n);

    /* The area below the drawing area */
    n = 0;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    info_x = XtCreateManagedWidget("formx", xmFormWidgetClass, frame_x, args, n);

    /* The area on the left side of the drawing area */
    n = 0;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    info_y = XtCreateManagedWidget("formy", xmFormWidgetClass, frame_y, args, n);

    /* Text to display the maximum Y value */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++; 
    str = XmStringCreate("10.0 ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    ymax_label = XtCreateManagedWidget("labelymax",xmLabelWidgetClass,info_y, args, n);
    XmStringFree(str);

    /* Text to display the minumum Y value */
    n = 0;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++; 
    str = XmStringCreate("0.0 ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    ymin_label = XtCreateManagedWidget("labelymin",xmLabelWidgetClass,info_y, args, n);
    XmStringFree(str);

    /* A scrollbar on the Y axis to change to viewport if the picture is too
    ** big to fit in the drawing area
    */
    n = 0;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNbottomWidget,ymin_label); n++;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,ymax_label); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    XtSetArg(args[n],XmNorientation,XmVERTICAL); n++;
    XtSetArg(args[n],XmNprocessingDirection,XmMAX_ON_TOP); n++;
    XtSetArg(args[n],XmNsliderSize,100); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;

    scale_y = XtCreateManagedWidget("scaley",xmScrollBarWidgetClass, info_y, args, n);

    /* A scrollbar on the X axis to change to viewport if the picture is too
    ** big to fit in the drawing area
    */
    n = 0;
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,13); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    XtSetArg(args[n],XmNorientation,XmHORIZONTAL); n++;
    XtSetArg(args[n],XmNsliderSize,100); n++;
    XtSetArg(args[n],XmNprocessingDirection,XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;

    scale_x = XtCreateManagedWidget("scalex",xmScrollBarWidgetClass, info_x, args, n);

    /* Text for the minimum X value */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,25); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,10); n++;
    str = XmStringCreate("0.0    ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    xmin_label = XtCreateManagedWidget("labelxmin",xmLabelWidgetClass,info_x, args, n);
    XmStringFree(str);

    /* Text for the maximum X value */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,90); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNrightPosition,100); n++;
    str = XmStringCreate("10.0 ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    xmax_label = XtCreateManagedWidget("labelxmax",xmLabelWidgetClass,info_x, args, n);

    /* Text in the top area to display the X cord of an atom if the
    ** mouse is clcked on the atom
    */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,13); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    x_text = XtCreateManagedWidget("textx",xmTextWidgetClass,info, args, n);

    /* Text in the top area to display the Y cord of an atom if the
    ** mouse is clcked on the atom
    */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,x_text); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,13); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    y_text = XtCreateManagedWidget("texty",xmTextWidgetClass,info, args, n);

    /* Text in the top area to display the Z cord of an atom if the
    ** mouse is clcked on the atom.
    */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,y_text); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition,13); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;  
    z_text = XtCreateManagedWidget("textz",xmTextWidgetClass,info, args, n);
    
    /* Text preceding X position data in top area */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNrightWidget,x_text); n++; 
    str = XmStringCreate("X Position: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    x_label = XtCreateManagedWidget("labelx",xmLabelWidgetClass,info, args, n);

    XmStringFree(str);

    /* Text preceding Y position data in top area */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,x_text); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNrightWidget,y_text); n++; 
    str = XmStringCreate("Y Position: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    y_label = XtCreateManagedWidget("labelx",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);

    /* Text preceding Z position data in top area */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,y_text); n++;
    XtSetArg(args[n],XmNrightAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNrightWidget,z_text); n++; 
    str = XmStringCreate("Z Position: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    z_label = XtCreateManagedWidget("labelx",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);
    
    /* Text preceding name field in top area */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,z_text); n++; 
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    str = XmStringCreate("Atom Name: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    name_label = XtCreateManagedWidget("labelname",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);

    /* An area to hold the name of the atom if the atom is clicked on
    ** with the mouse.
    */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,name_label); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;  
    XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition,100); n++;
    name_text = XtCreateManagedWidget("textname",xmTextWidgetClass,info, args, n);
      
    /* The Quit button */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,x_text); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    quit = XtCreateManagedWidget("Quit  ",xmPushButtonWidgetClass,info, args, n);

    /* The Help button */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,quit); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    help = XtCreateManagedWidget("Help  ",xmPushButtonWidgetClass,info, args, n);

#if 0
    /* The options button */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,help); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    options = XtCreateManagedWidget("Visible",xmPushButtonWidgetClass,info, args, n);
#endif

    /* Centering cleanup button */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,quit); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,x_text); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    center = XtCreateManagedWidget("Center",xmPushButtonWidgetClass,info, args, n);

#if 0
    /* Atom */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,help); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,center); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    atom_select = XtCreateManagedWidget("Atom  ",xmPushButtonWidgetClass,info, args, n);
#endif
    
    /* Just a line separator between buttons and arrows */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNleftPosition, 70); n++;
    XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition,0); n++;
    XtSetArg(args[n],XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNbottomPosition, 100); n++;
    XtSetArg(args[n],XmNorientation,XmVERTICAL); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    XtSetArg(args[n],XmNseparatorType, XmSHADOW_ETCHED_OUT); n++;
    arr_sep = XtCreateManagedWidget(" ",xmSeparatorWidgetClass, info, args, n);

    /* The arrow button for increasing the scale */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,arr_sep); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition, 0); n++;
    up_scale = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* Tells what the sixe arrows are for                                   */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,up_scale); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,arr_sep); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    str = XmStringCreate("SIZE ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    size_label = XtCreateManagedWidget("Size:",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);

    /* The arrow button for decreasing the scale */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,size_label); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,arr_sep); n++;
    XtSetArg(args[n],XmNarrowDirection,XmARROW_DOWN); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    down_scale = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* The arrow button for increasing the shading */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,size_label); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition, 0); n++;
    up_shade = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* Tells what the shading arrows are for */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,up_shade); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,size_label); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    str = XmStringCreate("SHADE ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    shade_label = XtCreateManagedWidget("Shade",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);

    /* The arrow button for decreasing the shading */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,shade_label); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,size_label); n++;
    XtSetArg(args[n],XmNarrowDirection,XmARROW_DOWN); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    down_shade = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* The arrow button for making the radii of the spheres bigger */
    n = 0;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,shade_label); n++;
    XtSetArg(args[n],XtNbackground,pix); n++;
    XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n],XmNtopPosition, 0); n++;
    up_radius = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* Tells what the radius arrows are for */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,up_shade); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,shade_label); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    str = XmStringCreate("RADIUS ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n],XmNlabelString,str); n++;
    radius_label = XtCreateManagedWidget("Shade",xmLabelWidgetClass,info, args, n);
    XmStringFree(str);

    /* The arrow button for making the radii of the spheres smaller */
    n = 0;
    XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNtopWidget,radius_label); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,shade_label); n++;
    XtSetArg(args[n],XmNarrowDirection,XmARROW_DOWN); n++;
    XtSetArg(args[n],XtNbackground,pix); n++; 
    down_radius = XtCreateManagedWidget("  ",xmArrowButtonWidgetClass,info, args, n);

    /* Attach all the callbacks to invoke the correct functions when the
    ** corresponding widget is activated by clicking the mouse.
    */
    XtAddCallback(quit,XmNactivateCallback,quit_app,NULL);
#if 0
    XtAddCallback(options,XmNactivateCallback,options_win,options);
#endif
    XtAddCallback(center,XmNactivateCallback,center_pic,NULL);
#if 0
    XtAddCallback(atom_select,XmNactivateCallback,atom_change,atom_select);
#endif
    XtAddCallback(scale_x,XmNincrementCallback,x_change,NULL);
    XtAddCallback(scale_x,XmNdecrementCallback,x_change,NULL);
    XtAddCallback(scale_x,XmNpageIncrementCallback,x_change,NULL);
    XtAddCallback(scale_x,XmNpageDecrementCallback,x_change,NULL);
    XtAddCallback(scale_y,XmNincrementCallback,y_change,NULL);
    XtAddCallback(scale_y,XmNdecrementCallback,y_change,NULL);
    XtAddCallback(scale_y,XmNpageIncrementCallback,y_change,NULL);
    XtAddCallback(scale_y,XmNpageDecrementCallback,y_change,NULL);
    XtAddCallback(up_scale,XmNactivateCallback, do_up_scale, NULL);
    XtAddCallback(down_scale,XmNactivateCallback, do_down_scale, NULL);
    XtAddCallback(up_shade,XmNactivateCallback, do_up_shade, NULL);
    XtAddCallback(down_shade,XmNactivateCallback, do_down_shade, NULL);
    XtAddCallback(up_radius,XmNactivateCallback, do_up_radius, NULL);
    XtAddCallback(down_radius,XmNactivateCallback, do_down_radius, NULL);
    XtAddCallback(help,XmNactivateCallback, do_help, NULL);
 
    /* Set the initlal rotation matrix to just be the identity matrix
    ** so the result will be no transformation of object coordinates.
    */
    global_rot[0][0] = global_rot[1][1] = global_rot[2][2] = 1;
    global_rot[0][1] = global_rot[0][2] = 0; 
    global_rot[1][0] = global_rot[1][2] = 0;
    global_rot[2][0] = global_rot[2][1] = 0;

}

/*
** max3()
**
** Returns the maximum of 3 floating point numbers.
**
*/
#ifndef _NO_PROTO
float max3(float a, float b, float c)
#else
float max3(a,b,c)
float a,b,c;
#endif
{

  float max;

  if (first_time)
     if (a > c)
        return(a);
     else
        return(c);
  else {
     if (a > b) max = a;
     else max = b;
     if (max > c) return(max);
     else return(c);
   }
}

/*
** min3()
**
** Returns the minumum of 3 floating point numbers.
*/
#ifndef _NO_PROTO
float min3(float a, float b, float c)
#else
float min3(a,b,c)
float a,b,c;
#endif
{
    float min;
    
    if (first_time)
       if (a < c)
	  return(a);
       else
	  return(c);
    else {
       if (a < b) min = a;
       else min = b;
       if (min < c) return(min);
       else return(c);
     }
}

/*
** set_world()
**
** This sets the world cords to the largest values. This will
** determine how big the slider sizes are in relation to the
** viewport that is actually shown.
*/
set_world()
{

    world_xmax = max3(xmax,world_xmax,pic_xmax);
    world_ymax = max3(ymax,world_ymax,pic_ymax);
    world_zmax = max3(zmax,world_zmax,pic_zmax);
    world_xmin = min3(xmin,world_xmin,pic_xmin);
    world_ymin = min3(ymin,world_ymin,pic_ymin);
    world_zmin = min3(zmin,world_zmin,pic_zmin);

}

/*
** set_limits()
**
** This function writes out the xmin,xmax,ymin,ymax values to
** the info areas of the window and also sets the slider
** bar sizes to be correct for the given range of data.
*/
set_limits()
{
  Display *dpy;

  String str;
  char buf[25];
  Arg args[10];
  int size,min;
  float range, range_world;

  if (!interactive)
     return(0);

  dpy = XtDisplay(canvas);

  sprintf(buf,"%f",xmax);
  str = XmStringCreate(buf,XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0],XmNlabelString,str);
  XtSetValues(xmax_label,args,1);
  XmStringFree(str); 

  sprintf(buf,"%f",xmin);
  str = XmStringCreate(buf,XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0],XmNlabelString,str);
  XtSetValues(xmin_label,args,1);
  XmStringFree(str);

  sprintf(buf,"%f",ymin);
  str = XmStringCreate(buf,XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0],XmNlabelString,str);
  XtSetValues(ymin_label,args,1);
  XmStringFree(str);

  sprintf(buf,"%f",ymax);
  str = XmStringCreate(buf,XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0],XmNlabelString,str);
  XtSetValues(ymax_label,args,1);
  XmStringFree(str);
  
  /* ymax and ymin define the viewport while the 
  ** world cords define the whole picture.
  */
  range_world = world_ymax - world_ymin;
  range = ymax - ymin;
  size = (range / range_world) * 100;
  
  min = ((ymin - world_ymin)/ range_world) * 100;
  
  XtSetArg(args[0],XmNvalue, min);
  XtSetArg(args[1],XmNsliderSize, size);
  XtSetValues(scale_y,args,2);

  range_world = world_xmax - world_xmin;
  range = xmax - xmin;
  size = (range / range_world) * 100;
  
  min = ((xmin - world_xmin) / range_world) * 100;

  XtSetArg(args[0],XmNvalue, min);
  XtSetArg(args[1],XmNsliderSize, size);
  XtSetValues(scale_x,args,2);

}

/*
** x_change()
**
** This function gets invoked when the user chnages the slider 
** to adjust the viewport.
*/
#ifndef _NO_PROTO
void x_change(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void x_change(w,client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
  float biggest_x, smallest_x,rangex,minf;
  int min;
  Arg args[10];

  if (total_objects <= 0)
     return;

  /* Read the value from the scrollbar */
  XtSetArg(args[0],XmNvalue,&min);
  XtGetValues(scale_x,args,1);
  
  minf = min;

  /* Calculate a new viewport - with the same range */
  rangex = xmax - xmin;
  xmin = world_xmin + (world_xmax - world_xmin) * (minf / 100);  
  xmax = xmin + rangex;
  set_limits();

  /* This will cause the part of the picture associatd with viewport
  ** to be drawn with the correct scale.
  */
  apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);
  
      
}

/*
** y_change()
**
** This function gets invoked when the user changes the Y slider 
** to adjust the viewport.
**
*/
#ifndef _NO_PROTO
void y_change(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void y_change(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{

  float biggest_y, smallest_y,rangey,minf;
  int min;
  Arg args[10];
   
  if (total_objects <= 0)
     return;

  /* Read the value from the scrollbar */
  XtSetArg(args[0],XmNvalue,&min);
  XtGetValues(scale_y,args,1);
  
  minf = min;

  /* Calculate a new viewport - with the same range */
  rangey = ymax - ymin;
  ymin = world_ymin + (world_ymax - world_ymin) * (minf / 100);  
  ymax = ymin + rangey;
  set_limits();

  /* This will cause the part of the picture associatd with viewport
  ** to be drawn with the correct scale.
  */
  apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);
      
}

/*
** save_postscript()
**
** This function is invoked when the SavePS button is hit, currently
** it is not implemented but the idea is to generate a color or bw
** postscript file of the drawing area.
*/
#ifndef _NO_PROTO
void save_postscript(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void save_postscript(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{

     fprintf(stderr,"\nUse the following command: xwd -bitmap -id %d -out filename\n",XtWindow(canvas));

}

/*
** do_help()
**
** This function gets invoked when the help button is pushed.
**
*/
#ifndef _NO_PROTO
void do_help(Widget w, Widget client_data, XmAnyCallbackStruct *call_data)
#else
void do_help(w, client_data, call_data)
Widget w;
Widget client_data;
XmAnyCallbackStruct *call_data;
#endif
{
     int n,i;
     XmString str;
     Arg args[10];
     Widget dialog,label;

     /* Create a help dialog popup */
     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
     dialog = XmCreateMessageDialog(w,"Help",args,n);

     XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_CANCEL_BUTTON));

     label = XmMessageBoxGetChild(dialog, XmDIALOG_MESSAGE_LABEL);
     
     n = 0;
     XtSetArg(args[n],XmNalignment, XmALIGNMENT_BEGINNING); n++;
     XtSetValues(label,args,n);

     XtAddCallback(dialog, XmNokCallback, ok_callback, NULL);

     i = 0;

     str = (XmString)NULL;

     /* Get the help text into an XmString structure */
     while (*help_str[i] != '\0') {
         if (i > 0) 
            str = XmStringConcat(str, XmStringSeparatorCreate());
	 str = XmStringConcat(str, XmStringCreate(help_str[i],XmSTRING_DEFAULT_CHARSET));
	 i++;
     }
     
     n = 0;
     XtSetArg(args[n],XmNmessageString, str); n++;
     XtSetValues(dialog,args,n);

     XmStringFree(str);

     /* This will put up the help window */
     XtManageChild(dialog);

}

/*
** ok_callback()
**
** This is a general function that take down a popup window when it
** is attached to one of the buttons on the window.
**
*/
#ifndef _NO_PROTO
void ok_callback(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void ok_callback(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{

     XtUnmanageChild(w);

}

/*
** test_app()
**
** This is for debugging purposes - it just dumps out some info
** if it is attached to a button callback.
**
*/
#ifndef _NO_PROTO
void test_app(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void test_app(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
     int vis_count,active_count,inactive_count,use_count,i;
     int act_line,act_sphere,inact_line,inact_sphere;
     OBJECT *obj;

     fprintf(stderr,"\n--------------\nThere are %d objects.\n",total_objects);
     fprintf(stderr,"There are %d current objects.\n",object_count);
     vis_count = 0;
     active_count = 0;
     inactive_count = 0;
     use_count = 0;
     act_line = 0;
     act_sphere = 0;
     inact_sphere = 0;
     inact_line = 0;

     obj = object_head.next;
     for (i=0;i<total_objects;i++) {
         
         if (obj->active) {
            if (obj->type == SPHERE)
               act_sphere++;
            else
               act_line++;
            active_count++;
            if (obj->visible)
               vis_count++;
	 } else {
            if (obj->type == SPHERE)
               inact_sphere++;
            else
	       inact_line++;
            inactive_count++;
	 }
         if (obj->use_this) 
            use_count++;
         obj = obj->next;
     }
     fprintf(stderr,"There are %d active objects.\n",active_count);
     fprintf(stderr,"There are %d inactive objects.\n",inactive_count);
     fprintf(stderr,"There are %d visible objects.\n",vis_count);
     fprintf(stderr,"There are %d Lines active and %d inactive.\n",act_line,inact_line);
     fprintf(stderr,"There are %d Spheres active and %d inactive.\n",act_sphere,inact_sphere);
     fprintf(stderr,"There are %d reused objects.\n-------------\n",use_count);
     clear_objects(); 
     return;

}

/*
** quit_app()
**
** This function gets invoked when the quit button is pushed.
**
*/
#ifndef _NO_PROTO
void quit_app(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void quit_app(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif

{
   XtCloseDisplay(XtDisplay(w));
   exit(0);

}
/*
** options_win()
**
** This function displays any other options in the program.
**
*/
#ifndef _NO_PROTO
void options_win(Widget w, Widget parent, XmAnyCallbackStruct *call_data)
#else
void options_win(w,parent,call_data)
Widget w;
Widget parent;
XmAnyCallbackStruct *call_data;
#endif

{

    Widget options_form,done_options,resize_all,resize_none,help_options,list;
    Arg args[10];
    int n,i,j,c_count;
    XmString *xmstr;
    char buf[100];

    n=0;
    XtSetArg(args[n], XtNheight, 300); n++;
    XtSetArg(args[n], XtNwidth, 300); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++; 
    options_form = XmCreateFormDialog(parent, "options_form", args, n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    done_options = XtCreateManagedWidget("Done",xmPushButtonWidgetClass,
					 options_form,args,n);
    XtAddCallback(done_options,XmNactivateCallback, done_opt_back, options_form);

    n=0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, done_options); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,50); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    resize_all = XtCreateManagedWidget("Resize All ",xmPushButtonWidgetClass,
				       options_form,args,n);
    XtAddCallback(resize_all,XmNactivateCallback, all_opt_back, resize_all);

    n=0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, resize_all); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    resize_none = XtCreateManagedWidget("Resize None",xmPushButtonWidgetClass,
					options_form,args,n);
    XtAddCallback(resize_none,XmNactivateCallback, none_opt_back, resize_none);

    n=0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, resize_none); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,100); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    help_options = XtCreateManagedWidget("Help",xmPushButtonWidgetClass,
                                        options_form,args,n);
    XtAddCallback(help_options,XmNactivateCallback, help_opt_back, help_options);
    
    /* Find out how many colors are active */
    for (i=c_count=0;i<100;i++) {
        if (colors[i][0] != '\0') 
	   c_count++;
    }
    
    xmstr = (XmString *) XtMalloc(sizeof(XmString) * c_count);
    
    for (i=j=0;i<100;i++) {
       if (colors[i][0] != '\0') {
	  if (apply_resize[i])
	     sprintf(buf,"%d  %s ON",i,colors[i]);
          else
	     sprintf(buf,"%d  %s OFF",i,colors[i]);
	  xmstr[j++] = XmStringCreate(buf,XmSTRING_DEFAULT_CHARSET);
       }
     }
     n = 0;
    XtSetArg(args[n], XmNitems, xmstr); n++;
    XtSetArg(args[n], XmNitemCount, c_count); n++;
    XtSetArg(args[n], XmNbottomAttachment,XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, resize_all); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++; 
    list = XtCreateManagedWidget("list",xmListWidgetClass, options_form,args,n);

    XtAddCallback(list, XmNbrowseSelectionCallback, browse_opt_back, NULL);
    XtManageChild(options_form);


#if 0
      for (i=0;i<MAX_NAMES;i++)
         if (name_table[i] != NULL)
	    fprintf(stderr,"%s %s\n",colors[i],name_table[i]);

#endif
}

/*
** browse_opt_back()
**
** The callback when a mouse button is released over a color entry.
**
*/
#ifndef _NO_PROTO
void browse_opt_back(Widget w, Widget dialog, XmListCallbackStruct *call_data)
#else
void browse_opt_back(w,dialog,call_data)
Widget w;
Widget dialog;
XmListCallbackStruct *call_data;
#endif
{
      char *text;
      char color_name[25];
      char state[10];
      char buf[100];
      int index,i;
      OBJECT *obj_ptr;
      Display *dpy;

      XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &text);
      
      sscanf(text,"%d %s %s",&index,color_name,state);

      apply_resize[index] = !apply_resize[index];
      
      if (apply_resize[index]) 
	 sprintf(buf,"%d %s ON",index,color_name);
      else
         sprintf(buf,"%d %s OFF",index,color_name);

      fprintf(stderr,"%\n%s\n",buf);

      /* At this point the new entry should be put back into the list */

      dpy = XtDisplay(canvas);
      obj_ptr = object_head.next;
      for (i=0;i<total_objects;i++) {
         if (obj_ptr->active) {
	   if ((obj_ptr->color_index == index) && (obj_ptr->type == SPHERE)) {
            if ((obj_ptr->use_this == NULL) && (obj_ptr->win_gc != NULL)) {
               XFreePixmap(XtDisplay(canvas),obj_ptr->pixmap);
               XFreePixmap(XtDisplay(canvas),obj_ptr->mask_pixmap);
               XFreeGC(XtDisplay(canvas),obj_ptr->pixmap_gc);
               XFreeGC(XtDisplay(canvas),obj_ptr->win_gc);
	     }
             obj_ptr->pixmap = (Pixmap) NULL;
             obj_ptr->mask_pixmap = (Pixmap) NULL;
             obj_ptr->pixmap_gc = NULL;
             obj_ptr->win_gc = NULL;
             obj_ptr->use_this = NULL;
	   }
	 }
         obj_ptr = obj_ptr->next;
       }

      if (object_count > 0) {
         apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);
#if 0
	 clear_canvas();
         draw_objects(TRUE,TRUE,FALSE);
#endif
       }
}
/*
** done_opt_back()
**
** The callback when the 'done' button is hit on the options form.
**
*/
#ifndef _NO_PROTO
void done_opt_back(Widget w, Widget dialog, XmAnyCallbackStruct *call_data)
#else
void done_opt_back(w,dialog,call_data)
Widget w;
Widget dialog;
XmAnyCallbackStruct *call_data;
#endif
{

     XtUnmanageChild(dialog);
}

/*
** help_opt_back()
**
** The callback when the 'help' button is hit on the options form.
**
*/
#ifndef _NO_PROTO
void help_opt_back(Widget w, Widget dialog, XmAnyCallbackStruct *call_data)
#else
void help_opt_back(w,dialog,call_data)
Widget w;
Widget dialog;
XmAnyCallbackStruct *call_data;
#endif
{

     
}

/*
** all_opt_back()
**
** The callback when the 'resize all' button is hit on the options form.
**
*/
#ifndef _NO_PROTO
void all_opt_back(Widget w, Widget dialog, XmAnyCallbackStruct *call_data)
#else
void all_opt_back(w,dialog,call_data)
Widget w;
Widget dialog;
XmAnyCallbackStruct *call_data;
#endif
{

     
}
/*
** none_opt_back()
**
** The callback when the 'resize none' button is hit on the options form.
**
*/
#ifndef _NO_PROTO
void none_opt_back(Widget w, Widget dialog, XmAnyCallbackStruct *call_data)
#else
void none_opt_back(w,dialog,call_data)
Widget w;
Widget dialog;
XmAnyCallbackStruct *call_data;
#endif
{

     
}


/*
** do_down_scale()
**
** This function is invoked when the down size arrow button is hit -
** it shrinks the picture. Since this will change the size of the 
** spheres the pixmaps should all be freed and new one allocated that
** are the correct size. 
**
** The function can also be invoked from the do_up_scale function 
** which will set client_data to 4.
**
*/
#ifndef _NO_PROTO
void do_down_scale(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_down_scale(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
  OBJECT *obj_ptr;
   OBJECT *prev_ptr;
   int i,j,l,radius;
   float range;
   Display *dpy;
   Window win;
   XColor new_color,end_color;
   Colormap cmap;
   int stepsize;
   unsigned long cstack[256];
   int decstepr,decstepb,decstepg;
   unsigned long planes = 0;
   int top = 0;

   if (total_objects <= 0)
      return;

   dpy = XtDisplay(canvas);

   biggest_r = 0;

   obj_ptr = object_head.next;
   j = total_objects;
   cmap =  DefaultColormap(dpy, DefaultScreen(dpy));
 
   /* Free all the pixmaps that define what the spheres look like -
   ** this needs to be done because they will have a different size.
   */
   for (i=0;i<j;i++) {
       /* Check if this object has its own pixmap allocated or if it uses
       ** another objects pixmap
       */
       if ((obj_ptr->type == SPHERE) && (obj_ptr->use_this == NULL) && (obj_ptr->win_gc != NULL)) {
          XFreePixmap(XtDisplay(canvas),obj_ptr->pixmap);
          XFreePixmap(XtDisplay(canvas),obj_ptr->mask_pixmap);
          XFreeGC(XtDisplay(canvas),obj_ptr->pixmap_gc);
          XFreeGC(XtDisplay(canvas),obj_ptr->win_gc);
                  
       }
       
       /* Check if we should sexpand or shrink the spheres */
       if ((obj_ptr->type == SPHERE) && (client_data == (caddr_t)3))
	  obj_ptr->radius = obj_ptr->radius / 2;
       else
          if ((obj_ptr->type == SPHERE) && (client_data == (caddr_t)4))
	    obj_ptr->radius = obj_ptr->radius * 2;

       /* Set all the flags so that new pixmaps will get built when the
       ** picture is redrawn.
       */
       obj_ptr->pixmap = (Pixmap)NULL;
       obj_ptr->mask_pixmap = (Pixmap)NULL;
       obj_ptr->pixmap_gc = NULL;
       obj_ptr->win_gc = NULL;
       obj_ptr->use_this = NULL;
       obj_ptr->visible = TRUE;
       obj_ptr = obj_ptr->next;
      
   } /* End for loop */


#ifdef ERRORMSG
   /* This was an experiment with trying to optimize color use */
   if (cshade != old_cshade) {
      for (i=0;i<num_colors;i++) {
          cstack[i] = cdata[i].pixel;
      }
      /* Need to figure out how to set the planes param */
      XFreeColors(dpy,cmap,cstack,num_colors,planes);
      num_colors = 0;

   }
#endif

   no_size = FALSE;
   first_time = TRUE;

   /* This makes the viewport boundaries smaller which causes
   ** the picture to be magnified.
   */
   if (client_data == (caddr_t)1) {
     range =  (xmax - xmin) / 6;
     xmin = xmin + range;
     xmax = xmax - range;
     range = (ymax - ymin) / 6;
     ymin = ymin + range;
     ymax = ymax - range;
   } else {
     /* This causes the viewport boundaries to become larger
     ** which causes the picture to shrink.
     */
     if (!client_data) {
        range = (xmax - xmin) / 4;
        xmin = xmin - range;
        xmax = xmax + range;
        range = (ymax -ymin) / 4;
        ymin = ymin - range;
        ymax = ymax + range;
     } 
   }

   /* Set up the new boundaries */
   set_world();
   set_limits();
   first_time = FALSE;

   /* This will cause the picture to be redrawn with all the objects
   ** scaled to the boundary conditions of the viewport.
   */
   apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);

}

/*
** add_pixel()
**
** This was an experiment in trying to optimize color allocation.
**
*/
#ifndef _NO_PROTO
add_pixel(unsigned long pix, int top, unsigned long cstack[])
#else
add_pixel(pix,top,cstack)
unsigned long pix;
int top;
unsigned long cstack[];
#endif
{
    int i;

        for (i=0;i<top;i++) {
           if (cstack[i] == pix)
	      return(top);
	}
        cstack[top++] = pix;
        return(top);

}

/* 
** do_up_scale()
**
** This is invoked when the up arrow button for the scale is pushed,
** it will magnify the picture - calling do_down_scale with the
** client data set to 1 will cause the correct adjustments in the
** viewport size.
**
*/
#ifndef _NO_PROTO
void do_up_scale(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_up_scale(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
    do_down_scale(w,(caddr_t)1,call_data);
}

/*
** do_up_radius()
**
** This will cause the radius of each of the spheres to be doubled,
** calling do_down_scale() with the client data set to 4 will cause
** the radius of each sphere to be adjusted.
**
*/
#ifndef _NO_PROTO
void do_up_radius(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_up_radius(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
    do_down_scale(w,(caddr_t)4,call_data);
}

/*
** do_up_shade()
**
** This function is invoked when the up arrow button for the shading
** is pushed. It increases the number of colors to shade a sphere
** and then call do_down_scale() which will go through and free all
** the pixmaps, finally when the picture is redrawn the shading will 
** finer.
**
*/
#ifndef _NO_PROTO
void do_up_shade(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_up_shade(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
    old_cshade = cshade;
    cshade += 3;
    do_down_scale(w,(caddr_t)2,call_data);
    old_cshade = cshade;
}

/*
** do_down_radius()
**
** This function is invoked by hitting the down arrow button in order
** to decrease the size of the radius of each spehere. Again, 
** do_down_scale() will adjust the radius of each spehere and free
** the existing pixmaps.
**
*/
#ifndef _NO_PROTO
void do_down_radius(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_down_radius(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
   
    do_down_scale(w,(caddr_t)3,call_data);
}

/*
** do_down_shade()
**
** This function is invoked when the down arrow button for the shading
** is pushed. It decreases the number of colors to shade a sphere
** and then call do_down_scale() which will go through and free all
** the pixmaps, finally when the picture is redrawn the shading will 
** coarser.
**
*/
#ifndef _NO_PROTO
void do_down_shade(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void do_down_shade(w,client_data,call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
     old_cshade = cshade;
     cshade -= 3;
     do_down_scale(w,(caddr_t)2,call_data);
     old_cshade = cshade;
     
}




