/*
** PlotMolecule -
**    An X-windows and Motif program to display ball and stick
**    representations of molecules along with interactive
**    control for scaling and rotating.
**
** Feel free to copy, modify and redistribute this program, but do not
** sell it. Include this notice in any copy.
**
** Author: Jim Frankenberger 1993
**         Laboratory of Atomic and Solid State Physics
**         Cornell University
**
*/
#include <stdio.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>

#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/MainW.h>
#include <Xm/Separator.h>
#include <Xm/BulletinB.h>
#include "show.h"

extern create_pixmap();
extern create_a_mask();

#ifndef _NO_PROTO
extern create_gc(int width, int height,  OBJECT *object);
extern create_alt_gc(int width, int height, OBJECT *object);
extern scale_picture(int count, int first);		
#else
extern create_gc();
extern create_alt_gc();
extern scale_picture();
#endif

extern float world_xmax,world_xmin,world_ymax,world_ymin;

/*
** apply_rotate()
**
** Given a rotation matrix this function will rotate all the objects
** according to the values of the matrix. The draw_flag indicates if
** if the figure should be drawn.
**
**
*/
#ifndef _NO_PROTO
apply_rotate(float rot[3][3],int object_count, OBJECT *object, int draw_flag)
#else
apply_rotate(rot,object_count,object,draw_flag)
float rot[3][3];
int object_count;
OBJECT *object;
int draw_flag;
#endif
{

    float new_x,new_y,new_z;
    int i,j;
    float old_x,old_y,old_z,del_x,del_y,del_z;

    /* These points represent the center of the structure */
    del_x = -1.0 * (pic_xmin +  ((pic_xmax - pic_xmin) / 2.0));
    del_y = -1.0 * (pic_ymin +  ((pic_ymax - pic_ymin) / 2.0));
    del_z = -1.0 * (pic_zmin +  ((pic_zmax - pic_zmin) / 2.0));
   
    /* Update the global rotation matrix */
    for (i=0;i<3;i++)
       for (j=0;j<3;j++)
	 global_rot[i][j] = rot[i][j];

    /* Rotate all the active objects */
    for (i=0;i<total_objects;i++) {
      if (object->active) {    

         if (object->type == SPHERE) {
	   /* Use the original cords as the value to rotate from */
           old_x = object->x1_orig+del_x; old_y = object->y1_orig+del_y; old_z = object->z1_orig+del_z;
	   
           /* Update the new cords from the rotation matrix and account for centering */
           new_x = (old_x*rot[0][0] + old_y*rot[0][1] + old_z*rot[0][2]) - del_x;
           new_y = (old_x*rot[1][0] + old_y*rot[1][1] + old_z*rot[1][2]) - del_y;
           new_z = (old_x*rot[2][0] + old_y*rot[2][1] + old_z*rot[2][2]) - del_z;
           
           /* Update the current cords */
           object->x1 = new_x;
           object->y1 = new_y;
           object->z1 = new_z;
        } else {
	   /* For a line update both endpoints */
           old_x = object->x1_orig+del_x; old_y = object->y1_orig+del_y; old_z = object->z1_orig+del_z;
           new_x = (old_x*rot[0][0] + old_y*rot[0][1] + old_z*rot[0][2]) - del_x;
           new_y = (old_x*rot[1][0] + old_y*rot[1][1] + old_z*rot[1][2]) - del_y;
           new_z = (old_x*rot[2][0] + old_y*rot[2][1] + old_z*rot[2][2]) - del_z;
           object->x1 = new_x;
           object->y1 = new_y;
           object->z1 = new_z;
           old_x = object->x2_orig+del_x; old_y = object->y2_orig+del_y; old_z = object->z2_orig+del_z;
           new_x = (old_x*rot[0][0] + old_y*rot[0][1] + old_z*rot[0][2]) - del_x;
           new_y = (old_x*rot[1][0] + old_y*rot[1][1] + old_z*rot[1][2]) - del_y;
           new_z = (old_x*rot[2][0] + old_y*rot[2][1] + old_z*rot[2][2]) - del_z;
           object->x2 = new_x;
           object->y2 = new_y;
           object->z2 = new_z;

        }
       }
       object = object->next;
    }

    if (draw_flag) {
    
       clear_canvas();

       draw_objects(TRUE,TRUE,TRUE);
    }
}

/* 
** clear_canvas()
**
** This will clear the offscreen pixmap area.
**
**
*/
clear_canvas()
{
    Window win;

    win = XtWindow(canvas);
    
    XFillRectangle(XtDisplay(canvas),canvas_pix,canvas_gc,0,0,canvas_width,canvas_height);
 
}

/*
** start_rotate()
**
** This function is called when the program begins, its purpose is to launch
** the rotate program and also set up pipes between this program and the
** rotate program. 
**
*/
#ifndef _NO_PROTO
start_rotate(char *argv[])
#else
start_rotate(argv)
char *argv[];
#endif
{
     char *rot_args[20];
     int i;


     if (pipe(pfd) == -1)
        fprintf(stderr,"\nCould not make pipe.\n");
     else {
        /* Create a copy of this process */
        switch (fork()) {
          case -1:
             fprintf(stderr,"\nCan not fork()\n");
             return;

          case 0:
	     /* This is a good child process */
	     
             /* close stdout */
             if (close(1) == -1) {
                fprintf(stderr,"\nCould not close\n");
                return;
	     }

	     /* Copy stdout file descriptor */
             if (dup(pfd[1]) != 1) {
                fprintf(stderr,"\nerror in dup()\n");
                return;
	     }
	     
	     /* Close stdin file descriptor */
             if ((close(pfd[0]) == -1) || (close(pfd[1]) == -1)) {
                fprintf(stderr,"\nclose2 error\n");
                return;
             }

             /* Copy command line arguments */
             i = 0;
             do {
                if (i >= 20) {
		  fprintf(stderr,"\nToo many args for rotate\n");
                  return;
		}
                rot_args[i+1] = argv[i];
	     } while (argv[i++] != NULL);

	     /* If the program was started with a -update pass this on to the
             ** rotate program, also copy all the arguments along.
	     */
	     if (update_rotate) 
                rot_args[i-1] = "-update";
	     rot_args[i] = NULL;
	     rot_args[0] = "rotate";

	     /* Start the program and do not return */
             execvp("rotate",rot_args);
             fprintf(stderr,"\nCan not run rotate\n");
             exit(1);
	   default:
	     /* This is the parent process, return to the main program */
	     break;

        } /* end switch */

      } /* end if */
}

