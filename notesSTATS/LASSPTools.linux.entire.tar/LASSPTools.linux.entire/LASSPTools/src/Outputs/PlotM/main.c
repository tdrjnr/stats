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
** ---------------------------------------------------------------------------
** X Window widget name structure:
**                 toplevel
**                    +
**                  board
**       +------------+-----------------+----------+
**      info        frame_x          frame_y  frame_canvas
**                    +                 +          +
**                  info_x           info_y      canvas
**
** info_x-> scale_x xmin_label xmax_label
** info_y-> scale_y ymin_label ymax_label
**
** info-> x_label y_label z_label x_text y_text z_text name_label name_text
**        quit help arr_sep up_scale down_scale up_radius down_radius
**        up_shade size_label shade_label 
**                  
*/

#include <stdio.h>
#include <math.h>
#include <fcntl.h>

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

extern int tester();
extern void redraw_canvas();
extern void resize_canvas();
extern void show_mouse_pos();
extern create_pixmap();
extern create_a_mask();

#ifndef _NO_PROTO
extern int scale_picture(float x, float x1, float y, float y1);
extern int add1_sphere(float x, float y, float z, float r, char *color, char *name, int color_in);
extern int add1_line(float x1, float y1, float z1, float x2, float y2, float z2,
     float r, char *color, char *name);
void input_handle(char buf[]);
void rotate_queue(caddr_t data, int *file_num, XtInputId *id);
void input_queue(caddr_t data, int *file_num, XtInputId *id);
extern void handle_timeout(caddr_t data, XtIntervalId *id);
extern create_gc(float width, float height,  OBJECT *object);
extern create_alt_gc(float width, float height, OBJECT *object);
extern double sqrt(double x);
#else
extern int scale_picture();
extern int add1_sphere();
extern int add1_line();
extern void input_handle();
extern void rotate_queue();
extern void input_queue();
extern void handle_timeout();
extern int create_gc();
extern int create_alt_gc();
extern double sqrt();
#endif

typedef struct buf_type {
     char buf[80];
     struct buf_type *next;
} BUF_TYPE;

/*
** global variables 
*/
Widget toplevel;
Widget frame_canvas;
Widget canvas;
Widget form;
Widget frame_x;
Widget frame_y;
Widget frame_canvas;
Widget board;
GC gc;
GC gc_clip;
GC canvas_gc;
GC text_gc;
Pixmap pix;
Pixmap canvas_pix;
XColor start_color;
XtInputId curr_id;
XGCValues values;
XColor cdata[512];
XtAppContext context;
char *name_table[MAX_NAMES];

float xmin,ymin,xmax,ymax,zmin,zmax;           
double xp,yp,zp;
PROJ proj_type;
float biggest_r = 0;
OBJECT object_head;
OBJECT *object = &object_head;
int object_count = 0;
int lshade = 1;
int cshade = 10;
int old_cshade = 10;

Dimension canvas_width,canvas_height;
int pfd[2];
DEPTH *depth_list;
int first_z;
int do_proj = FALSE;
int no_size = TRUE;
float z_proj;
int old_object_count,total_objects = 0;
int load_rotate = TRUE;

int interactive = TRUE;
int stdin_valid = TRUE;
int zclip_active = FALSE;
int last_crlf = FALSE;
int end_depth;
float offset_width,offset_height;
int radius_scale = TRUE;
int input_2D = FALSE;
int update_rotate = FALSE;
int first_time = TRUE;
float global_rot[3][3];
int mono_only = FALSE;
float default_radius = 0.0;
float default_scale = 1.0;
float radius_map[100];
int xyt_format = FALSE;
int fix_radius = FALSE;
int depth_cues = 0;
float pic_xmin, pic_ymin, pic_zmin, pic_xmax, pic_ymax, pic_zmax;

int num_colors = 0;

/*
** This defines the default set of colors that the spheres and
** lines are drawn in.
**
*/
char colors[100][25] = {
    "blue", 
    "red", 
    "firebrick", 
    "navy", 
    "yellow",
    "green", 
    "brown", 
    "gray", 
    "cyan", 
    "violet", 
    "white", 
    "khaki"
};
int apply_resize[100];

BUF_TYPE begin_buf = {{'\0'},(BUF_TYPE *)NULL};
BUF_TYPE *curr_buf_ptr = NULL;
BUF_TYPE *last_buf_ptr = &begin_buf;
long pix_back;
#define IS_MONO(dpy,scr) DefaultDepth(dpy,scr)

