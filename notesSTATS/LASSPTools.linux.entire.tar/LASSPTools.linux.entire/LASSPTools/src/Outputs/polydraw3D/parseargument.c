/*****************************************************************************
 *	This file is intended to be a programming tool used to generalize    *
 *	the manner in which our programs accept input from the command       *
 *	line.								     *
 *****************************************************************************/


#include <stdio.h>
#include <string.h>


#ifndef _NO_PROTO
  int parseargument( int argc, char **argv, int start, char **s,
		     char *optionflag)
#else
  int parseargument( argc, argv, start, s, optionflag)
    int argc, start;
    char **argv, **s;
    char *optionflag;
#endif
{
  int i;
  
  /*  Find the index in argv[] which holds the option flag.  */
  for (	i = start; i < argc && strcmp( argv[i], optionflag ) != 0; i++);

  /*  Return a pointer to the next argv[] element.  */
  if ( i < argc )
    {	
      *s = argv[i];
      return i;
    }
  else
    {	
      *s = (char *)NULL;
      return -1;
    }
}

#ifndef _NO_PROTO
  int parsenextargument( int argc, char **argv, int start, char **s,
			  char *optionflag)
#else
  int parsenextargument( argc, argv, start, s, optionflag)
  int argc, start; 
  char **argv, **s;
  char *optionflag;
#endif
{	
  int i;
  
  /*  Find the index in argv[] which holds the option flag.  */
  for (	i = start;i < argc && strcmp( argv[i], optionflag ) != 0; i++ );

  /*  Return a pointer to the next argv[] element.  */
  if ( ++i < argc )
    {	
      *s = argv[ i ];
      return i;
    }
  else
    {	
      *s = (char *)NULL;
      return -1;
    }
}