/*
** sort_objects()
**
** The purpose of this function is to arrange the objects into a list
** from the most negative Z value to the most positive Z value, the 
** drawing will then be from back to front.
**
**
*/
sort_objects()
{
          int i,j,k,prev;
          float real_z;
          OBJECT *object = object_head.next;
          
          first_z = 0;
          prev = 0;
          end_depth = 0;

	  /* Look at each object */
          for (i=0;i<total_objects;i++) {
            if ((object->active == TRUE) && (object->visible == TRUE)) {
              if (object->type == SPHERE) {

		 /* Find the objects position in the list */
                 j = first_z; k = 0; prev = -1;
                 while ((depth_list[j].z1 < object->z1) && (k < end_depth)) {
                   prev = j;
                   j = depth_list[j].next;
                   k++;
                 }
                 
                 /* See if it at the end of the list */
                 if (k == end_depth) {
                   depth_list[end_depth].z1 = object->z1;
                   depth_list[end_depth].ptr = object;
                   depth_list[end_depth].next = 0;
                   if (end_depth != 0) 
                     depth_list[prev].next = end_depth;
                  
                    
                 } else {
		   
		   /* It is someplace before the end */
                   depth_list[end_depth].z1 = object->z1;
                   depth_list[end_depth].ptr = object;
                   depth_list[end_depth].next = j;

		   /* Check if it is the first element in the list */
                   if (prev != -1)
                     depth_list[prev].next = end_depth;
                   else 
                     first_z = end_depth;
                 }
	       } else {

		 /* The object is a line, check that both endpoints of the
		 ** line find the correct place in the list.
		 */
                 j = first_z; k = 0; prev = -1;
                 while (((depth_list[j].z1 < object->z1) || 
                         (depth_list[j].z1 < object->z2)) && (k < end_depth)) {
                    prev = j;
                    j = depth_list[j].next;
                     k++;
                 }

		 /* Check which Z value is the least */
                 if (object->z1 < object->z2)
                    real_z = object->z1;
                 else 
                    real_z = object->z2;

		 /* See if it is the last object in the list */
                 if (k == end_depth) {
                   depth_list[end_depth].z1 = real_z;
                   depth_list[end_depth].ptr = object;
                   depth_list[end_depth].next = 0;
                   if (end_depth != 0) 
                     depth_list[prev].next = end_depth;
                    
                 } else {
		   
		   /* It is someplace before the last element of the list */
                   depth_list[end_depth].z1 = real_z;
                   depth_list[end_depth].ptr = object;
                   depth_list[end_depth].next = j;
		   
		   /* Check if it is the first object in the list */
                   if (prev != -1)
                     depth_list[prev].next = end_depth;
                   else
                     first_z = end_depth;
                 }
	      } /* Endif for LINE */
              end_depth++;
	    } /* Endif for active and visible */

            object = object->next;    

          } /* End for loop */         

}

/* 
** clear_objects()
**
** This function will deallocate all memory that was allocated to hold
** pixmaps and objects and then sent the count of the number of objects
** to zero.  
**
*/
clear_objects()
{
   OBJECT *obj_ptr;
   OBJECT *prev_ptr;
   int i;
   Display *dpy;
   Window win;

   dpy = XtDisplay(canvas);

   biggest_r = 0;

   obj_ptr = object_head.next;

   /* Free all the pixmaps and GC's - check that use_this == NULL to
   ** be sure that the pointers are valid and that this object is
   ** not using the pixmaps and GC's from another object.
   */
   for (i=0;i<total_objects;i++) {
     if (obj_ptr->active) {
       if ((obj_ptr->type == SPHERE) && (obj_ptr->use_this == NULL) && (obj_ptr->win_gc != NULL)) {
          XFreePixmap(XtDisplay(canvas),obj_ptr->pixmap);
          XFreePixmap(XtDisplay(canvas),obj_ptr->mask_pixmap);
          XFreeGC(XtDisplay(canvas),obj_ptr->pixmap_gc);
          XFreeGC(XtDisplay(canvas),obj_ptr->win_gc);
       } else {
	  if (obj_ptr->type == SPHERE)
	     obj_ptr->use_this = NULL;
       }
     }
     obj_ptr->pixmap = (Pixmap) NULL;
     obj_ptr->mask_pixmap = (Pixmap) NULL;
     obj_ptr->pixmap_gc = NULL;
     obj_ptr->win_gc = NULL;
     prev_ptr = obj_ptr;
     obj_ptr = obj_ptr->next;
     free(prev_ptr);
   }

   /* Set the counts to zero and clear the window */
   object_count = 0;
   total_objects = 0;
   object = &object_head;
   object_head.next = (OBJECT *)&object;
   no_size = TRUE;
   first_time=TRUE;
   clear_canvas();
   win = XtWindow(canvas);
   XCopyArea(dpy,canvas_pix,win,canvas_gc,0,0,canvas_width, canvas_height, 0,0);
   XFlush(dpy);

   
}

/*
** add1_sphere()
**
** Given the x,y,z, radius, color and text associated with a sphere
** (as read from the input) this function will create a node on the
** linked list for the sphere or it will find an old node with the
** same properties and use its information. object is the global variable that
** contains the minimum specification for and object - namely is position, radius,
** color, type  and active flags.
**
*/
#ifndef _NO_PROTO
add1_sphere(float x1, float y1, float z1, float radius, char *color, char *name, int color_index)
#else
add1_sphere(x1,y1,z1,radius,color,name,color_index)
float x1,y1,z1,radius;
char *color,*name;
int color_index;
#endif
{
    OBJECT * old_obj, *save;
    int i;
    OBJECT * use_this = NULL;

    /* See if there is an old object that has the same properties, only 
    ** look at object that are not currently active 
    */
    old_obj = object_head.next;
    i = 0;
    while ((old_obj != NULL) && (i<total_objects)) {
       if ((old_obj->active == FALSE) && (old_obj->radius == radius) && 
           (old_obj->type == SPHERE) && (!(strcmp(old_obj->color,color)))) {
          old_obj->active = TRUE;
	  old_obj->visible = TRUE;
          use_this = old_obj;
          old_obj = NULL;
	} else
          old_obj = old_obj->next;
        i++;
    }

    old_obj = object;
    
    /* If use_this == NULL it means that we could not
    ** find an object that looks like this one, so create
    ** a new one.
    */
    if (use_this == NULL) {    
       
       object->next = (OBJECT *)malloc(sizeof(OBJECT));
 
       object = object->next;
       save = object;

       object->next = NULL;
       object->type = SPHERE;
       strcpy(object->color,color);
       object->win_gc = NULL;
       object->radius = radius;
       object->radius_scaled = (int) radius;
       object->use_this = NULL;
       total_objects++;

    } else {
       save = object;
       object = use_this;
    }
    
    /* Make the object active and setup its position */
    object->active = TRUE; 
    object->visible = TRUE;
    object->x1 = object->x1_orig = x1;
    object->y1 = object->y1_orig = y1;
    object->z1 = object->z1_orig = z1;
    object->color_index = color_index;

    /* link any text with the object */
    if (name[0] != '\0') 
       strcpy(object->name,name);
    else
      object->name[0] = '\0';

    /* Restore object which is the last node in the list */
    object = save;
    
    object_count++; 

    /* Figure out if the min and max's of the picture have changed */
    if (old_obj != &object_head) {
      if (x1-radius < pic_xmin)
         pic_xmin = x1-radius;
  
      if (y1-radius < pic_ymin)
         pic_ymin = y1-radius;

      if (z1-radius < pic_zmin)
         pic_zmin = z1-radius;

      if (x1+radius > pic_xmax)
         pic_xmax = x1+radius;

      if (y1+radius > pic_ymax)
         pic_ymax = y1+radius;

      if (z1+radius > pic_zmax)
         pic_zmax = z1+radius;

      if (radius > biggest_r)
         biggest_r = radius;

    } else {
         /* This is the first object so set the min's and max's */
         pic_xmax = x1+radius;
         pic_ymax = y1+radius;
         pic_zmax = z1+radius;
         pic_xmin = x1-radius;
         pic_ymin = y1-radius;
         pic_zmin = z1-radius;

         biggest_r = radius;
    }

}