/*
** main()
**
**
*/
#ifndef _NO_PROTO
main(int argc, char *argv[])
#else
main (argc, argv)
int argc;
char *argv[];
#endif
{
    Pixmap tile;
    int i,n,button_width,button_height,xpos,ypos,j,count,scr;
    Dimension width, height;
    Arg args[20];
    Display *dpy;
    Window win;
    XGCValues values;

    int foreground_data, background_data;


    for (i=0;i<100;i++)
      apply_resize[i] = TRUE;

    for (i=0;i<MAX_NAMES;i++)
       name_table[i] = NULL;

    object_head.next = (OBJECT *) &object;

    /* Create a top level Widget */
    toplevel = XtInitialize(argv[0],"Formedit",args,0, &argc,argv);


    dpy = XtDisplay(toplevel);
    scr = DefaultScreen(dpy);


    if (IS_MONO(dpy,scr) == 1) {
       mono_only = TRUE;
       cshade = 1;
       pix_back = get_color(toplevel,"white");
    } else
       pix_back = get_color(toplevel,"gray");

  
    n = 0;
    XtSetArg(args[n],XtNwidth,&width); n++;
    XtSetArg(args[n],XtNheight,&height); n++;
    XtGetValues(toplevel,args,n);

    /* Set a minimum size - this can be overridden via command
    ** line options.
    */
    if ((width < 600) || (height < 600)) {
      width = height = 600; 
      n = 0;
      XtSetArg(args[n],XtNwidth, width); n++;
      XtSetArg(args[n],XtNheight, height); n++;
    } else
      n = 0;

    XtSetArg(args[n],XtNbackground,pix_back); n++;
    XtSetValues(toplevel,args,n);
    
    /* Check any command line arguments */
    parse_arguments(argc,argv);

    /* Check if we are only doing output or we want the
    ** to be able to do interactive things.
    */
    if (interactive) {
      
       /* Start the rotate program */
       start_rotate(argv);

       /* Create widgets for the drawing area and X and Y axis areas */
       board = XtCreateManagedWidget("form", xmFormWidgetClass, toplevel, args,n);

       n = 0;
       XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNtopPosition,15); n++;
       XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNbottomPosition,93); n++;
       XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNleftPosition,13); n++;
       XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNrightPosition,100); n++;
       XtSetArg(args[n],XtNbackground,pix_back); n++;
       frame_canvas = XtCreateManagedWidget("frame", xmFrameWidgetClass, board, args, n);


       n = 0;
       XtSetArg(args[n],XmNtopAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNtopPosition,15); n++;
       XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNbottomPosition,93); n++;
       XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNleftPosition,0); n++;
       XtSetArg(args[n],XmNrightAttachment,XmATTACH_WIDGET); n++;
       XtSetArg(args[n],XmNrightWidget,frame_canvas); n++;
       XtSetArg(args[n],XtNbackground,pix_back); n++;
       frame_y = XtCreateManagedWidget("frame", xmFrameWidgetClass, board, args, n);

       n = 0;
       XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET); n++;
       XtSetArg(args[n],XmNtopWidget,frame_canvas); n++;
       XtSetArg(args[n],XmNbottomAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNbottomPosition,100); n++;
       XtSetArg(args[n],XmNleftAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNleftPosition,0); n++;
       XtSetArg(args[n],XmNrightAttachment,XmATTACH_POSITION); n++;
       XtSetArg(args[n],XmNrightPosition,100); n++;
       XtSetArg(args[n],XtNbackground,pix_back); n++;
       frame_x = XtCreateManagedWidget("frame", xmFrameWidgetClass, board, args, n);

       n = 0;
       XtSetArg(args[n],XtNwidth,&canvas_width); n++;
       XtSetArg(args[n],XtNheight,&canvas_height); n++;
       XtGetValues(board,args,n);

       /* Set the drawing area to include about 75% of the window */
       canvas_width = (canvas_width*(0.75)) - 10;
       canvas_height = (canvas_height*(0.75)) - 10;

       n = 0;
       XtSetArg(args[n],XtNwidth,canvas_width); n++;
       XtSetArg(args[n],XtNheight,canvas_height); n++;
       canvas = XtCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
                                       frame_canvas, args, n);

       n = 0;
       XtSetArg(args[n], XtNforeground, &foreground_data); n++;
       XtSetArg(args[n], XtNbackground, &background_data); n++;
    
       XtGetValues(canvas, args, 2);

       values.foreground = foreground_data;
       values.background = background_data;

       /* Set up the drawing GC */
       canvas_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
           (GCForeground | GCBackground), &values);

       values.foreground = WhitePixel(dpy,scr);
       values.background = BlackPixel(dpy,scr);
       values.font = XLoadFont(dpy,"Fixed");

       /* Set up a GC for any text we may write */
       text_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
           (GCForeground | GCBackground | GCFont), &values);

       context=XtWidgetToApplicationContext(toplevel);

       XtRealizeWidget(toplevel);

       /* Define a memory mapped pixmap - we will draw to this and then transfer
       ** the whole image to the actual display to cut down on all the X calls
       ** over the network.
       */
       n = 0;
       XtSetArg(args[n],XtNwidth,&canvas_width); n++;
       XtSetArg(args[n],XtNheight,&canvas_height); n++;
       XtGetValues(canvas,args,n);
       canvas_pix = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)), canvas_width,
                     canvas_height, DefaultDepthOfScreen(XtScreen(canvas)));

       /* Set a default background color */
       XFillRectangle(XtDisplay(canvas),canvas_pix,canvas_gc,0,0,canvas_width,canvas_height);

       /* Attach a function to stdin - when data is available then the function
       ** input_queue will be called.
       */
       XtAppAddInput(context,fileno(stdin),XtInputReadMask,input_queue,NULL); 
    
       /* If the rotate program is up and running attach the function
       ** rotate_queue to be called whenever there is data to be read.
       */
       if (load_rotate)
          XtAppAddInput(context,pfd[0],XtInputReadMask,rotate_queue,NULL); 
    
       /* Attach a function so that the atom coordinates will be displayed when
       ** the mouse button is clicked over an atom.
       */
       XtAddEventHandler(canvas,ButtonPressMask, FALSE, show_mouse_pos, NULL);

       /* Attach a function to handle redrawing the picture when it is
       ** uncovered.
       */
       XtAddEventHandler(canvas,ExposureMask, FALSE, redraw_canvas, NULL);

       /* Attach a function to handle a resize event.
       */
       XtAddEventHandler(canvas,StructureNotifyMask, FALSE, resize_canvas, NULL);

       /* This will set up all the other widgets */
       create_info(pix_back);

    } else {
  
       /* There is no interactive control - all that is needed is a 
       ** area to draw the picture.
       */
       n = 0;
       XtSetArg(args[n],XtNwidth,&canvas_width); n++;
       XtSetArg(args[n],XtNheight,&canvas_height); n++;
       XtGetValues(toplevel,args,n);

       n = 0;
       XtSetArg(args[n],XtNwidth,canvas_width); n++;
       XtSetArg(args[n],XtNheight,canvas_height); n++;

       canvas = XtCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
                                       toplevel, args, n);

       n = 0;
       XtSetArg(args[n], XtNforeground, &foreground_data); n++;
       /* XtSetArg(args[n], XtNbackground, &background_data); n++; */
       XtGetValues(canvas, args, 1);

       values.foreground = foreground_data;
       values.background = background_data;

       /* Set up the drawing GC */
       canvas_gc = XCreateGC(XtDisplay(canvas),DefaultRootWindow(dpy),
            (GCForeground | GCBackground), &values);
 
       XtRealizeWidget(toplevel);

       n = 0;
       XtSetArg(args[n],XtNwidth,&canvas_width); n++;
       XtSetArg(args[n],XtNheight,&canvas_height); n++;
       XtGetValues(canvas,args,n);
       canvas_pix = XCreatePixmap(XtDisplay(canvas),RootWindowOfScreen(XtScreen(canvas)), canvas_width,
                     canvas_height, DefaultDepthOfScreen(XtScreen(canvas)));

       XFillRectangle(XtDisplay(canvas),canvas_pix,canvas_gc,0,0,canvas_width,canvas_height);

       context=XtWidgetToApplicationContext(toplevel);

       XtAppAddInput(context,fileno(stdin),XtInputReadMask,input_queue,NULL); 

    }

   XFlush(dpy);

   XtMainLoop();
}


