/*
** up_make.c
**
** This program is used to update a makefile by replacing the
** definitions with those from a specified file.
**
** Examples:
** up_make Makefile sun.defs
** up_make Makefile ibm.defs
**
**
*/
#include <stdio.h>
typedef struct line_str {
  char *data;
  struct line_str *next;
} Line;


main ( argc, argv)
int argc;
char *argv[];
{

      FILE *fp;
      FILE *fp1;
      Line line_head;
      Line defs_head;
      Line *prev_line;
      Line *cur_line;
      char *ch_ptr;
      char buf[256];
 
      if (argc == 3) { 
         /* First read the Makefile */
         if (fp = fopen(argv[1],"r")) {
            prev_line = cur_line = &line_head;
            while (ch_ptr = fgets(buf, 256, fp)) {
	         cur_line->data = (char *) malloc(strlen(buf)+1);
		 strcpy(cur_line->data,buf);
		 cur_line->next = NULL;
		 if (prev_line != cur_line)
		    prev_line->next = cur_line;
		 prev_line = cur_line;
		 cur_line = (Line *) malloc(sizeof(Line));
	    }
	    
            /* Read the defs file */
	    if (fp1 = fopen(argv[2],"r")) {
	       prev_line = cur_line = &defs_head;
	       while (ch_ptr = fgets(buf,256,fp1)) {
		 cur_line->data = (char *) malloc(strlen(buf)+1);
		 strcpy(cur_line->data,buf);
		 cur_line->next = NULL;
		 if (prev_line != cur_line)
		    prev_line->next = cur_line;
		 prev_line = cur_line;
		 cur_line = (Line *)malloc(sizeof(Line));
               }
	       
	       /* Do all the subsitutions */
	       substitute(&line_head,&defs_head);

               /* Just write out Makefile with new defs */
	       fclose(fp);
	       fclose(fp1);
	       if (fp = fopen(argv[1],"w")) {
		  cur_line = &line_head;
		  while (cur_line != NULL) {
		     fprintf(fp,"%s", cur_line->data);
		     cur_line = cur_line->next;
		  }
		  fclose(fp);
	       } else
		  fprintf(stderr,"Could not open %s for writing\n",argv[1]);
	     } else
	        fprintf(stderr,"Could not open %s for reading\n",argv[2]);
	  } else
	     fprintf(stderr,"Could not open %s for reading\n",argv[1]);
       } else
	   fprintf(stderr,"Usage: up_make Makefile defs\n");
    }

substitute(makefile, defs)
Line *makefile;
Line *defs;
{
              int i,j;
	      char buf[256];
	      char keyword[100];
	      char new_word[100];
	      char *ch_ptr;
	      char *line_ptr;
	      Line *cur_def,*save_ptr,*ptr;
	      Line *cur_line;
	      int found;

              cur_def = defs;
              while (cur_def != NULL) {

                    ch_ptr = cur_def->data;
		    /* Skip spaces */
		    i = 0;
		    while ((*ch_ptr == ' ') || (*ch_ptr == '\t')) ch_ptr++;
		    while ((*ch_ptr != '=') && (*ch_ptr != ' ') && (*ch_ptr != '\t') && 
                           (*ch_ptr != '\n')) keyword[i++] = *ch_ptr++;
		    keyword[i] = NULL;
		    
		    if (*ch_ptr != '\n') {
		      /* Search the Makefile for the same definition */
		      cur_line = makefile;
		      found = 0;

		      while ((!found) && (cur_line != NULL)) {
		        line_ptr = cur_line->data;
		        while ((*line_ptr == ' ') || (*line_ptr == '\t')) line_ptr++;
			j = 0;
			while ((*line_ptr != '=') && (*line_ptr != ' ') && (*line_ptr != '\t') &&
			       (*line_ptr != '\n')) new_word[j++] = *line_ptr++;
			new_word[j] = NULL;
		        while ((*line_ptr == ' ') || (*line_ptr == '\t')) line_ptr++;
			if (*line_ptr == '=')
                           if (!strcmp(new_word,keyword)) {
			      /* they are the same, replace the Makefile definition */
			      found = 1;
			      free(cur_line->data);
			      cur_line->data = cur_def->data;
			   }
                        cur_line = cur_line->next;

		    }
		  }
		  cur_def = cur_def->next;
		} /* end while cur_def != NULL */
                           
}
	    
	 



