#include "xy_plot.h"

#ifndef _NO_PROTO
   void initial_help(XyiAllDataStruct *all)
#else
   void initial_help(all)
      XyiAllDataStruct *all;
#endif
{
  caddr_t call_data, client_data;

  help(all->tmp1, client_data, call_data);

}


#ifndef _NO_PROTO
   void help(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void help(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  static char *message[] = {
    "Usage:   XYinput [-rec [-xmax {x maximum}] [-xmin {x minimum}] ",
    "                       [-ymax {y maximum}] [-ymin {y minimum}] ]", 
    "                 | [-polar  [-rmax {r maximum}] ",
    "                            [-deg | -rad | -radtimepi] ]  ",
    "                 | [-sphere [-deg | -rad | -radtimepi] ] ",
    "                 [-axis] | [-grid] | [-tick] | [-plain] ",
    "                 [-point] | [-square] | [-circle] ",
    "                 | [-cross] | [-nodisp] ",
    "                 [-help] ",
    "     ",
    "Mouse Buttons:    left   - pipe one set of data out", 
    "                           when the button is pressed",
    "                  center - pipe one set of data out",
    "                           when the button is released",
    "                  right  - pipe data continuously",
    "    ",
    "Defaults:    rectangular coordinates, axis and tick marks, points",
    "             X-range = -1.000000 to 1.000000",
    "             Y-range = -1.000000 to 1.000000",
    ""};

  display_message(w, message);
}


#ifndef _NO_PROTO
   void display_message(Widget w, char **message)
#else
   void display_message(w, message)
      Widget w;
      char **message;
#endif
{
  int i,n;
  Widget dialog;
  Widget label;
  Arg warg[5];
  XmString str;
  extern WindowStruct windows;

  n = 0;
  XtSetArg(warg[n], XmNautoUnmanage, FALSE); n++;
  XtSetArg(warg[n], XtNbackground, windows.color.background.pixel);
  dialog = XmCreateMessageDialog (w,"Help",warg,n);

  XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

  label = XmMessageBoxGetChild(dialog, XmDIALOG_MESSAGE_LABEL);

  n = 0;
  XtSetArg(warg[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
  XtSetValues (label, warg, n);

  XtAddCallback(dialog, XmNokCallback, quit_dialog, NULL);

  for (i = 0; message[i][0] != '\0'; i++); 
  
  str = xs_str_array_to_xmstr(message,i);

  n=0;
  XtSetArg(warg[n], XmNmessageString, str); n++;
  XtSetValues (dialog, warg, n);

  XtManageChild(dialog);
}


#ifndef _NO_PROTO
   XmString xs_str_array_to_xmstr(char *cs[], int n) 
#else
   XmString xs_str_array_to_xmstr(cs, n)
     char *cs[];
     int n;
#endif
{
  XmString xmstr;
  int i;

  if (n <= 0)
    return (XmStringCreate ("", XmSTRING_DEFAULT_CHARSET));

  xmstr = (XmString) NULL;
  
  for (i = 0; i<n; i++) {
    if (i>0)
      xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreate(cs[i],XmSTRING_DEFAULT_CHARSET));
  }
  return (xmstr);
}


#ifndef _NO_PROTO
   void quit_dialog(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void quit_dialog(w,client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  XtUnmanageChild (w);
}













