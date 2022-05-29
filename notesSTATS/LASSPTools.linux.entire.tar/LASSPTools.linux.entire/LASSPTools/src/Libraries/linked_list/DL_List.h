/***************************************************************************//*

	This is DL_List.H  .

        List handler modified from code by Dean Cirielli.
        Chris Myers assumes all responsibility herein.

	Code started 2-6-90.
	Code finished 7-19-90.


*//***************************************************************************/

#ifndef DL_LIST_H		/*  prevent multiple copies  */
#define DL_LIST_H

#include "Boolean.h"



typedef struct _list LIST;


struct _list
{	double 	*data;		/*  ptr to the data  */
	LIST	*last, *next;	/*  the next and last list entries  */
};

#ifndef _NO_PROTO
  LIST *load_list(LIST *list, void *data);
  LIST *dump_list(LIST *list);
  LIST *top_list(LIST *list);
  LIST *last_list(LIST *list);
  LIST *next_list(LIST *list);
  double *list_data(LIST *list);
#else
  LIST *load_list();
  LIST *dump_list();
  LIST *top_list();
  LIST *last_list();
  LIST *next_list();
  double *list_data();
#endif


#endif				/*  end of #ifndef DL_LIST_H  */
