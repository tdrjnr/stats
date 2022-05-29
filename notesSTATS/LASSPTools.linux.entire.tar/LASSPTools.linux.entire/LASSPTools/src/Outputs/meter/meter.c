/*
** meter.c
**
** This program can act as both an output meter to display data or it can
** accept data from from the user, when the dial is rotated.
**
** 
*/
#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#ifndef _NO_PROTO 
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <string.h>
#include "Dial.h"

#define MAXLINELENGTH 100

/* These shoudl be removed or changed */
Boolean discrete_output = False;
Boolean continuous_output = False;
Boolean exit_on_eof = True;

#ifndef _NO_PROTO
extern void quit(Widget w, caddr_t client_data,
                  XmAnyCallbackStruct *call_data);
extern void help_callback();
extern void key_handler();
extern void send(Widget w, caddr_t client_data, 
                  XmAnyCallbackStruct *call_data);
extern void reset_callback();

#else
extern void quit();
extern void help_callback();
extern void key_handler();
extern void send();
extern void reset_callback();
#endif

/*
** Global Widgets and also some flags to control how meter works
** when sending data.
**
*/
Widget dial, help_button, quit_button, send_button, meterform, reset_button;

Boolean echo = False;                 /* True if meter should echo input,
					 values to output */
Boolean update = False;               /* True if meter should output 
					 continuous values when the indicator
					 is grabbed */

#ifndef _NO_PROTO
void input_handle(caddr_t client_data,int *source, XtInputId *id);
#else
void input_handle();
#endif

/*
** These are some resources that can be set via the command line.
** the resources can also be set using the .Xdefaults file
**
*/
static XrmOptionDescRec options[] = {
  {"-min", "*minimum", XrmoptionSepArg, "0"},
  {"-max", "*maximum", XrmoptionSepArg, "100"},
  {"-markers","*markers", XrmoptionSepArg, "11"},
  {"-label","*title",XrmoptionSepArg, "Meter"},
};

/*
** main()
**
** 
*/ 
#ifndef _NO_PROTO
  main( int argc, char **argv)
#else
  main( argc, argv)
    int argc;
    char **argv;
#endif
{
  extern Widget dial, meterform, help_button, quit_button, adjust_button, 
                reset_button;

  Widget toplevel;
  XtAppContext  context;
  Arg    args[20];
  short temp;
  Cursor hand;
  int i;
  int    n=0;
  

  toplevel = XtInitialize(argv[0], "Meter", options, XtNumber(options),
                          &argc, argv);

  /* Allocate the form that is to hold the meter widget and the button */
  meterform = XtCreateWidget("meterform",xmFormWidgetClass, toplevel,
     args,n);

  n=0;
  XtSetArg(args[n],XmNshadowThickness, (short)6); n++;
  XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n],XmNtopPosition, 5); n++;
  XtSetArg(args[n],XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n],XmNleftPosition, 5); n++;
  XtSetArg(args[n],XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n],XmNrightPosition, 80); n++;
  XtSetArg(args[n],XmNbottomAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n],XmNbottomPosition, 95); n++;
  dial = XtCreateWidget("dial", XsdialWidgetClass,meterform, args, n);
  n=0;

  /* Set up the buttons to control the operation of the meter */
  help_button = XtCreateWidget("Help", xmPushButtonWidgetClass, 
                meterform, args, n);

  quit_button = XtCreateWidget("Quit", xmPushButtonWidgetClass,
                meterform, args, n);

  send_button = XtCreateWidget("Send", xmPushButtonWidgetClass,
                meterform, args, n);

  reset_button = XtCreateWidget("Reset", xmPushButtonWidgetClass,
                meterform, args, n);

    
  n=0;
  XtSetArg(args[n],XmNwidth, 220); n++;
  XtSetArg(args[n],XmNheight, 185); n++;
  XtSetValues(toplevel, args, n);

  /* Layout where the quit button is to appear */
  n=0;
  XtSetArg(args[n],XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n],XmNtopPosition, 5); n++;
  XtSetArg(args[n],XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNleftWidget, dial); n++;
  XtSetArg(args[n],XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n],XmNrightOffset, 5); n++;
  XtSetValues(quit_button, args, n);

  /* Layout where the help button is to appear */
  n=0;
  XtSetArg(args[n],XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNtopWidget, quit_button); n++;
  XtSetArg(args[n],XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNleftWidget, dial); n++;
  XtSetArg(args[n],XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n],XmNrightOffset, 5); n++;
  XtSetValues(help_button, args, n);

  /* Layout where the send_button is to appear */
  n=0;
  XtSetArg(args[n],XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNtopWidget, help_button); n++;
  XtSetArg(args[n],XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNleftWidget, dial); n++;
  XtSetArg(args[n],XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n],XmNrightOffset, 5); n++;
  XtSetValues(send_button, args, n);

  /* Layout where the reset button is to appear */
  n=0;
  XtSetArg(args[n],XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNtopWidget, send_button); n++;
  XtSetArg(args[n],XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n],XmNleftWidget, dial); n++;
  XtSetArg(args[n],XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n],XmNrightOffset, 5); n++;
  XtSetValues(reset_button, args, n);


  XtManageChild(meterform);
  XtManageChild(dial);
  XtManageChild(quit_button);
  XtManageChild(help_button);
  XtManageChild(send_button);
  XtManageChild(reset_button);

  /* Attach the Quit button callback */
  XtAddEventHandler(quit_button, ButtonPressMask, FALSE, quit, NULL);

  /* Attach the Send button callback */
  XtAddEventHandler(send_button, ButtonPressMask, FALSE, send, NULL);

  /* Attach the help button callback */
  XtAddEventHandler(help_button, ButtonPressMask, FALSE, help_callback, NULL);

  /* Attach the reset button callback */
  XtAddEventHandler(reset_button, ButtonPressMask, FALSE, reset_callback,NULL);
  
  /* Attach an event handler to handle moving the arrow by 1 unit */
  XtAddEventHandler(dial, KeyPressMask, FALSE, key_handler, NULL);

  context=XtWidgetToApplicationContext(toplevel);


  XtRealizeWidget(toplevel);
  XtAppAddInput(context,fileno(stdin),XtInputReadMask,input_handle,NULL);

  /* Change cursors when the pointer goes inside the dial widget, 
  ** so that users will have some notion that it can be used as an
  ** input device
  */
  hand=XCreateFontCursor(XtDisplay(toplevel), XC_hand2);
  XDefineCursor(XtDisplay(toplevel), XtWindow(dial),hand);

  /* Get all the commandline options */
  for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-echo"))
         echo = True;
      if (!strcmp(argv[i],"-eof"))
         exit_on_eof = False;
  }


  XtMainLoop();
}


