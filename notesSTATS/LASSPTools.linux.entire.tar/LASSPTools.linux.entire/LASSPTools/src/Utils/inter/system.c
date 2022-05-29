#include "menudef.h"
#include <stdio.h>
#include <sys/param.h>
#include <string.h>
#include <ctype.h>

char *getenv(), *getwd();
static char *argptr;
static char dir1[MAXPATHLEN], dir2[MAXPATHLEN];
static char *curdir = dir1;  /* Pointers to directory names      */
static char *olddir = dir2;  /* allow them to be easily switched */

int changedir() {
   /* If this is the first call, find the current directory */
   if(curdir[0] == '\0') getwd(curdir);

   if((argptr=argline()) == NULL)
      chdir(getenv("HOME"));

   else {
      if(chdir(argptr))            /* Change directory system call */
         perror(argptr);           /* Error found by chdir */
      else {
         olddir = curdir;          /* Save old directory name */
         curdir = (olddir == dir1) ? dir2 : dir1;
         strcpy(curdir, argptr);   /* Save new directory name */
      }
   }
   return 1;
}

/******************************************************/

int pwd() {      /* Print working directory */
   char dir[MAXPATHLEN];
   fputs(getwd(dir), stderr);
   putc('\n', stderr);
   return 1;
}

/******************************************************/

int cdback() {   /* Go back to old directory */
   char *temp;

   if(olddir[0] == '\0') {
      fputs("No previous directory!\n", stderr);
      return 1;
   }

   fprintf(stderr, "%s\n", olddir);
   if(chdir(olddir))
      perror(olddir);
   else {
      temp = curdir;
      curdir = olddir;
      olddir = temp;
   }
   return 1;
}

/******************************************************/

int lpq() {
   sysrun("/usr/ucb/lpq");
   return 1;
}

/******************************************************/

int ls() {
   sysrun("ls -sF");
   return 1;
}

/******************************************************/

int c_system() {
   char line[128];
   if((argptr = argline()) != NULL)
      sysrun(argptr);
   else {
      fputs("*Enter shell command: ", stderr);
      gets(line);
      sysrun(line);
   }
   putc('\n', stderr);
   return 1;
}

/*******************************************************/

#include <sys/wait.h>
char *malloc(), *calloc(), *realloc();

sysrun(cmd)         /* Run command cmd in sh, but don't send to stdout. */
char *cmd;
{
   char *argv[4];
   char *shell = "/bin/sh";
   char *option = "-c";

   argv[0] = shell;
   argv[1] = option;
   argv[2] = cmd;
   argv[3] = (char *) 0;

   switch (fork()) {
      case -1: /* fork failed */
         perror("spawn: Can't fork!");
         exit(1);

      case 0: /* child process */
         if(dup2(2, 1) == -1) {  /* Connect stdout to stderr */
            perror("Can't reconnect stdout!");
            exit(1);
         }
         /* Turn child process into the desired command */
         execv(shell, argv);
         /* This line should not be reached */
         perror("spawn: Can't run command!");
         _exit(1);

      default: /* parent process */
         break;
   }
   (void) wait((union wait *) 0);
}
