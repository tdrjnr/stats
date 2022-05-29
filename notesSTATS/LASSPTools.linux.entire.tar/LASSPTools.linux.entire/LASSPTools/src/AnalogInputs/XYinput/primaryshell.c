#include "xy_plot.h"
#include "menus.h"

#ifndef _NO_PROTO
   void initialize_primary_window(Widget toplevel, XyiAllDataStruct *all)
#else
   void initialize_primary_window(toplevel, all)
      Widget toplevel;
      XyiAllDataStruct *all;
#endif
{
  Widget framework, pointer_data, x_data, y_data;
  Widget drawframe, dataframe, utilframe, help_button, option_button, menu_bar;
  XyiRecStruct *rectangle;
  XyiPolarStruct *polar;
  extern WindowStruct windows;
  int i, n;
  Arg wargs[10];

  framework = XtCreateManagedWidget("framework", xmFormWidgetClass, toplevel, NULL, 0);
  utilframe = XtCreateManagedWidget("utilframe", xmBulletinBoardWidgetClass, framework, NULL, 0);

  windows.quit = XtCreateManagedWidget("quit", xmPushButtonWidgetClass, utilframe, NULL, 0);
 
  menu_bar = XmCreateMenuBar (utilframe,"menubar",NULL, 0);
  XtManageChild(menu_bar);

  drawframe = XtCreateManagedWidget("drawframe", xmFrameWidgetClass, framework, NULL, 0);
  windows.cartesian = XtCreateManagedWidget("cartesian", xmDrawingAreaWidgetClass, drawframe, NULL,0);
   
  dataframe = XtCreateManagedWidget("dataframe", xmFrameWidgetClass, utilframe, NULL, 0);  

  pointer_data = XtCreateManagedWidget("pointer_data", xmRowColumnWidgetClass, dataframe, NULL, 0);
  x_data = XtCreateManagedWidget("x_data", xmLabelWidgetClass, pointer_data, NULL, 0);
  y_data = XtCreateManagedWidget("y_data", xmLabelWidgetClass, pointer_data, NULL, 0);
  
  create_menu_button(NULL, menu_bar,first_popup, XtNumber(first_popup));

  n=0;
  XtSetArg(wargs[n], XtNwidth, 650); n++;
  XtSetArg(wargs[n], XtNheight, 500); n++;
  XtSetValues(framework, wargs, n);

  n=0;
  XtSetArg(wargs[n], XtNwidth, 500); n++;
  XtSetArg(wargs[n], XtNheight, 500); n++;
  XtSetValues(windows.cartesian, wargs, n);

  n=0;
  XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(wargs[n], XmNbottomAttachment, XmATTACH_FORM);n++;
  XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(wargs[n], XtNwidth, 170); n++;
  XtSetValues(utilframe, wargs, n);

  n=0;
  XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(wargs[n], XmNleftWidget, utilframe); n++;
  XtSetArg(wargs[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(wargs[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetValues(drawframe, wargs, n);

  n=0;
  XtSetArg(wargs[n], XtNy, 160); n++;
  XtSetValues(dataframe, wargs, n);

  n=0;
  XtSetArg(wargs[n], XmNspacing, 5); n++;
  XtSetArg(wargs[n], XtNwidth, 120); n++;
  XtSetValues(pointer_data, wargs, n);

  n=0;
  XtSetArg(wargs[n], XtNy, 80); n++;
  XtSetArg(wargs[n], XmNspacing, 15); n++;
  XtSetValues(menu_bar, wargs, n);

  n=0;
  XtSetArg(wargs[n], XtNwidth, 100); n++;
  XtSetArg(wargs[n], XtNy, 20);n++;
  XtSetValues(windows.quit, wargs, n);

  all->tmp1 = x_data;
  all->tmp2 = y_data;

}