static char local_buf[200];
static int partial_buf = 0;
static char local_rot_buf[200];
static int partial_rot_buf = 0;

/*
** rotate_queue()
**
** This function gets invoked when the rotate program has data
** to send to PlotMolcule. The function is actually invoked via
** X from the XtMainLoop() function. 
** To make pipes and X work together a few special things 
** need to be done. The data does not necessarily arrive
** in nice fixed size packets so they must be built up, when we
** have a complete rotation matrix packet the function read_rotate()
** is called to build the rotation matrix.
**
*/
#ifndef _NO_PROTO
void rotate_queue(caddr_t client_data, int *file_num,XtInputId *id)
#else
void rotate_queue(client_data, file_num, id)
caddr_t client_data;
int *file_num;
XtInputId *id;
#endif
{
      BUF_TYPE *tmp_buf_ptr;
      int flags,block,noblock,n,i,j;
      char buf[1001];
  
      /* Set the pipe to rotate to be non-blocking, this is
      ** needed so that if less data then requested is 
      ** available we will not hang.
      */
      flags = fcntl(pfd[0],F_GETFL,0);
      block = flags | O_NDELAY;
      noblock = flags & ~O_NDELAY;
      fcntl(pfd[0],F_SETFL,noblock); 

      /* Read whatever data is in the pipe */
      n = read(pfd[0],buf,1000);
      
      /* Restore the pipe to blocking IO */
      fcntl(pfd[0],F_SETFL,block); 
      if (n <= 0) {
	 /* If we get here it means that we got an X event
	 ** for data but found no data.
	 */
         fprintf(stderr,"rotate pipe, 0 bytes\n");
	 return;
      }

      buf[n] = '\0';
      
      /* This loop builds the data into a nice packet consisting
      ** of 3 lines by 3 cols from whci the rotation matrix will
      ** be built.
      */
      i = 0;
      while (i < n) {
        /* This is leftover data from a previous event so tack 
	** this data until the last chunk that was seen 
	*/
        j = partial_rot_buf;
        while ((buf[i] != '\n') && (buf[i] != '\0')) 
          local_rot_buf[j++] = buf[i++];

        /* When we run out of data, mark how much is in the buffer
	** and return - the next X event will send us the remainder
	** of the data.
	*/
        if (buf[i] == '\0') {
           partial_rot_buf = j;
           return;
	}
        
        /* This is reached when and end of line is seen - check 
	** if there where two end-of-lines in a row, this means
	** that we have all the data for the rotation matrix to
	** built from. If this is just middle of the packet eol
	** then mark where we are and go on to any more data.
	*/
        i++;
        local_rot_buf[j++] = '\n';
        if ((j != 1) && (local_rot_buf[j-2] == '\n')) {
           local_rot_buf[j] = '\0';
	   read_rotate(local_rot_buf);
	   partial_rot_buf = 0;
        } else
	   partial_rot_buf = j;
    } /* end while */
}


