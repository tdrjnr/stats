#include <stdio.h>
extern int iget(), igetd();
extern int lget(), lgetd();
extern float rget(), rgetd();
extern double dget(), dgetd();
extern char *sget();
extern FILE *opfile(), *opfiled(), *opargfile(), *findopfile();

extern FILE *instream;   /* Commands are read from this file */
extern FILE *outstream;  /* Prompts are sent to this file.
                          * Explicit requests for input
                          * are always made to stderr. */
