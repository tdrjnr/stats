/* slider.h - header file for slider.c */

#ifndef _NO_PROTO
#ifndef XTFUNCPROTO
#define XTFUNCPROTO
#endif
#endif

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/MessageB.h>
#include <string.h>
#ifndef _NO_PROTO
  #include <stdlib.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#define MAX_STRING 30		     /* Maximum length of one line strings */
#define YES        1
#define NO         0
#define MIN_CHANGE 0
#define MAX_CHANGE 1
#define UNSET      2

typedef struct SliderSct {
  Widget slider;
  double min;
  double max;
  double value;
  char *label;
  double scale;
  short num_digits;
} SliderStruct;

typedef struct changesct{
  SliderStruct *s;
  double new_value;
  int    which;
} ChangeStruct;


/*********************   Function Prototypes   *****************************/

#ifndef _NO_PROTO
  XtCallbackProc update_text(Widget, caddr_t, caddr_t);
  void send_data(Widget, caddr_t, caddr_t);
  void help(void);
  void check_with_user(char *);
  void ok_callback(Widget, caddr_t,caddr_t);
  void cancel_callback(Widget, caddr_t, caddr_t);
  void warn_user(char*);
  void quit(Widget,caddr_t,caddr_t);
  void SliderScaler(SliderStruct *,Arg *,int *,XmString );
  void clear_val();
  void push_val(char *value);
  void print_vals();

#else 
  XtCallbackProc update_text();
  void send_data();
  void help();
  void check_with_user();
  void ok_callback();
  void cancel_callback();
  void warn_user();
  void quit();
  void SliderScaler();
  void clear_val();
  void push_val();
  void print_vals();
#endif

/*********************   Define External Variables  *************************/

int                num_sliders;	       /* The number of sliders to draw     */
Widget             *text_box;	       /* Pointer to the first text_box     */
Widget             yes_or_no;          /*  The yes-or-no input dialog       */
Widget             warning;            /*  The warning dialog box           */
int                update_flag=FALSE;  /* Continuously update?              */
ChangeStruct       change;             /* What slider values to change      */
SliderStruct       *Slider;            /*Pointer to the first slider        */
int                accuracy = 5;       /*The number of decimal places to use*/

