#include <stdio.h>

extern int igetd();
extern double dgetd(), dget();
extern FILE *opfile();

#define DATALINE 20000

/* column of data is a linked list */
typedef struct DATAPT {
    double x;
    struct DATAPT *next;
} DATAPT;

typedef struct {
    int npts;
    DATAPT *firstpt;
} CURVE;
