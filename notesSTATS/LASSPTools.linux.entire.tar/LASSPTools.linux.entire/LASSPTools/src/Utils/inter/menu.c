/* read commands, and call up functions */

/* Copyright 1988 by Stephen A. Langer.  *
 * Feel free to copy and distribute this *
 * software, but don't sell it.          *
 * Include this notice in all copies.    */

#include "menudef.h"
#include "iodefs.h"
#include <ctype.h>
#include <string.h>
char *malloc();
char *realloc();
char *skipspace();

#define LINE 100
static char inputline1[LINE], inputline2[LINE]; /* Input line buffers */
static char *inputline = inputline1;     /* Pointer to current buffer */
static char inputwordbuf[LINE];          /* Input word buffer */
static char *inptr = inputline1;         /* Placeholder on input line */
char *inputword(), *word();
static MENU *thismenu;                   /* Current menu */
static int grouplevel = 0;               /* Parenthesis nesting level */
static int stopsearch = 0;

int moreargs = 0;
int eoffound = 0;
FILE *instream = stdin;           /* Commands are read from this file */
FILE *outstream = stderr;         /* Prompts are sent to this file.
                                   * Explicit requests for input
                                   * are always made to stderr. */
static FILE *savefile = NULL;     /* File for saved commands */
static int save  = 0;             /* Save commands in a file? */
static int echo  = 0;             /* Echo commands to keyboard? */
static int silex = 1;

#define COMMENTCHAR     '%'
#define DELIMITER  ';'  /* Separates commands on input lines */
#define BEGINGROUP '('  /* Starts group of commands to be executed from
                         * a single submenu */
#define QUOTE      '"'
#define ENDGROUP   ')'  /* Ends the group of commands */

/**********************************************************/

int commands(menu)      /* Select item from menu */
MENU *menu;
{
   int cnum;            /* number of command */
   int status;          /* return status of command or sub-menu */
   char *input;         /* pointer to command */
   int startlevel = grouplevel;  /* Initial group nesting level */

   thismenu = menu;

   input = inputword(menu->name); /* get command from input line */
   if(eoffound) return -1;        /* End of file -- special case */
   if(input == NULL) return 1;    /* group began or ended, probably */

   cnum = getcnum(input,menu);    /* get number of command */
   if(cnum < 0) {                 /* command not valid */
      menuhelp(input,menu);       /* print options and error message */
      trashline();                /* throw out input line */
      return 0;                   /* unsuccessful return */
   }
   else if (menu->opt[cnum].function == commands) {
      /* command requires submenu -- use submenu until successful *
       * and repeat until the current group is over. Don't repeat *
       * if at the lowest level, though.                          */
      do  {
         while((status = commands(menu->opt[cnum].submenu)) == 0);
      } while(grouplevel > startlevel && grouplevel > 0);
      return status;
   }
   else {
      status = (int)(menu->opt[cnum].function)();  /* execute command */
      return status;
   }
}

/**********************************************************/

int getcnum(input, menu)  /* Match input word with menu options */
char *input;
MENU *menu;
{
   int k = -1;
   int nmatches = 0;   /* number of matches */
   int lastmatch;
   char *opt;

/* Look for matches, allowing abbreviations */
   while((opt = menu->opt[++k].option) != 0) {
      if(match(input, opt)) {
         lastmatch = k;
         nmatches++;
      }
   }
   if(nmatches == 1) return lastmatch;
   if(nmatches == 0) return -1;   /* No match */

/* More than one match, check for exact matches (no abbreviations) */
   k = -1;
   while((opt = menu->opt[++k].option) != 0)
      if(!strcmp(input, opt))
         return k;

   return -1;   /* No unique match */
}

/**********************************************************/

int match(str1, str2)   /* Do two strings match                    */
char *str1, *str2;      /* up to the length of the first string ?  */
{
   char c1, c2;
   while((c1= *str1++) != '\0')
      if((c2= *str2++) == '\0' || c1 != c2) return 0;
   return 1;
}

/**********************************************************/

menuhelp(input,menu)
char *input;
MENU *menu;
{
   if(*input != '?') {
      fprintf(stderr,"??? Command %s not understood ???\n",input);
      fprintf(stderr,"    Use '?' to list possible commands.\n");
   }
   else {
      fputs("Choices are:\n", stderr);
      printchoices(menu->opt, stderr);
   }
}

