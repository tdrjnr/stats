/*
** grab.c
**
** This file contains functions for tracking the mouse postion within the
** meter window and then sending values.
**
*/
#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif


#include <stdio.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <Xm/XmP.h>
#include <Xm/TextFP.h>
#include "DialP.h"
#include "Dial.h"

#define  RADIANS(x)  (M_PI * 2.0 * (x) / 360.0)
#define  DEGREES(x)  ((x) / (M_PI * 2.0) * 360.0)
#define  MIN_ANGLE   225.0
#define  MAX_ANGLE   270.0
#define  MIN(a,b)    (((a) < (b)) ? (a) :  (b))
#define  DRAW_IT     TRUE
#define  ERASE_IT    FALSE


#ifndef _NO_PROTO
extern void StartTracking(XsDialWidget ArrowArea, XtPointer clientData,
                   XEvent *theEventPtr);
extern void EndTracking(XsDialWidget ArrowArea, XtPointer clientData,
                 XEvent *theEventPtr);
extern void TrackMousePosition(XsDialWidget ArrowArea, XtPointer clientData,
                        XEvent *theEventPtr);
extern int update_indicator(XsDialWidget ArrowArea, XtPointer clientData,
                        XEvent *theEventPtr);

#else
extern void StartTracking();
extern void TrackMousePosition();
extern void EndTracking();
extern int  update_indicator();
#endif

extern Widget dial;
extern Boolean continuous_output, discrete_output;
static double previous_angle = 0;
static Boolean ignore_crossing;
/*
** CreateMouseTracker()
**
** This function sets up the widget so that it will receive mouse
** events.
**
*/
#ifndef _NO_PROTO
void CreateMouseTracker(XsDialWidget ArrowArea)
#else
void CreateMouseTracker(ArrowArea)
XsDialWidget ArrowArea;
#endif
{
  XtAddEventHandler(ArrowArea, ButtonPressMask, FALSE,
                    (XtEventHandler) StartTracking, NULL);
  XtAddEventHandler(ArrowArea, ButtonMotionMask, FALSE,
                    (XtEventHandler) TrackMousePosition, NULL);
  XtAddEventHandler(ArrowArea, ButtonReleaseMask, FALSE,
                     (XtEventHandler) EndTracking, NULL);
}

/*
** StartTracking()
**
** This function will begin following the mouse pointer.
**
*/
#ifndef _NO_PROTO
void StartTracking(XsDialWidget ArrowArea, XtPointer clientData,
                   XEvent *theEventPtr)
#else
void StartTracking(ArrowArea, clientData, theEventPtr)
XsDialWidget ArrowArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{ 
  int x,y,pos;
  Arg args[10];
  double angle;
  int n = 0;

  x = theEventPtr->xbutton.x;
  y = theEventPtr->xbutton.y;

  /* Check if the indicator is outside the dial face, if it is then 
  ** ignore the first crossing event 
  */
  angle=atan2((double)(theEventPtr->xbutton.y - ArrowArea->dial.center_y),
               (double)(theEventPtr->xbutton.x - ArrowArea->dial.center_x));

  angle = DEGREES(angle) - (MIN_ANGLE - 90.0);
  if (angle < 0)
     angle += 360.0;

  if (angle > (double)MAX_ANGLE)
     ignore_crossing = True;
  else
     ignore_crossing  = False; 
  update_indicator(ArrowArea, clientData, theEventPtr);
  

}

/*
** EndTracking()
**
** This is invoked when the mouse button is released.
**
*/
#ifndef _NO_PROTO
void EndTracking(XsDialWidget ArrowArea, XtPointer clientData,
                 XEvent *theEventPtr)
#else
void EndTracking(ArrowArea, clientData, theEventPtr)
XsDialWidget ArrowArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{
  int x,y,position;
  char line [10];

   
  x = theEventPtr->xbutton.x;
  y = theEventPtr->xbutton.y;


  /* Send the results to the output pipe or queue */
  if (((x >=0) && (x <= ArrowArea->core.width)) &&
     ((y >=0) && (y <= ArrowArea->core.height))) {
       position = update_indicator(ArrowArea,clientData, theEventPtr);
       if (discrete_output) {
          sprintf(line,"%d\n",position);
          fputs(line,stdout);
       }  
   }

  
}



/*
** TrackMousePosition()
**
** Follow the mouse as it is moved, this function is invoked whenever the
** pointer is moved after the left mouse button has been held down while
** in the dial window.
*/
#ifndef _NO_PROTO
void TrackMousePosition(XsDialWidget ArrowArea, XtPointer clientData,
                        XEvent *theEventPtr)
