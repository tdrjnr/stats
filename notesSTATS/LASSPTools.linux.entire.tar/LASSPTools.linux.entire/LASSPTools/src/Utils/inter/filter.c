extern char *filter;
#ifndef SUN
void *malloc(), *realloc();
void free();
#else
#include <stdlib.h>
#endif

char *dupstr();    /* strdup() isn't in all libraries! */

#include "menudef.h"
#include <string.h>

set_filter() {
   char *argptr, line[1024];
   if(argptr = arg())
      filter = dupstr(argptr);
   else {
        fprintf(stderr,"*Enter filter: ");
        while(gets(line) == NULL);
        if(!line[0]) {
           clear_filter();
           return 1;  /* No filter */
        }
        filter = dupstr(line);
   }
   return 1;
}

set_tbirdfilter() {
   char *argptr, line[1024], *tbirdargs;
   if(argptr = arg())
      tbirdargs = argptr;
   else {
      fprintf(stderr, "*Enter tbird arguments: ");
      while(gets(line) == NULL);
      if(!line[0]) {
         clear_filter();
         return 1;
      }
      tbirdargs = dupstr(line);
   }
   filter = (char *) malloc((size_t) (strlen(tbirdargs) + 12));
   strcpy(filter, "tbird -p \'");
   strcat(filter, tbirdargs);
   strcat(filter, "\'");
}

clear_filter() {
   if(filter) {
      free((void *) filter);
      filter = (char *) 0;
   }
   return 1;
}

show_filter() {
   if(filter)
      fprintf(stderr, "%s\n", filter);
   else
      fputs("No current filter.\n", stderr);
   return 1;
}

/***************************************************************/

/*
 * FILE *addfilter(filter, file, mode)
 * char *filter;
 * FILE *file;
 * char *mode;
 *
 * Insert the given filter ("filter") into an existing input or
 * output stream ("file").  "mode" is "r" for input and "w" for output.
 * The original stream is closed, and the new one is returned.
 */

#include <stdio.h>
#include <ctype.h>

static char **makeargs();
extern int errno;

FILE *addfilter(fltr, file, modestr)
char *fltr;               /* Filter to be used */
FILE *file;               /* Stream to be filtered */
char *modestr;            /* "r" or "w" */
{
   char **argv;
   int filedes[2];
   char mode = modestr[0];
   int fd = fileno(file);
   FILE *newfile;

   if(file == NULL) return NULL;
   if(mode == 'w') fflush(file);

   /* Create a pipe */
   if(pipe(filedes) == -1) {
      perror("addfilter: can't open pipe!");
      errno = 0;
      return NULL;
   }
   
   switch(fork()) {
      case -1:
         perror("addfilter: can't fork!");
         return NULL;

/* Child */
      case 0:
         if(mode == 'r') {               /* Child is writing to parent */
            if(dup2(filedes[1], 1) == -1) {     /* Connect stdout to pipe */
               perror("addfilter: Child process can't write to pipe!");
               exit(1);
            }
            if(dup2(fd, 0) == -1) {      /* Connect stdin to file */
               perror("addfilter: Child process can't read from file!");
               exit(1);
            }
         }
         else if(mode == 'w') {          /* Child is reading from parent */
            if(dup2(filedes[0], 0) == -1) {     /* Connect stdin to pipe */
               perror("addfilter: Child process can't read from pipe!");
               exit(1);
            }
            if(dup2(fd, 1) == -1) {            /* Connect stdout to file */
               perror("addfilter: Child process can't write to file!");
               exit(1);
            }
         }
         close(filedes[0]);
         close(filedes[1]);
         close(fd);

         argv = makeargs(fltr);
         execvp(argv[0], argv);
         perror("addfilter: Can't execute!");
         _exit(1);

/* Parent */
      default:
         if(mode == 'r') {          /* Parent reads from child */
            newfile = fdopen(filedes[0], "r");
            close(filedes[1]);
         }
         else if(mode == 'w') {     /* Parent writes to child */
            newfile = fdopen(filedes[1], "w");
            close(filedes[0]);
         }
         fclose(file);
         return newfile;
   }
}


/********************************************************************/

/* 
 * Construct argc and argv from a string. The argv list must end
 * with char *0. This is called only within the child process and
 * before execl(), so arrays can be allocated but don't have to be
 * freed.
 */

#define NULLSTRING ""

static char **makeargs(str)
char *str;
{
   char *ptr = str;
   int argc = 0;
   char **argv = 0;
   int quoting = 0;

   while(isspace(*ptr)) ptr++;
   if(*ptr == '\0') return argv;

   for(;;) {            /* Loop over string */
      if(argc == 0)     /* Allocate space for arg */
         argv = (char **) malloc(sizeof(char *));
      else
         argv = (char **) realloc((char *) argv,
                                 (unsigned)((argc+1)*sizeof(char *)));
      argv[argc] = ptr;
      argc++;
      while((!isspace(*ptr) || quoting) && *ptr != '\0') {
          if(quoting==1 && *ptr=='\'') quoting = 0;
          else if(quoting==2 && *ptr=='"') quoting = 0;
          else {
              if(*ptr == '\'') quoting = 1;
              if(*ptr == '"') quoting = 2;
          }
          ptr++;
      }
      if(isspace(*ptr)) {
         *ptr = '\0';    /* Terminate arg */
         ptr++;          /* Beginning of next arg */
      }
      else {             /* At end of string */
         argv = (char **) realloc((char *) argv,
                                 (unsigned)((argc+1)*sizeof(char *)));
         argv[argc] = (char *) 0;
         return argv;
      }
   }
}

char *dupstr(str)   /* strdup */
char *str;
{
   char *new = malloc((unsigned)(strlen(str) + 1));
   char *ptr = new;
   while(*ptr++ = *str++);
   return new;
}