/* The following relies on termcap to get the screen width */
printchoices(opt, file)
FILE *file;
OPT opt[];
{
   int i, k, len, columns, width;
   int maxlen = 0;
   char *optname;
   char *getenv(), termcap[1024];
   static int scrwidth = 0;

   if(!scrwidth) {                   /* Find screen size, just once */
      if(tgetent(termcap, getenv("TERM")) == 1)
         scrwidth = tgetnum("co") - 1;
      else
         scrwidth = -1;
   }

   if(scrwidth > 0) {
      k = 0;
      while(optname = opt[k++].option)
         if((len = strlen(optname)) > maxlen)
            maxlen = len;
      columns = scrwidth/(maxlen + 1);

      if(columns > 0) {
         width = -scrwidth/columns;      /* - left justifies */
         k = 0;
         while(opt[k].option) {
            for(i=0; i<columns; i++) {
               if(optname = opt[k].option) {
                  fprintf(file, "%*s", width, optname);
                  k++;
               }
               else
                  break;
            }
            putc('\n', file);
         }
         return;
      }
   }

   /* Don't know screen width, use ugly output */
   k = 0;
   while(optname = opt[k++].option) fprintf(file, "%s ", optname);
   putc('\n', file);
}

/**********************************************************/

/* Input and parsing routines.
 *
 * inputword() gets a word from input line,
 * and reads input lines if necessary.
 * It returns NULL if commands() should take action other than normal.
 * word() returns a pointer to the next word, or NULL if there is
 * no next word.
 * arg() returns a pointer to the next argument.
 * arg() is provided for use in the functions called by commands.
 * An argument list consists of an optional  = followed by
 * arguments separated by optional commas. Arguments of the
 * form "m-n" where m and n are integers are expanded to
 * "m,m+1,m+2,...,n". Argument list are terminated by the
 * DELIMITER character, or by a newline.
 */

/**********************************************************/

char *inputword(prompt)
char *prompt;
{
   char *wptr;
   stopsearch = 0;
   while ((wptr = word(inputwordbuf)) == NULL
                              && !eoffound && !stopsearch)
      getline(prompt);
   return wptr;   /* wptr == NULL if eof or group delimiter found */
}

/**********************************************************/

char *word(buf) /* Copy word into buf from input line */
char *buf;
{
   char *first, c, *firstbuf;
   int success;    /* Was anything found? */

   firstbuf = buf;
   inptr = skipspace(inptr);               /* ignore spaces */

   /* Initial punctuation marks */
   if((c = *inptr) == DELIMITER)           /* skip command delimiter */
      inptr = skipspace(inptr+1);
   else if (c == BEGINGROUP) {             /* Beginning of command group. */
      inptr = skipspace(inptr+1);          /* Skip '(' */
      grouplevel++;
      stopsearch = 1;
      return NULL;
   }
   else if(c == ENDGROUP) {
      inptr = skipspace(inptr+1);
      grouplevel--;
      if(grouplevel < 0) {
         fprintf(stderr, " *** Too many '%c's! ***\n", ENDGROUP);
         grouplevel = 0;
      }
      stopsearch = 1;
      return NULL;
   }
   else if(c == COMMENTCHAR)
      return NULL;

   first = inptr;                          /* start of word */
   while ((c = *inptr) != '\0' &&
          !isspace(c)          &&
          c != '='             &&
          c != DELIMITER       &&
          c != COMMENTCHAR     &&
          c != ENDGROUP && c != BEGINGROUP)
      *buf++ = *inptr++;                   /* copy word */
   *buf = '\0';                            /* terminate word */

   success = (inptr != first);        /* Was anything found? */
   moreargs = 1;                      /* There may be args */

   if(*inptr == ENDGROUP) {        /* Group of commands ended immediately */
      inptr = skipspace(inptr+1);
      grouplevel--;
      moreargs = 0;                   /* There aren't any args, after all */
      stopsearch = 1;
      if(grouplevel < 0) {
         fprintf(stderr, " *** Too many '%c's!\n", ENDGROUP);
         grouplevel = 0;
      }
   }
   if(!success) return NULL;         /* no word found */
   return firstbuf;
}

/**********************************************************/

