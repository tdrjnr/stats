/* fake set of low level routines for axis, to make it independent	*
*  of plot library.							*/

#include <stdio.h>

/* debug-- sign extension??? */
/* put a coordinate in the file. Coordinates are short integers */

#define putcoord(i) { putchar(i & 255); putchar(( i>>8 ) & 255); }

/* move to a point */

move (ix, iy)
{
   putchar ('m');
   putcoord (ix);
   putcoord (iy);
}

/* plot a point */

point (ix, iy)
{
   putchar ('p');
   putcoord (ix);
   putcoord (iy);
}

/* line from here to there */

line (ix1, iy1, ix2, iy2)
{
   putchar ('l');
   putcoord (ix1);
   putcoord (iy1);
   putcoord (ix2);
   putcoord (iy2);
}

/* continue to a point */

cont (ix, iy)
{
   putchar ('n');
   putcoord (ix);
   putcoord (iy);
}

/* define plotting space */

space (ix1, iy1, ix2, iy2)
{
   putchar ('s');
   putcoord (ix1);
   putcoord (iy1);
   putcoord (ix2);
   putcoord (iy2);
}

/* make a label */

label (s)
char *s;
{
   putchar ('t');
   puts (s);
}

/* change linestyle */

linemod (s)
char *s;
{
   putchar ('f');
   puts (s);
}

/* erase the screen */

erase ()
{
   putchar ('e');
}

/* provide the string index function */

char * index (s, c)
 char *s, c;
{
   while (*s != '\0') {
      if (*s == c) return(s);
      s++;
   }

   return (NULL);
}