/* 
** create_gc()
**
** This will create a graphics context for a sphere that is used
** when drawing.
**
**
*/
#ifndef _NO_PROTO
create_gc(int width, int height, OBJECT *object)
#else
create_gc(width, height,object)
int width,height;
OBJECT *object;
#endif
{
    int foreground_data,background_data;
    int n;
    Arg args[10];
    Display *dpy;
    int radius;

    /* This is radius converted to actual pixel length */
    radius  = object->radius_scaled;


    dpy = XtDisplay(canvas);
    
    n = 0;
    XtSetArg(args[n], XtNforeground, &foreground_data); n++;
    XtSetArg(args[n], XtNbackground, &background_data); n++;
    XtGetValues(canvas, args, 2);

    /* Set the approiate fields in the GC for this sphere */
    values.foreground = foreground_data;
    values.background = background_data;
    values.line_width = 10;
    values.fill_style = FillSolid;
    values.cap_style = CapRound;
    values.clip_mask = object->mask_pixmap;

    /* Attach the GC to the object */
    object->win_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
         (GCForeground | GCBackground | GCLineWidth | GCClipMask | GCCapStyle), &values);

    /* For clipping make sure that the clip origin points to the upper left
    ** corner of the pixmap.
    */
    XSetClipOrigin(XtDisplay(canvas),object->win_gc,object->x1r-radius,object->y1r-radius);

   
 }

/*
** create_alt_gc()
**
** This creates a GC that is used when creating the clipping pixmap.
**
**
*/
#ifndef _NO_PROTO
create_alt_gc(int width, int height, OBJECT *object)
#else
create_alt_gc(width,height,object)
int width,height;
OBJECT *object;
#endif
{
   int foreground_data,background_data;
   int n;
   Arg args[10];

   Display *dpy;

   dpy = XtDisplay(canvas);

    n = 0;
    XtSetArg(args[n], XtNforeground, &foreground_data); n++;
    XtSetArg(args[n], XtNbackground, &background_data); n++;
    XtGetValues(canvas, args, 2);

    
    values.foreground = foreground_data;
    values.background = background_data;
    values.line_width = 10;
    values.fill_style = FillSolid;

    object->pixmap_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
         (GCForeground | GCBackground | GCLineWidth ), &values);

 }

/*
** create_pixmap()
**
** This will create the real pixmap that contains the shaded sphere.
**
**
*/
#ifndef _NO_PROTO
create_pixmap(OBJECT *object)
#else
create_pixmap(object)
OBJECT *object;
#endif
{
    Display *dpy;
    Window win;
    int n,xpos,ypos,width,height,dstep,i,loopcnt,save_width,save_height;
    Arg args[10];
    int foreground_data,background_data;
    XColor new_color,end_color;
    int stepsize;
    Colormap cmap; 
    int radius;
    int decstepr,decstepb,decstepg;

    height = width = object->radius_scaled * 2;

    /* Allocate memory for the pixmap */
    object->pixmap = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)), width,
                     height, DefaultDepthOfScreen(XtScreen(canvas)));

    if (depth_cues) {
       object->save_pixmap = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)), width,
                     height, DefaultDepthOfScreen(XtScreen(canvas)));
       object->stipple_pixmap = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)), width,
                     height, 1);

       save_width = save_height = width;
    }

    radius = object->radius_scaled;

    dpy = XtDisplay(canvas);
    win = XtWindow(canvas);

    cmap =  DefaultColormap(dpy, DefaultScreen(dpy));

    n = 0;
    XtSetArg(args[n], XtNforeground, &foreground_data); n++;
    XtSetArg(args[n], XtNbackground, &background_data); n++;
    XtGetValues(canvas, args, 2);

    XFillRectangle(dpy,object->pixmap,object->pixmap_gc,0,0,width,height);

    /* Get the color specified for this sphere */
    new_color.pixel = get_color(canvas,object->color);

    XQueryColor(dpy,cmap,&new_color);

    /* Get the color that the center will end up being */
    end_color.pixel = get_color(canvas,"white");
    XQueryColor(dpy,cmap,&end_color);

    /* Make a black circle - this will serve as a boundary to distinuish two
    ** spheres when they overlap.
    */
    XSetForeground(dpy,object->pixmap_gc,BlackPixel(dpy,DefaultScreen(dpy)));
    XFillArc(dpy,object->pixmap, object->pixmap_gc, xpos, ypos, width, height, 0, 64*360);

    /* Adjust the radius and size to account for the black border that was
    ** just drawn.
    */
    if (radius > 1) {
       radius = radius - 1;
       xpos = ypos = 1;
       width = width - 2;
       height = height - 2;
     } else {
       xpos = ypos = 0;
     }

    /* See how many colors can be used in shading the sphere */
    stepsize = radius;
    if (radius < cshade)
       loopcnt = radius;
    else {
       loopcnt = cshade;
       stepsize = cshade;
    }

    /* Calculate how much the red, green and blue components will
    ** need to be adjusted on each step to go from the original
    ** color to white 
    */
    decstepr = (end_color.red - new_color.red) / stepsize;
    decstepg = (end_color.green - new_color.green) / stepsize;
    decstepb = (end_color.blue - new_color.blue) / stepsize;

    /* Calculate how much the size of the circle will decrease on
    ** each step.
    */
    dstep = (radius*2) / stepsize;

    /* Draw the circle then adjust the colors and go to the next one */
    for (i=0;i<loopcnt;i++) {
        MyXAllocColor(dpy,cmap, &new_color);
        XSetForeground(dpy,object->pixmap_gc,new_color.pixel);
        XFillArc(dpy,object->pixmap, object->pixmap_gc, xpos, ypos, width, height, 0, 64*360);
        new_color.red = new_color.red + decstepr;
        new_color.green = new_color.green + decstepg;
        new_color.blue = new_color.blue +  decstepb;

	/* Calculate a new size and upper left coordinate */
        width-=dstep; height-=dstep;
        xpos += (dstep/2); ypos += (dstep/2);
    }
    if (depth_cues)
       XCopyArea(dpy,object->pixmap,object->save_pixmap,object->pixmap_gc,0,0,save_width,save_height,0,0);
 
  }

