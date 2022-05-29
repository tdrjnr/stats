/*
** histogram.c
**
** This is an updated version of the histogram program that will make a new 
** histogram each time a blank line is seen in the input stream.
** 
** Options:
**    -n value: Map the input into n bins, the bin width is by default the
**         difference between the largest and smallest data values 
**         divided by ten - for 10 bins.
**    -s value : make the size of each bin value.
**    -x min max : exclude values below value and above max.
**    -log : generates bins in log fashion.
**    -sep : Recognize a blank line as the start of a new set of data that is
**           to be processed. The default is to ingore blank lines make one
**           histogram for all of the data.
**
** The outline of the program:
**    0.) Process any command line options.
**    1.) Read the data values in from standard input, allocating blocks
**        of memory to hold them. Each block has 1000 values, if there are
**        more values additional blocks are allocated in a linked list. As
**        values are read keep track of the min and max.
**    2.) Keep reading data until a blank line is seen (if -sep) or read
**        all the data (default).
**    3.) Figure out how many bins and their sizes to use. Group data into
**        bins based on the graph type (log or normal). 
**    4.) If the counts are to be normalized then do that and also print
**        out the bins as a bar graph or as single points.
**    5.) If there is more data then build a new histogram and put it out, 
**        keep doing this until the end-of-file is seen.
**    6.) Free the memory that was being used to hold the data values.
**
** Note: this version does not have the secert -o (override) option.
**
** 12-20-92 : Jim Frankenberger
*/
#include <stdio.h>
#include <math.h>
#include "Boolean.h"
#include"histogram.h"
extern void init_frame();
extern BLOCK *get_block();

double binsize = 0.0;           /* Width of each bin */
boolean binsize_set = false;    /* Set if binsize specified on command line */
int nbins = 10;                 /* Number of bins to group data into */
boolean barflag = true;         /* Generate bar graphs */
boolean posflag = false;        /* Output only positive bin counts */
boolean range_size_set = false; /* Set to true if xmin xmax specified */
double xmin,xmax;               /* The min and max of the data being read */
boolean nbins_set = false;      /* Set if number of bins specified on command line */

/*
** main()
**
**
*/

#ifndef _NO_PROTO
main(int argc, char **argv)
#else
main(argc,argv) 
    int argc;
    char **argv; 
#endif
{
  FRAME hist_frame;
  FRAME hist_head;
  double *hist_out;

 
  NORM_TYPE normflag = NONE;    /* If the data should be normalized */
  GRAPH logbinflag = NORMAL;   /* Generate log bin sizes */
  boolean iflag = false;       /* Output suitable for axis() */
  boolean sep_hists = false;   /* Recognize blank lines */
  int i,count;
  
  /* Initialize the structures */
  init_frame(&hist_frame);
  init_frame(&hist_head);
  
  hist_head.frame = &hist_frame;

  /* Parse the command line options */
  for(i=1; i<argc; i++){
    /* specify number of bins */
    if(strcmp(argv[i],"-n")==0){
      if(sscanf(argv[++i],"%d",&nbins) != 1)
	fprintf(stderr,"bad argument %s\n",argv[i]);
      nbins_set = true;
    }

    /* Check if help requested */
    else if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"-help")==0))
         help();
    
    /* specify bin size */
    else if(strcmp(argv[i],"-s")==0){
      if(sscanf(argv[++i],"%lf",&binsize) != 1)
	fprintf(stderr,"bad argument %s\n",argv[i]);
      binsize_set = true;
    }
    /* replace xmin and xmax if desired */
    else if(strcmp(argv[i],"-x")==0){
      if(sscanf(argv[++i],"%lf",&hist_frame.min) != 1)
	fprintf(stderr,"bad argument %s\n",argv[i]);
      if(sscanf(argv[++i],"%lf",&hist_frame.max) != 1)
	fprintf(stderr,"bad argument %s\n",argv[i]);
      range_size_set = true;
    }
    /* normalize bin counts */
    else if(strcmp(argv[i],"-N")==0){
      if(sscanf(argv[++i],"%d",&normflag) != 1)
	fprintf(stderr,"bad argument %s\n",argv[i]);
    }
    else if(strcmp(argv[i],"-log")==0){
      logbinflag = SEMI_LOG;	/* turn on logbinflag */
    }
    else if(strcmp(argv[i],"-g")==0){
      barflag = true;	/* turn on barflag */
    }
    else if(strcmp(argv[i],"-ng")==0){
      barflag = false;	/* turn off barflag */
    }
    else if(strcmp(argv[i],"-pos")==0){
      posflag = true;	/* output only positive bin counts */
    }
    else if(strcmp(argv[i],"-i")==0){
      iflag = true;	/* make output suitable for axis */
    }
    else if (strcmp(argv[i],"-sep")==0) {
      sep_hists = true;   
    }   
    else fprintf(stderr,"bad argument %s\n",argv[i]);
  }
   
 
  /* Read a frame of data and then put out its histogram 
  ** (A frame of data is terminated by a blank line )
  */
  while (count = read_a_frame(&hist_frame,&hist_head,sep_hists)) {
             
      /* Check if it should be output right away */
      if (hist_frame.count) {
         make_hist(&hist_frame,hist_out,logbinflag,normflag);

         /* Put out a blank line at the end of the frame */
         fprintf(stdout,"\n");
         
      }
  }  
   
  /* If we were accumulating each frame then output one histogram */
  if ((!sep_hists) && (hist_frame.count)) {
       make_hist(&hist_head,hist_out,logbinflag,normflag);
  }

  /* Free the memory that we have been consuming */  
  free_frame(&hist_head);
  
  
}

