#include <stdio.h>
#include "ListLoad.h"

#ifndef _NO_PROTO
  LIST *file_load_scalar_list(FILE *fp,LIST *s_list)
#else
  LIST *file_load_scalar_list(fp,s_list)
    FILE *fp;
    LIST *s_list;
#endif
{       
  int i;
  double *d;
  LIST *l = s_list;
  
  do {
    i = fscanf( fp, "%lG",
	       d = (double *) malloc( sizeof( double ) ) );
    l = load_list(l, d);
  }
  while (i == 1);
  
  return l;
}
/*  file_load_scalar_list  */

#ifndef _NO_PROTO
  LIST *dump_scalar_list(LIST *sf)
#else
  LIST *dump_scalar_list(sf)
  LIST *sf;
#endif
{	
  LIST *t = top_list( sf ), *l = t;
  
  while ( t != NULL )
    {	
      free( (double *)(t->data) );
      t->data = NULL;
      t = t->next;
    }

  return (LIST *) dump_list( l );
}
/*  dump_scalar_field_list  */