#else
void TrackMousePosition(ArrowArea, clientData, theEventPtr)
XsDialWidget ArrowArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{

  int x,y,position;
  char line[10];

  x = theEventPtr->xbutton.x;
  y = theEventPtr->xbutton.y;

  

  /* Calculate a line from the mouse position to the center of the 
  ** meter. This line intersects the outer dial circle at some point,
  ** this is the value that will be used in displaying the digital value
  ** as well as the indicator.
  */
  
  /* Check if the position is inside the window */
  if (((x >=0) && (x <= ArrowArea->core.width)) &&
      ((y >=0) && (y <= ArrowArea->core.height))) {
        position = update_indicator(ArrowArea,clientData, theEventPtr);
        if (continuous_output) {
           sprintf(line,"%d\n",position);
           fputs(line,stdout);
       }  
  } 
     
}

/*
** update_indicator()
**
** This function will redraw the indicator when it has been moved 
** by dragging the mouse.
*/
#ifndef _NO_PROTO
int update_indicator(XsDialWidget ArrowArea, XtPointer clientData,
                        XEvent *theEventPtr)
#else
int update_indicator(ArrowArea, clientData, theEventPtr)
XsDialWidget ArrowArea;
XtPointer clientData;
XEvent *theEventPtr;
#endif
{
  
  
  int pos,midpoint,old_pos,origpos;
  Position temp_pos,real_position;
  double angle, midangle;
  Arg args[10];
  int n = 0;

  old_pos = pos = ArrowArea->dial.position;

  /*
  ** Get the angle in radians.
  */
  angle=atan2((double)(theEventPtr->xbutton.y - ArrowArea->dial.center_y),
               (double)(theEventPtr->xbutton.x - ArrowArea->dial.center_x));

  
  /*
  ** Convert to degrees from the MIN_ANGLE.
  */
  angle = DEGREES(angle) - (MIN_ANGLE - 90.0);
  if (angle < 0)
     angle += 360.0;

  
  midpoint = (ArrowArea->dial.maximum - ArrowArea->dial.minimum) / 2;
  midangle = (double) (MAX_ANGLE / 2);

  /* Check if the pointer is within the numbered part of the meter */ 
  if (((angle >= (double)0) && (angle <= (double)MAX_ANGLE))) {
     if (ignore_crossing)
        ignore_crossing = False;
     else {     
     /*
     ** Convert the angle to a position, between min and max.
     */
     pos = origpos = ArrowArea->dial.minimum + (angle /
           MAX_ANGLE * (ArrowArea->dial.maximum - ArrowArea->dial.minimum));
     
     /* We just crossed clockwise from 0 */
     if ((previous_angle > (double)MAX_ANGLE) && (angle < midangle)) {
          
          /* Adjust previous position to be at the end of the dial and then 
             add in the new position 
          */
          if (ArrowArea->dial.position % ArrowArea->dial.maximum)
             pos = ArrowArea->dial.position + (ArrowArea->dial.maximum - 
               (ArrowArea->dial.position % ArrowArea->dial.maximum)) + pos -
                ArrowArea->dial.minimum;
          else
             pos = ArrowArea->dial.position + pos - ArrowArea->dial.minimum;

          
     } else 
         if ((previous_angle > (double)MAX_ANGLE) && (angle >= midangle)) {
          
           if (ArrowArea->dial.position % ArrowArea->dial.maximum) {
              pos = ArrowArea->dial.position - 
                    (ArrowArea->dial.position % ArrowArea->dial.maximum);
              /* Now bring the position back from */
              pos = pos - (ArrowArea->dial.maximum - origpos);
           } else
              pos = ArrowArea->dial.position;

            

         } else {
            /* This is just a normal move */
          
            if (ArrowArea->dial.position % ArrowArea->dial.maximum)
               
               pos = pos + ((ArrowArea->dial.position / ArrowArea->dial.maximum) *
                         ArrowArea->dial.maximum);
              
            else
               if (angle > midangle) {
                 pos = pos + ((ArrowArea->dial.position / 
                     ArrowArea->dial.maximum) - 1 )*ArrowArea->dial.maximum;
               } else 
                  pos = pos + ((ArrowArea->dial.position / ArrowArea->dial.maximum) *
                    ArrowArea->dial.maximum);
	  }
         
      }
   } 

   
   if (pos != old_pos) {
      
     
     /*
     ** Change the dial to this new position 
     */
     n=0;
     XtSetArg(args[n],XtNposition,pos); n++;
     XtSetValues(dial,args,n);
   }
  
   previous_angle = angle;
   
   return(pos);

}
