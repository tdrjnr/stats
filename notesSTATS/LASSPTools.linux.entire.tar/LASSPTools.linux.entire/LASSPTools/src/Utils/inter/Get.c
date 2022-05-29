/* Copyright 1988 by Stephen A. Langer.  *
 * Feel free to copy and distribute this *
 * software, but don't sell it.          *
 * Include this notice in all copies.    */

/*
   Interactive input routines.

SYNOPSIS

       int iget(mesg)
       char *mesg;

       float rget(mesg)
       char *mesg;

       double dget(mesg)
       char *mesg;

   These routines print "*Enter 'mesg': " on standard output,
read a value from current input stream, and return the value.
If nothing is entered, the prompt is repeated.

       int igetd(mesg,deflt)
       char *mesg;
       int deflt;

       float rgetd(mesg,deflt)
       char *mesg;
       float deflt;

       double dgetd(mesg,deflt)
       char *mesg;
       double deflt;

   These routines do the same things as iget, etc, but they also
print the value deflt in brackets. If nothing is entered, deflt is
returned.

       int lget(mesg)
       char *mesg;

       int lgetd(mesg,deflt)
       char *mesg;
       int deflt;

   These routines type mesg, and return 0 or 1, depending on
whether the input is n or y. (Capital N or Y are also accepted.)
The argument deflt should be 0 or 1 (not 'n' or 'y').

        #include <stdio.h>

        FILE *opfile(mesg,status)
        char *mesg;
        char *status;

   Type mesg on standard output, read a file name from current input
stream and open it with type status. See the manual page for fopen (3S)
for a description of the available types. opfile returns the pointer to
be used in fprintf and fscanf.
Example:
  FILE *fd;
  fd = opfile("file for input","r");
  fscanf(fd,"%d",&i);

        #include <stdio.h>

        FILE *opargfile(mesg,status,unit)
        char *mesg;
        char *status;
        FILE *unit;

   If there is an argument in the command line, treat it as a file name
and open it with type status, returning the file pointer. If there is no
argument, then if unit is NULL, pass mesg and status to opfile, and return
the result; otherwise return unit.

        #include <stdio.h>

        FILE *findopfile(filename,status,silent)
        char *filename;
        char *status;
        int  silent;

   Attempts to open the file with name filename with the specified status
first in the current directory, then in the user's HOME directory. Returns
a pointer to the file, or NULL if not found.
*/

#include "iodefs.h"
#include <ctype.h>
#include <string.h>

#define LINE 80
#define sget(s, l)  gets(s);      /* Overrides tiler version */

extern int errno;

iget(mesg)
char *mesg;
{
    char s[LINE];
    int i;
    do {
        fprintf(stderr,"*Enter %s: ",mesg);
        fflush(stderr);
        sget(s, LINE);
    } while (s[0] == '\0');
    sscanf(s,"%d%>",&i);
    return(i);
}

igetd(mesg,deflt)
char *mesg;
int deflt;
{
    int i;
    char s[LINE];
    fprintf(stderr,"*Enter %s [%d]: ",mesg,deflt);
    fflush(stderr);
    sget(s, LINE);
    if(sscanf(s,"%d%>",&i)==EOF)
        return(deflt);
    else
        return(i);
}

double dget(mesg)
char *mesg;
        {
    double x;
    char s[LINE];
    do {
        fprintf(stderr,"*Enter %s: ",mesg);
        fflush(stderr);
        sget(s, LINE);
    } while (s[0] == '\0');
    sscanf(s,"%lf%>",&x);
    return(x);
}


double dgetd(mesg,deflt)
char *mesg;
double deflt;
{
    char s[LINE];
    double x;
    fprintf(stderr,"*Enter %s [%lg]: ",mesg,deflt);
    fflush(stderr);
    sget(s, LINE);
    if(sscanf(s,"%lf%>",&x)==EOF)
        return(deflt);
    else
        return(x);
}


