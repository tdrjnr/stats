/*
** connect -
**    A program to connect points with lines.
**
** Feel free to copy, modify and redistribute this program, but do not
** sell it. Include this notice in any copy.
**
** Author: Jim Frankenberger 1993
**         Laboratory of Atomic and Solid State Physice
**         Cornell University
**
**
*/ 
#include <stdio.h>
#include <math.h>

int object_count;

typedef struct objecttype {
     double x1,y1,z1,radius;
     struct objecttype *next;
   } OBJECT;
OBJECT object_head;
OBJECT *object;
float epsilon;
float radius;
#ifndef _NO_PROTO
extern add_object(float x, float y, float z);
#else
extern add_object();
#endif

/*
** main()
**
** This program given a set of points in 3D space will connect points
** less than a specified distance away from each other with lines.
** The output of the program is intended to be sent to the PlotMolecule
** program.
**
*/
#ifndef _NO_PROTO
main(int argc, char *argv[])
#else
main(argc,argv)
int argc;
char *argv[];
#endif
{

  float vars[10];
  char line[100];
  int n;

  object_count = 0;
  object = &object_head;
  fflush(stdout);
  sscanf(argv[1],"%f",&epsilon); 

  sscanf(argv[2],"%f",&radius);

  /* If we see an eof just bag out */
  while ( fgets(line,100,stdin) != NULL ) {

     /* A blank line is detected so generate the lines between points */
     if ((line[0] == '\n') && (object_count > 0)) {
         nearest_neighbors(epsilon);
         fprintf(stdout,"\n");      
	 fflush(stdout);
         clear_data();
     } else {
         /* Check for a blank line */
         if (line[0] != '\n') {
	    /* Pass these lines though unmodified */
	    if (line[0] != '@') {
	       /* Read the point in and add it to the existing list */
               n  = sscanf(line,"%f%f%f",&vars[0],&vars[1],&vars[2]);
               add_object(vars[0],vars[1],vars[2]);
	    }
            fprintf(stdout,"%s",line);
	  }
     }
  }

}

/*
** clear_data()
**
** Get rid of all the points that have been accumulating.
**
**
*/
clear_data()
{
   OBJECT *old_ptr;
   OBJECT *new_ptr;

   old_ptr = object_head.next;
   if (old_ptr != NULL) {
      new_ptr = old_ptr->next;
      while (new_ptr != NULL) {
        free(old_ptr);
        old_ptr = new_ptr;
        new_ptr = new_ptr->next;
      }
      free(old_ptr);
   }
   object_count = 0;
   object_head.next = NULL;
   object = &object_head;
}

/*
** add_object()
**
** Create a node a linked list to hold the point just read in.
**
**
*/
#ifndef _NO_PROTO
add_object(float x, float y, float z)
#else
add_object(x,y,z)
float x;
float y;
float z;
#endif
{

       object->next = (OBJECT *) malloc(sizeof(OBJECT));

       object = object->next;

       object->x1 = x;
       object->y1 = y;
       object->z1 = z;
       object->next = NULL;
       object_count++;

}

/* 
** nearest_neighbors()
**
** Go through the list of accumulated points and find out which 
** ones lie close enough together to justify a line being drawn
** between them. 
**
*/
#ifndef _NO_PROTO
nearest_neighbors(double epsilon)
#else
nearest_neighbors(epsilon)
double epsilon;
#endif

{     int i,j;
      float x1,y1,z1,x2,y2,z2,len,b,a;
      float x1_new,y1_new,x2_new,y2_new,angle,xsign,ysign;
      OBJECT *this_obj;

      object = object_head.next;
      
      /* Start at the beginning and find all the points that lie
      ** within a specified distance of the given point 
      */
      for (i=0;i<object_count;i++) {
           this_obj = object->next;
           x1 = object->x1; y1 = object->y1; z1 = object->z1;
           for (j=i+1;j<object_count;j++) {
               x2 = this_obj->x1; y2 = this_obj->y1; z2 = this_obj->z1;
               len = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
               if (len < epsilon) {
		  /* Write out the line specification */
                  fprintf(stdout,"%f %f %f %f %f %f r=%f c=7\n",x1,y1,z1,x2,y2,z2,radius);
	       } /* endif epsilon */
               this_obj = this_obj->next;
           } /* end for */
           object = object->next;
       } /* end for */
}    
              





