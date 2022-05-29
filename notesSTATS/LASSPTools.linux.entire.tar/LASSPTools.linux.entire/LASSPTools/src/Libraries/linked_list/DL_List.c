/***************************************************************************//*

	This is DL_List.c  .

	This code is a generic doubly-list manager.

	This code was written by Dean Cirielli

	Code started 1-22-90.
	Code finished 9-4-90.


*//***************************************************************************/



#include <stdio.h>

#include <varargs.h>
#include <malloc.h>

/*
#include <stdarg.h>
#include <stdlib.h>
*/

#include "DL_List.h"


/***************************************************************************//*

			The LIST definition.

			Resides in DL_List.H  .


*//***************************************************************************/




/***************************************************************************//*

		The user-friendly LIST field-assignment routines.

			load_list();


*//***************************************************************************/

/*  load_list entails:  going to the end of the list
			creating new endlist (node allocation)
			setting endlist->next (to NULL by alloc_list() )
			setting endlist->last (poss. NULL by set_list_next() )
			setting endlist->data
			return endlist.

   NOTE:  When using load_list to actually create a list -- that is when
		using it for allocation -- it is imperative that the function
		be used in an assignment.
		ie.	new_list = load_list( NULL, &some_data);
		After creation of course, assignment is optional.
*/

#ifndef _NO_PROTO
  LIST *load_list(LIST *list, void *data)
#else
  LIST *load_list(list, data)
    LIST *list;
    void *data;
#endif
{
  LIST *l = list, *set_list_next(), *alloc_list();
  void *set_list_data();
  
  if ( l != NULL )
    {	
      for( ; l->next != NULL; l = l->next);
      l = set_list_next( l, alloc_list() );
    }
  else
    l = alloc_list();
  
  set_list_data( l, data);
  
  return l;
}
/*  load_list  */



/*  Destroying a list also destroys its data objects.

	This is done since it is assumed that the data storage
	mechanism is done purely by list.

	List destruction implies data destruction.

*/
#ifndef _NO_PROTO
  LIST *dump_list(LIST *list)
#else
  LIST *dump_list(list)
    LIST *list;
#endif
{	
  LIST *l = top_list( list );
  
  while ( l != NULL )
    {	
      if ( l->data != NULL )
	{	
	  free( l->data );
	  l->data = NULL;
	}

      if ( l->next != NULL )
	{	
	  l = l->next;
	  free( l->last );
	  l->last = NULL;
	}
      else
	{	
	  free( l );
	  l = NULL;
	}
    }
  
  return l;
}
/*  dump_list  */





/***************************************************************************//*

    The individual LIST field-assignment routines excluding list->property.

			alloc_list()
			set_list_last()
			set_list_next()
			set_list_data()

*//***************************************************************************/



/*  The list allocation routine.  */
#ifndef _NO_PROTO
  LIST *alloc_list()
#else
  LIST *alloc_list()
#endif
{	
  LIST *list = (LIST *) malloc( sizeof( LIST ) );
  
  if ( list != NULL )
    {	
      list->data = (void *) NULL;
      list->last = list->next = (LIST *) NULL;
    }
  
  return list;
}
/*  alloc_list  */




/*  Set the ptr's list->last and last->next; then return a ptr to last.
*/

#ifndef _NO_PROTO
  LIST *set_list_last(LIST *list, LIST *last)
#else
  LIST *set_list_last(list, last)
    LIST *list;
    LIST *last;
#endif
{	
  if ( list != NULL )
    {	
      list->last = last;
      
      if ( last != NULL )
	last->next = list;
      
      return last;
    }
  else
    {	
      fprintf( stderr, "NULL list passed to set_list_last().\n" );
      
      return (LIST *) NULL;
    }
}
/*  set_list_last  */


/*  Set the ptr's list->next and next->last; then return a ptr to next.
*/
#ifndef _NO_PROTO
  LIST *set_list_next(LIST *list, LIST *next)
#else
  LIST *set_list_next(list, next)
    LIST *list;
    LIST *next;
#endif
{	
  if ( list != NULL )
    {	
      list->next = next;
      
      if ( next != NULL )
	next->last = list;
      
      return next;
    }
  else
    {	
      fprintf( stderr, "NULL list passed to set_list_next().\n" );
      
      return (LIST *) NULL;
    }
}
/*  set_list_next  */




/*  Set the ptr list->data; then  return a ptr to data.
*/
#ifndef _NO_PROTO
  void *set_list_data(LIST *list, void *data)
#else
  void *set_list_data(list, data)
    LIST *list;
    void *data;
#endif
{	
  if ( list != NULL )
    {	
      list->data = data;
	
      return data;
    }
  else
    {	
      fprintf( stderr, "NULL list passed to set_list_data().\n" );
      
      return (void *) NULL;
    }
}
/*  set_list_data  */



/***************************************************************************//*

		The LIST positioning routines.

			top_list()
			next_list()
			last_list()

*//***************************************************************************/

#ifndef _NO_PROTO
  LIST *top_list(LIST *list)
#else
  LIST *top_list(list)
    LIST *list;
#endif
{	
  LIST *l = list;

  if ( list != NULL )
    for(l = list; l->last != NULL; l = l->last);
  else
    fprintf( stderr, "NULL list passed to top_list().\n" );
  
  return l;
}
/*  top_list  */

#ifndef _NO_PROTO
  LIST *last_list(LIST *list)
#else
  LIST *last_list(list)
    LIST *list;
#endif
{	
  if ( list != NULL )
    return list->last;
  else
    {	
      fprintf( stderr, "NULL list passed to last_list().\n" );
      return (LIST *) NULL;
    }
}
/*  last_list  */

#ifndef _NO_PROTO
  LIST *next_list(LIST *list)
#else
  LIST *next_list(list)
    LIST *list;
#endif
{	
  if ( list != NULL )
    return list->next;
  else
    {	
      fprintf( stderr, "NULL list passed to next_list().\n" );
      return (LIST *) NULL;
    }
}
/*  next_list  */



/***************************************************************************//*

		The LIST data-field-retrieval routine.

			list_data()

*//***************************************************************************/


/*  Return ptr to the  data.  */

#ifndef _NO_PROTO
  double *list_data(LIST *list)
#else
  double *list_data(list)
    LIST *list;
#endif
{	
  if ( list != NULL )
    return list->data;
  else
    {	
      fprintf( stderr, "NULL list passed to list_data().\n" );
      return (void *) NULL;
    }
}
/*  list_data  */



