/* 
** init_frame()
** 
** Set the default values for the items in the frame.
** 
** frame_ptr - pointer to data in this particular histogram.
*/
#ifndef _NO_PROTO
void init_frame(FRAME *frame_ptr)
#else
void init_frame(frame_ptr)
FRAME *frame_ptr;
#endif
{
    frame_ptr->min = 0.0;
    frame_ptr->max = 0.0;
    frame_ptr->count = 0;
    frame_ptr->blk_ptr = (BLOCK *)NULL;
    frame_ptr->frame = (FRAME *)NULL;
}

/*
** free_frame()
**
** Free all the memory we have been using before leaving.
**
** hist_head - Pointer to data that was dynamically allocated 
*/
#ifndef _NO_PROTO
free_frame(FRAME *hist_head)
#else
free_frame(hist_head)
FRAME *hist_head;
#endif
{
    BLOCK *temp_ptr, *next_ptr;

    temp_ptr = hist_head->blk_ptr;

    /* Free each block of data in the list */
    while (temp_ptr != NULL) {
       next_ptr = temp_ptr->next;
       free(temp_ptr);
       temp_ptr = next_ptr;
    }
}


/*
** make_hist()
**
** Make a normal or semi-log histogram.
**
** frame_ptr - points to dat that was read
** hist_ptr -
** graph_type - whether to make a normal or semi-log graph
*/
#ifndef _NO_PROTO
make_hist(FRAME *frame_ptr, double *hist_ptr, GRAPH graph_type, NORM_TYPE normflag)
#else
make_hist(frame_ptr, hist_ptr, graph_type, normflag)
FRAME *frame_ptr;
double *hist_ptr;
GRAPH graph_type;
NORM_TYPE normflag;
#endif

