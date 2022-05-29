#include "menudef.h"
#include <stdio.h>
#include <string.h>
char *getenv();
extern FILE *instream, *outstream;

FILE *findrc() {
/* Find out if there is a file .interrc in the current directory
 * or the users home directory. If there is, open the file, and
 * return the file descriptor. If there isn't, return NULL.
 */
   char home[100];
   FILE *fd;

   if ((fd = fopen(".interrc","r")) != NULL)
      return fd;

   fd = fopen(strcat(strncpy(home, getenv("HOME"), sizeof(home)),
                                             "/.interrc"), "r");
   return fd;
}

interrunfile(fd)              /* run the commands in the file fd.  */
FILE *fd;                     /* slightly different from runfile in menu.c */
{
   extern MENU rootmenu;
   FILE *lastin, *lastout;

   /* Save current I/O streams */
   lastin = instream;
   lastout = outstream;

   /* Adjust I/O streams */
   instream = fd;
   outstream = fopen("/dev/null","w");

   /* Process commands */
   while(commands(&rootmenu) != -1);
   eoffound = 0;

   /* Readjust I/O streams */
   fclose(outstream);
   instream = lastin;
   outstream = lastout;
}

runrcfile() {           /* run the .interrc file, if there is one */
   FILE *rcfile;
   if((rcfile = findrc()) != NULL) {
      fprintf(stderr," Reading .interrc ...\n");
      interrunfile(rcfile);
   }
}

runcomfile(comfile) /* run the command file */
char *comfile;
{
   FILE *cfile;
   if((cfile = fopen(comfile, "r")) != NULL) {
      fprintf(stderr," Reading command file %s ...\n", comfile);
      interrunfile(cfile);
   }
   else
      fprintf(stderr," Can't find command file %s ...\n", comfile);
}