/*
** fix_up_radii()
**
**
**
*/
fix_up_radii()
{
     float range,xrange,yrange,radius;
     int i;
     OBJECT *object = object_head.next;
     

     return(0);

     xrange = xmax - xmin;
     yrange = ymax - ymin;
   
     if (xrange > yrange) range = xrange;
     else range = yrange;

     radius = range*0.10;
     for (i=0;i<total_objects;i++) {
         if ((object->radius == 0.0) && (object->type == SPHERE))
	    object->radius = radius;
	 if ((object->radius == 0.0) && (object->type == LINE))
	    object->radius = radius / 2.0;
         object = object->next;
     }
     pic_xmax = pic_xmax + radius;
     pic_ymax = pic_ymax + radius;
     pic_xmin = pic_xmin - radius;
     pic_ymin = pic_ymin - radius;
	    
}

static char time_char;

/*
** handle_timeout()
**
** This function is invoked by the X intrinsics, basically it
** gets called every second to check for any input, if there
** is some input it invokes the function input_queue(), if there
** is no input it just reinstalls itself.
**
*/
#ifndef _NO_PROTO
void handle_timeout(caddr_t data, XtIntervalId *id)
#else
void handle_timeout(data,id)
caddr_t data;
XtIntervalId *id;
#endif
{
         int flags, n,noblock,block;

         /* The the IO to non-blocking and see if there is any data */
         flags = fcntl(fileno(stdin),F_GETFL,0);
      
         noblock = flags & ~O_NDELAY;
         fcntl(fileno(stdin),F_SETFL,noblock); 

         n = read(0,&time_char,1);

	 if (n <= 0) {
	    /* Reinstall so it is called again in 1 second */
            XtAppAddTimeOut(context,(unsigned long)1000,handle_timeout,0);
            block = flags | O_NDELAY;
            fcntl(fileno(stdin),F_SETFL,block); 
	 } else {
	    /* There is some data - call input_queue to read it */
            input_queue((caddr_t)flags,(int *)NULL,(XtInputId *)NULL);

         }

}
/*
** input_queue()
**
** This function gets invoked when there is input available from stdin.
** This function is actually invoked from X in the XtMainLoop() function,
** in order to make X and pipes work together and also not hog the cpu
** a timeout function must be used.
**
*/
#ifndef _NO_PROTO
void input_queue(caddr_t client_data, int *file_num, XtInputId *id)
#else
void input_queue(client_data, file_num, id)
caddr_t client_data;
int *file_num;
XtInputId *id;
#endif
{
      BUF_TYPE *tmp_buf_ptr;
      int flags,block,noblock,n,i,j;
      char buf[1001];
 
      /* If client data is 0 then we got here via X, else
      ** we got here from the timeout handler that saw some
      ** input.
      */
      if (client_data == 0) {
         flags = fcntl(fileno(stdin),F_GETFL,0);
      
         noblock = flags & ~O_NDELAY;
          fcntl(fileno(stdin),F_SETFL,noblock); 

         n = read(0,buf,1000);

      } else {
	 /* Attach this function so we can see any more data */
         curr_id = XtAppAddInput(context,fileno(stdin),XtInputReadMask,input_queue,NULL); 
	 stdin_valid = TRUE;
	 
	 /* time_char is the 1 character that was read in the timeout handler */
         buf[0] = time_char;
	 n = read(0,&buf[1],999) + 1;
	 flags = (int)client_data; 
      } 

      block = flags | O_NDELAY;
      fcntl(fileno(stdin),F_SETFL,block); 

      if (n <= 0) {
          /* If there is no input then we should remove the
          ** callback, add a wakeup for 1 second from now
	  ** and install the timeout handler, when the timeout
	  ** handler gets invoked it checks for input - if there
	  ** is none it stays installed, if there is a char then
	  ** it CALLS this handler with the client data flag set 
	  ** indicating that this function should install itself.
	  ** The timeout handler is uninstalled automatically by the
	  ** X-Intrinsics.
	  */
          if (stdin_valid) {
             /* Remove this handler */
	     XtRemoveInput(*id);
             /* Add a wakeup for 1 second from now */
	     XtAppAddTimeOut(context,(unsigned long)1000, handle_timeout, 0);
	     /* Set a flag indicating that that this function is NOT attached */
	     stdin_valid = FALSE;
	   } 
          return;
      }
      

      buf[n] = '\0';
      
     /* At this point there is a buffer with some data, although it
     ** may not be a full line - basically this loops looks for
     ** eol's and then calls input_handle() to deal with a
     ** complete line of data. If the data we read is not a complete
     ** line then keep what was read and then and get the remainder
     ** the next time the function is invoked.
     */
     i = 0;
     while (i < n) {
        /* Look for the end of line or a NULL if the
	** whole line has not been read 
	*/
        j = partial_buf;
        while ((buf[i] != '\n') && (buf[i] != '\0')) 
          local_buf[j++] = buf[i++];
        
        /* See if we got a whole line */ 
        if (buf[i] == '\0') {
           partial_buf = j;
           return;
	} else
           partial_buf = 0;
        
        i++;
        local_buf[j] = '\n';
        local_buf[j+1] = '\0';
        input_handle(local_buf);
     }

}

