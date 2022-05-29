/*------------------------------------------------------------------------------
 * parse_argument.h
 */

#ifndef _NO_PROTO
   int parse_argument( int argc, char **argv, int start, char **s,
              char *option_flag );
   int parse_next_argument( int argc, char **argv, int start, char **s,
              char *option_flag );
#else
   int parse_argument();
   int parse_next_argument();
#endif
