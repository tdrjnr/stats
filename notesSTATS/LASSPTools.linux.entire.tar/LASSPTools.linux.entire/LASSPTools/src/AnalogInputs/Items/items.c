/* items.c - source for items, an input device */
/* Simplified from "slider" by JPS 12/17/94 */

#include"items.h"

/**************************************
        main();
   This routine sets up the items,
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
  extern ItemStruct *Item;        /*  Pointer to the first slider           */
  extern Widget  yes_or_no;       /*  The yes-or-no input dialog            */
  extern Widget  warning;         /*  The warning dialog box                */
  extern int     num_items;       /*  The number of items                   */
  extern int     update_flag;     /*  Sees if you need to continually update*/
  extern int     accuracy;        /*  The number of decimal places to use   */

  Widget         toplevel;        /*  The overall container widget          */ 
  Widget         text_box_frame;  /*  The frame around text_box_rc          */
  Widget         text_box_rc;	  /*  The widget which holds the text_boxes */
  Widget         *text_box_label; /*  Pointer to the text_boxs' labels      */
  Widget         big_rc;	  /*  The overall management widget         */
  Widget         button_rc;       /*  The widget which holds the buttons    */
  Widget         send_btn;        /*  The "send" button                     */
  Widget         quit_btn;        /*  The "quit" button                     */
  Widget         update_btn;      /*  The "Update" button                   */
                                  /* for future expansion */  
  Arg            args[10];        /*  Arguments to the toolkit calls        */

  XmString       string;          /*  Generic string                        */
  char           init_label[40];  /*  Text representing the initial value   */
  char           *label_text;     /*  XtString version of label_text        */
  char           found_an_option; /*  checks to make sure args are valid    */

  int            i=0, j=0, n=0;   /*  Generic integers                      */
  int            initial_flag=FALSE; /*Set to true if  items given initial*/
                                     /* values                              */

/******************   Toolkit Initialization   *************************/

  toplevel=XtInitialize(argv[0],"items", NULL, 0, &argc, argv);

  if (argc==1) {                     /*  Determine the number of items  */
    num_items = 1;
  }
  else {
    if (isdigit(*argv[1]))
      num_items=atoi(argv[1]);
    else
      num_items=1;
  }

/*****************    Memory Allocation   ******************************/

  text_box=      (Widget *) malloc(num_items*sizeof(Widget));
  text_box_label=(Widget *) malloc(num_items*sizeof(Widget));
  Item =       (ItemStruct *) malloc(num_items*sizeof(ItemStruct));
  label_text=    (char *)   malloc(MAX_STRING*sizeof(char));

/*****************   Initialization   *********************************/

  for (i=0; i<num_items; i++){
    Item[i].label=(char *) malloc((size_t)MAX_STRING*sizeof(char));
    *(Item[i].label) = ' ';            /* default to no labels       */
    Item[i].value  = 0.0;              /* default initial value of 0 */
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


      if (!strcmp(argv[j], "-initial")) { /*  Found initial values     */
	found_an_option = TRUE;
	{for (i=0; i<num_items; i++){
	  j++;
	  if (!isdigit(*argv[j])&&(*argv[j]!='-')&&(*argv[j]!='.')) {/*If I don't get a digit*/
	    fprintf(stderr,"slider:initial expecting float, got %s\n",argv[j]);
	    exit(0);
	  }
	   sscanf(argv[j],"%lf",&(Item[i].value));
         }
        initial_flag = TRUE;
	}
      }

      else if (!strcmp(argv[j], "-labels")){   /*  Found labels             */
        found_an_option = TRUE;
	for (i=0; i<num_items; i++){
	  j++;
	  strcpy(Item[i].label, argv[j]);
	}
      }

      if (!found_an_option)                  /*  Unknown option read in     */
	if (j==1){                           /*  Skip error for num_items */
	  if(!isdigit(*argv[1])) {
	    fprintf(stderr,"items: unknown option. Ignoring it.\n");
	  }
	}
	else{
	  fprintf(stderr,"items: unknown option. Ignoring it.\n");
 	}
      j++;
    } /* ends while(j<argc) */
  } /* ends if(argc>1) */




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

  for (i=0; i<num_items; i++){
    n=0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNshowValue, TRUE); n++;
    if (initial_flag==FALSE)
      Item[i].value = 0.;
    XtFree(string);
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
  XtSetArg(args[n], XmNnumColumns, num_items); n++;
  text_box_rc=XtCreateManagedWidget("text_box_rc", xmRowColumnWidgetClass,
				    text_box_frame,args, n);

  for (i=0; i<num_items; i++){
    n=0;
    string=XmStringCreate(Item[i].label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER);n++;

/* these two don't seem to do anything */
    
    XtSetArg(args[n], XmNlabelString, string); n++;
    text_box_label[i]=XtCreateManagedWidget("text_box", xmLabelWidgetClass,
					    text_box_rc, args, n);
    XtFree(string);

    n=0;
    sprintf(init_label,"%f",Item[i].value);
    XtSetArg(args[n], XmNvalue, init_label); n++;
    text_box[i]=XtCreateManagedWidget("text_box", xmTextWidgetClass,
				      text_box_rc, args, n);
  }


  XtAddCallback(send_btn,XmNactivateCallback,send_data,NULL);
  XtAddCallback(quit_btn,XmNactivateCallback,quit,NULL);

  XtRealizeWidget(toplevel);


  XtMainLoop();
  return 1;
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
  extern ItemStruct *Item;
  extern Widget yes_or_no;
  extern ChangeStruct change;
  extern int    num_items;

  Widget        ok_button;
  Arg           args[10];
  int           i, n=0;
  int           minimum, maximum;
  char          *value;
  char          message[100];
  float         new_position;

  ok_button=XmMessageBoxGetChild(yes_or_no,XmDIALOG_OK_BUTTON);
  
  clear_val();

  for (i=0; i<num_items; i++){

    value=XmTextGetString(text_box[i]);
    push_val(value);

    sscanf(value,"%f",&new_position);

    n = 0;
    XtSetArg(args[n],XmNminimum,&minimum); n++;
    XtSetArg(args[n],XmNmaximum,&maximum); n++;

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
       fprintf(stderr,"\nToo many items active.\n");
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


void help()
{
  printf("\n");
  printf("Usage:   items num_items [-labels labels] \n");
  printf("\n");
  printf("Where:   num_items=the number of items to put up\n");
  printf("         labels  = labels for the items\n");
  printf("\n");
  printf("Note:   -labels takes the same number of arguments as\n");
  printf("        there are items.\n");
  printf("\n");
  printf("Example: items 3 -labels one two three\n");
  printf("\n");
  exit(0);
}