/*
** MyXAllocColor()
**
** This will allocate a color from the given colormap and
** return the pixel value in new_color. Currently this
** just uses the X function but could be extended to 
** keep a cache of colors.
**
**
*/
#ifndef _NO_PROTO
MyXAllocColor(Display *dpy, Colormap cmap, XColor *new_color)
#else
MyXAllocColor(dpy, cmap, new_color)
Display *dpy;
Colormap cmap;
XColor *new_color;
#endif
{
       int i,tmp;

#ifdef ERRORMSG
       /* Search through the colors we have */
       for (i=0;i<num_colors;i++) {
	  if ((new_color->red == cdata[i].red) && (new_color->green == cdata[i].green) &&
	      (new_color->blue == cdata[i].blue)) {
	      new_color->pixel = cdata[i].pixel;
	      return(1);
	   }
	}
	cdata[num_colors].red = new_color->red;
	cdata[num_colors].green = new_color->green;
	cdata[num_colors].blue = new_color->blue;
	if (tmp = XAllocColor(dpy,cmap,new_color)) {
           cdata[num_colors].pixel = new_color->pixel;
	   num_colors++;
	   fprintf(stderr,"\num_colors = %d",num_colors);
	}
	return(tmp);
#else
        return(XAllocColor(dpy,cmap,new_color));

#endif
}

/*
** create_a_mask()
**
** This creates a pixmap that is used for clipping when drawing a
** sphere.
**
**
**
*/
#ifndef _NO_PROTO
create_a_mask(int width, int height, OBJECT *object)
#else
create_a_mask(width, height, object)
int width;
int height;
OBJECT *object;
#endif
{
    Display *dpy;
    int scr;
    GC gc_glip;
    int n;
    int foreground_data, background_data;
    Arg args[10];
    Colormap cmap; 

       
    float radius = width / 2;

    dpy = XtDisplay(toplevel);
    scr = DefaultScreen(dpy);

    /* Allocate the pixmap */
    object->mask_pixmap = XCreatePixmap(dpy,DefaultRootWindow(dpy),
                                  width,height,1);
    n = 0;
    XtSetArg(args[n], XtNforeground, &foreground_data); n++;
    XtSetArg(args[n], XtNbackground, &background_data); n++;
    XtGetValues(canvas, args, 2);

    /* Create a GC so that the mask area can be drawn */
    values.function = GXclear;
    values.foreground = foreground_data;
    values.background = background_data;
    gc_clip = XCreateGC(dpy,object->mask_pixmap,GCFunction | GCForeground | GCBackground, &values);

    /* Clear the whole area */
    XFillRectangle(XtDisplay(canvas), object->mask_pixmap,gc_clip,0,0,width,height);

    /* Set bits for a mask that fill the circle */ 
    XSetFunction(dpy,gc_clip,GXset);
    XFillArc(XtDisplay(canvas),object->mask_pixmap, gc_clip, 0, 0, width, height, 0, 64*360);

}

/*
** add1_line()
**
** This function adds a line with the given endpoints, thickness(radius),
** color and name to the list of current objects that will be drawn.
**
*/
#ifndef _NO_PROTO
add1_line(float x1, float y1, float z1, float x2, float y2, float z2,
         float radius, char *color, char *name)
#else
add1_line(x1,y1,z1,x2,y2,z2,radius,color,name)
float x1,y1,z1,x2,y2,z2,radius;
char *color,*name;
#endif

{
    OBJECT *old_obj, *use_this, *save;
    int i;

    use_this = NULL;

    /* See if there is an old object that has the same properties */
    old_obj = object_head.next;
    i = 0;
    while ((old_obj != NULL) && (i<total_objects)) {
       if ((old_obj->active == FALSE) && (old_obj->radius == radius) && 
           (old_obj->type == LINE) && (!(strcmp(old_obj->color,color)))) {
          old_obj->active = TRUE;
	  old_obj->visible = TRUE;
          use_this = old_obj;
          old_obj = NULL;
	} else
          old_obj = old_obj->next;
          i++;
    }
    
    /* if there is no similar object that is not being used then
    ** allocate space for a new object
    */
    if (use_this == NULL) {
       
       object->next = (OBJECT *)malloc(sizeof(OBJECT));

       save = object->next;
       object = object->next;

       object->next = NULL;
       object->type = LINE;
       object->radius = radius;
       object->radius_scaled = (int) radius;
       strcpy(object->color,color);
       object->use_this = NULL;
       object->win_gc = NULL;
       total_objects++;

    } else {
      
       /* Use the object that was found above */
       save = object;
       object = use_this;
    }
    
    /* Fill in all the fields for this object */
    object->x1 = object->x1_orig = x1;
    object->y1 = object->y1_orig = y1;
    object->z1 = object->z1_orig = z1;
    object->x2 = object->x2_orig = x2;
    object->y2 = object->y2_orig = y2;
    object->z2 = object->z2_orig = z2;
    object->active = TRUE;
    object->visible = TRUE;
    object->sphere_1 = NULL;
    object->sphere_2 = NULL;

    object = save;
    
    object_count++;
    
    /* Check if the limits should be updated */
    if (x1 < pic_xmin)
       pic_xmin = x1;

    if (x2 < pic_xmin)
       pic_xmin = x2;
 
    if (x1 > pic_xmax)
       pic_xmax = x1;

    if (x2 > pic_xmax)
       pic_xmax = x2;

    if (y1 < pic_ymin)
       pic_xmin = y1;

    if (y2 < pic_ymin)
       pic_ymin = y2;
 
    if (y1 > pic_ymax)
       pic_ymax = y1;

    if (y2 > pic_ymax)
       pic_ymax = y2;

    if (z1 < pic_zmin)
       pic_zmin = z1;

    if (z2 < pic_zmin)
       pic_zmin = z2;
 
    if (z1 > pic_zmax)
       pic_zmax = z1;

    if (z2 > pic_zmax)
       pic_zmax = z2;
 
    if (radius > biggest_r)
       biggest_r = radius;

}

