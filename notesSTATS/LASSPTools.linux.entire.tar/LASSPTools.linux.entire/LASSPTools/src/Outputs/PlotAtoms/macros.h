/* some useful C macros */

#define MAX(a,b)      ( (a) > (b) ? (a) : (b) )
#define MIN(a,b)      ( (a) < (b) ? (a) : (b) )
#define SWAP(a,b)     { a^=b;b^=a;a^=b;}          /*only fixed point numbers*/
#define SQUARE(a)     ( (a)*(a) )
#define CUBE(a)       ( (a)*(a)*(a) )
#define CAT(a,b)      a##b
#define XCAT(a,b)     CAT(a,b)
#define STRING(a)     #a
#define XSTRING(a)    STRING(a)
#define ABS(a)        ( (a)>0 ? (a): -1*(a) )
#define ABSDIFF(a,b)  ( (a)>(b) ? ((a)-(b)) : ((b)-(a)) )

/* MAX returns the maximium of two numbers */
/* MIN returns the minimum of two numbers */
/* SWAP exchanges two fixed-point (int, long, char) variables without 
   an extra temporary variable */
/* SQUARE returns the square of a number */
/* CUBE returns the cube of a number */
/* CAT concatenates two expressions ( at the preprocessor level ). Note that
   it is not nestable CAT(a,CAT(b,c)) will not work */
/* XCAT is CAT, but it is now nestable: XCAT(a,XCAT(b,c)) will work. Refer
   to p. 231 in Kernighan and Ritchie */
/* STRING creates a string from expression a at preprocessing time.  Note 
   that #define's will not be expanded in the string. */
/* XSTRING allows #define's to be expanded in the string.  Again see K & R */

/* BE AWARE that some of these only work with ANSI C */
