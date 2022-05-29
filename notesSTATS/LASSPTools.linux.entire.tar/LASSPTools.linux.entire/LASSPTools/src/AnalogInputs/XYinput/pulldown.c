#include "xy_plot.h"
#include "menus.h"

#ifndef _NO_PROTO
   void create_menu_button(char *title, Widget  menu, MenuStruct *menulist, int nterm)
#else
   void create_menu_button(title, menu, menulist, nterm)
      char *title;
      Widget menu;
      MenuStruct *menulist;
      int nterm;
#endif
{
  Arg warg[5];
  int i;
  WidgetList buttons;
  int separator = 0;

  buttons = (WidgetList) XtMalloc(nterm * sizeof(Widget));

  if (title) {
    XtCreateManagedWidget(title, xmLabelWidgetClass, menu, NULL, 0);
    XtCreateManagedWidget("seperator", xmSeparatorWidgetClass, menu, NULL, 0);
  }
  
  for (i = 0; i<nterm; i++) {
    if (menulist[i].name == NULL) {
      XtCreateManagedWidget("separator", xmSeparatorWidgetClass, menu, NULL,0);
      separator ++;
    }
    else if (menulist[i].func) {
      buttons[i-separator] = XtCreateWidget(menulist[i].name,xmPushButtonWidgetClass, menu, NULL, 0);
      XtAddCallback(buttons[i-separator], XmNactivateCallback, menulist[i].func, menulist[i].data);
    }
    else if (!menulist[i].submenu) {
      buttons[i-separator] = XtCreateWidget(menulist[i].name, xmLabelWidgetClass, menu, NULL, 0);
    }
    else {
      Widget sub;
      sub = XmCreatePulldownMenu (menu, menulist[i].submenu_title, NULL, 0);
      XtSetArg(warg[0], XmNsubMenuId, sub);
      buttons[i-separator] = XtCreateWidget(menulist[i].name, xmCascadeButtonWidgetClass, menu, warg, 1);
      create_menu_button(menulist[i].submenu_title, sub, menulist[i].submenu, menulist[i].submenu_entries);
    }
  }
  XtManageChildren(buttons, nterm - separator);
}