/*
** draw_objects()
**
** This function draws converts the objects in the list to actual 
** images in memory.
**
** do_scale - convert the positions, lengths, etc to pixel values - this
**            is TRUE or FALSE depending on whether the operation has been
**            done already on the objects.
** fix_line - Figure out where the endpoints of lines intersect spheres -
**            this is TRUE or FALSE depending on whether the operation has been
**            done already on the objects.
** sort_obj - set to TRUE if the object list has yet to be sorted.            
*/
#ifndef _NO_PROTO
draw_objects(int do_scale, int fix_line, int sort_obj)
#else
draw_objects(do_scale, fix_line, sort_obj)
int do_scale,fix_line,sort_obj;
#endif
{
   int i,j;
   Display *dpy;
   Window win;
   OBJECT *object;
   float rangex,rangey,range,oldmax,oldmin;
   dpy = XtDisplay(canvas);
   win = XtWindow(canvas);
 
  
   
   /* if the size of the window changed then make sure that
   ** that the image will remain centered as much as possible.
   */
   rangex = canvas_width/(xmax - xmin);
   rangey = canvas_height/(ymax - ymin);
  
   if (rangex <  rangey) {
         range = ((canvas_height*(xmax - xmin)/canvas_width));
	 oldmax = ymax;
	 oldmin = ymin;
         ymax = ymax + ((range - (ymax - ymin))/2);
         ymin = ymin - ((range - (oldmax - ymin))/2);
	 if (oldmin == world_ymin) 
            world_ymin = ymin;
	 if (oldmax == world_ymax)
	    world_ymax = ymax;

   } else
     if (rangex > rangey) {
         range = ((canvas_width*(ymax - ymin)/canvas_height));
	 oldmax = xmax;
	 oldmin = xmin;
         xmax = xmax + ((range - (xmax - xmin))/2);
         xmin = xmin - ((range - (oldmax - xmin))/2);
         if (oldmin == world_xmin)
	    world_xmin = xmin;
	 if (oldmax == world_xmax)
	    world_xmax = xmax;
     }

  
   offset_width = offset_height = 0;

   /* Find out where lines intersect spheres - if needed */   
   if (fix_line)      
     fix_lines();

   /* If the whole image is not visible then set the "visible"
   ** flag to FALSE on those objects that are outside the viewing area
   */
   clip_z(); 
   clip_xy();

   /* This is to support parallel or perspective projections */
   if (do_proj)
      project_pic();

   /* Sort the objects by by Z values if needed */
   if (sort_obj)
     sort_objects();

   /* Strart from the object that is farthest away */
   object = depth_list[first_z].ptr;     
   j = first_z;  
   
   /* Convert values to real pixel values if needed */
   if (do_scale)
     scale_picture(end_depth,first_z);
 
   /* Go through the list and draw the objects */
   for (i=0;i<end_depth;i++) {
                    
          if ((object->type == LINE) || (object->type == BOUNDS))
             draw_line(object);
          if (object->type == SPHERE)
             draw_sphere(object);
          object = depth_list[depth_list[j].next].ptr;
          j = depth_list[j].next;
   }

   /* Last - draw any text on the figure */
   draw_text();

   /* Force the image to be sent to the display */
   XCopyArea(dpy,canvas_pix,win,canvas_gc,0,0,canvas_width, canvas_height, 0,0);
   XFlush(dpy);
}

/*
** draw_text()
**
** This may be added later - draw any text on the image.
**
**
**
*/
draw_text()
{

    Display *dpy = XtDisplay(canvas);

    /* XDrawString(dpy,canvas_pix,text_gc, 20, 200, "A Sample Molecule",17); */

}

/*
** scale_picture()
**
** This function converts the actual lengths to the scale
** used on the display - pixels. 
**
**
**
*/
#ifndef _NO_PROTO
scale_picture(int count, int first)
#else
scale_picture(count,first)
int count,first;
#endif
{
      int i,j,offset_height,offset_width,min;
      float range,offset,biggest_radius,rangey;
      OBJECT *old_obj_ptr, *obj_ptr;

      obj_ptr = depth_list[first].ptr;
      old_obj_ptr = obj_ptr;

      /* Set up some default sizes if the ones given 
      ** are too small.
      */
      if (xmin == xmax) {
         xmin = obj_ptr->x1 - (obj_ptr->radius * 2);
         xmax = obj_ptr->x1 + (obj_ptr->radius * 2);
      }

      if (ymin == ymax) {
         ymin = obj_ptr->y1 - (obj_ptr->radius * 2);
         ymax = obj_ptr->y1 + (obj_ptr->radius * 2);
      }   
   
      range = xmax - xmin;
      offset_width = offset_height = 0;
      
      /* This loop adjust all the X values */      
      j = first;      
      for (i=0;i<count;i++) { 

         /* Calculate how far over the object is */
         offset = obj_ptr->x1-xmin;
         if (offset < 0)
            offset = -offset;

         if (obj_ptr->x1 < xmin)
            offset = -offset;

         /* Adjust the real (pixel) coordinates */
         obj_ptr->x1r = (int)((offset/range)*(canvas_width)) + offset_width;

         /* If this is a line then adjust the second point */
         if ((obj_ptr->type == LINE) || (obj_ptr->type == BOUNDS)) {
             offset = obj_ptr->x2-xmin;
             if (offset < 0)
                offset = -offset;
             if (obj_ptr->x2 < xmin)
                offset = -offset;

             obj_ptr->x2r = (int)((offset/range)*(canvas_width));

	 }

        
	 /* Adjust the radius */
         obj_ptr->radius_scaled = (obj_ptr->radius/range)*(canvas_width);
         if (obj_ptr->radius_scaled <= 0) obj_ptr->radius_scaled = 1;
	 /* Make it invisible */
	 if (obj_ptr->type == SPHERE) 
	    if (!apply_resize[obj_ptr->color_index]) 
	       obj_ptr->radius_scaled = 1;
         obj_ptr = depth_list[depth_list[j].next].ptr;  
         j = depth_list[j].next;

     } /* end for loop */

      obj_ptr = old_obj_ptr;

      range = ymax - ymin; 
      if (range < 0)
         range = -range;

      /* This loop adjusts all the Y values */      
      j = first;
      for (i=0;i<count;i++) {    
         offset = obj_ptr->y1-ymin;
         if (offset < 0)
            offset = -offset;
         if (obj_ptr->y1 < ymin)
            offset = -offset;
         obj_ptr->y1r = (int) (canvas_height - ((offset/range)*(canvas_height))) - offset_height; 
         /* if (obj_ptr->y1r <= 0) obj_ptr->y1r = 1; */
         if ((obj_ptr->type == LINE) || (obj_ptr->type == BOUNDS)) {
             offset = obj_ptr->y2-ymin;
             if (offset < 0)
                offset = -offset;
             if (obj_ptr->y2 < ymin)
                offset = -offset;

             obj_ptr->y2r = (int) (canvas_height -  ((offset/range)*(canvas_height))) - offset_height;
             /* if (obj_ptr->y2r <= 0) obj_ptr->y2r = 1; */
	 }
         obj_ptr = depth_list[depth_list[j].next].ptr;  
         j = depth_list[j].next;
         
      }

}

/* 
** fix_lines()
**
** This function will look at each line object and call a function
** to fixe the endpoints to account for sphere intersection..
**
*/
fix_lines()
{
      OBJECT *object;
      int i;

      object = object_head.next;
      for (i=0;i<total_objects;i++) {
         if (object->active) 
          if (object->type == LINE) 
             adjust_line(object);
            
          object = object->next;
      }

}

