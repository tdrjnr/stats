/* slider.c - source for slider, an analog input device */

#include"slider.h"

/**************************************
        main();
   This routine sets up the sliders,
 draws them, and installs callbacks to
 the updating routine.
***************************************/

char out_str[100][MAX_STRING];
int val_top = 0;

#ifndef _NO_PROTO
  main(unsigned int argc, char *argv[])
#else
  main(argc, argv)
    unsigned int argc;
    char *argv[];
#endif
{

/*********************   Define Variables   *******************************/

  extern Widget  *text_box;	  /*  Pointer to the text_boxes             */
  extern SliderStruct *Slider;    /*  Pointer to the first slider           */
  extern Widget  yes_or_no;       /*  The yes-or-no input dialog            */
  extern Widget  warning;         /*  The warning dialog box                */
  extern int     num_sliders;     /*  The number of sliders                 */
  extern int     update_flag;     /*  Sees if you need to continually update*/
  extern int     accuracy;        /*  The number of decimal places to use   */

  Widget         toplevel;        /*  The overall container widget          */ 
  Widget         slider_box_rc;   /*  The widget which holds the sliders    */ 
  Widget         text_box_frame;  /*  The frame around text_box_rc          */
  Widget         text_box_rc;	  /*  The widget which holds the text_boxes */
  Widget         *text_box_label; /*  Pointer to the text_boxs' labels      */
  Widget         big_rc;	  /*  The overall management widget         */
  Widget         button_rc;       /*  The widget which holds the buttons    */
  Widget         send_btn;        /*  The "send" button                     */
  Widget         quit_btn;        /*  The "quit" button                     */
  Widget         update_btn;      /*  The "Update" button                   */
                                  /* for future expansion */  
  Cursor         hand;            /*  The cursor for the window             */
  Arg            args[10];        /*  Arguments to the toolkit calls        */

  XmString       string;          /*  Generic string                        */
  char           init_label[40];  /*  Text representing the initial value   */
  char           *label_text;     /*  XtString version of label_text        */
  char           found_an_option; /*  checks to make sure args are valid    */

  int            i=0, j=0, n=0;   /*  Generic integers                      */
  int            initial_flag=FALSE; /*Set to true if  sliders given initial*/
                                     /* values                              */

/******************   Toolkit Initialization   *************************/

  toplevel=XtInitialize(argv[0],"sliders", NULL, 0, &argc, argv);

  if (argc==1) {                     /*  Determine the number of sliders  */
    num_sliders = 1;
  }
  else {
    if (isdigit(*argv[1]))
      num_sliders=atoi(argv[1]);
    else
      num_sliders=1;
  }

/*****************    Memory Allocation   ******************************/

  text_box=      (Widget *) malloc(num_sliders*sizeof(Widget));
  text_box_label=(Widget *) malloc(num_sliders*sizeof(Widget));
  Slider =       (SliderStruct *) malloc(num_sliders*sizeof(SliderStruct));
  label_text=    (char *)   malloc(MAX_STRING*sizeof(char));

/*****************   Initialization   *********************************/

  for (i=0; i<num_sliders; i++){
    Slider[i].label=(char *) malloc((size_t)MAX_STRING*sizeof(char));
    *(Slider[i].label) = (char)0;            /* default to no labels       */
    Slider[i].max = 100.0;               /* default max of 100         */
    Slider[i].min = 0.0;                 /* default min of 0           */
    Slider[i].value  = 0.0;              /* default initial value of 0 */
  }


/*****************    Read Values for Initialization   *****************/

  if ((argc>1)&&((!strcmp(*(argv+1),"-h"))
		 || (!strcmp(*(argv+1),"-help")))) {
    help();                     /*  go to help routine         */
  }


  if(argc>1) {             /*  If given more than one argument, parse them  */
    j=1;
    while (j<argc){
      found_an_option = FALSE;

      if (!strcmp(argv[j], "-min")) {	       /*  found minimum values     */
	found_an_option = TRUE;
	for (i=0; i<num_sliders; i++){
	  j++;
	  if (!isdigit(*argv[j])&&(*argv[j]!='-')&&(*argv[j]!='.')) { /*If I don't get a digit */
	    fprintf(stderr,"slider: min expecting float, got %s\n",argv[j]);
	    exit(0);
	  }
	  sscanf(argv[j],"%lf",&(Slider[i].min));
	  if(initial_flag==FALSE) Slider[i].value = Slider[i].min;
	}
      }

      else if (!strcmp(argv[j], "-max")) {     /*  Found maximum values     */
        found_an_option = TRUE;
	for (i=0; i<num_sliders; i++){
	  j++;
	  if (!isdigit(*argv[j])&&(*argv[j]!='-')&&(*argv[j]!='.')){ /*If I don't get a digit */
	    fprintf(stderr,"slider: max expecting float, got %s\n",argv[j]);
	    exit(0);
	  }
	  sscanf(argv[j],"%lf",&(Slider[i].max));
	}
      }

      else if (!strcmp(argv[j], "-initial")) { /*  Found initial values     */
	found_an_option = TRUE;
	{for (i=0; i<num_sliders; i++){
	  j++;
	  if (!isdigit(*argv[j])&&(*argv[j]!='-')&&(*argv[j]!='.')) {/*If I don't get a digit*/
	    fprintf(stderr,"slider:initial expecting float, got %s\n",argv[j]);
	    exit(0);
	  }
	   sscanf(argv[j],"%lf",&(Slider[i].value));
         }
        initial_flag = TRUE;
	}
      }

      else if (!strcmp(argv[j], "-accuracy")){ /*  Found accuracy           */
        found_an_option = TRUE;
	{
	  j++;
	  if (!isdigit(*argv[j])&&(*argv[j]!='-')){/*If I don't get a digit */
	    fprintf(stderr,"slider: accuracy expecting int, got %s\n",
		    argv[j]);
	    exit(0);
	  }
	  sscanf(argv[j],"%i",&accuracy);
	}
      }

      else if (!strcmp(argv[j], "-labels")){   /*  Found labels             */
        found_an_option = TRUE;
	for (i=0; i<num_sliders; i++){
	  j++;
	  strcpy(Slider[i].label, argv[j]);
	}
      }

      else if (!strcmp(argv[j], "-update")){   /*  Found update option      */
	found_an_option = TRUE;
	update_flag = TRUE;
      }

      if (!found_an_option)                  /*  Unknown option read in     */
	if (j==1){                           /*  Skip error for num_sliders */
	  if(!isdigit(*argv[1])) {
	    fprintf(stderr,"sliders: unknown option. Ignoring it.\n");
	  }
	}
	else{
	  fprintf(stderr,"sliders: unknown option. Ignoring it.\n");
 	}
      j++;
    } /* ends while(j<argc) */
  } /* ends if(argc>1) */


/****************   Range Checking   ***************************************/

  for (i=0; i<num_sliders;i++){
    if (Slider[i].max < Slider[i].min){
      fprintf(stderr,"warning: max value %i was less than min value %i.",i,i);
      fprintf(stderr," Max value %i set to %f\n",i,Slider[i].min+1);
      Slider[i].max=Slider[i].min+1;
      }

    if (Slider[i].value < Slider[i].min){
      fprintf(stderr,"warning: initial value %i was less than min value %i.",
	      i,i);
      fprintf(stderr," Initial %i set to %f",i,Slider[i].min);
      Slider[i].value = Slider[i].min;
    }

    if (Slider[i].value > Slider[i].max){
      fprintf(stderr,"warning: initial %i was greater than max %i.",i,i);
      fprintf(stderr," Initial %i set to %f",i,Slider[i].max);
      Slider[i].value = Slider[i].max;
    }
  }


/***********************   Widget Creation    *******************************
 *     What follows is a lot of fairly standard  *
 *  toolkit calls.  Disregard at will.           *
 *************************************************/

  n=0;
  yes_or_no=XmCreateQuestionDialog(toplevel,"yes-or-no",args, n);

  n=0;
  warning = XmCreateWarningDialog(toplevel,"warning",args,n);

  n=0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  big_rc=XtCreateManagedWidget("big_rc", xmRowColumnWidgetClass, toplevel,
			       args, n);

  n=0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);n++;
  slider_box_rc=XtCreateManagedWidget("slider_box_rc",xmRowColumnWidgetClass,
				      big_rc,args,n);

  for (i=0; i<num_sliders; i++){
    n=0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNshowValue, TRUE); n++;
    if (initial_flag==FALSE)
      Slider[i].value = Slider[i].min;
    SliderScaler( &(Slider[i]), args, &n, string);
    Slider[i].slider=XtCreateManagedWidget("slider", xmScaleWidgetClass, 
				    slider_box_rc, args, n);
    XmStringFree(string);
  }

  n=0;
  text_box_frame=XtCreateManagedWidget("text_frame",xmFrameWidgetClass, big_rc,
				       args, n);

  n=0;
  XtSetArg(args[n],XmNorientation,XmVERTICAL);n++;
  XtSetArg(args[n],XmNpacking,XmPACK_COLUMN);n++;
  button_rc=XtCreateManagedWidget("button_rc",xmRowColumnWidgetClass,big_rc,
				  args,n);

  n=0;
  send_btn=XtCreateManagedWidget("Send",xmPushButtonWidgetClass,button_rc,
				 args,n);

  n=0;
  quit_btn=XtCreateManagedWidget("Quit",xmPushButtonWidgetClass,button_rc,
				 args,n);

  n=0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, num_sliders); n++;
  text_box_rc=XtCreateManagedWidget("text_box_rc", xmRowColumnWidgetClass,
				    text_box_frame,args, n);

  for (i=0; i<num_sliders; i++){
    n=0;
    string=XmStringCreate(Slider[i].label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER);n++;

/* these two don't seem to do anything */
    
    XtSetArg(args[n], XmNlabelString, string); n++;
    text_box_label[i]=XtCreateManagedWidget("text_box", xmLabelWidgetClass,
					    text_box_rc, args, n);
    XmStringFree(string);

    n=0;
    sprintf(init_label,"%f",Slider[i].value);
    XtSetArg(args[n], XmNvalue, init_label); n++;
    text_box[i]=XtCreateManagedWidget("text_box", xmTextWidgetClass,
				      text_box_rc, args, n);
  }

  for (i=0; i<num_sliders; i++){
    XtAddCallback(Slider[i].slider, XmNdragCallback, update_text, NULL);
    XtAddCallback(Slider[i].slider,XmNvalueChangedCallback, update_text,NULL);
  }

  XtAddCallback(send_btn,XmNactivateCallback,send_data,NULL);
  XtAddCallback(quit_btn,XmNactivateCallback,quit,NULL);

  XtRealizeWidget(toplevel);

  hand=XCreateFontCursor(XtDisplay(toplevel), XC_hand2);
  for (i=0; i<num_sliders; i++){
    XDefineCursor(XtDisplay(toplevel), XtWindow(Slider[i].slider),hand);
  }
  XDefineCursor(XtDisplay(toplevel), XtWindow(slider_box_rc),hand); 

  XtMainLoop();
  return 1;
}