float rget(mesg)
char *mesg;
{
    float x;
    char s[LINE];
    do {
        fprintf(stderr,"*Enter %s: ",mesg);
        fflush(stderr);
        sget(s, LINE);
    } while (s[0] == '\0');
    sscanf(s,"%f%>",&x);
    return(x);
}


float rgetd(mesg,deflt)
char *mesg;
float deflt;
{
    float x;
    char s[LINE];
    fprintf(stderr,"*Enter %s [%g]: ",mesg,deflt);
    fflush(stderr);
    sget(s, LINE);
    if(sscanf(s,"%f%>",&x)==EOF)
       return(deflt);
    else
        return(x);
}

/***************************************************************/

#define FSIZE   40

FILE *findopfile(filename,status,silent)
char *filename;
char *status;
int  silent;
{
   char file[100];
   char *getenv();
   FILE *unit;

   /* Try to open filename locally */
   if((unit = fopen(filename, status)) != NULL)
      return unit;

   /* Try to open HOME/filename */
   if((unit = fopen(
         strcat(strcat(strcpy(file, getenv("HOME")),"/"), filename),
         status)) == NULL && !silent)
      fprintf(stderr, "Can't find file %s!\n", filename);

   return unit;
}

FILE *opargfile(mesg,status,unit)
char *mesg;
char *status;
FILE *unit;
{
   char *arg(), *argptr;

   if((argptr = arg()) == NULL) {
      if(unit == NULL)
         return opfile(mesg,status);
   }
   else {
      if((unit = fopen(argptr,status)) == NULL)
         fprintf(stderr, "Can't open file %s!\n", argptr);
   }

   return unit;
}

FILE *opfile(mesg,status)
char *mesg;
char *status;
{
    FILE *fopen(),*fp;
    char filename[FSIZE];

    do {
       fprintf(stderr,"*Enter %s: ",mesg);
       fflush(stderr);
       sget(filename, FSIZE);
       if((fp = fopen(filename,status)) == NULL) {
          perror("opfile");
          errno = 0;
          fprintf(stderr," *** Error opening file %s ***\n",filename);
          fprintf(stderr," *** Try again ***\n");
       }
    } while (fp == NULL);
    return(fp);
}

FILE *opfiled(mesg,status,deffile)
char *mesg;
char *status;
char *deffile;
{
    FILE *fopen(),*fp;
    char filename[FSIZE];
    char *file;

    do {
       fprintf(stderr,"*Enter %s [%s]: ",mesg,deffile);
       fflush(stderr);
       sget(filename, FSIZE);
       if(filename[0]=='\0')
           file= deffile;
       else
           file = filename;
       if((fp = fopen(file,status)) == NULL) {
          perror("opfile");
          errno = 0;
          fprintf(stderr," *** Error opening file %s ***\n",file);
          fprintf(stderr," *** Try again ***\n");
       }
    } while (fp == NULL);
    return(fp);
}


lget(mesg)
char *mesg;
{
    char s[LINE], *sptr;
    for(;;) {
          fprintf(stderr,"*%s: ",mesg);
          fflush(stderr);
        sptr = sget(s, LINE);
        while(isspace(*sptr)) sptr++;
          if(isupper(*sptr)) *sptr = tolower(*sptr);
          if(*sptr == 'y')
                return(1);
          else if(*sptr == 'n')
                return(0);
          else
                fprintf(stderr,"   Say what? Enter y or n.\n");
    }
}

lgetd(mesg,deflt)
char *mesg;
int deflt;
{
    char s[LINE], *sptr;
    for(;;) {
        fprintf(stderr,"*%s [%c]: ",mesg,(deflt ? 'y' : 'n'));
        fflush(stderr);
        sptr = sget(s, LINE);
        while(isspace(*sptr)) sptr++;
        if(*sptr=='\0') return(deflt);
        if(isupper(*sptr)) *sptr = tolower(*sptr);
          if(*sptr == 'y')
                return(1);
          else if(*sptr == 'n')
                return(0);
          else
                fprintf(stderr,"   Say what? Enter y or n.\n");
    }
}