/*
** read_rotate()
**
** This function is invoked when a complete rotation matrix has been
** read from the pipe connected to the rotate program. It checks that
** the format is correct and then calls apply rotate to rotate all
** the objects in the picture.
*/
#ifndef _NO_PROTO
read_rotate(char line[])
#else
read_rotate(line)
char line[];
#endif
{
    float rot[3][3];
    int n,i;

    n = sscanf(line,"%f %f %f %f %f %f %f %f %f",&rot[0][0], &rot[0][1], &rot[0][2],
               &rot[1][0], &rot[1][1], &rot[1][2],
               &rot[2][0], &rot[2][1], &rot[2][2]);

    if (n != 9) {
       fprintf(stderr,"\nError in rotate format\n");
    } else {
       if (total_objects > 0)
          apply_rotate(&rot[0][0],object_count,object_head.next,!bounds_set);
       else
	  return(0);
     }
}
/*
** parse_argument()
**
** This function handles command line arguments and sets up 
** global variables.
** 
** -l number : Number of different colors to use in shing a line the default is 1.
** -c number : Number of different colors to use in shading a circle, default is 10
** -xmin number : Minimum value for X 
** -ymin number : Minimum value for Y
** -zmin number : Minimum value for Z
** -xmax number : Maximum value for X
** -ymax number : Maximum value for Y
** -zmax number : Maximum value for Z
** -2D : There is no Z cord in the input
** -abs : Use the value given for the radius as the number of pixels - No scaling.
** 
**
*/
#ifndef _NO_PROTO
parse_arguments(int argc, char *argv[])
#else
parse_arguments(argc,argv)
int argc;
char *argv[];
#endif

