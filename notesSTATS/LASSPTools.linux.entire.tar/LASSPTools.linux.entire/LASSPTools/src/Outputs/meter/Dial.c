/**********************************************************************************
 * Dial.c: The Dial Widget
  *         From:
  *                   The  Window System, 
  *            Programming and Applications with Xt
  *                   OSF/Motif Edition
  *         by
  *                Douglas Young
  *              Prentice Hall, 1990
  *
  *                 Example described on pages:  344-362
  *
  *
  *  Copyright 1989 by Prentice Hall
  *  All Rights Reserved
  *
  * This code is based on the OSF/Motif widget set and the X Window System
  *
  * Permission to use, copy, modify, and distribute this software for 
  * any purpose and without fee is hereby granted, provided that the above
  * copyright notice appear in all copies and that both the copyright notice
  * and this permission notice appear in supporting documentation.
  *
  * Prentice Hall and the author disclaim all warranties with regard to 
  * this software, including all implied warranties of merchantability and fitness.
  * In no event shall Prentice Hall or the author be liable for any special,
  * indirect or cosequential damages or any damages whatsoever resulting from 
  * loss of use, data or profits, whether in an action of contract, negligence 
  * or other tortious action, arising out of or in connection with the use 
  * or performance of this software.
  *
  * Open Software Foundation is a trademark of The Open Software Foundation, Inc.
  * OSF is a trademark of Open Software Foundation, Inc.
  * OSF/Motif is a trademark of Open Software Foundation, Inc.
  * Motif is a trademark of Open Software Foundation, Inc.
  * DEC is a registered trademark of Digital Equipment Corporation
  * HP is a registered trademark of the Hewlett Packard Company
  * DIGITAL is a registered trademark of Digital Equipment Corporation
  * X Window System is a trademark of the Massachusetts Institute of Technology
  **********************************************************************************/

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

/* static void    select_dial(); */
static void    Initialize();
static void    Redisplay();
static void    Resize();
static void    Destroy();
static Boolean SetValues();
extern  void    draw_or_erase_indicator();
static Boolean GetValues();

static char defaultTranslations[] = "<Btn1Down>: select()";

/*
**
** select_dial()
**
** This proceedure was moved to the beginning to prevent the actions list
** declaration from redefining the identifier "select_dial"
*/
static void select_dial(w, event, args, n_args)
     XsDialWidget   w;
     XEvent        *event;
     char          *args[];
     int            n_args;
{
  Position   pos;
  double     angle;
  xsdialCallbackStruct cb;
  
  pos = w->dial.position;
  if(event->type == ButtonPress || 
         event->type == MotionNotify){
    /* 
     * Get the angle in radians.
     */
   angle=atan2((double)(event->xbutton.y - w->dial.center_y),
               (double)(event->xbutton.x - w->dial.center_x));
   /*
    * Convert to degrees from the MIN_ANGLE.
    */ 
   angle = DEGREES(angle) - (MIN_ANGLE - 90.0); 
   if (angle < 0)
     angle = 360.0 + angle;
   /*  
    * Convert the angle to a position. 
    */ 
   pos = w->dial.minimum + (angle / 
             MAX_ANGLE * (w->dial.maximum - w->dial.minimum));
 }  
 
 /*
   * Invoke the callback, report the position in the call_data
   * structure
   */  
  cb.reason   = Xs_SELECTED;
  cb.event    = event;
  cb.position = pos;
  XtCallCallbacks (w, XtNselectCallback, &cb); 
} 

static XtActionsRec actionsList[] = {
  { "select",   (XtActionProc) select_dial},
};


