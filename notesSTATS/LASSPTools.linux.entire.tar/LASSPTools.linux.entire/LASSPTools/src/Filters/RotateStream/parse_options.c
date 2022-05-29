/***************************************************************************
Program RotateStream
Options: 
-rf, -rotate_file
-d,  -defaults
-i,  -initial
Implemented in rotate_aid:
-rf, -rotate_file
-d,  -defaults
-r,  -rotate
-e,  -euler angles the -e should be optional if the angles are the 1st 3 args
-s,  -slider
-po, -popen
-i,  -initial
----------------------
-b,  -binary
-f,  -file_in  the -f should be made optional if the first arg is a filename
-h,  -help
-l,  -light
-p,  -project (3D-2D)
-rc, -remove_comments
-rt, -remove_text
-s,  -sort
-q,  -quit_eof
-w,  -wait_eof
-x,  -axis, -movieaxis
***************************************************************************/

/*---------------------------------------------------------------------------
 *parse_options(int argc, char **argv)
 */

#ifndef _NO_PROTO
void parse_options(int argc, char **argv)
#else
void parse_options(argc, argv)
int argc;
char **argv;
#endif
{
  int i;

  for(i=0; i<argc; i++) 
    if(*argv[i] = '-') {
      
