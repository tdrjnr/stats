#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

#ifdef X11R3
#ifdef __STDC__
typedef void*           XtPointer;
#else
typedef char*           XtPointer;
#endif
#endif


/*==========================================================================*/

#ifndef _NO_PROTO
static void arm_callback(Widget w, XtPointer *flag, 
			 XmAnyCallbackStruct call_data)
#else
static void arm_callback(w, flag, call_data)
Widget w;
XtPointer *flag;
XmAnyCallbackStruct call_data;
#endif
{
  *(Boolean *)flag = FALSE;
}

/*==========================================================================*/

#ifndef _NO_PROTO
static void activate_callback(Widget w, XtPointer *flag,
			      XmAnyCallbackStruct call_data)
#else
static void activate_callback(w, flag, call_data)
Widget w;
XtPointer *flag;
XmAnyCallbackStruct call_data;
#endif
{
 *(Boolean *)flag = TRUE;
}

/*==========================================================================*/

#ifndef _NO_PROTO
static void disarm_callback(Widget w, XtPointer *flag,
			    XmAnyCallbackStruct call_data)
#else
static void disarm_callback(w, flag, call_data)
Widget w;
XtPointer *flag;
XmAnyCallbackStruct call_data;
#endif
{
  if(*(Boolean *)flag){
    XtCloseDisplay(XtDisplay(w));
    exit(0);
  }
}
#ifndef _NO_PROTO
static void disarm_reset_callback(Widget w, XtPointer *flag,
			    XmAnyCallbackStruct call_data)
#else
static void disarm_reset_callback(w, flag, call_data)
Widget w;
XtPointer *flag;
XmAnyCallbackStruct call_data;
#endif
{
  if(*(Boolean *)flag){
    set_initial(NULL);
    HandleResize(NULL,NULL,NULL);
    HandleExposure(NULL,NULL,NULL);
    PrintRotationMatrix();
  }
}

/*==========================================================================*/

#ifndef _NO_PROTO
Widget CreateQuitButton(Widget parent)
#else
Widget CreateQuitButton(parent)
Widget parent;
#endif
{
   Widget     w;

   static int reallyQuit;

   
   w = XtCreateManagedWidget("quit", xmPushButtonWidgetClass, parent, NULL, 0);
   XtAddCallback(w, XmNarmCallback, 
                (XtCallbackProc) arm_callback, (XtPointer) &reallyQuit);
   XtAddCallback(w, XmNdisarmCallback, 
                (XtCallbackProc) disarm_callback, (XtPointer) &reallyQuit);
   XtAddCallback(w, XmNactivateCallback, 
                (XtCallbackProc) activate_callback, (XtPointer) &reallyQuit);     
   return (w);
}

#ifndef _NO_PROTO
Widget CreateResetButton(Widget parent)
#else
Widget CreateResetButton(parent)
Widget parent;
#endif
{
   Widget     w;
   Arg        wargs[10];
   int n;
   Dimension width, height,border,pwidth,pheight,xpos;
   static int reallyReset;

   
   w = XtCreateManagedWidget("Reset", xmPushButtonWidgetClass, parent, NULL, 0);
   /* Get the size of the widget */
   n=0;
   XtSetArg(wargs[n], XmNwidth, &width); n++;
   XtSetArg(wargs[n], XmNheight, &height); n++;
   XtSetArg(wargs[n], XmNborderWidth, &border); n++;
   XtGetValues(w, wargs,n);
   /* Get the size of the parent */
   n = 0;
   XtSetArg(wargs[n], XmNwidth, &pwidth); n++;
   XtSetArg(wargs[n], XmNheight, &pheight); n++;
   XtGetValues(parent, wargs,n);   
   /* Change the location of the pushbutton */
   xpos = pwidth-width-border;
   XtSetArg(wargs[0],XmNx,xpos);
   XtSetValues(w,wargs,1);
   
   XtAddCallback(w, XmNarmCallback, 
                (XtCallbackProc) arm_callback, (XtPointer) &reallyReset);
   XtAddCallback(w, XmNdisarmCallback, 
                (XtCallbackProc) disarm_reset_callback, (XtPointer) &reallyReset);
   XtAddCallback(w, XmNactivateCallback, 
                (XtCallbackProc) activate_callback, (XtPointer) &reallyReset);     
   return (w);
}

/*==========================================================================*/