#ifndef _NO_PROTO
  XtCallbackProc update_text(Widget w, caddr_t call_data, caddr_t client_data)
#else
  XtCallbackProc update_text(w, call_data, client_data)
    Widget w; 
    caddr_t call_data, client_data;
#endif
{
  extern SliderStruct *Slider;
  extern Widget *text_box;
  extern int    num_sliders;
  extern int    update_flag;

  Arg    args[5];
  char   value[30];
  char   format[7];
  int    i=0, n=0, value_int;

  for (i=0;i<num_sliders; i++){
    n=0;
    XmScaleGetValue(Slider[i].slider, &value_int);
    if (Slider[i].num_digits>0)
      sprintf(format,"%%.%if",Slider[i].num_digits);
    else
      sprintf(format,"%%.0f");
    sprintf(value,format, ((double)value_int)/(Slider[i].scale));
    XtSetArg(args[n], XmNvalue, value); n++;
    XtSetValues(text_box[i], args, n);
  }
  if (update_flag==TRUE) send_data(w, call_data, client_data);
}

#ifndef _NO_PROTO
  void send_data(Widget w, caddr_t call_data, caddr_t client_data)
#else
  void send_data(w, call_data, client_data)
    Widget w;
    caddr_t call_data, client_data;
#endif
{
  extern Widget *text_box;
  extern SliderStruct *Slider;
  extern Widget yes_or_no;
  extern ChangeStruct change;
  extern int    num_sliders;

  Widget        ok_button;
  Arg           args[10];
  int           i, n=0;
  int           minimum, maximum;
  char          *value;
  char          message[100];
  float         new_position;

  ok_button=XmMessageBoxGetChild(yes_or_no,XmDIALOG_OK_BUTTON);
  
  clear_val();
 
  for (i=0; i<num_sliders; i++){
    XtRemoveCallback(Slider[i].slider, XmNvalueChangedCallback, 
		     update_text, NULL);

    value=XmTextGetString(text_box[i]);
    push_val(value);

    sscanf(value,"%f",&new_position);

    n = 0;
    XtSetArg(args[n],XmNminimum,&minimum); n++;
    XtSetArg(args[n],XmNmaximum,&maximum); n++;
    XtGetValues(Slider[i].slider,args,n);

    if ((int)((new_position * Slider[i].scale)+.5)>maximum){
      change.s = &(Slider[i]);
      change.new_value=new_position;
      change.which=MAX_CHANGE;
      XtAddCallback(ok_button,XmNactivateCallback,ok_callback,NULL);
      sprintf(message,"Set maximum value of slider %i to %f?",
	      i+1,new_position);
      check_with_user(message);
      XtAddCallback(Slider[i].slider, XmNvalueChangedCallback, 
		    update_text, NULL);
      clear_val();
      return;
    }

    if ((int)((new_position * Slider[i].scale)+.5)<minimum){
      change.s = &(Slider[i]);
      change.new_value=new_position;
      change.which=MIN_CHANGE;
      XtAddCallback(ok_button,XmNactivateCallback,ok_callback,NULL);
      sprintf(message,"Set minimum value of slider %i to %f?",
	      i+1,new_position);
      check_with_user(message);
      XtAddCallback(Slider[i].slider, XmNvalueChangedCallback, 
		    update_text, NULL);
      clear_val();
      return;
    }

    XmScaleSetValue(Slider[i].slider,(int)((new_position*Slider[i].scale)+.5));
    XtAddCallback(Slider[i].slider, XmNvalueChangedCallback, 
		  update_text, NULL);

       
  }
  
  /* Made it all the way through with no resetting */
  print_vals();  

}