{
     int i,j,n,index;
     char first_char;
     char *buf;

     for (i=1;i<argc;) {
         first_char = *((argv[i])+1);
         if (!(strcmp(argv[i],"-lshade"))) {
            n = sscanf(argv[++i],"%d",&lshade);
            if (n != 1) {
               fprintf(stderr,"\n-lshade argument should specify an integer\n");
               exit(0);
            }
            
         } else
            if (!(strcmp(argv[i],"-cshade"))) {
               n = sscanf(argv[++i],"%d",&cshade);
	       old_cshade = cshade;
               if (n != 1) {
                  fprintf(stderr,"\n-cshade argument should specify an integer\n");
                  exit(0);
               }
            
             } else
               if (!(strcmp(argv[i],"-xmin"))) {
                  n = sscanf(argv[++i],"%f",&xmin);
		  no_size = FALSE;
                } else
		  if (!(strcmp(argv[i],"-update"))) {
                     update_rotate = TRUE;
		  } else 
		  if (!(strcmp(argv[i],"-pic"))) {
                     interactive = FALSE;
		  } else
                  if (!(strcmp(argv[i],"-xmax"))) {
                     n = sscanf(argv[++i],"%f",&xmax);
		     no_size = FALSE;
                   } else
                     if (!(strcmp(argv[i],"-ymin"))) {
                        n = sscanf(argv[++i],"%f",&ymin);
			no_size = FALSE;
                      } else
                        if (!(strcmp(argv[i],"-ymax"))) {
                           n = sscanf(argv[++i],"%f",&ymax);
			   no_size = FALSE;
                        } else
                     if (!(strcmp(argv[i],"-zmin"))) {
                        n = sscanf(argv[++i],"%f",&zmin);
			no_size = FALSE;
                      } else
                        if (!(strcmp(argv[i],"-zmax"))) {
                           n = sscanf(argv[++i],"%f",&zmax);
                           no_size = FALSE;
			 
			 } else 
                        if (!(strcmp(argv[i],"-proj"))) {
                           n = sscanf(argv[++i],"%f",&xp);
                           n = sscanf(argv[++i],"%f",&yp);
                           n = sscanf(argv[++i],"%f",&zp);
                           if (n >= 1)
			      proj_type = PARALLEL;
                           else
			      proj_type = PERSPECTIVE;

                           do_proj = 1;
                         } else
                         if (!(strcmp(argv[i],"-2D"))) {
			    input_2D = TRUE;
			  } else
			 if (!(strcmp(argv[i],"-radius"))) {
                            sscanf(argv[++i],"%f",&default_radius);
			    for (j=0;j<100;j++)
                               if (radius_map[j] == 0.0)
			          radius_map[j] = default_radius;
			 } else
	                 if (!(strcmp(argv[i],"-type"))) {
			    xyt_format = TRUE;
			 } else
	                 if (!(strcmp(argv[i],"-scale"))) {
			    sscanf(argv[++i],"%f",&default_scale);
			 } else
	                 if (first_char == 'r') {
			    buf = argv[i]+2;
       			    sscanf(buf,"%d",&index);
			    if (index < 100) {
			       sscanf(argv[++i],"%f",&radius_map[index]);
			    } else
			       fprintf(stderr,"PlotMolecule: radius index too large\n");
                         } else
			 if (first_char == 'c') {
			    buf = argv[i]+2;
			    sscanf(buf,"%d",&index);
			    if (index < 100) {
			       sscanf(argv[++i],"%s",&(colors[index]));
			    } else
			       fprintf(stderr,"PlotMolecule: color index too large\n");
			 }
             else {
              if (n != 1) {
                 fprintf(stderr,"\nUnknown option\n");
                 exit(0);
              }
            }
        i+=1;
     } /* end for */

}

/* 
** input_handle()
**
** This function is called from input_queue() when a full line of
** input from stdin is available.
**
*/
#ifndef _NO_PROTO
  void input_handle(char line[])
#else
  void input_handle(line)