static XtResource resources[] = {
  {XtNmarkers, XtCMarkers, XtRInt, sizeof (int),
    XtOffset(XsDialWidget, dial.markers), XtRString, "11"  },
  {XtNminimum, XtCMin, XtRInt, sizeof (int),
    XtOffset(XsDialWidget, dial.minimum), XtRString, "0"   },
  {XtNmaximum, XtCMax, XtRInt, sizeof (int),
    XtOffset(XsDialWidget, dial.maximum), XtRString, "100" },
  {XtNindicatorColor, XtCColor, XtRPixel, sizeof (Pixel),
    XtOffset(XsDialWidget, dial.indicator_color), 
    XtRString, "red"                                     },
  {XtNposition, XtCPosition, XtRPosition, sizeof (Position),
    XtOffset(XsDialWidget, dial.position), XtRString, "0"  },
  {XtNmarkerLength,XtCLength,XtRDimension,sizeof (Dimension),
    XtOffset(XsDialWidget, dial.marker_length),
    XtRString, "5"                                         },
  {XtNforeground, XtCForeground, XtRPixel, sizeof (Pixel),
    XtOffset(XsDialWidget, dial.foreground), 
    XtRString, "Black"                                     },
  {XtNselectCallback,XtCCallback,XtRCallback,sizeof(caddr_t),
    XtOffset (XsDialWidget, dial.select), 
    XtRCallback, NULL                                      },
  {XtNfont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
    XtOffset(XsDialWidget, dial.font), XtRString, "Fixed" },
  {XtNtitle, XtCTitle, XtRString, sizeof (char *),
    XtOffset(XsDialWidget, dial.title), XtRString, "Meter"} 
    
};

XsDialClassRec  XsdialClassRec = {
     /* CoreClassPart */
  { 
   (WidgetClass) &xmPrimitiveClassRec,  /* superclass            */
   "Dial",                         /* class_name            */
   sizeof(XsDialRec),              /* widget_size           */
   NULL,                           /* class_initialize      */
   NULL,                           /* class_part_initialize */
   FALSE,                          /* class_inited          */
   Initialize,                     /* initialize            */
   NULL,                           /* initialize_hook       */
   XtInheritRealize,               /* realize               */
   actionsList,                    /* actions               */
   XtNumber(actionsList),          /* num_actions           */
   resources,                      /* resources             */
   XtNumber(resources),            /* num_resources         */
   NULLQUARK,                      /* xrm_class             */
   TRUE,                           /* compress_motion       */
   TRUE,                           /* compress_exposure     */
   TRUE,                           /* compress_enterleave   */
   TRUE,                           /* visible_interest      */
   Destroy,                        /* destroy               */
   Resize,                         /* resize                */
   Redisplay,                      /* expose                */
   SetValues,                      /* set_values            */
   NULL,                           /* set_values_hook       */
   XtInheritSetValuesAlmost,       /* set_values_almost     */
   NULL,                           /* get_values_hook       */
   NULL,                           /* accept_focus          */
   XtVersion,                      /* version               */
   NULL,                           /* callback private      */
   defaultTranslations,            /* tm_table              */
   NULL,                           /* query_geometry        */
   XtInheritDisplayAccelerator,    /* display_accelerator   */
   NULL,                           /* extension             */
   },

      /* PrimitiveClasspart */
  {
   _XtInherit,                     /* border_highlight      */
   _XtInherit,                     /* border unhighlight    */
   defaultTranslations,            /* translations          */
   NULL,                           /* arm_and_activate      */
   NULL,                           /* syn_resources         */
   0,                              /* num syn resources     */
   NULL,                           /* extension             */
  },

       /* Dial class fields */
  {
   0,                               /* ignore                */
  },

       /* TextField class fields */
  {
   NULL                             /* Pointer to extension  */
   }
};

WidgetClass XsdialWidgetClass = (WidgetClass) &XsdialClassRec;

/*
** Initialize()
**
*/
static void Initialize (request, new)
    XsDialWidget request, new;
{
  XGCValues values;
  XtGCMask  valueMask;
  short temp;
  Arg args[20];
  int n=0;

  n=0;
  XtSetArg(args[n], XmNshadowThickness, &temp);n++;
  XtGetValues(new, args, n);
  
  XSetLineAttributes(XtDisplay(new), new->primitive.top_shadow_GC, 
     new->primitive.shadow_thickness, LineSolid, CapButt, JoinMiter);

  XSetLineAttributes(XtDisplay(new), new->primitive.bottom_shadow_GC, 
     new->primitive.shadow_thickness, LineSolid, CapButt, JoinMiter);

  /*
   * Make sure the window size is not zero. The Core 
   * Initialize() method doesn't do this.
   */

  
  if (request->core.width == 0)
    new->core.width = 250;
  if (request->core.height == 0)
    new->core.height = 185;
  
  /*
   * Make sure the min and max dial settings are valid.
   */
  if (new->dial.minimum >= new->dial.maximum) {
    new->dial.minimum = new->dial.maximum - 1;
  }

  if (new->dial.position < new->dial.minimum) {
    new->dial.position =  new->dial.minimum;
  }
  /*
   * Allow only MAXSEGMENTS / 2 markers
   */
  if(new->dial.markers > MAXSEGMENTS / 2){
    XtWarning ("Too many markers");
    new->dial.markers = MAXSEGMENTS / 2;
  }
  /*
   * Create the graphics contexts used for the dial face 
   * and the indicator.
   */
  valueMask = GCForeground | GCBackground;


  values.foreground = new->dial.foreground;
  values.background = new->core.background_pixel;
  new->dial.dial_GC = XtGetGC (new, valueMask, &values);  

  values.foreground = new->dial.indicator_color;
  new->dial.indicator_GC = XtGetGC (new, valueMask,&values);  

  valueMask = GCForeground | GCBackground;
  values.foreground = new->core.background_pixel;
  values.background = new->dial.indicator_color;
  new->dial.inverse_GC = XtGetGC (new, valueMask, &values);

  /* Creat the label with the dial as the parent ?? 
  new->dial.label_widget = XtCreateManagedWidget("Label",xmLabelWidgetClass,
      new, NULL, 0);
  */
  
  Resize (new);
}