/*
** input_handle()
**
** This function is invoked when any data appears on standard input. 
** The value is read and then sent to the dial widget indicating that
** the XtNposition value is being changed.
*/
#ifndef _NO_PROTO
  void input_handle(caddr_t client_data, int *source, XtInputId *id)
#else
  void input_handle( client_data, source, id)
    caddr_t client_data;
    int *source;
    XtInputId *id;
#endif
{
  extern Widget dial;

  char line[MAXLINELENGTH];
  int pos;
  float float_pos;
  Arg args[10];
  int n=0;

  /* If we see an eof just bag out */
  if ( fgets(line,MAXLINELENGTH,stdin) == NULL ) 
     if (exit_on_eof)
        exit(0);
     else
        return;

  /* Try to determine if the value is an int or a float 
  **   - look for '.' or 'e' to indicate a float
  **   - otherwise go with an int
  */
  if (strpbrk(line,".e")) {
     sscanf(line,"%f",&float_pos);
  } else {
     sscanf(line,"%d",&pos);

  }
  
  if (echo) {
     fprintf(stdout,"%s",line);
     fflush(stdout);
  }
  

  sscanf(line,"%d",&pos);

  XtSetArg(args[n],XtNposition,pos); n++;
  XtSetValues(dial,args,n);

  return;
}

/*
** quit()
**
** This function is attached to the quit button, it is invoked when the
** button is pressed. Basically it ends the application gracefully.
**
*/
#ifndef _NO_PROTO
void quit(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void quit(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
   XtCloseDisplay(XtDisplay(w));
   exit(0);
}


/*
** send()
**
** This function is invoked when the send button is pressed. It sends
** the dial widget a message requesting that the current position value 
** be returned. It then writes the current position to the standard output
**
**
*/
#ifndef _NO_PROTO
void send(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void send(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
  Arg args[10];
  int n;
  Position pos;

  n=0;
  /* Read the current position */
  XtSetArg(args[n], XtNposition, &pos); n++;
  XtGetValues(dial, args, n);

  /* Write the current position to standard output */
  fprintf(stdout,"%d\n",pos);
  fflush(stdout);
}

/*
** reset_callback()
**
** This function is invoked when the reset button is pressed. It reads the
** minimum value of the meter and then sets the indicator position to the
** minimum.
**
*/
#ifndef _NO_PROTO
void reset_callback(Widget parent, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void reset_callback(parent, client_data, call_data)
Widget parent;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{
  int n;
  Position pos;
  Arg args[10];

  n=0;
  /* Read the current minimum value */
  XtSetArg(args[n], XtNminimum, &pos); n++;
  XtGetValues(dial, args, n);
  
  /* Set the position to the minimum */
  n=0;
  XtSetArg(args[n],XtNposition,pos); n++;
  XtSetValues(dial,args,n);

    
}