char line[];
#endif
{


  int pos,i,color_index;
  float float_pos,radius,type;
  Arg args[10];
  int n=0;
  char *colorname;
  char default_color[25];
  float vars[10];
  char name[25];

  /* Skip over any leading white space */
  i = 0;
  while (((line[i] == ' ') || (line[i] =='\t')) && ((line[i] != '\n') && (line[i] != '\0'))) i++;

  /* Check for a blank line, this means draw the picture */
  if ((line[i] == '\n') || (line[i] == '\0')) {
     /* Check if we have the same number of objects to draw */
     if ((object_count != old_object_count) && (depth_list != NULL)) {
         free(depth_list);
         depth_list = NULL;
         
     }
     
      
     connect_lines();

     /* Allocate storage that will contain a list of objects that will
     ** be drawn from back to from.
     */
     if (depth_list == NULL) 
        depth_list = (DEPTH *)malloc(object_count * sizeof(DEPTH));

     /* Clear the drawing area pixmap */
     clear_canvas();

     /* Check if the pictire should define the viewport */
     if (no_size) {
        xmin = pic_xmin;
	xmax = pic_xmax;
	ymin = pic_ymin;
	ymax = pic_ymax;
	zmin = pic_zmin;
	zmax = pic_zmax;

     }

     if (first_time) {
        set_world();
        /* set_limits(); */
     }

     /* This does the real work of drawing a frames worth of data */
     draw_objects(TRUE,TRUE,TRUE);
     
     if (first_time) {
        set_world();
        set_limits();
     }
     
     old_object_count = object_count;
     
     /* Set some flags indicating that a picture has been drawn */
     last_crlf = TRUE;
     first_time = FALSE;
     no_size = FALSE;
     
  } else {
     /* Process one line of input */     
     /* Check for a command */
     if (line[i] == '@') 
        process_command(&line[i+1]);
     else {
        /* If this flag is set it means that this line is the first line
	** of a new frame of data - so inactivate all the objects in the
	** old frame.
	*/
        if (last_crlf) {
           inactivate_objects();
           object_count = 0;
	   last_crlf = FALSE;
        }

        /* Check if the input is in 2D format */
        if (input_2D) {
           /* The possible forms of input include:
           **    X Y
           **    X Y r=radius
           **    X Y c=color
	   **    X Y r=radius c=color
	   **    X1 Y1 X2 Y2 
           **    X1 Y1 X2 Y2 r=radius
	   **    X1 Y1 X2 Y2 c=color 
	   **    X1 Y1 X2 Y2 r=radius c=color
	   ** This means that we can have 2,3,4 or 5 parameters on a line.
	   ** 
           */
        
           n  = sscanf(line,"%f %f %f %f %f ",&vars[0],&vars[1],&vars[2],
                  &vars[3],&vars[4]);

           if (xyt_format) 
              if (n == 3)
		 type = vars[2];
	      else
		 type = vars[4];
	   

	   name[0] = '\0';
           get_rad_color(&line[i],&radius,&color_index,&name[0],(int)type);
           if (n < 4) {
              /* This is a sphere */
              add1_sphere(vars[0],vars[1],0.0,radius,colors[color_index],name,color_index);         
           } else {
              /* This is a line */
              add1_line(vars[0],vars[1],0.0,vars[2],vars[3],0.0,
                          radius,colors[color_index],name);

           }
        } else {
           /* Input is in 3D format (the default) */
           n  = sscanf(line,"%f %f %f %f %f %f %f",&vars[0],&vars[1],&vars[2],
                  &vars[3],&vars[4],&vars[5],&vars[6]);

           if (xyt_format)
	      if (n == 4)
		 type = vars[3];
	      else
		 type = vars[6];

	   name[0] = '\0';
           
	   /* Get the radius can color info from the line */
           get_rad_color(&line[i],&radius,&color_index,&name[0],(int)type);
           if (n < 6) {
              /* This is a sphere */
              add1_sphere(vars[0],vars[1],vars[2],radius,colors[color_index],name,color_index);              
           } else {
              /* This is a line */
              add1_line(vars[0],vars[1],vars[2],vars[3],vars[4],vars[5],
                          radius,colors[color_index],name);
           }
	}
      } /* endif if for process-command */
   } /* Endif for raw-data present */     
}

/*
** get_rad_color()
**
** This function picks up the color (c), radius (r) and text (t) info
** for each line of input.
**   buf - the line that was read 
**   radius - set to the radius (r)
**   index - index in default color array to use (c)
**   name - text assoicated with object (t)
**   xyt_type - type of object was defined via command line
**       parameters, this function then maps those parameters
**       to this object.
*/
#ifndef _NO_PROTO
get_rad_color(char *buf, float *radius, int *index, char *name, int xyt_type)
#else
get_rad_color(buf,radius,index,name,xyt_type)
char *buf;
float *radius;
int *index;
char *name;
int xyt_type;
#endif
{
       char *tmp_buf;
       int n,temp;
       char type;

       /* Look for the r= and c= and t= tokens in buf */
       tmp_buf = buf;

       /* Check if we should map types from command line to what
       ** this line indicates 
       */
       if (xyt_format == TRUE) {
	  if (xyt_type < 100) {
	     *radius = radius_map[xyt_type];
	     *index = xyt_type;
	   } else {
	     fprintf(stderr,"\nPlotMolecule: error in type\n");
	     *radius = default_radius;
	     *index = 0;
           }

       } else {
          if (default_radius > 0.0)
            *radius = default_radius;
          else
            *radius = -1; 

          *index = -1;
       }

       /* This goes through the line and pulls out the c,r,t parameters.
       */
       while ((*tmp_buf != '\0') && ((*radius < 0) || (*index < 0) || (*name == '\0'))) {
          while ((*tmp_buf == ' ') || (*tmp_buf == '\t')) tmp_buf++;
          if ((*tmp_buf == 'r') || (*tmp_buf == 'c') || (*tmp_buf == 't')) {
             type = *tmp_buf;
             tmp_buf++;

             while ((*tmp_buf == ' ') || (*tmp_buf == '\t')) tmp_buf++;
             if (*tmp_buf == '=') {
                tmp_buf++;
                while ((*tmp_buf == ' ') || (*tmp_buf == '\t')) tmp_buf++;
		if (type == 'c') 
                   n = sscanf(tmp_buf,"%d",index);
                else
                   if (type == 't')
		      n = sscanf(tmp_buf,"%s",name);
                   else
                      n = sscanf(tmp_buf,"%f",radius); 
                if (n != 1) {
                   fprintf(stderr,"\nError reading radius, color or name\n");
                   *tmp_buf = '\0';
		} else {
                   /* Skip to the first blank, tab, NULL */
                   while (((*tmp_buf != ' ') && (*tmp_buf != '\t') && (*tmp_buf != '\0'))) tmp_buf++;
		}
             } else {
                fprintf(stderr,"\nError in radius, color or name  notation\n");
                *tmp_buf = '\0';
	     }
          } else
            tmp_buf++;
       }

       if (*radius <= 0) {
          /* Use the default */
          *radius = 0;
          fix_radius = TRUE;
       }
   
       if (*index < 0)
          /* Use the first color */
          *index = 0;
         
}