/*
** Destroy()
**
*/
static void Destroy (w)
     XsDialWidget w;
{
  XtReleaseGC (w, w->dial.indicator_GC);
  XtReleaseGC (w, w->dial.dial_GC);
  XtReleaseGC (w, w->dial.inverse_GC);
  XtRemoveAllCallbacks (w, XtNselectCallback);
}


/*
** Resize()
**
*/
static void Resize (w)
    XsDialWidget w;
{
  double    angle, cosine, sine, increment;
  int       i; 
  XPoint   *ptr;
  /*
   * Get the address of the first line segment.
   */
  ptr = w->dial.segments;
  /*
   * calculate the center of the widget
   */
  w->dial.center_x = w->core.width/2; 
  w->dial.center_y = w->core.height/2;   
  /* 
   *  Generate the segment array containing the    
   *  face of the dial.    
   */ 
  increment = RADIANS(MAX_ANGLE) /(float)(w->dial.markers -1);
  w->dial.outer_diam = (MIN(w->core.width, w->core.height) / 2) - 40;
  w->dial.inner_diam=w->dial.outer_diam-w->dial.marker_length;
  angle = RADIANS(MIN_ANGLE);  

  for (i = 0; i < w->dial.markers;i++){   
    cosine = cos(angle);   
    sine   = sin(angle); 
    ptr->x   = w->dial.center_x + w->dial.outer_diam * sine; 
    ptr++->y = w->dial.center_y - w->dial.outer_diam * cosine;
    ptr->x   = w->dial.center_x + w->dial.inner_diam * sine; 
    ptr++->y = w->dial.center_y - w->dial.inner_diam * cosine;
    angle += increment; 
  }  
 calculate_indicator_pos(w);
 CreateMouseTracker(w); 
} 

/*
** calculate_indicator_pos()
**
**
*/
static calculate_indicator_pos(w)
     XsDialWidget w;
{
  double   normalized_pos, angle, tempx_len, tempy_len;
  Position indicator_length,arm_length, real_position;
  /*
   * Make the indicator two pixels shorter than the  
   * inner edge of the markers.
   */
  indicator_length=w->dial.outer_diam-w->dial.marker_length-2;

  /* Make the arm lengths be 20 persent of the main length - this should
     give a consistent look when the window is resized 
  */
  arm_length = indicator_length * 0.20;
  /*
   * Normalize the indicator position to lie between zero
   * and 1, and then convert it to an angle.
   */
  if (w->dial.position > w->dial.maximum)
     real_position = w->dial.position % w->dial.maximum;
  else 
     real_position = w->dial.position;

  normalized_pos = (real_position - w->dial.minimum)/
                 (float)(w->dial.maximum - w->dial.minimum);
  angle = RADIANS(MIN_ANGLE + MAX_ANGLE  * normalized_pos);  
   /*
    * Find the x,y coordinates of the tip of the indicator.   
    */ 
   w->dial.indicator_x = w->dial.center_x + 
                               indicator_length * sin(angle); 
   w->dial.indicator_y = w->dial.center_y - 
                               indicator_length  * cos(angle);

   /* Make the arms of the dial be +-90 degress from the main part */
   w->dial.arm1_x = w->dial.center_x + arm_length * sin(angle+(M_PI/2));
   w->dial.arm1_y = w->dial.center_y - arm_length * cos(angle+(M_PI/2));
   w->dial.arm2_x = w->dial.center_x + arm_length * sin(angle-(M_PI/2));
   w->dial.arm2_y = w->dial.center_y - arm_length * cos(angle-(M_PI/2));
}
 
