#include <math.h>
#include <stdio.h>

float   x[2000], y[2000];
int npts, bbits[2000];

/*
   ptext - pointer to ugs text string
   xorg  - origin x coordinate
   yorg  - origin y coordinate
   size  - scale factor
 */

/* text as is */

ascii (ptext, xorg, yorg, size)
 char* ptext;
 int xorg, yorg;
 float size;
{
   int i;
   float z;

   z = size;
   i = 0;
   while ((ptext[i] != '\n') && (ptext[i] != '\0')) i++;
   ptext[i] = '\0';
   npts = strokes (ptext, x, y, bbits);

   for (i = 0; i < npts; i++) {
      if (bbits[i] == 0)
         move (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
      else
         cont (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
   }
}

/* right justified string */

rascii (ptext, xorg, yorg, size)
 char* ptext;
 int xorg, yorg;
 float size;
{
   int i;
   float z;
   float xmax = -(float)(HUGE);

   z = size;
   i = 0;
   while ((ptext[i] != '\n') && (ptext[i] != '\0')) i++;
   ptext[i] = '\0';
   npts = strokes (ptext, x, y, bbits);

   for (i = 0; i < npts; i++)
      xmax = (xmax > x[i]) ? xmax : x[i];

   xorg -= z*xmax;

   for (i = 0; i < npts; i++) {
      if (bbits[i] == 0)
         move(xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
      else
         cont (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
   }
}

/* centered text */

cascii (ptext, xorg, yorg, size)
 char* ptext;
 int xorg, yorg;
 float size;
{
   int i;
   float z;
   float xmin = (float)(HUGE);
   float xmax = -(float)(HUGE);

   z = size;
   i = 0;
   while ((ptext[i] != '\n') && (ptext[i] != '\0')) i++;
   ptext[i] = '\0';
   npts = strokes (ptext, x, y, bbits);

   for (i = 0;i < npts; i++) {
      xmin = (xmin < x[i]) ? xmin : x[i];
      xmax = (xmax >  x[i]) ? xmax : x[i];
   }
   xorg -= z*(xmax+xmin)/2.;

   for (i = 0; i < npts; i++) {
      if (bbits[i] == 0)
         move (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
      else
         cont (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
   }
}

/* centered vertical text */

cvascii (ptext, xorg, yorg, size)
 char* ptext;
 int xorg, yorg;
 float size;
{
   int i;
   float z,temp;
   float ymin = (float)(HUGE);
   float ymax = -(float)(HUGE);

   z = size;
   i = 0;
   while ((ptext[i] != '\n') && (ptext[i] != '\0')) i++;
   ptext[i] = '\0';
   npts = strokes (ptext, x, y, bbits);

   for (i = 0; i < npts; i++) {
      temp = y[i];
      y[i] = x[i];
      x[i] = -temp;
      ymin = (ymin < y[i]) ? ymin : y[i];
      ymax = (ymax > y[i]) ? ymax : y[i];
   }
   yorg -= z*(ymax+ymin)/2.;

   for (i = 0; i < npts; i++) {
      if (bbits[i] == 0)
         move (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
      else
         cont (xorg+(int)(z*x[i]), yorg+(int)(z*y[i]));
   }
}