{
      BLOCK *block_ptr;
      int i,j,k;
      double binw;
      double *bins;

  
      /* Get a pointer to a real block of data */
      block_ptr = frame_ptr->blk_ptr;
  

      /* Determinae how big each bin should be */
      if (!binsize_set) 
          switch (graph_type) {
	     case NORMAL :
               binsize = (frame_ptr->max - frame_ptr->min)/nbins;
	       break;
	     case SEMI_LOG :
               if ((frame_ptr->max > 0.0) && (frame_ptr->min > 0.0))
                  binsize = (log(frame_ptr->max) - log(frame_ptr->min))/nbins;
               else
                  return(0);
               break;
	  }

      else {
         if (!nbins_set) 
            switch (graph_type) {
               case NORMAL: 
                  nbins = ceil((frame_ptr->max - frame_ptr->min) / binsize); 
                  frame_ptr->max = frame_ptr->min + (nbins * binsize);
                  break;
               case SEMI_LOG:
	          nbins = ceil((log(frame_ptr->max) - log(frame_ptr->min))/binsize);
                  frame_ptr->max = exp(log(frame_ptr->min) + nbins * binsize);

            }
      }  
      if (binsize_set && nbins_set) {
         fprintf(stderr,"Warning: conflicting options -n and -s\n");
         switch (graph_type) {
             case NORMAL:
                 frame_ptr->max = frame_ptr->min + nbins * binsize;
                 break;
	     case SEMI_LOG:
                 frame_ptr->max = exp(log(frame_ptr->min) + nbins * binsize);
                 break;
	 }
      }           

      /* Allocate memory to hold the bin counts */
      bins = (double *)malloc(sizeof(double)*nbins);

      /* Clear all the bins */
      for (i=0;i<nbins;i++)
          bins[i] = 0;
      
      /* Assume that the frame pointer points to the head node
      ** This will look at each data value and then put it into
      ** an approiate bin.
      */
      for (i=0,k=0;i<frame_ptr->count;i++) { 
         
         /* See if we are at the end of a block of data, advance
         ** to the next block where there is more data
         */
         if ((i != 0) && (!(i % BLOCK_SIZE))) {
               block_ptr = block_ptr->next; 
               k = 0;
	 }
         
         /* For different histograms group data differently. */
         switch (graph_type) {
	       case NORMAL:
                  j = floor((block_ptr->elements[k++] - frame_ptr->min)/binsize);
                  break;
	       case SEMI_LOG:
                  if (block_ptr->elements[k] > 0.0)
                     j = floor((log(block_ptr->elements[k++]) - 
                                log(frame_ptr->min))/binsize);
                  else
                     j = 0;
                  break;
	 }
         /* Bump the bin count for the specified bin */
         if ((j >= 0) && (j < nbins)) bins[j]++;
         if (j == nbins) bins[nbins-1]++;
         
       }
       
       if (graph_type == NORMAL)
          draw_hist_normal(&bins[0],nbins,frame_ptr->min,
                           frame_ptr->count*binsize,frame_ptr->count,normflag);
       else
          draw_hist_log(&bins[0],nbins,frame_ptr->min,binsize,frame_ptr->count,normflag);
      
       free(bins); 
}

/*
** draw_hist_normal()
**
** Take a series of bins and draw the rectangles forming the histogram.
**
** bins - pointer to bins to output
** nbins - number of bins
** xmin - starting point to the left 
** area_element - total width of histogram
** count - number of data points inputs (sum of bin counts
** normflag - type of normalization (if any)
*/
#ifndef _NO_PROTO
draw_hist_normal(double *bins,int nbins,double xmin, double area_element, long count, NORM_TYPE normflag)
#else
draw_hist_normal(bins,nbins,xmin,area_element,count,normflag)
double *bins;
int nbins;
double xmin;
double area_element;
long count;
NORM_TYPE normflag;
#endif
{
   double x1,y1,x2,y2,x3,y3;
   int i;

   
   /* Loop through all the bin counts */
   for (i=0;i<nbins;i++) {

       /* Check if any normalization should be done */
       switch (normflag) {
          case BY_COUNT:
             bins[i] = bins[i] / count;
             break;
          case BY_AREA:
             bins[i] = bins[i] / area_element;
             break;
          
       }

       /* If only positive bin counts were specified then check */
       if ((!posflag) || (posflag && (bins[i] > 0))) {

         /* These statements draw a rectangle up, and then over
         ** this requires 3 points to be generated.
         */
         y1 = 0.0;
         x1 = x2 = (binsize * i) + xmin;
         y2 = bins[i];
         x3 = x1 + binsize;
         y3 = bins[i];
         if (barflag)
            fprintf(stdout,"\n%G %G\n%G %G\n%G %G",x1,y1,x2,y2,x3,y3);
         else
            fprintf(stdout,"%e %e\n",x1,bins[i]);
       }
   }

   if (barflag) { 
      /* Put out the last line */
      y1 = 0.0;
      fprintf(stdout,"\n%G %G\n%G %G",x3,y3,x3,y1);
   }

}

