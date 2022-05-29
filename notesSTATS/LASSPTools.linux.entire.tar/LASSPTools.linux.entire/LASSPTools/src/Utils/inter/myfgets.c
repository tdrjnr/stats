/* Just like fgets, but no restriction on line length */

#include <stdio.h>
#include <string.h>

char *myfgets(s, n, stream)
char *s;
int n;
FILE *stream;
{
   int toread;
   char *first = s;
   int c;

   if(n <= BUFSIZ) return fgets(s, n, stream);

   while(--n > 0) {
      c =  getc(stream);
      if(c == EOF) return NULL;
      if(c == '\n') c = '\0';   /* End of line */
      *s++ = (char) c;          /* Copy character */
      if(c == '\0') return first;
   }
   *s++ = '\0';
   return first;
}
