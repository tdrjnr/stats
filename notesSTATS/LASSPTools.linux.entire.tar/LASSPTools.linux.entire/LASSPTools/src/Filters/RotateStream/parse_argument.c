/**************************************************************************//*

	This file is intended to be a programming tool used to generalize
	the manner in which our programs accept input from the command
	line.

*//**************************************************************************/


#include <stdio.h>
#include <string.h>


#ifndef _NO_PROTO
  int parse_argument( int argc, char **argv, int start, char **s,
		     char *option_flag)
#else
  int parse_argument( argc, argv, start, s, option_flag)
    int argc, start;
    char **argv, **s;
    char *option_flag;
#endif
{
  int i,j;
  
  /*  Find the index in argv[] which holds the option_flag.  */
  for (	i = start; i < argc && (j = strcmp( argv[i], option_flag )) != 0; i++);

  /*  Return a pointer to the next argv[] element.  */
  if ( j == 0 )
    {	
      *s = argv[++i];
      return i;
    }
  else
    {	
      *s = (char *)NULL;
      return -1;
    }
}

#ifndef _NO_PROTO
  int parse_next_argument( int argc, char **argv, int start, char **s,
			  char *option_flag)
#else
  int parse_next_argument( argc, argv, start, s, option_flag)
  int argc, start; 
  char **argv, **s;
  char *option_flag;
#endif
{	
  int i;
  
  /*  Find the index in argv[] which holds the option_flag.  */
  for (	i = start;i < argc && strcmp( argv[i], option_flag ) != 0; i++ );

  /*  Return a pointer to the next argv[] element.  */
  if ( ++i < argc )
    {	
      *s = argv[ i ];
      return i;
    } else {	
      *s = (char *)NULL;
      return -1;
    }
}