void clear_val()
{
    val_top = 0;
}

#ifndef _NO_PROTO
void push_val(char *str)
#else
void push_val(str)
char *str;
#endif
{
    
    if (val_top >= 100 )
       fprintf(stderr,"\nToo many sliders active.\n");
    else 
       strcpy(&out_str[val_top++][0],str);
 
    
}

void print_vals()
{
    int i;
   
    for (i=0;i<val_top;i++) 
       printf("%s\n",out_str[i]);

    fflush(stdout);

}


#ifndef _NO_PROTO
  void check_with_user(char *message)
#else
  void check_with_user(message)
    char *message;
#endif
{
  extern Widget yes_or_no;

  Arg        args[10];
  XmString   label;
  int        n;

  label=XmStringCreate(message, XmSTRING_DEFAULT_CHARSET);

  n=0;
  XtSetArg(args[n], XmNmessageString,label);n++;
  XtSetValues(yes_or_no,args,n);
  XtManageChild(yes_or_no);
}

#ifndef _NO_PROTO
  void ok_callback(Widget w, caddr_t client_data, caddr_t call_data)
#else
  void ok_callback( w, client_data, call_data)
    Widget w;
    caddr_t client_data, call_data;
#endif
{
  extern int          result;
  extern Widget       yes_or_no;
  extern ChangeStruct change;

  char       *string;
  int        n=0;
  Arg        args[10];

  if (change.which==MIN_CHANGE){
    (change.s)->min = change.new_value;
    (change.s)->value = change.new_value;
  }
  if (change.which==MAX_CHANGE){
    (change.s)->max = change.new_value;
    (change.s)->value = change.new_value;
  }

  SliderScaler(change.s,args,&n,string);
  XtSetValues((change.s)->slider,args,n);

  XtUnmanageChild(yes_or_no);

  /* Added 2-1-92 */
  
}

