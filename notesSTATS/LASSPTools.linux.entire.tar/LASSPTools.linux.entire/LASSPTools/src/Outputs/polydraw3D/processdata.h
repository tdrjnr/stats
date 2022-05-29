/*processdata.h*/

#ifndef __processdata.h__
#define __processdata.h__

#define MAXSIDES 60
#define MAXSIDESp1 61
#define MAXPOLY 4000
#define MAXLENGTH 2000

typedef struct polytype {
	double *x, *y, *z;
	int red;
	int green;
	int blue;
	short numsides;
	double depth;
      } polygon;






#endif __processdata.h__