char *arg() /* get next command argument from string */
{
   char *first, *last, c;
   int quotes = 0;

   if(!moreargs) return NULL;
   inptr = skipspace(inptr);           /* ignore spaces */
   if(*inptr == '=')            /* skip = sign and following spaces */
      inptr = skipspace(inptr+1);
   if(*inptr == ',') {          /* arg was skipped, ',' found first */
      inptr++;
      return NULL;
   }
   if((c = *inptr) == '\0' ||
      c == DELIMITER || c == BEGINGROUP || c == ENDGROUP) {
      moreargs = 0;                    /* End of command */
      return NULL;
   }
   if(c == COMMENTCHAR) {
      trashline();
      return NULL;
   }
   if(*inptr != QUOTE) {
      first = inptr;                      /* start of arg */
      while ((c = *inptr) != '\0' &&
          !isspace(c)          &&
          c != ','             &&
          c != '='             &&
          c != DELIMITER       &&
          c != ENDGROUP        &&
          c != BEGINGROUP)
         inptr++;                         /* pass over arg */
   }
   else {                /* arg is delimited by quotes */
     first = ++inptr;    /* skip initial quote */
     quotes = 1;
     while((c = *inptr) != '\0' && c != QUOTE) inptr++;  /* Skip to end */
     if(c == '\0') fputs(" *** Unbalanced quotation marks! ***\n", stderr);
   }

   last  = inptr;                      /* char just past end of arg */
   inptr = skipspace(inptr);
   if((c = *inptr) == ',' || c == '=' || (quotes && c == QUOTE))
      inptr = skipspace(inptr+1);     /* skip , " or = at end of arg */
   moreargs = ((c = *inptr) != '\0' &&
          c != DELIMITER && c != BEGINGROUP && c != ENDGROUP);

   /* Here we have to deal with the punctuation at the end of the arg,
    * since it may be overwritten when the arg is terminated. */
   if(c == DELIMITER)
      inptr = skipspace(inptr+1);
   else if(c == BEGINGROUP) {
      inptr = skipspace(inptr+1);
      fprintf(stderr,
      " *** Unexpected '%c' found while reading arguments!\n", BEGINGROUP);
   }
   else if(c == ENDGROUP) {
      inptr = skipspace(inptr+1);
      grouplevel--;
      if(grouplevel < 0) {
         fprintf(stderr, " *** Too many '%c's!\n", ENDGROUP);
         grouplevel = 0;
      }
   }

   *last = '\0';                       /* terminate properly */
   return first;
}

/**********************************************************/

char *argline() /* return pointer to rest of input line */
{
   char *first;
   inptr = skipspace(inptr);                  /* ignore spaces */
   if(*(first = inptr) == '\0') return NULL;  /* nothing found */
   while(*inptr != '\0' && *inptr != '\n')
      inptr++;                                /* go to EOL */
   *inptr = '\0';                             /* Remove '\n' */
   moreargs = 0;
   return first;
}

/**********************************************************/

/* This routine reads a line from the current input stream into
 * line[], and returns a pointer to its argument. The terminal newline
 * will be discarded, and the returned string will be null-terminated.
 * Sget() should be used for ALL interactive input to permit command
 * input from files.
 */

#ifndef _NO_PROTO
char *sget(char* inline, int maxlen)
#else
char *sget(inline, maxlen)
char *inline;
int  maxlen;
#endif
{
   int len;

   if(fgets(inline, maxlen, instream) == NULL) {
      if(instream == stdin)
         return NULL;
      else { /* Revert to terminal input and try again */
         fputs("EOF on input!\n", stderr);
         resetinstream(0);
         return sget(inline, maxlen);
      }
   }
   if(echo) fputs(inline, outstream);
   if(save) {
      fputs(inline, savefile);
      fflush(savefile);
   }
   if((len = strlen(inline)-1) >= 0 && inline[len] == '\n')
      inline[len] = '\0';
   return inline;
}

getline(prompt) /* prompt user and read input line */
char *prompt;
{

/* read input line */
   switchinputlines();
   fprintf(outstream,"%s >> ", prompt);
   if(fgets(inputline, LINE, instream) == NULL) { /* EOF detected */
      eoffound = 1;
      return;
   }
   if(echo) fputs(inputline, outstream);
   if(save) {
      fputs(inputline, savefile);
      fflush(savefile);
   }
   parse(inputline);
}

/**********************************************************/

/* PARSE copy input and expand abbreviations */

parse(line)
char *line;
{
   static char *parseline = NULL;
   char *pptr, c;
   int lp;     /* lp is amount of space allocated for parsed line */
   int n, m, i, inc;
   char *last, *next;
   char lastchar = ' ';
   int nchars;

   inptr = line;
   lp = strlen(inptr) + 1;   /* + 1 avoids problems with null input */
   if(parseline != NULL) free(parseline);
   parseline = malloc((unsigned) lp);
   pptr = parseline;

   inptr = skipspace(inptr);
   while ((c= *inptr) != '\0') {
      if(isdigit(c) && expanddigits(&inptr, &m, &n)) { /* is input "m-n"? */
         inc = (m < n ? 1 : -1);
         for(i=m; (n-i)*inc>=0; i+=inc) {
            if((nchars=pptr-parseline) >= lp-10) {
               lp *= 2;
               parseline = realloc(parseline, (unsigned) lp);
               pptr = parseline + nchars;
            }
            if(i!=m) *pptr++ = ',';
            sprintf(pptr, "%d", i);
            while(isdigit(*pptr)) pptr++;
         }
      }
      else {
         *pptr++ = *inptr++;            /* copy char into parseline */
         if((nchars=pptr-parseline) >= lp-1) {   /* need more space */
            lp *= 2;
            parseline = realloc(parseline, (unsigned) lp);
            pptr = parseline + nchars;
         }
      }
   }                           /* next char */

   *pptr = '\0';               /* terminate correctly */
   inptr = parseline;
}

