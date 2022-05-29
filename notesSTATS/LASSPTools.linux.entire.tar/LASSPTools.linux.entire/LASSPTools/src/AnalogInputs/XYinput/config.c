#include "xy_plot.h"

#ifndef _NO_PROTO
   void callback_config(WindowStruct *window)
#else
   void callback_config(window)
      WindowStruct *window;
#endif
{
  XyiAllDataStruct *all;
  all = window->all;

  XtAddCallback(window->cartesian, XmNresizeCallback, (*(all->resize)), &(all->data));

}


#ifndef _NO_PROTO
   void callback_deconfig(WindowStruct *window)
#else
   void callback_deconfig(window)
      WindowStruct *window;
#endif
{
  XyiAllDataStruct *all;
  all = window->all;

  XtRemoveCallback(window->cartesian, XmNresizeCallback, (*(all->resize)), &(all->data));

}


#ifndef _NO_PROTO
   void reconfig (Widget w, caddr_t client_data, caddr_t call_data)
#else
   void reconfig (w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  extern WindowStruct windows;
#ifndef _NO_PROTO
  void (*func)(XyiAllDataStruct *);
#else
  void (*func) ();
#endif
  func = (void (*)) client_data;
  callback_deconfig(&windows);
  (*func)(windows.all);
  (*(windows.all->resize))(windows.cartesian, windows.all, NULL);
  callback_config(&windows);
}