/*
** clip_z()
**
** This looks at all the active objects and sends them to a function
** to decide if they are visible given the current viewport.
**
*/
clip_z()
{
      OBJECT *object;
      int i;

      object = object_head.next;
      for (i=0;i<total_objects;i++) {
          if (object->active == TRUE) {
             object->visible = in_region_z(object);
             object = object->next;
	  }
      }

}

/*
** clip_xy()
**
** This looks at all the active objects and sends them to a function
** to decide if they are visible given the current viewport.

**
**
*/
clip_xy()
{
      OBJECT *object;
      int i;

      object = object_head.next;
      for (i=0;i<total_objects;i++) {
          if (object->active == TRUE) {
             object->visible = in_region_xy(object);
             object = object->next;
	   }
      }
}

/*
** adjust_line()
**
** Given an object this function checks to see if the endpoints
** are the centers of spheres, if the endpoints terminate at 
** spheres then the intersecting points are calculated so that
** when the line is drawn it will look like it meets the solid
** sphere.
**
*/
#ifndef _NO_PROTO
adjust_line(OBJECT *obj)
#else
adjust_line(obj)
OBJECT *obj;
#endif
{
    OBJECT *guess;

    
    double x1,y1,x2,y2,z1,z2,xsign,ysign,zsign,anglex_y,anglez_y,anglez_x;
    double delta_x,delta_y,delta_z,x_new_rad;
    double x1_new,y1_new,x2_new,y2_new,z1_new,z2_new,a,b,c;
    double a1,a2,a3,temp,t;
    double radius, radius_2;
    float s1,s2;
  
       x2 = (double)obj->x2;
       x1 = (double)obj->x1;
       y2 = (double)obj->y2;
       y1 = (double)obj->y1;
       z2 = (double)obj->z2;
       z1 = (double)obj->z1;
 
 

       a1 = x2 - x1;
       a2 = y2 - y1;
       a3 = z2 - z1;

    /* Check for the endpint being a sphere */       
       if (object->sphere_1 != NULL) {

          /* Calculate the parametric equation for the line and then use
	  ** resulting coefficients to determine the intersection point.
          */
	  radius =  (double) obj->sphere_1->radius;
	  if (!apply_resize[obj->sphere_1->color_index]) radius = 0.0;
          temp = (radius*radius)/((a1*a1) + (a2*a2) + (a3*a3));
 
          t = sqrt(temp);
          s1 = t;

          x1_new = x1 + a1*t;
          y1_new = y1 + a2*t;
          z1_new = z1 + a3*t;
       } else {
          
          /* Endpoint does not terminate at sphere */
	  s1 = 0;
          x1_new = x1;
	  y1_new = y1;
	  z1_new = z1;
       }
       
       /* Do the same as above for the opposite endpoint */
       a1 = x1 - x2;
       a2 = y1 - y2;
       a3 = z1 - z2;

       if (object->sphere_2 != NULL) {
          radius_2 = obj->sphere_2->radius;
	  if (!apply_resize[obj->sphere_2->color_index]) radius_2 = 0.0;
          temp = (radius_2*radius_2)/((a1*a1) + (a2*a2) + (a3*a3));

          t = sqrt(temp);
	  s2 = t;
  
          x2_new = x2 + a1*t;
          y2_new = y2 + a2*t;
          z2_new = z2 + a3*t;
       } else { 
	 s2 = 0;
	 x2_new = x2;
         y2_new = y2;
         z2_new = z2;
       }

      /* Check if the spheres overlap */
      
      if ((s1+s2)<1.0) {

         obj->x1 = x1_new;
         obj->y1 = y1_new;
         obj->z1 = z1_new;

         obj->x2 = x2_new;
         obj->y2 = y2_new;
         obj->z2 = z2_new;
      } else {
         /* spheres touch so no line is visible */
         obj->visible = FALSE;
         obj->x1 = xmin - 1;
         obj->x2 = xmin - 1;
      }
   }

/*
** draw_sphere()
**
** This draws the sphere which involves positioning it in the correct
** place and copying the sphere pixmpa to the window pixmap.
**
**
*/
#ifndef _NO_PROTO
draw_sphere(OBJECT *object)
#else
draw_sphere(object)
OBJECT *object;
#endif
{

    int xpos, ypos, width,height,k,start_x,start_y;
    Display *dpy;
    Window win;
    OBJECT *thisobj;


    width = object->radius_scaled*2;
    height = object->radius_scaled*2;

    /* First see if there is something that looks like this already allocated
    **   - the radius must be the same and the color must be the same.
    */
    thisobj = depth_list[first_z].ptr;
    
    k = first_z;
    while (thisobj != object) {
           if ((thisobj->radius_scaled == object->radius_scaled) && 
               (!strcmp(thisobj->color,object->color)) &&
               (thisobj->win_gc)) {
                 object->use_this = thisobj;
                 thisobj = object;
           } else {
                 thisobj = depth_list[depth_list[k].next].ptr;
                 k = depth_list[k].next;
	   }
    }

   /* Calculate the upper left corner position */  
   start_x = start_y = 0;
   xpos = object->x1r - object->radius_scaled;
   ypos = object->y1r - object->radius_scaled;

   /*
   if (xpos < 0) {
      start_x = -xpos;
      width = width + xpos;
      xpos = 0;
   } else
      start_x = 0;

   if (ypos < 0) {
      start_y = -ypos;
      height = height + ypos;
      ypos = 0;
   } else
      start_y = 0;

   if ((xpos+width) > canvas_width)
      width = width - (canvas_width - xpos);

   if ((ypos+height) > canvas_height)
      height = height - (canvas_height - ypos);

   */

   dpy = XtDisplay(toplevel);
   win = XtWindow(canvas);

   /* Check if another objects pixmap should be used */
   if (object->use_this == NULL) {

       /* Check if a context already exists */
       if (object->win_gc == NULL) {
	  /* This is the first time drawing this */
          create_alt_gc(width,height,object);
          create_a_mask(width,height,object); 
          create_gc(width,height,object);
          create_pixmap(object);
       } else {
	  /* This was drawn before - just need to set a new position */
          XSetClipOrigin(XtDisplay(canvas),object->win_gc,xpos,ypos);
       }
       /* This looks like the place to do depth adjustments... */
       sphere_depth_adj(object,width,height);
       XCopyArea(dpy,object->pixmap,canvas_pix,object->win_gc,start_x,start_y, width, height, xpos, ypos);
    } else {
          /* Use another object that was already drawn - set new poistion */
          XSetClipOrigin(XtDisplay(canvas),object->use_this->win_gc,xpos,ypos);
          /* This looks like the place to do depth adjustments... */
	  sphere_depth_adj(object,width,height);
          XCopyArea(dpy,object->use_this->pixmap,canvas_pix,object->use_this->win_gc,start_x,start_y, width, height, xpos, ypos);

     
    }
}

