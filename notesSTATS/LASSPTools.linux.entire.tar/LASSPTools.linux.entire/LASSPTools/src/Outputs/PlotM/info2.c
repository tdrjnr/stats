/*
** PlotM (info2.c)
**    An X-windows and Motif program to display ball and stick
**    representations of molecules along with interactive
**    control for scaling and rotating.
**    This file contains additional Xt callback functions
**
** Feel free to copy, modify and redistribute this program, but do not
** sell it. Include this notice in any copy.
**
** Author: Jim Frankenberger 1993
**         Laboratory of Atomic and Solid State Physics
**         Cornell University
**
**
*/
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/ArrowB.h>

#include "show.h"

#define X_INCREASE 1
#define X_DECREASE 2
#define Y_INCREASE 3
#define Y_DECREASE 4
#define Z_INCREASE 5
#define Z_DECREASE 6
#define COLOR_INCREASE 7
#define COLOR_DECREASE 8

extern void atom_field_back();
extern void done_atom_opt_back();
extern void save_atom_back();
extern void cancel_atom_back();
extern void update_atom_window();

extern float world_xmax,world_ymax,world_zmax,world_xmin,world_ymin,world_zmin;

static Widget x_text_edit,y_text_edit,z_text_edit,color_text_edit,name_text_edit,radius_text_edit;
static int atom_window_active = FALSE;

static OBJECT *atom_window_object;

/*
** center_pic()
**
** This function gets invoked when the centering button is hit, basically
** this should find out how to set the boundary values so that resulting
** picture is center.
**
*/
#ifndef _NO_PROTO
void center_pic(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
#else
void center_pic(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmAnyCallbackStruct *call_data;
#endif
{

   double del_x,del_y,del_z, xrange,yrange,zrange;
   if (object_count > 0) {
      
      if ((world_xmin != xmin) || (world_ymin != ymin) || (world_zmin != zmin) ||
	  (world_xmax != xmax) || (world_ymax != ymax) || (world_zmax != zmax)) {
        /*
        ** Keep the world coordinates the same so that the same scale is
        ** maintained, also the range of the viewport must be kept the same.
        ** But change the actual viewport so that it looks at the center of
        ** the object. (rotation angle is kept the same).
        */

        xrange = (xmax-xmin) / 2.0;
        yrange = (ymax-ymin) / 2.0;
        zrange = (zmax-zmin) / 2.0;

        /* These points represent the center of the structure */
        del_x = (pic_xmin +  ((pic_xmax - pic_xmin) / 2.0));
        del_y = (pic_ymin +  ((pic_ymax - pic_ymin) / 2.0));
        del_z = (pic_zmin +  ((pic_zmax - pic_zmin) / 2.0));

        xmin = del_x - xrange;
        xmax = del_x + xrange;
        ymin = del_y - yrange;
        ymax = del_y + yrange;
        zmin = del_z - zrange;
        zmax = del_z + zrange;


        set_limits();

        apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);
      }
   }
}

/*
** atom_change()
**
** This function gets invoked when the atom button is hit. It allows
** the user to change some characteristics of the given atom whose 
** coordinates appear in the window.
**
*/
#ifndef _NO_PROTO
void atom_change(Widget w, Widget parent, XmAnyCallbackStruct *call_data)
#else
void atom_change(w, parent, call_data)
Widget w;
Widget parent;
XmAnyCallbackStruct *call_data;
#endif
{    
    Widget color_text,name_text,radius_text,x_text,y_text,z_text;
    Widget atom_form,save_atom,cancel_atom,done_atom;
    Widget x_increase,x_decrease,y_increase,y_decrease;
    Widget z_increase,z_decrease,color_increase,color_decrease;
    Arg args[20];
    XmString str;
    Position xpos,ypos;
    Dimension width,bwidth;

    int n;

    
    if (atom_window_active)
       return;

    n=0;
    XtSetArg(args[n],XtNx, &xpos); n++;
    XtSetArg(args[n],XtNy, &ypos); n++;
    XtSetArg(args[n],XtNwidth, &width); n++;
    XtSetArg(args[n],XtNborderWidth, &bwidth); n++;
    XtGetValues(toplevel,args,n);

    xpos = xpos + width + (bwidth * 3);
    
    n=0;
    XtSetArg(args[n], XtNx,xpos); n++;
    XtSetArg(args[n], XtNy,ypos); n++;
    XtSetArg(args[n], XtNheight, 250); n++;
    XtSetArg(args[n], XtNwidth, 270); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
    XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;    
    atom_form = XmCreateFormDialog(parent, "options_form", args, n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    save_atom = XtCreateManagedWidget("Save",xmPushButtonWidgetClass,
                                         atom_form,args,n);
    XtAddCallback(save_atom,XmNactivateCallback, save_atom_back, atom_form);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, save_atom); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,50); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    cancel_atom = XtCreateManagedWidget("Cancel",xmPushButtonWidgetClass,
                                         atom_form,args,n);

    XtAddCallback(cancel_atom,XmNactivateCallback, cancel_atom_back,atom_form);   

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, cancel_atom); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    done_atom = XtCreateManagedWidget("Exit",xmPushButtonWidgetClass,
                                         atom_form,args,n);
    XtAddCallback(done_atom,XmNactivateCallback, done_atom_opt_back, atom_form);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopPosition, 15); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,27); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("X:    ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    x_text = XtCreateManagedWidget("xpos",xmLabelWidgetClass,
                                         atom_form,args,n);
    XmStringFree(str);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, x_text); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,39); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("Y:    ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    y_text = XtCreateManagedWidget("ypos",xmLabelWidgetClass,
                                         atom_form,args,n);
    XmStringFree(str);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, y_text); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,51); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("Z:    ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    z_text = XtCreateManagedWidget("zpos",xmLabelWidgetClass,
                                         atom_form,args,n);

    XmStringFree(str);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, z_text); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,63); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("COLOR: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    color_text = XtCreateManagedWidget("color",xmLabelWidgetClass,
                                         atom_form,args,n);
    XmStringFree(str);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, color_text); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,75); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("TEXT: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    name_text = XtCreateManagedWidget("name",xmLabelWidgetClass,
                                         atom_form,args,n);
    XmStringFree(str);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, name_text); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,89); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,25); n++;
    str = XmStringCreate("RADIUS: ",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString,str); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    radius_text = XtCreateManagedWidget("radius",xmLabelWidgetClass,
                                         atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopPosition, 15); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,27); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    x_text_edit = XtCreateManagedWidget("xpos_edit",xmTextWidgetClass,
                                         atom_form,args,n);