expanddigits(pinptr, pm, pn)  /* parse "m-n" */
char **pinptr;
int *pm, *pn;
{
   char *ptr;
   ptr = *pinptr;                 /* points to a digit */
   while(isdigit(*ptr)) ptr++;    /* skip first number */
   while(isspace(*ptr)) ptr++;    /* and following spaces */
   if(*ptr++ != '-') return 0;      /* look for '-' */
   while(isspace(*ptr)) ptr++;    /* skip more spaces */
   if(!isdigit(*ptr)) return 0;   /* look for digit */
   *pm = atoi(*pinptr);           /* read first number */
   *pn = atoi(ptr);               /* read second number */
   while(isdigit(*ptr)) ptr++;    /* advance ptr over second number */
   *pinptr = ptr;
   return 1;
}

/**********************************************************/

trashline() {   /* Force new line to be read */
/* resetinstream(silex); */
   *inptr = '\0';
}

/**********************************************************/

setcommandline(s)
char *s;
{
   parse(s);
}

/**********************************************************/

rereadline() {  /* Redo the last input line */
   switchinputlines();
   fprintf(outstream, "%s", inputline);
   parse(inputline);
   return 1;
}

/**********************************************************/

switchinputlines() {
   if(inputline == inputline1)
      inputline = inputline2;
   else
      inputline = inputline1;
}

/**********************************************************/

char *skipspace(ptr)
char *ptr;
{
   while(isspace(*ptr)) ptr++;
   return ptr;
}

/**********************************************************/

ifyes() {         /* execute the command on the following line,
                   * if the user wishes. */
   char *question;
   int answer;
   question = argline();
   if(question != NULL) {
       answer = lgetd(question, 0);
       getline("if");   /* read new line */
       if(answer == 0) trashline();     /* throw out new line */
   }
   return 1;
}

/*************************************************************/

int help()      /* Get help for item from menu */
{
   char *input;                        /* pointer to command */
   input = word(inputwordbuf); /* get word from input line */

   if(input == NULL)
      longhelp();
   else                               /* short help on a command */
      while(!helpcommand(thismenu, input))
         input = inputword("help");
   return 1;
}

/**********************************************************/

int helpcommand(menu, input)
MENU *menu;
char *input;
{
   int cnum;
   char promptline[100];
   cnum = getcnum(input, menu);  /* get number of command */

   if(cnum < 0) {                /* command is not valid */
      menuhelp(input, menu);     /* list possible commands */
      trashline();
      return 0;                  /* unsuccessful return */
   }

   input = word(inputwordbuf);   /* get next command on line */

   if(input == NULL || menu->opt[cnum].submenu == NULLMENU) {
      /* end of the list of commands, print help */
      fprintf(stderr, "    %s\n", menu->opt[cnum].hlp);
      trashline();    /* get rid of rest of line */
      return 1;       /* successful return */
   }
   else {      /* keep reading commands until successful */
      while(!helpcommand(menu->opt[cnum].submenu, input))
         /* read new line if unsuccessful */
         input = inputword(strcat(strcpy(promptline,"help "),
                               menu->opt[cnum].submenu->name));
      return 1;
   }
}

/**********************************************************/

runfile(infile, silentexit)
FILE *infile;
int  silentexit;
{
   resetinstream(1);     /* Close previous runfile, if any */
   instream = infile;
   silex = silentexit;
   startecho();
   return 1;
}

resetinstream(opt)
int opt;
{
   if(instream != stdin) {
      fclose(instream);
      instream = stdin;
      stopecho();
      if(opt == 0) {
         fputs(" *** Waiting for terminal input ***\n", stderr);
         fflush(stderr);
      }
   }
}

/**********************************************************/

int c_logstart() {
   if((savefile =
         opargfile("file to log input in", "a", savefile)) == NULL)
      return 0;

   startsave();
   return 1;
}

c_logstop()
{
   stopsave();
   return 1;
}

startsave() {       /* Start saving input commands */
   save = 1;
}

stopsave() {        /* Stop saving input commands */
   save = 0;
}

startecho() {       /* Start echoing input commands */
  echo = 1;
}

stopecho() {        /* Stop echoing input commands */
   echo = 0;
}

/************************************************************/

int noop() {
   return 1;
}