/*
** Redisplay()
**
*/
static void Redisplay (w, event, region)
     XsDialWidget  w;
     XEvent       *event;
     Region        region;
{
  short x_pos,y_pos;
  unsigned short width, height;
  int i,k,x1,y1;
  char buf[10];
  int temp,inc,center;
     
  /* Set the correct linestyle */
  XSetLineAttributes(XtDisplay(w), w->dial.dial_GC, 3, LineSolid,
        CapRound, JoinMiter);

  XSetLineAttributes(XtDisplay(w), w->dial.indicator_GC, 0, LineSolid, 
        CapRound, JoinMiter);

  if(w->core.visible){
    /* Try to draw the 3D Motif box here ??? */
   
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w->primitive.top_shadow_GC,
	      0,
	      0,
	      w->core.width,
              0);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w->primitive.top_shadow_GC,
	      0,
	      0,
	      0,
              w->core.height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w->primitive.bottom_shadow_GC,
	      w->core.width,
	      0,
	      w->core.width,
              w->core.height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w->primitive.bottom_shadow_GC,
	      w->core.width,
	      w->core.height,
	      0,
              w->core.height);
  
    /*
     * Draw the markers used for the dial face.
     */
   
    XDrawSegments(XtDisplay(w), XtWindow(w),
                 w->dial.dial_GC, 
                 w->dial.segments,
                 w->dial.markers);
    
    draw_or_erase_indicator(w,DRAW_IT);  
      
    /* Try Some text */
    temp = w->dial.minimum;
    inc = (w->dial.maximum - w->dial.minimum) / (w->dial.markers - 1);
    for (i=0,k=0;i<w->dial.markers;i++,k+=2) {

       /* Check if this is the last marker, if it is then label it as the last */
       if (i == (w->dial.markers-1))
          temp = w->dial.maximum;

       sprintf(buf,"%d",temp);
       temp+=inc;
       width = XTextWidth(w->dial.font,buf,strlen(buf));

       /* Three possible positions:
       **    bottom right corner of text is on end of marker
       **        (this woule be for the marks on left side of dial)
       **      middle bottom of text string
       **         (top of dial)
       **      left bottom corner of text is on end of marker)
       **         (this would be for the marks on the right side of the dial)
       */
       if (w->dial.segments[k].x < w->dial.segments[k+1].x) {
         /* Point is on left of dial */
         x_pos = w->dial.segments[k].x - width - 5;
         y_pos = w->dial.segments[k].y;
       } else {
          if (w->dial.segments[k].x == w->dial.segments[k+1].x) {
             /* Point is on top of dial */
             x_pos = w->dial.segments[k].x - (width/2);
             y_pos = w->dial.segments[k].y - 5;
          } else {
             /* Point is on right of dial */
             x_pos = w->dial.segments[k].x + 5;
             y_pos = w->dial.segments[k].y;
          }
       }
       
       XDrawImageString(XtDisplay(w), XtWindow(w), w->dial.dial_GC,
		     x_pos, y_pos , buf, strlen(buf));
    }
    

  }
  /* Put Up the Title for this meter */
  width = XTextWidth(w->dial.font,w->dial.title,strlen(w->dial.title));
  center = w->core.width/2 - (width/2);
  XDrawImageString(XtDisplay(w), XtWindow(w), w->dial.dial_GC, 
    center, 25, w->dial.title, strlen(w->dial.title)); 
}

