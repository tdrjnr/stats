/*
 * parseargument.h
 */

#ifndef _NO_PROTO
   int parseargument( int argc, char **argv, int start, char **s,
              char *optionflag );
   int parsenextargument( int argc, char **argv, int start, char **s,
              char *optionflag );
#else
   int parseargument();
   int parsenextargument();
#endif