#if 0
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopPosition, 15); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_LEFT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,x_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    x_decrease = XtCreateManagedWidget("x_decrease",xmArrowButtonWidgetClass, atom_form,args,n);
#endif
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopPosition, 15); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_RIGHT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,x_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    x_increase = XtCreateManagedWidget("x_increase",xmArrowButtonWidgetClass, atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, x_text_edit); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,39); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    y_text_edit = XtCreateManagedWidget("y_text_edit",xmTextWidgetClass,
                                         atom_form,args,n);
#if 0
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, x_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_LEFT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,y_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    y_decrease = XtCreateManagedWidget("y_decrease",xmArrowButtonWidgetClass, atom_form,args,n);
#endif
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, x_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_RIGHT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,y_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    y_increase = XtCreateManagedWidget("y_increase",xmArrowButtonWidgetClass, atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, y_text_edit); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,51); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    z_text_edit = XtCreateManagedWidget("z_text_edit",xmTextWidgetClass,
                                         atom_form,args,n);

#if 0
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, y_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_LEFT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,z_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    z_decrease = XtCreateManagedWidget("z_decrease",xmArrowButtonWidgetClass, atom_form,args,n);
#endif
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, y_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_RIGHT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,z_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    z_increase = XtCreateManagedWidget("z_increase",xmArrowButtonWidgetClass, atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, z_text_edit); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,63); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    color_text_edit = XtCreateManagedWidget("color_edit",xmTextWidgetClass,
                                         atom_form,args,n);
#if 0
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, z_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_LEFT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,color_text_edit); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    color_decrease = XtCreateManagedWidget("color_decrease",xmArrowButtonWidgetClass, atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, z_text_edit); n++;
    XtSetArg(args[n], XmNarrowDirection,XmARROW_RIGHT); n++;
    XtSetArg(args[n],XmNleftAttachment,XmATTACH_WIDGET); n++;
    XtSetArg(args[n],XmNleftWidget,color_decrease); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    color_increase = XtCreateManagedWidget("color_increase",xmArrowButtonWidgetClass, atom_form,args,n);
#endif
    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, color_text_edit); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,75); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    name_text_edit = XtCreateManagedWidget("name_text_edit",xmTextWidgetClass,
                                         atom_form,args,n);

    n=0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, name_text_edit); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 30); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,89); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,75); n++;
    XtSetArg(args[n], XtNbackground,pix_back); n++;
    radius_text_edit = XtCreateManagedWidget("radius_edit",xmTextWidgetClass,
                                         atom_form,args,n);

    XtAddCallback(x_increase,XmNactivateCallback, atom_field_back, X_INCREASE);
    XtAddCallback(y_increase,XmNactivateCallback, atom_field_back, Y_INCREASE);
    XtAddCallback(z_increase,XmNactivateCallback, atom_field_back, Z_INCREASE);