static Boolean SetValues (current, request, new)
     XsDialWidget current, request, new;
{
  XGCValues  values;
  int width, height, x_pos, y_pos;
  XtGCMask   valueMask;
  Boolean    redraw = FALSE;
  Boolean    redraw_indicator = FALSE;

   
  
  if (new->dial.position == -1)
       new->dial.position = current->dial.position + 1;
  
  if (new->dial.position == -2)
       new->dial.position = current->dial.position - 1;
 
  /*
   * Make sure the new dial values are reasonable.
   */
  if (new->dial.minimum >= new->dial.maximum) {
    new->dial.minimum = 0;
    new->dial.maximum = 100;
  }

  if (new->dial.position < new->dial.minimum) {
     new->dial.position = new->dial.minimum;
  }

  /*
   * If the indicator color or background color 
   * has changed, generate the GC's.
   */
  if(new->dial.indicator_color!=current->dial.indicator_color||
     new->core.background_pixel !=current->core.background_pixel){
    valueMask = GCForeground | GCBackground;
    values.foreground = new->dial.indicator_color;
    values.background = new->core.background_pixel;
    XtReleaseGC(new, new->dial.indicator_GC);
    new->dial.indicator_GC = XtGetGC(new, valueMask,&values);
    values.foreground = new->core.background_pixel;
    values.background = new->dial.indicator_color;
    XtReleaseGC(new, new->dial.inverse_GC);
    new->dial.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw_indicator = TRUE;     
  }
  /*
   * If the marker color has changed, generate the GC.
   */
  if (new->dial.foreground != current->dial.foreground){
    valueMask = GCForeground | GCBackground;
    values.foreground = new->dial.foreground;
    values.background = new->core.background_pixel;
    XtReleaseGC(new, new->dial.dial_GC);
    new->dial.dial_GC = XtGetGC (new, valueMask, &values);   
    redraw = TRUE;     
  }
  /*
   * If the indicator position has changed, or if the min/max
   * values have changed, recompute the indicator coordinates.
   */
  if (new->dial.position != current->dial.position ||
      new->dial.minimum != current->dial.minimum ||
      new->dial.maximum != current->dial.maximum){
    calculate_indicator_pos(new);
    redraw_indicator = TRUE;
  }
  /*
   * If only the indicator needs to be redrawn and
   * the widget is realized, erase the current indicator
   * and draw the new one.
   */
  if(redraw_indicator && ! redraw &&
     XtIsRealized(new) && new->core.visible){
     draw_or_erase_indicator(current,ERASE_IT);
     draw_or_erase_indicator(new,DRAW_IT);
  } 
  return (redraw); 
} 

/*
** draw_or_erase_indicator()
**
** Draw the current indicator or erase it 
*/
void draw_or_erase_indicator(w,draw)
XsDialWidget w;
Boolean draw;
{
    GC gc;
    XPoint points[3];
    char buf[10];
    int xpos,ypos,width;
   
    strcpy(buf,"         ");
    if (draw)
       gc = w->dial.indicator_GC;
    else
       gc = w->dial.inverse_GC;

    /* Draw one arm and the side of the arrow  */
    XDrawLine(XtDisplay(w), XtWindow(w),
	      gc,
	      w->dial.center_x,
	      w->dial.center_y,
	      w->dial.arm1_x,
              w->dial.arm1_y);

    XDrawLine(XtDisplay(w), XtWindow(w),
	      gc,
	      w->dial.indicator_x,
	      w->dial.indicator_y,
	      w->dial.arm1_x,
              w->dial.arm1_y);

   /* Draw the other arm and the side of the arrow */
   XDrawLine(XtDisplay(w), XtWindow(w),
	      gc,
	      w->dial.center_x,
	      w->dial.center_y,
	      w->dial.arm2_x,
              w->dial.arm2_y);
	     
   XDrawLine(XtDisplay(w), XtWindow(w),
	      gc,
	      w->dial.indicator_x,
	      w->dial.indicator_y,
	      w->dial.arm2_x,
              w->dial.arm2_y);

   points[0].x = w->dial.arm1_x;
   points[0].y = w->dial.arm1_y;
   points[1].x = w->dial.indicator_x;
   points[1].y = w->dial.indicator_y;
   points[2].x = w->dial.arm2_x;
   points[2].y = w->dial.arm2_y;
  
   XFillPolygon(XtDisplay(w), XtWindow(w), gc, points, 3, Nonconvex,
       CoordModeOrigin);

   /* Clear out the old stuff first */
   width = XTextWidth(w->dial.font,buf,strlen(buf));
   xpos = w->core.width/2 - (width/2);
   ypos = w->core.height * 0.80;
   /* Clear out area first */
   XDrawImageString(XtDisplay(w), XtWindow(w), w->dial.dial_GC,
    xpos, ypos, buf, strlen(buf));

   /* Put up the new value */
   sprintf(buf,"%d", w->dial.position); 
      
   width = XTextWidth(w->dial.font,buf,strlen(buf));
   xpos = w->core.width/2 - (width/2);
   ypos = w->core.height * 0.80;
   XDrawImageString(XtDisplay(w), XtWindow(w), w->dial.dial_GC, 
    xpos, ypos, buf, strlen(buf)); 
   
}