/*
** draw_hist_log()
**
** Take a series of bins and draw the rectangles forming the histogram.
** 
** bins - bins containing counts to output.
** nbins - number of bins
** xmin - minimum
** binsize - width of each bin
** count - number of input data points
** normflag - type of normalization to be done.
*/
#ifndef _NO_PROTO
draw_hist_log(double *bins,int nbins,double xmin,double binsize,int count,NORM_TYPE normflag)
#else
draw_hist_log(bins,nbins,xmin,binsize,count,normflag)
double *bins;
int nbins;
double xmin;
double binsize;
int count;
NORM_TYPE normflag;
#endif
{
   double x,oldx,binwidth;
   int i;

   
   for (x=xmin,i=0; i<nbins; i++, x *= exp(binsize)) {

       switch (normflag) {
          case BY_COUNT:
             binwidth = x - oldx;
             bins[i] = bins[i] / binwidth;
             break;

       }

       if ((!posflag) || (posflag && (bins[i] > 0))) {
          fprintf(stdout,"%e %e\n",x,bins[i]);
       }
       oldx = x;
   }

}


/*
** read_a_frame()
**
** Reads data from standard input.
**
** head_ptr - Keeps track of all frames
** frame_ptr - Where the data is to be stored
** accumulate - Set to true if values should be accumulated
** sep_hists - true if each frame is a seperate histogram.
**
** returns a 1 indicating a frame was read.
*/
#ifndef _NO_PROTO
int read_a_frame(FRAME *frame_ptr, FRAME *head_ptr, boolean sep_hists)
#else
int read_a_frame(frame_ptr, head_ptr, sep_hists)
FRAME *frame_ptr;
FRAME *head_ptr;
boolean sep_hists;
#endif
{
     double temp;
     boolean found_one, in_frame = true;
     int count,i;
     int frame_count = 0;
     char buf[257];
     BLOCK *block_ptr;
     BLOCK *temp_ptr;
     
     frame_ptr->count = 0;

     if (!range_size_set) {
        frame_ptr->min = HUGE_VAL;
        frame_ptr->max = -HUGE_VAL;
     } 

     /* Mark all the blocks invalid, this is in effect "freeing" the 
     ** memory
     */
     temp_ptr = frame_ptr->blk_ptr;
     while (temp_ptr != NULL) {
        temp_ptr->valid = false;
        temp_ptr = temp_ptr->next;
     }
     count = 0;
     
     /* Set up where to store the first 1000 values read */
     block_ptr = get_block(frame_ptr);
         
     /* Read in all the data associated with this frame */
     while (in_frame) {
        /* Read some data until a carraige return */
        if (fgets(buf,256,stdin)) {
          frame_count = 1;
          i = 0; found_one = false;

          /* See if there is anything on the line */
          while ((buf[i] != NULL) && (sscanf(&buf[i],"%lf",&temp) == 1)) { 
    
              block_ptr->elements[count] = temp;
              frame_ptr->count++; count++; found_one = true;
              
              /* Keep track of the min and max */
              if (temp < frame_ptr->min) 
                 frame_ptr->min = temp;
              if (temp > frame_ptr->max)
                 frame_ptr->max = temp;
	      

              /* If this block is full get another */
              if (count >= BLOCK_SIZE) {
                 block_ptr = get_block(frame_ptr);
                 count = 0;
              }
             
              /* Advance any leading whitespace */
              while ((buf[i] == ' ') || (buf[i] == '\t')) i++;
              /* Advance past the value just read */
              while ((buf[i] != ' ') && (buf[i] != '\t') && (buf[i] != NULL)) i++;
	  }
          if ((!found_one) && (sep_hists)) {
       
             in_frame = false;
	  }
        } else {
          in_frame = false;
          
        }
     }

     head_ptr->count = frame_ptr->count;
     
     return(frame_count);
}

