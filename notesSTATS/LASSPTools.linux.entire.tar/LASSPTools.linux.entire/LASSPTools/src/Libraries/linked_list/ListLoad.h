
#ifndef LISTLOAD_H		/*  prohibit multiple copies  */
#define LISTLOAD_H

#include <stdio.h>

#include <malloc.h>

/*
#include <stdlib.h>
*/

#include "DL_List.h"

#ifndef _NO_PROTO
  LIST *file_load_scalar_list(FILE *fp, LIST *s_list);
  LIST *dump_scalar_list(LIST *sf);
#else
  LIST *file_load_scalar_list();
  LIST *dump_scalar_list();
#endif

#endif				/*  end of #ifndef LISTLOAD_H  */