/*
** draw_line()
**
** This draws a line, since lines do not use pixmaps the line is drawn 
** from scratch each time. This also supports shading the line which makes
** it look like a cylinder - this can get to be slow because the shading
** is done from scratch for each line. In the case of spheres the shading
** is done in off screen pixmaps.
** The reason that offscreen pixmaps are not used for lines is that 
** an extensize amount of masking would have to be done if the line did
** not lie along one of the three axis.
**
*/
#ifndef _NO_PROTO
draw_line(OBJECT *object)
#else
draw_line(object)
OBJECT *object;
#endif
{
 
    int i, xpos, ypos, width,height;
    Display *dpy;
    Window win;
    int color,x1,y1,x2,y2;
    int xposf,yposf;
    Colormap cmap;
    float distance;
    XColor new_color,end_color;
    int dist,decstepr,decstepg,decstepb;
    Arg args[10];
    int n,linestep;
    double dlen;
    XGCValues values;
    int background_data, foreground_data;
    int stepsize;
    int dstep,len;
    int radius,k;
    OBJECT *thisobj;
    int found_it;

    /* First see if there is something that looks like this already allocated
    **   - the radius must be the same and the color must be the same.
    */
    thisobj = depth_list[first_z].ptr;
    k = first_z;
    found_it = FALSE;
    while (thisobj != object) {
           if ((thisobj->radius_scaled == object->radius_scaled) &&
               (!strcmp(thisobj->color,object->color)) &&
               (thisobj->win_gc)) {
                 object->use_this = thisobj;
                 thisobj = object;
                 found_it = TRUE;
           } else {
                 thisobj = depth_list[depth_list[k].next].ptr;
                 k = depth_list[k].next;
	   }
    }  


    dpy = XtDisplay(canvas);
    win = XtWindow(canvas);

    x1 = object->x1r;
    y1 = object->y1r;
    x2 = object->x2r;
    y2 = object->y2r;
 
    /* Allocate a GC for the line */
    if (((object->use_this == NULL) && (object->win_gc == NULL)) || (lshade > 1)) {

        cmap =  DefaultColormap(dpy, DefaultScreen(dpy));

        n = 0;
        XtSetArg(args[n], XtNforeground, &foreground_data); n++;
        XtSetArg(args[n], XtNbackground, &background_data); n++;
        XtGetValues(canvas, args, 2);

        new_color.pixel = get_color(canvas,object->color);

        XQueryColor(dpy,cmap,&new_color);

        end_color.pixel = get_color(canvas,"white");
        XQueryColor(dpy,cmap,&end_color);

        if ((object->use_this) && (object->use_this->win_gc)) 
           object->win_gc = object->use_this->win_gc;
        else {
           if (object->win_gc == NULL) {

              values.foreground = foreground_data;
              values.background = background_data;
              values.line_width = object->radius_scaled;
              values.fill_style = FillSolid;
              values.cap_style = CapRound;

              object->win_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
                (GCForeground | GCBackground | GCLineWidth | GCCapStyle ), &values);
	   }
	}
	
	/* Shade the line from the outside to the inside to make it
	** look like a cylinder.
	*/
        radius = object->radius_scaled;
    
        if (radius < lshade)
           stepsize = radius;
        else
           stepsize = lshade;

        decstepr = (end_color.red - new_color.red) / stepsize;
        decstepg = (end_color.green - new_color.green) / stepsize;
        decstepb = (end_color.blue - new_color.blue) / stepsize;
        
        linestep = radius / stepsize;
        values.line_width = radius;
        dstep = (radius*2) / stepsize;
        for (i=0;i<stepsize;i++) {

           XAllocColor(dpy,cmap, &new_color);
           values.foreground = new_color.pixel;

           XChangeGC(XtDisplay(canvas),object->win_gc,
              GCForeground | GCLineWidth,  &values);

           XDrawLine(dpy,canvas_pix, object->win_gc, x1, y1, x2, y2);
           values.line_width = values.line_width - linestep;
           new_color.red = new_color.red + decstepr;
           new_color.green = new_color.green + decstepg;
           new_color.blue = new_color.blue +  decstepb;
        }

      } else {
	  /* The line is only 1 color and the GC was allocated before */
          if (object->win_gc == NULL) 
            XDrawLine(dpy,canvas_pix, object->use_this->win_gc, x1, y1, x2, y2);
          else 
            XDrawLine(dpy,canvas_pix, object->win_gc, x1, y1, x2, y2);
      }
}

/*
** in_region_xy()
**
** This function calculates if an object is at least partly visible
** in a given viewport. If the object is a line the endpoints are
** clipped if part of the line is outside the viewport.
**
*/
#ifndef _NO_PROTO
in_region_xy(OBJECT *object)
#else
in_region_xy(object)
OBJECT *object;
#endif
{
    float minx,miny,maxx,maxy,rxmin,rymin,rxmax,rymax;
    float minx2,miny2,maxx2,maxy2,a1,a2,a3,t;

    if (object->type == SPHERE) {
       minx = object->x1-object->radius;
       miny = object->y1-object->radius;
       maxx = object->x1+object->radius;
       maxy = object->y1+object->radius;
       rxmin = xmin - offset_width; rxmax = xmax + offset_width;
       rymin = ymin - offset_height; rymax = ymax + offset_height;
       if ((maxx < rxmin) || (maxy < rymin) || (minx > rxmax) || (miny > rymax))
          return(FALSE);
       else
          return(TRUE);

     } else {
       if ((object->x1 >= xmin) && (object->x1 <= xmax) &&
           (object->x2 >= xmin) && (object->x2 <= xmax) &&
           (object->y1 >= ymin) && (object->y1 <= ymax) &&
	   (object->y2 >= ymin) && (object->y2 <= ymax))
	      /* The whole line is inside the window */
              return(TRUE);
       else {
         /* Check if any of the line is visible */
         if (((object->x1 < xmin) && (object->x2 < xmin)) || 
	    ((object->x1 > xmax) && (object->x2 > xmax)) ||
	    ((object->y1 < ymin) && (object->y2 < ymin)) ||
	    ((object->y1 > ymax) && (object->y2 > ymax)))
	       return(FALSE);

	  a1 = object->x2 - object->x1;
	  a2 = object->y2 - object->y1;
	  a3 = object->z2 - object->z1;


         /* Clip the left hand side */
	 if (object->x1 < xmin) {
	    t = (xmin - object->x1)/a1;
	    object->x1 = xmin;
	    object->y1 = object->y1 + a2*t;
	    object->z1 = object->z1 + a3*t;
          } else
	     if (object->x2 < xmin) {
               t = (xmin - object->x2)/(-a1);
	       object->x2 = xmin;
	       object->y2 = object->y2 - a2*t;
               object->z2 = object->z2 - a3*t;
             } else         	     
                /* Clip the right hand side */
	        if (object->x1 > xmax) {
	          t  = (xmax - object->x1)/a1;
		  object->x1 = xmax;
		  object->y1 = object->y1 + a2*t;
	          object->z1 = object->z1 + a3*t;
	          
	        } else
	           if (object->x2 > xmax) {
                     t  = (xmax - object->x2)/(-a1);
		     object->x2 = xmax;
		     object->y2 = object->y2 - a2*t;
	             object->z2 = object->z2 - a3*t;
		   }
         /* Clip the bottom */
	 if (object->y1 < ymin) {
	    t = (ymin - object->y1)/a2;
	    object->y1 = ymin;
	    object->x1 = object->x1 + a1*t;
	    object->z1 = object->z1 + a3*t;
	 } else
	    if (object->y2 < ymin) {
               t = (ymin - object->y2)/(-a2);
	       object->y2 = ymin;
	       object->x2 = object->x2 - a1*t;
               object->z2 = object->z2 - a3*t;
            } else
               /* Clip the top */
	       if (object->y1 > ymax) {
	          t  = (ymax - object->y1)/a2;
		  object->y1 = ymax;
		  object->x1 = object->x1 + a1*t;
	          object->z1 = object->z1 + a3*t;
	       } else
	          if (object->y2 > ymax) {
                     t  = (ymax - object->y2)/(-a2);
		     object->y2 = ymax;
		     object->x2 = object->x2 - a1*t;
	             object->z2 = object->z2 - a3*t;
		  }

         /* If we get this far then the linw is clipped and visible */
         return(TRUE);         
       }

     }

}