#if 0
    XtAddCallback(x_decrease,XmNactivateCallback, atom_field_back, X_DECREASE);
    XtAddCallback(y_decrease,XmNactivateCallback, atom_field_back, Y_DECREASE);
    XtAddCallback(z_decrease,XmNactivateCallback, atom_field_back, Z_DECREASE);
    XtAddCallback(color_increase,XmNactivateCallback, atom_field_back, COLOR_INCREASE);
    XtAddCallback(color_decrease,XmNactivateCallback, atom_field_back, COLOR_DECREASE);
#endif

#if 0
    XmTextSetString(x_text_edit," Blah");
    XmTextSetString(y_text_edit," Blah");
    XmTextSetString(z_text_edit," Blah");
    XmTextSetString(color_text_edit," Blah");
    XmTextSetString(name_text_edit," Blah");
    XmTextSetString(radius_text_edit," Blah");
#endif
    XmTextSetEditable(x_text_edit,True);
    XmTextSetEditable(y_text_edit,True);
    XmTextSetEditable(z_text_edit,True);
    XmTextSetEditable(color_text_edit,True);
    XmTextSetEditable(name_text_edit,True);
    
    atom_window_active = TRUE;

    XtManageChild(atom_form);

}

/*
** atom_field_back()
**
** Handle the arrow buttons
**
*/
#ifndef _NO_PROTO
void atom_field_back(Widget w, caddr_t arrow_type, XmAnyCallbackStruct *call_data)
#else
void atom_field_back(w, arrow_type, call_data)
Widget w;
caddr_t arrow_type;
XmAnyCallbackStruct *call_data;
#endif
{
     int i;
     int atype = (int) arrow_type;
     OBJECT *object;
     OBJECT *prev_x,*prev_y,*prev_z;

     /* Find the object in the depth_list */
     object = depth_list[first_z].ptr;
     i = first_z;
     while (object != atom_window_object) {
         if (object->x1_orig != atom_window_object->x1_orig)
	    prev_x = object;
         if (object->y1_orig != atom_window_object->y1_orig)
	    prev_y = object;
         if (object->z1_orig != atom_window_object->z1_orig)
	    prev_z = object;
	 object = depth_list[depth_list[i].next].ptr;
	 i = depth_list[i].next;
     }
     
     switch (atype) {
        case X_INCREASE:
           /* Find the next object in the depth list with a bigger X */
           while ((object->x1_orig == atom_window_object->x1_orig)&&(object != NULL) &&
		  (object->type == SPHERE)) {
	      object = depth_list[depth_list[i].next].ptr;
	      i = depth_list[i].next;
	   }
	   if (object == NULL)
	      atom_window_object = depth_list[first_z].ptr;
	   else
	      atom_window_object = object;
           break;
        case Y_INCREASE: 
           /* Find the next object in the depth list with a bigger Y */
           while ((object->y1_orig == atom_window_object->y1_orig)&&(object != NULL) &&
		  (object->type == SPHERE)) {
	      object = depth_list[depth_list[i].next].ptr;
	      i = depth_list[i].next;
	   }
	   if (object == NULL)
	      atom_window_object = depth_list[first_z].ptr;
	   else
	      atom_window_object = object;
           break;
        case Z_INCREASE:
           /* Find the next object in the depth list with a bigger Z */
           while ((object->z1_orig == atom_window_object->z1_orig)&&(object != NULL) && 
		  (object->type == SPHERE)) {
	      object = depth_list[depth_list[i].next].ptr;
	      i = depth_list[i].next;
	   }
	   if (object == NULL)
	      atom_window_object = depth_list[first_z].ptr;
	   else
	      atom_window_object = object;
           break;
        case X_DECREASE:
           /* Find the next object in the depth list with a smaller X */
           
           break;
        case Y_DECREASE:
           /* Find the next object in the depth list with a smaller Y */

           break;
        case Z_DECREASE:
           /* Find the next object in the depth list with a smaller Z */

           break;
        case COLOR_INCREASE:
        case COLOR_DECREASE:
            break;
     }
     update_atom_window(NULL,atom_window_object);

}
/*
** cancel_atom_back()
**
** Put the default entries back in the 
**
*/
#ifndef _NO_PROTO
void cancel_atom_back(Widget w, Widget parent, XmAnyCallbackStruct *call_data)
#else
void cancel_atom_back(w, parent, call_data)
Widget w;
Widget parent;
XmAnyCallbackStruct *call_data;
#endif
{
      update_atom_window(w,atom_window_object);
}
/*
** done_atom_opt_back()
**
** Get rid of the atom window
**
*/
#ifndef _NO_PROTO
void done_atom_opt_back(Widget w, Widget dialog, XmAnyCallbackStruct *call_data)
#else
void done_atom_opt_back(w, dialog, call_data)
Widget w;
Widget dialog;
XmAnyCallbackStruct *call_data;
#endif
{
    XtUnmanageChild(dialog);
    atom_window_active = FALSE;
}

