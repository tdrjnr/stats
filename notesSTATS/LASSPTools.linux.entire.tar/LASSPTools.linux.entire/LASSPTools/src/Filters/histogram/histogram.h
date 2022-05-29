
#include <stdio.h>
#include <math.h>

/* Number of data points in each block */
#define BLOCK_SIZE 1000

#ifndef _NO_PROTO
  void help(void);
#else
  void help();
#endif

/* Type of graph to generate */
typedef enum gtype {NORMAL,SEMI_LOG} GRAPH;

/* How to do normalization */
typedef enum norm_type {BY_COUNT,BY_AREA,NONE} NORM_TYPE;

/* Defines a node in a linked list, this defines how data values
** are stored 
*/
typedef struct block_type {
   double elements[BLOCK_SIZE];  /* Holds entries               */
   struct block_type *next;      /* The next frame in the series */
   boolean valid;                /* Whether a block is free     */
} BLOCK;

/* This defines how a group of data points that make up a frame
** is stored 
*/
typedef struct frame_type {
   double min;     /* Minimum value in this frame          */
   double max;     /* Maximum value in this frame          */
   long count;     /* Number of data items in frame        */
   BLOCK *blk_ptr; /* Pointer to a bunch of values         */
   struct frame_type *frame; /* Pointer to another frame        */
} FRAME;



