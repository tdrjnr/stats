#include"parse.h"

#ifndef _NO_PROTO
 XrmDatabase parse(int *argc, char **argv)
#else
 XrmDatabase parse(argc, argv)
 int *argc; 
 char **argv;
#endif
{
  extern XrmOptionDescRec opTable[];
  extern int table_count;
  extern char *op_defaults;
  extern char *file;

  XrmDatabase default_db, file_db, options_db;
  char *tooldir;
  char *path;

  options_db = NULL;

  XrmInitialize();
  default_db = XrmGetStringDatabase(op_defaults);

/* check current directory for database file - else check in the directory */
/* specified by the environment variable TOOLDIR */ 
  if( (file_db=XrmGetFileDatabase(file) ) != NULL) {
    XrmMergeDatabases(file_db,&default_db);   
    /* merge with  hard-coded defaults */ 
  } else {
    if((tooldir = getenv("TOOLDIR")) != NULL) {
      path = (char *) malloc(sizeof(char)*(strlen(file)+strlen(tooldir)+5) );
      strcat(strcat(strcpy(path,tooldir),"/"),file);  
/*you don't want to know*/
      if( (file_db=XrmGetFileDatabase(path)) != NULL) {
	XrmMergeDatabases(file_db,&default_db);   
	/* merge with  hard-coded defaults */ 
      }
      free(path);
    }  
  }

  XrmParseCommand(&options_db,opTable,table_count,"PlotAtoms",argc,argv); 
  XrmMergeDatabases(options_db,&default_db);   /* merge with file defaults */

  return default_db;

}

/* add safety features */
/* parser.c */
/* last modified 5-2-1991 */







