/*
** keys.c
**
** Contains functions handling keystrokes.
**
*/
#include <stdio.h>
#include <X11/keysym.h>
#include <Xm/MessageB.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <string.h>
#include "Dial.h"

extern Widget dial;

/*
** key_handler()
**
** This function will be invoked when the user presses a key
** within the dial widget. Currently only the up and down
** arrow keys are supported. This event handler was attached
** in main().
**
*/
void key_handler(parent,client_data,event)
Widget parent;
caddr_t client_data;
XEvent *event;
{
   char buffer[20];
   XComposeStatus compose;
   KeySym keysym;
   Arg args[10];
   int n=0;

   /* Determine if the key was an up or down arrow and adjust the 
      pointer accordingly
   */
   XLookupString(event,buffer,20,&keysym,&compose);
   
   if (keysym == XK_Up) {
      XtSetArg(args[n],XtNposition,-1); n++;
      XtSetValues(dial,args,n);
   } else if  (keysym == XK_Down) {
      n=0;
      XtSetArg(args[n],XtNposition,-2); n++;
      XtSetValues(dial,args,n);
      } 
}
