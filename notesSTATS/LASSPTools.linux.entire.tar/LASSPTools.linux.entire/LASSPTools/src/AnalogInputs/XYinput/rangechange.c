#include "xy_plot.h"
#include <X11/Shell.h>

#ifndef _NO_PROTO
   void range_change(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void range_change(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  static RangeChangeStruct *change_sphere;
  static double zero = 0;

  extern WindowStruct windows;
  XyiAllDataStruct *all = (XyiAllDataStruct *) windows.all;
  XyiRecStruct *rect = (XyiRecStruct *) all;
  XyiPolarStruct *polar = (XyiPolarStruct *) all;

  static  RangeChangeStruct change_rec[4] = {
    {"X maximum :",NULL,NULL},
    {"X minimum :",NULL,NULL},
    {"Y maximum :",NULL,NULL},
    {"Y minimum :",NULL,NULL}

  };
  static RangeChangeStruct change_polar[2] = {
    {"maximum radius", NULL, NULL},
    {"dummy",NULL,NULL}
  };

  change_rec[0].data = (double *) &(rect->x_max);
  change_rec[0].greater_than = (double *) &(rect->x_min);
  change_rec[1].data = (double *) &(rect->x_min);
  change_rec[2].data = (double *) &(rect->y_max);
  change_rec[2].greater_than = (double *) &(rect->y_min);
  change_rec[3].data = (double *) &(rect->y_max);
 

  
  change_polar[0].data = (double *)&(polar->max);
  change_polar[0].greater_than = (double *) &(zero); 

  if (all->coordinate == XyiRect) 
    range_change_private(w, change_rec, XtNumber(change_rec));
  else if (all->coordinate == XyiPolar)
    range_change_private(w, change_polar, XtNumber(change_polar));
  else if (all->coordinate == XyiSpherical)
    range_change_private(w, change_sphere, 0);

}


#ifndef _NO_PROTO
   void range_change_private(Widget w, RangeChangeStruct *change, int number)
#else
   void range_change_private(w, change, number)
      Widget w;
      RangeChangeStruct *change;
      int number;
#endif
{ 
  static char *no_setting[] = {
    "No range setting is allowed in this coordinate!!!", 
    ""};

  static char *space = {
    "                                                                              "};

  RangeChangeWindowStruct *freedom;  
  Widget *heading, *text, *row;
  Widget board0, board, ok, button, cancel;
  Arg warg[5];
  int i, n;
  char str[20];
  char **spaces;
  XmString strg;
  unsigned int argc = 0;
  char **argv;
  extern WindowStruct windows;

  freedom = (RangeChangeWindowStruct *) malloc(sizeof(RangeChangeWindowStruct));
  heading = (Widget *) malloc (number * sizeof(Widget));
  text = (Widget *) malloc (number * sizeof(Widget));
  row = (Widget *) malloc (number * sizeof(Widget));
  freedom->change = (RangeChangeStruct *) malloc (number * sizeof(RangeChangeStruct));

  freedom->heading = heading;
  freedom->text = text;
  freedom->row = row;
  freedom->number = number;

  if (number == 0) {
    display_message(w, no_setting);
    return;
  }

  freedom->board0 = XtAppCreateShell("Range Change", "range_range", applicationShellWidgetClass, XtDisplay(windows.cartesian), warg, 0);

  board = XtCreateManagedWidget("Range Change", xmRowColumnWidgetClass, freedom->board0, NULL, 0);

  for (i = 0; i < number; i++) {
    row[i] = XtCreateManagedWidget(change[i].label, xmRowColumnWidgetClass, board, NULL, 0);
    n=0;
    XtSetArg(warg[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetValues(row[i], warg, n);

    heading[i] = XtCreateManagedWidget(change[i].label, xmLabelWidgetClass, row[i], NULL ,0);

    text[i] = XtCreateManagedWidget (change[i].label, xmTextWidgetClass, row[i], NULL, 0);
    sprintf(str,"%05f",*(change[i].data));
    XmTextSetString(text[i], str);

    freedom->change[i].data = change[i].data;
    freedom->change[i].greater_than = change[i].greater_than;
  }

  XtCreateManagedWidget("seperator", xmSeparatorWidgetClass, board, NULL, 0);

  button = XtCreateManagedWidget("hello", xmRowColumnWidgetClass, board, NULL, 0);
  n=0;
  XtSetArg(warg[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetValues(button, warg, n);
 
  ok = XtCreateManagedWidget("OK", xmPushButtonWidgetClass, button, NULL, 0);
  cancel = XtCreateManagedWidget("Cancel", xmPushButtonWidgetClass, button, NULL, 0);

  XtAddCallback(cancel, XmNactivateCallback, quit_range_change_private, freedom);
  XtAddCallback(ok, XmNactivateCallback, confirm_changes, freedom);

  XtRealizeWidget(freedom->board0);

}


#ifndef _NO_PROTO
   void quit_range_change_private(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void quit_range_change_private(w,client_data, call_data)
      Widget w;
      caddr_t client_data,call_data;
#endif
{
  RangeChangeWindowStruct *freedom;
  freedom = (RangeChangeWindowStruct *) client_data;

  free(freedom->heading);
  free(freedom->text);
  free(freedom->row);

  XtDestroyWidget(freedom->board0);

  free(freedom);
}


#ifndef _NO_PROTO
   void confirm_changes (Widget w, caddr_t client_data, caddr_t call_data)
#else
   void confirm_changes (w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{

  static char *error_message[] = {
    "Please check your boundary again!",
    ""};

  int i;
  char *str;
  double *value;
  RangeChangeWindowStruct *freedom;
  RangeChangeStruct *change;
  extern WindowStruct windows;

  freedom = (RangeChangeWindowStruct *) client_data;
  change = freedom->change;

  value = (double *) malloc (freedom->number * sizeof(double));
  
  for (i = 0; i < freedom->number; i++)  {
    str = XmTextGetString((freedom->text)[i]);
    value[i] = *(change[i].data);
    *(change[i].data) = (double) atof(str);
  }

  for (i = 0; i < freedom->number; i++) {
    if (change[i].greater_than != NULL) {
      if (*(change[i].data) < *(change[i].greater_than)) {
	for (i = 0; i < freedom->number; i++) {
	  *(change[i].data) = value[i];
	  free(value);
	  display_message(w, error_message);
	  return;
	}
      }
    }
  }

  (*(windows.all->resize))(windows.cartesian, windows.all, NULL);
  quit_range_change_private (w, client_data, call_data);

}
	
