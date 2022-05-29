#include "xy_plot.h"
#include "menus.h"

WindowStruct windows;

#ifndef _NO_PROTO
   main (unsigned int argc, char *argv[])
#else
   main(argc, argv)
      unsigned int argc;
      char *argv[];
#endif
{
  Widget toplevel;
  XyiAllDataStruct all;
  XyiRecStruct *rectangle;
  XyiPolarStruct *polar;
  extern WindowStruct windows;
  ImageDataStruct graph;


  static char *startup[] = {
    "This program has been updated on 7/18/91",
    " ",
    "If it bombs anywhere, please send me a mail",
    " at cchan@helios.tn.cornell.edu",
    "  ",
    "                Good Luck, Chris Chan",
    ""};

  Arg wargs[8];
  int i,n;

  windows.all = &all;
  windows.image = &graph;
  all.window = &windows;

  /* Set pixmap to null 9-9-92 jwf */
  windows.image->pix =  NULL;
 
  graph.draw_point = NULL;

  default_rec(&all);

/********* Create Primary Display Windows ********************/

  toplevel = XtInitialize(argv[0], "Xy_plot", NULL, 0, &argc, argv);


#ifdef DEBUG
  XSynchronize(XtDisplay(toplevel), 1);
#endif


  initialize_primary_window(toplevel, &all);
 

/******** Parsing input arguments **********************/

  parse_option(&all,argc,argv);   
 

/*********  Add Callbacks and Event Handlers *************/

  XtAddCallback(windows.quit, XmNactivateCallback, quit_xy, NULL);
  callback_config(&windows);
  XtAddEventHandler(windows.cartesian,
                    ButtonPressMask | ButtonReleaseMask, 
                    FALSE, select_output_PR, &all);
  XtAddEventHandler(windows.cartesian,
		    PointerMotionMask,
		    FALSE, select_output_motion, &all);

  XtRealizeWidget(toplevel);

  XGrabButton(XtDisplay(windows.cartesian), AnyButton, AnyModifier, 
	      XtWindow(windows.cartesian), TRUE, 
	      ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync, XtWindow(windows.cartesian), 
	      XCreateFontCursor(XtDisplay(windows.cartesian), XC_crosshair));


/************ initialize Graphics Screen *****************/

  init_color(&windows, toplevel);
  default_graphics(windows.cartesian, &graph);
  (*(all.resize))(windows.cartesian, &(all.data), &graph);

/************ Realize the primary window *****************/

/*  display_message(toplevel, startup); */
/* All this line did was display an annoying window that you had to get */
/* rid of at the start */

  XtMainLoop();
}


#ifndef _NO_PROTO
   void quit_xy(Widget w, caddr_t client_data, caddr_t call_data)
#else
   void quit_xy(w, client_data, call_data)
      Widget w;
      caddr_t client_data, call_data;
#endif
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}


#ifndef _NO_PROTO
   void select_output_PR (Widget w, caddr_t client_data, XButtonEvent *event)
#else
   void select_output_PR (w, client_data, event)
      Widget w;
      caddr_t client_data;
      XButtonEvent *event;
#endif
{
  XyiAllDataStruct *all;
  all = (XyiAllDataStruct *) client_data;

  if (event->button == 2) {
    if (event->type == ButtonRelease) {
      (*(all->track_position))(w,all,event);
      (*(all->send_data))(w,&(all->data),event);
    }
  }
  else if (event->button == 3) {
    (*(all->track_position))(w,all,event);
    (*(all->send_data))(w,&(all->data),event);
  }
  else if (event->button == 1) {
    (*(all->track_position))(w,all,event);
    if (event->type == ButtonPress) (*(all->send_data))(w,&(all->data),event);
  };
  }


#ifndef _NO_PROTO
   void select_output_motion (Widget w, caddr_t client_data, XMotionEvent *event)
#else
   void select_output_motion(w, client_data, event)
      Widget w;
      caddr_t client_data;
      XMotionEvent *event;
#endif
{
  XyiAllDataStruct *all;
  all = (XyiAllDataStruct *) client_data;

  (*(all->track_position))(w,all,event);
  if (event->state == Button3Mask) (*(all->send_data))(w,&(all->data),event);
}


















