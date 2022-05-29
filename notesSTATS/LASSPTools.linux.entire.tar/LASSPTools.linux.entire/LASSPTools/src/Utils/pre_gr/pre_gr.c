/*
** pre_gr.c
**
** This program takes raw data and inserts xmgr (or other) drawing commands 
** where blank lines  are and also at the beginning and end of the data.
** The program is designed to be used in a pipe between some program 
** producing raw data and xmgr.  
** 
** The base file is read from stdin. The default sequences for the first
** (header) group are put in the file and then each time a blank line
** is encountered the middle sequence is inserted. The ending sequence is
** attached to the end of the file.
**
** 
** file-in : (optional) file that has the xmgr sequences with a blank line
** seperating each of the groups.
**
** For further information see the xmgr/xvgr uses guide.
**
** Note that the program could be used to insert any commands, not just xmgr
** specific commands into a data stream.
**
** 12-23-92 : Jim Frankenberger 
*/
#include <stdio.h>

/*
** This is the default code that gets inserted at the beginning of the 
** file this is for xmgr.
*/
char *strings[200] = 
{"@with g0\n@redraw\n",
"&\n@with g0\n@autoscale\n@redraw\n@sleep 2\n@kill s0\n@with s0\n",
"&\n@redraw\n"};

int total_strings = 3;

/*
** main()
**
*/
#ifndef _NO_PROTO
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
       FILE *fp_in,*fp_out,*fp_db;
       char buf[80];
       char *line;
       int i;
       int frame_count = 0;
       int data_present = 0;
    
       fp_in = stdin;
       fp_out = stdout;

       /* 
       ** If a filename was supplied then read in the sequences
       */

       if (argc > 1) {
          if (fp_db = fopen(argv[1],"r")) {
             /* Load the database */
            total_strings = load_database(fp_db,strings,0);
          } else {
            fprintf(stderr,"\nCould not open: %s\n",argv[1]);
            exit(1);
          }
       }

       /* Put out the beginning sequence */
       print_xmgr(fp_out,strings,0);

       /* Start reading the raw data file */
       line = fgets(buf,80,fp_in);

       while (line) {
          i = 0;

          /* Skip over any blank space */
          while (((buf[i] == ' ') || (buf[i] == '\t') || (buf[i] == '\n')) &&
                  (buf[i] != NULL))
            i++;

          /* See if this was a blank line */
          if ((buf[i] == NULL) && (data_present)) {
/* This puts the plot out promptly.  If the file ends with a blank line,
   this will put out an extra set of print commands! */
             if (frame_count < (total_strings - 2)) frame_count++;
             print_xmgr(fp_out,strings,frame_count);
             /* Skip any extra blank lines */
             line = skip_lines(buf,fp_in);
             data_present = 0;
          } else {
             
             if (buf[i] != NULL) {
                fprintf(fp_out,"%s",buf);
                data_present = 1;
	     }
             line = fgets(buf,80,fp_in);
	   }
       }

       /* Put out the ending */
       print_xmgr(fp_out,strings,total_strings-1);
}

/*
** skip_lines()
**
** Reduces multiple blank lines to a single blank line.
**
*/
#ifndef _NO_PROTO
int skip_lines(char *buf, FILE *fp)
#else
int skip_lines(buf,fp)
char *buf;
FILE *fp;
#endif
{
       char * eof_set;
       char * nothing ;
       int i;

       /* Read blank lines until a non-blank or until EOF is seen */
       do {
          /* Read the next line and proceed to check for data */
          if (nothing = eof_set = fgets(buf,80,fp)) {
             /* Skip any spaces */
             i = 0;

             while (((buf[i] == ' ') || (buf[i] == '\t') || (buf[i] == '\n')) && (buf[i] != NULL))
               i++;

             /* If there is something here then end the skipping */
             if (buf[i] != NULL)
                nothing = NULL;
	  } 
            
      } while (nothing); 
      
      /* Return an indication of whether the buffer contains good data or
      ** an EOF was seen 
      */
      return(eof_set);
}

/*
** load_database()
**
** This function reads in the file containing the sequences of xmgr commands
** that will be put out at the beginning of the file, everytime a blank
** line is seen and at the send of the file.
**
*/
#ifndef _NO_PROTO
load_database(FILE *fp,char *strings[])
#else
load_database(fp,strings)
FILE *fp;
char *strings[];
#endif
{
   char buf[81];
   int i = 0;
   int index = 0;
   char *current_str;
   int total_len = 0;
   int data_present = 0;
   int line;
   
   /* Read each line and count to get a size of the memory to alloc */
   while (line = fgets(buf,80,fp)) {
      /* Skip over any blanks */
      i = 0;
      while (((buf[i] == ' ') || (buf[i] == '\t') || (buf[i] == '\n')) && (buf[i] != NULL))
         i++;

      /* See if this was a blank line */
      if ((buf[i] == NULL) && (data_present))  {
         /* A blank line was seen -
         ** Allocate a block of memory that is the sum of each of the line
	 ** lengths 
         */
         strings[index++] = malloc(total_len);
         *(strings[index-1]) = NULL;
         total_len = 0;
         data_present = 0;
       } else {
         /* See how long the line is and keep a running total */ 
         total_len += strlen(buf);
         data_present = 1;
       }
       
   }
   
   /* Allocate memory for the last group */
   if (data_present)
      strings[index] = malloc(total_len);

   /* At this point all the memory is allocated for each of the sequences. */

   /* Now we just read each group up to a blank line,
   ** each line is concatenated to the rest in the sequence. 
   ** When a blank line is seen this indicates to move to the
   ** next index. 
   */
   
   /* Reset to beginning of the file */
   fseek(fp,0L,0);

   index = data_present= 0;
   current_str = strings[0];
 
   /* Read through the whole file again */
   while (line = fgets(buf,80,fp)) {
      /* Move past the blank space */
      i = 0;
      while (((buf[i] == ' ') || (buf[i] == '\t') || (buf[i] == '\n')) && (buf[i] != NULL))
         i++;

      /* See if this was a blank line, if so advance to the next group  */
      if ((buf[i] == NULL) && (data_present)) {
         current_str = strings[++index];
         data_present = 0;
       } else {
          /* Join the line to those already in the group */
          strcat(current_str,buf);
          data_present = 1;
       }
   }

   /* Return a count of the number of groups found */
   return(index+1);
      
}

/* 
** print_xmgr()
**
** This function prints out a group
*/
#ifndef _NO_PROTO
print_xmgr(FILE *fp,char *data[], int index)
#else
print_xmgr(fp, data, index)
FILE *fp;
char *data[];
int index;
#endif
{
     int i = 0;

     if (index < total_strings) {
        fprintf(fp,data[index]);
        /* Make sure this group gets sent */
        /* Added 1-28-93 jwf */
        fflush(fp);
     }
        
   
}