/*
** save_atom_back()
**
** Updates an object when the 'save' button is hit in the atom window.
**
*/
#ifndef _NO_PROTO
void save_atom_back(Widget w, Widget parent, XmAnyCallbackStruct *call_data)
#else
void save_atom_back(w, parent, call_data)
Widget w;
Widget parent;
XmAnyCallbackStruct *call_data;
#endif
{
       char *str;
       char *color;
       float x1pos,y1pos,z1pos,radius;
       OBJECT *obj_pt;
       int do_major_redraw = FALSE;

       str = XmTextGetString(x_text_edit);
       sscanf(str,"%f",&x1pos);
       str = XmTextGetString(y_text_edit);
       sscanf(str,"%f",&y1pos);
       str = XmTextGetString(z_text_edit);
       sscanf(str,"%f",&z1pos);
       str = XmTextGetString(radius_text_edit);
       sscanf(str,"%f",&radius);
       str = XmTextGetString(name_text_edit);  
       strcpy(atom_window_object->name,str);
       color = XmTextGetString(color_text_edit);


       /* Need to Free pixmaps if radius or color changed */
       if (atom_window_object->radius != radius) {
	  atom_window_object->radius = radius;

	  do_major_redraw = TRUE;
       }

       /* Need to set correct color index */
       if  (strcmp(color,atom_window_object->color)) {
	   do_major_redraw = TRUE;
           strcpy(atom_window_object->color,color);	  
	   
       }

       /* Need to figure out new original coordinates */
       atom_window_object->x1_orig =  x1pos;
       atom_window_object->y1_orig =  y1pos;
       atom_window_object->z1_orig =  z1pos;
       
       if (xmax < (atom_window_object->x1_orig + atom_window_object->radius))
	  xmax = atom_window_object->x1_orig + atom_window_object->radius;
       if (ymax < (atom_window_object->y1_orig + atom_window_object->radius))
	  ymax = atom_window_object->y1_orig + atom_window_object->radius;
       if (xmin > (atom_window_object->x1_orig - atom_window_object->radius))
	  xmin = atom_window_object->x1_orig - atom_window_object->radius;
       if (ymin > (atom_window_object->y1_orig - atom_window_object->radius))
	  ymin = atom_window_object->y1_orig - atom_window_object->radius;

       obj_pt = object_head.next;
       while (obj_pt->next != NULL) {
	   if (obj_pt->type == LINE) {
              if (obj_pt->sphere_1 == atom_window_object) {
		 obj_pt->x1_orig = atom_window_object->x1_orig;
		 obj_pt->y1_orig = atom_window_object->y1_orig;
		 obj_pt->z1_orig = atom_window_object->z1_orig;
	       } else { 
		 if (obj_pt->sphere_2 == atom_window_object) {
		    obj_pt->x2_orig = atom_window_object->x1_orig;
		    obj_pt->y2_orig = atom_window_object->y1_orig;
		    obj_pt->z2_orig = atom_window_object->z1_orig;
                 }
               }
	   }
	   obj_pt = obj_pt->next;
       }

       /* Force a redraw of the picture */ 
       if (do_major_redraw)  
          do_down_scale(w,2,call_data);
       else {
          set_limits();
          apply_rotate(&global_rot[0][0],object_count,object_head.next,TRUE);
       }
}
/*
** update_atom_window()
**
** This function is at the same time that the X,Y,Z fields are
** updated in the main window.
**
*/
#ifndef _NO_PROTO
void update_atom_window(Widget w, OBJECT *obj_ptr)
#else
void update_atom_window(w, obj_ptr)
Widget w;
OBJECT *obj_ptr;
#endif
{
    char buf[100];
     
    atom_window_object = obj_ptr;
    
    if (!atom_window_active) {
       atom_change(NULL,w,NULL);      
    }

    if (atom_window_active) {
       sprintf(buf,"%f",obj_ptr->x1_orig);
       XmTextSetString(x_text_edit,buf);
       sprintf(buf,"%f",obj_ptr->y1_orig);
       XmTextSetString(y_text_edit,buf);
       sprintf(buf,"%f",obj_ptr->z1_orig);
       XmTextSetString(z_text_edit,buf);
       XmTextSetString(color_text_edit,obj_ptr->color);
       XmTextSetString(name_text_edit,obj_ptr->name);
       sprintf(buf,"%lf",obj_ptr->radius);
       XmTextSetString(radius_text_edit,buf);
    }

}