/*
** get_block()
**
** This function allocates a block of memory to be used to hold some
** input data. The blocks are chained together to hold all the input
** values.
**
** frame_ptr - Starting point of the list where the new block is to 
**             chained to.
** 
*/
#ifndef _NO_PROTO
BLOCK * get_block(FRAME *frame_ptr)
#else
BLOCK * get_block(frame_ptr)
FRAME *frame_ptr;
#endif
{
     BLOCK * ret_ptr,*temp_ptr,*temp_last;

     /* Find the end of the chain */
     if (frame_ptr->blk_ptr != NULL) {
        temp_ptr= temp_last = frame_ptr->blk_ptr;

        /* Find the end of the list, to reduce the amount of
        ** time spent allocating and freeing blocks a valid flag
        ** is used. valid=false means the block is already allocated
        ** and can be reused.
        */
        while ((temp_ptr != NULL) && (temp_ptr->valid)){
           temp_last = temp_ptr;
           temp_ptr = temp_ptr->next;
        }

        /* We did not reach the end of the list, reuse a previous block */
        if (temp_ptr != NULL) {
           ret_ptr = temp_ptr;
           temp_ptr->valid = true;
        } else {
           /* We reached the end of the list and there is still more data so
           ** a new block must be allocated.
           */
           ret_ptr = temp_last->next = malloc(sizeof(BLOCK));
           ret_ptr = temp_last->next;
           ret_ptr->valid = true;
           ret_ptr->next = (BLOCK *) NULL;
	}
     } else {
        /* This is the first block of data */
        ret_ptr = frame_ptr->blk_ptr = malloc(sizeof(BLOCK));
        ret_ptr->next = (BLOCK *) NULL;
        ret_ptr->valid = true;
     }

     /* Return a pointer to a block where the incoming data is
     ** to be stored.
     */
     return(ret_ptr);   
            
}

/*
** help()
**
** This prints out help information if the user asked for it.
*/
#ifndef _NO_PROTO
void help(void)
#else
void help()
#endif
{
  fprintf(stderr, "\nhistogram:\n");
  fprintf(stderr, "\t input one column of numbers from stdin\n");
  fprintf(stderr, "\t output two columns, the first containing the starting point of a bin\n");
  fprintf(stderr, "\t    and the second containing the number of counts in that bin.\n");
  fprintf(stderr, "\n\t sample usage: histogram<input_file | axis | xplot \n");
  fprintf(stderr, "\t NOTE: awk '{print $n}' < input_file | histogram | axis | xplot \n");
  fprintf(stderr, "\t    will create histogram of nth column of input_file, for n integer\n");
  
  fprintf(stderr, "\nAvailable options are:\n");
  fprintf(stderr, "-n nbins \t sets number of bins to nbins (default = 10)\n");
  fprintf(stderr, "-s binsize \t sets binsize (default = (xmax-xmin)/10)\n");
  fprintf(stderr, "-x xmin xmax \t return data with xmin < x < xmax (default = range of data)\n");
  fprintf(stderr, "-N norm \t normalizes counts (norm = 0: scale counts by no. of data, norm = 1: scale counts by to data*binsize, norm=2: leave counts alone(default)\n");
  fprintf(stderr, "\t\t norm = 1: scale so that integrated area = 1)\n");
  fprintf(stderr, "-log \t\t sorts data in logarithmic bins (-N 0 normalizes by bin width)\n");
  fprintf(stderr, "-pos \t\t outputs only bins with positive bin counts,\n");
  fprintf(stderr, "\t\t\t useful if taking logs of the distribution\n");
  fprintf(stderr, "-g \t\t turns on bar graphics (this is the default)\n");
  fprintf(stderr, "-ng \t\t turns off bar graphics, printing {bin,count} pairs\n");
  fprintf(stderr, "-sep\t : Starts a new histogram when a blank line is seen\n");
  fprintf(stderr, "-i \t\t prints extra info to screen: ndata, nbins, xmin and xmax\n");
  fprintf(stderr, "\n");
  exit(-1);
}