#ifndef _NO_PROTO
  void cancel_callback(Widget w, caddr_t client_data,caddr_t call_data)
#else
  void cancel_callback( w, client_data, call_data)
    Widget w;
    caddr_t client_data,call_data;
#endif
{
  extern Widget yes_or_no;

  XtUnmanageChild(yes_or_no);
}

#ifndef _NO_PROTO
  void quit(Widget w, caddr_t client_data,caddr_t call_data)
#else
  void quit( w, client_data, call_data)
    Widget w; 
    caddr_t client_data, call_data;
#endif
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

#ifndef _NO_PROTO
  void SliderScaler(SliderStruct *Slide, Arg *args, int *n, XmString string)
#else
  void SliderScaler(Slide, args, n, string)
    SliderStruct *Slide;
    Arg *args;
    int *n;
    XmString string;
#endif
{
  extern int accuracy;

  int magnitude = 0;
  char *label_text;

  label_text = (char *) malloc(MAX_STRING*sizeof(char));

  Slide->scale = 1;
  magnitude = (int)log10((double)(Slide->max*(1.0001) - Slide->min));
  Slide->num_digits = accuracy - magnitude;
  Slide->scale = (double)pow(10.0,(double)Slide->num_digits);

  XtSetArg(args[*n], XmNminimum, (int)(Slide->min * Slide->scale)); (*n)++;
  XtSetArg(args[*n], XmNmaximum, (int)(Slide->max * Slide->scale)); (*n)++;
  XtSetArg(args[*n], XmNvalue, (int)(Slide->value * Slide->scale)); (*n)++;
  XtSetArg(args[*n], XmNdecimalPoints, Slide->num_digits); (*n)++;
  
  if (Slide->num_digits < 0)
    sprintf(label_text,"%s (x10^%d)",Slide->label,-(Slide->num_digits));
  else
    sprintf(label_text,"%s         ",Slide->label);
  string = XmStringCreate(label_text, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[*n], XmNtitleString, string); (*n)++;
  
}


void help()
{
  printf("\n");
  printf("Usage:   slider num_sliders [-max maximums] [-min minimums] [-labels labels] \n");
  printf("         [-accuracy accuracy] [-update]\n");
  printf("\n");
  printf("Where:   num_sliders=the number of sliders to put up\n");
  printf("         maximums=the highest value on the slider range (float)\n");
  printf("         minimums=the lowest value on the slider range (float)\n");
  printf("         labels  =labels for the sliders\n");
  printf("         accuracy=number of decimal places of accuracy\n");
  printf("\n");
  printf("Note:   -max, -min, and -labels all take the same number of arguments as\n");
  printf("        there are sliders.\n");
  printf("\n");
  printf("        -update turns on continuous updating of the slider output\n");
  printf("\n");
  printf("\n");
  printf("Example: slider 3 -min 1 4.89 5 -max 1e5 10.01 6 -labels one two three\n");
  printf("\n");
  exit(0);
}