/*
** process_command()
**
** This function gets invoked when the first character on an input line
** is a '@' indicating a command.
**
** Possible commands are the following :
**     @COLOR index string
**     @ROTATE 1 1 1 2 2 2 3 3 3
**     @CLEAR
**     @SLEEP
**     @CLEAR-BOUNDS
**     @NAME index string
*/
#ifndef _NO_PROTO
process_command(char *buf)
#else
process_command(buf)
char *buf;
#endif
{
       char cmd[100];
       char *tmp_buf;
       int i,index;
       char str[50];

       /* Find the end of the command */
       tmp_buf = buf;

       i = 0;
       while ((*tmp_buf != ' ') && (*tmp_buf != '\t') && (*tmp_buf != '\0') && (*tmp_buf != '\n'))
            cmd[i++] = *tmp_buf++;       
 
       cmd[i] = '\0';

       if (!(strcmp(cmd,"color"))) {
            sscanf(tmp_buf,"%d %s",&index,str);
            strcpy(colors[index],str);
       } else 
       if (!(strcmp(cmd,"rotate"))) {
            read_rotate(tmp_buf);
       } else 
       if (!(strcmp(cmd,"clear"))) {
           clear_objects();
       } else
      
       if (!(strcmp(cmd,"sleep"))) {
          sscanf(tmp_buf,"%d",&index);
	  sleep(index);
       } else

       if (!(strcmp(cmd,"name"))) {
          add_name(tmp_buf);
       } else

       if (!(strcmp(cmd,"clear-bounds"))) {
	  clear_bounds();
       }	
    
}

/*
** clear_bounds()
**
** Allows this frame of data to determine its own boundaries for the
** picture.
**
*/
int clear_bounds()
{
    no_size=TRUE;
    first_time=TRUE;
}

/*
** add_name()
**
** Adds an association between a color index and a string. This is
** used to indicate what the various colors mean in english, for example
** red = Carbon
**
*/
#ifndef _NO_PROTO
add_name(char *buf)
#else
add_name(buf)
char *buf;
#endif
{
     char str[256];
     int index;

     sscanf(buf,"%d %s",&index,str);

     if ((index >= 0) && (index < MAX_NAMES)) {
        if (name_table[index] != NULL)
	   free(name_table[index]);
        name_table[index] = (char *) malloc(strlen(str)+1);
	strcpy(name_table[index],str);
     } else
        fprintf(stderr,"\nNAME statement not correct\n");
} 

/*
** inactivate_objects()
**
** This function just goes through the list of objects and marks
** each one inactive (not deleting it).
**
*/
inactivate_objects()
{

    OBJECT *obj;
    int i;

    obj = object_head.next;

    for (i=0;i<total_objects;i++) {
       obj->active = FALSE;
       obj->visible = FALSE;
       obj = obj->next;
    }        
}

/*
** connect_lines()
**
** This function goes through the list of objects and finds out which
** line endpoints are connected to the center of spheres. A line 
** connected to the center of a sphere will have lengths that vary
** according to the sphere radius. The links produced by this function
** are used when lines are drawn to calculate the correct endpoints
** based on sphere size.
**
*/
connect_lines()
{
      OBJECT *obj;
      OBJECT *obj1;
      int i;

      obj = object_head.next;
    
      for (i=0;i<total_objects;i++) {
     
          if (obj->active) {
             if (obj->type == LINE) {
                obj1 = object_head.next;
                /* Try to find the speheres attached to the endpoints */
		while (((obj->sphere_1 == NULL) || (obj->sphere_2 == NULL)) && (obj1 != NULL))  {
                   if (obj1->type == SPHERE) {
		      if ((obj1->x1 == obj->x1) && (obj1->y1 == obj->y1) && (obj1->z1 == obj->z1))   
                         obj->sphere_1 = obj1;
                      else
			 if ((obj1->x1 == obj->x2) && (obj1->y1 == obj->y2) && (obj1->z1 == obj->z2))
			   obj->sphere_2 = obj1;
                      obj1 = obj1->next;
		   } else  
                      obj1 = obj1->next;
		}
	      }
	   }
          obj = obj->next;
      }
}                   


