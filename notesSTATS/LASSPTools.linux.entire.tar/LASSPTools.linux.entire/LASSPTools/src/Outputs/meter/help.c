/*
** help.c
**
** Contains functions and data for help window
**
*/
#include <stdio.h>
#include <Xm/MessageB.h>
char *help_text[] = {
   "             METER Options\n",
   "-min :  specify the starting marker value\n",
   "-max : specify the ending marker value\n",
   "-markers : specify the number of markers\n",
   "-echo : copy input to output (as in a pipe) \n",
   "-label : put a text label on the meter\n\n",
   "The indicator can be rotated by pressing and holding the \n",
   "left mouse button and moving around the dial, the indicator can\n",
   "also be rotated around several\n",
   "times (position is cumulative). The up and down arrow keys\n",
   "cause the position to be incremented or decremented by one unit.\n",
   "The SEND button causes the current position to be output to stdout\n",
   "The RESET button causes the indicator to return to the beginning\n",
   NULL,
};


/*
** help_ok()
**
** This function will be invoked when the OK button on the help dialog
** window is pressed.
**
*/

void help_ok(dialog, dialog_ptr)
Widget dialog, *dialog_ptr;
{
   if (!*dialog_ptr)
      return;          /* Window was already killed */
   XtDestroyWidget(dialog);
   *dialog_ptr = NULL;
}  

/*
** help_callback()
**
** This function will be invoked when the user presses the help button
** on the main window.
**
*/
void help_callback(parent,client_data,event)
Widget parent;
caddr_t client_data;
XEvent *event;
{
    static Widget dialog = NULL;
    static Widget *dialog_ptr = NULL;
    XmString ok;
    Arg args[2];
    char buf[BUFSIZ];
    XmString help_string;
    int i;

    if (dialog) {
       XtPopup(XtParent(dialog), XtGrabNone);
       XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
    } else {
    
       i = 0;
       buf[0] = NULL;
       while (help_text[i] != NULL)
          strcat(buf,help_text[i++]);

         ok = XmStringCreateSimple("OK");
       dialog = XmCreateInformationDialog(parent, "help", args, 0);
       help_string = XmStringCreateLtoR(buf,XmSTRING_DEFAULT_CHARSET);
       XtVaSetValues(dialog, XmNmessageString, help_string, NULL);
       XmStringFree(help_text);

       XtAddCallback(dialog,XmNokCallback,help_ok,&dialog);
       XtAddCallback(dialog,XmNdestroyCallback, help_ok, &dialog);

       XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_CANCEL_BUTTON),
           False);
       XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
           False);
       XtManageChild(dialog);
       XtPopup(XtParent(dialog), XtGrabNone);
  }
}