/*
** in_region_z()
**
** Clipping of the Z values is done similar to the above routine for
** the X,Y coordinates.
**
*/
#ifndef _NO_PROTO
in_region_z(OBJECT *object)
#else
in_region_z(object)
OBJECT *object;
#endif
{
  float a1,a2,a3,t;

  if (zclip_active) {
  if (object->type == SPHERE) {
    
    if ((object->z1 >= zmin) && (object->z1 <= zmax)) 
         return(TRUE);
    else 
         return(FALSE);
  } else {
    if ((object->z1 >= zmin) && (object->z1 <= zmax) ||
        (object->z2 >= zmin) && (object->z1 <= zmax)) 
        return(TRUE);
    else {
        if ((object->z1 < zmin) && (object->z1 <= zmax) && (object->z2 >= zmin) && (object->z2 <= zmax)) {
           /* one in, one out */
           a1 = object->x2 - object->x1;
           a2 = object->y2 - object->y1;
           a3 = object->z2 - object->z1;
           t = (zmin - object->z1)/a3;
           object->x1 = object->x1 + (a1*t);
           object->y1 = object->y1 + (a2*t);
           object->z1 = zmin;
           return(TRUE);

        } else
        if ((object->z1 >= zmin) && (object->z1 <= zmax) && (object->z2 < zmin) && (object->z2 <= zmax)) {
           /* one in, one out */
           a1 = object->x1 - object->x2;
           a2 = object->y1 - object->y2;
           a3 = object->z1 - object->z2;
           t = (zmin - object->z2)/a3;
           object->x2 = object->x2 + (a1*t);
           object->y2 = object->y2 + (a2*t);
           object->z2 = zmin;
           return(TRUE);
       } else
       if ((object->z1 < zmin) && (object->z1 <= zmax) && (object->z2 < zmin) && (object->z2 <= zmax)) {        
          return(FALSE);
       } else
       if ((object->z1 >= zmin) && (object->z1 > zmax) && (object->z2 >= zmin) && (object->z2 > zmax)) {        
          return(FALSE);
       } else
       if ((object->z1 >= zmin) && (object->z1 > zmax) && (object->z2 >= zmin) && (object->z2 <= zmax)) {
           a1 = object->x2 - object->x1;
           a2 = object->y2 - object->y1;
           a3 = object->z2 - object->z1;
           t = (zmax - object->z1)/a3;
           object->x1 = object->x1 + (a1*t);
           object->y1 = object->y1 + (a2*t);
           object->z1 = zmax;
           return(TRUE);
       }
       if ((object->z1 >= zmin) && (object->z1 <= zmax) && (object->z2 >= zmin) && (object->z2 > zmax)) { 
           a1 = object->x1 - object->x2;
           a2 = object->y1 - object->y2;
           a3 = object->z1 - object->z2;
           t = (zmax - object->z2)/a3;
           object->x2 = object->x2 + (a1*t);
           object->y2 = object->y2 + (a2*t);
           object->z2 = zmax;
           return(TRUE);
	 }
    } 
  }
  } else
     return(TRUE); 
} 
int next = 0;
int old_x =0, old_y =0;

/*
** get_color()
**
** This returns the pixel value for a given color name 
** which is specified as a string.
**
*/    
#ifndef _NO_PROTO
get_color(Widget w, char *name)
#else
get_color(w,name)
Widget w;
char *name;
#endif
{
    Display *dpy = XtDisplay(w);
    int scr = DefaultScreen(dpy);
    Colormap cmap = DefaultColormap(dpy, scr);
    XColor  color, ignore;

    if (mono_only) 
       return(WhitePixel(dpy,scr));

    XLookupColor(dpy,cmap,name,&color, &ignore);

    if (MyXAllocColor(dpy, cmap, &color))
       
       return(color.pixel);
    else {
       fprintf(stderr,"Warning: Could not allocate color %s\n",name);
       return(BlackPixel(dpy,scr));
     }
  }

/*
** project_pic()
**
** This does either a parallel or perspective projection of the points
** depending on how the global variable proj_type was set from the
** command line. Currently no projection produces the best quality 
** structure.
**
*/
project_pic()
{

    OBJECT *object;
    int i;

    if (proj_type == PERSPECTIVE) {
    xp = ((xmax - xmin)/2.0) + xmin;

    yp = ((ymax - ymin)/2.0) + ymin;

    zp = -100;

    
    object = object_head.next;

    for (i=0;i<object_count;i++) {
       
       object->x1 = ((xp*object->z1) - (zp*object->x1))/(object->z1 - zp);
       object->y1 = ((yp*object->z1) - (object->y1*zp))/(object->z1 - zp);

       if ((object->type == LINE) || (object->type == BOUNDS)) {
          object->x2 = ((xp*object->z2) - (object->x2*zp))/(object->z2 - zp);
          object->y2 = ((yp*object->z2) - (object->y2*zp))/(object->z2 - zp);
	}
        object = object->next;
    }

  } else {
    object = object_head.next;

    for (i=0;i<object_count;i++) {
       object->x1 = object->x1 - object->z1*(xp/zp);
       object->y1 = object->y1 - object->z1*(yp/zp);
       if (object->type == LINE) {
          object->x2 = object->x2 - object->z2*(xp/zp);
          object->y2 = object->y2 - object->z2*(yp/zp);
       }
       object = object->next;
    }

  }
          
}







