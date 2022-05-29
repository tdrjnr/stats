# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <math.h>

# include "math_io.h"

/********************************  COMMON  ***********************************/

# define INITIAL_DATA_SPACE 400
# define INITIAL_EL_SPACE 200


/******************************  UTILITIES  **********************************/

#ifndef _NO_PROTO
void    error(char *error_text);
int     skip_comments(FILE *in);
int     skip_line(FILE *in);
int     skip_punctuation(int end_char, FILE *in);
#endif

/*===========================================================================*/

#ifndef _NO_PROTO
void error(char *error_text)
#else
void error(error_text)
char *error_text;
#endif
/* standard error handler */
{ 
    fprintf(stderr,"Run-time error... Exiting to system\n");
    fprintf(stderr,"%s\n",error_text);
    exit(2);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int skip_line(FILE *in)
#else
int skip_line(in)
FILE *in;
#endif
/* This routine skips to the end of a line, and returns 1		*/
{
    int c;

    while(((c=getc(in)) != '\n') && (c != EOF));
    return(c);
}

/*==========================================================================*/

#ifndef _NO_PROTO
int skip_comments(FILE *in)
#else
int skip_comments(in)
FILE *in;
#endif
/* This routine skips stuff which is legal before the first number	*
 * in a vector or a matrix.  It skips stuff which might end a previous  *
 * matrix.  It returns EOF if the file ends.				*
 * It skips the rest of a line if it sees % or # or C.  It should skip 	*
 * C comments too.							*/
{
    int c,x;
    x = !EOF;

    while((c=getc(in)) == ' ' ||
	c == '\t' ||
	c == '\n' ||
        c=='}' || 
	c==')' || 
	c==']' || 
	c==',' || 
	c==';' || 
	c==':' ||
	(c=='%' && (x=skip_line(in))) ||
	(c=='#' && (x=skip_line(in))) ||
	(c=='C' && (x=skip_line(in))) ); 
    if ((c==EOF) || (x == EOF)) return(EOF);
    ungetc(c,in);
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int skip_punctuation(int end_char, FILE *in)
#else
int skip_punctuation(end_char, in)
int end_char;
FILE *in;
#endif
/* This routine skips stuff which is legal between numbers in a row,	*
 * or legal at the start of a row.  It returns the last read character.	*/
{
        int c;
        
        while((c=getc(in)) == ' ' ||
                c == '\t' ||
                c == ',' ||
                c == 13 ||				/* ^M for Mosaic */
                (end_char != ';' && c == ';') ||
                (end_char != ':' && c == ':') ||
                (end_char == ']' && c == '[') ||
                (end_char == '}' && c == '{') ||
                (end_char != '\n' && c == '\n'));
        return c;
}

/*===========================================================================*/

#ifndef _NO_PROTO
int skip_white(int end_char,FILE *in)
#else
int skip_white(end_char, in)
int end_char;
FILE *in;
#endif
/* This routine skips spaces and tabs but not commas.  The last         *
 * character read is returned.                                          */
{
        int c;

        while((c=getc(in)) == ' ' ||
                c == '\t'  ||
               (c == '\n' && end_char !='\n'));
        return c;
}

/****************************************************************************/
/*******************************  VECTORS  **********************************/

/*******************************  DOUBLE  ***********************************/

#ifndef _NO_PROTO
void put_binary_vector(Vector V, FILE *out)
#else
void put_binary_vector(V, out)
Vector V;
FILE *out;
#endif
/****************************************************************
 * outputs a vector of doubles in binary format into file out   *
 * from vector V. 		                                *
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&V.N),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write N in put_binary_vector");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (N)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_vector!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for N", written_numbers);
            error("Unexpected negative value in put_binary_vector");
	}
    }

    numbers_left = V.N;
    start_write = (char *)(&V.el[COLSTART]);

    if((written_numbers = fwrite((char *) start_write, 
			   (int)sizeof(double), numbers_left, out))
		!= numbers_left)
    {
	if(written_numbers == 0) 
		error("fwrite failed in put_binary_vector");
        if(written_numbers > numbers_left)
	{
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_binary_vector!");
	}
	if(written_numbers > 0)
	{
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += written_numbers * sizeof(double)/sizeof(char);
	}
	if(written_numbers < 0)
	{
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
                error("Unexpected negative value in put_binary_vector");
	}
    }
    fflush(out);
}

#ifndef _NO_PROTO
void put_binary_int_vector(Vector V, FILE *out)
#else
void put_binary_int_vector(V, out)
Vector V;
FILE *out;
#endif
/****************************************************************
 * outputs a vector of ints in binary format into file out   *
 * from vector V. 		                                *
 ****************************************************************/
{
    int i, j, count, written_numbers, numbers_left;
    char *start_write;
    int tbuf[101];

    if((written_numbers=fwrite((char *)(&V.N),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write N in put_binary_int_vector");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (N)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_int_vector!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for N", written_numbers);
            error("Unexpected negative value in put_binary_int_vector");
	}
    }

    numbers_left = V.N;
    j = COLSTART;
    while (numbers_left > 0) {
         if (numbers_left > 100)
	    count = 100;
	 else
	    count = numbers_left;
	 for (i=0;i<count;i++) 
	    tbuf[i] = (int) V.el[j++];
         start_write = (char *)(&tbuf[0]);

         if((written_numbers = fwrite((char *) start_write, 
			   (int)sizeof(int), count, out)) != count) {
	     if(written_numbers == 0) 
		  error("fwrite failed in put_binary_int_vector");
             if(written_numbers != count) {
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, count);
		    error("Too many (or few) numbers written in put_binary_int_vector!");
	     }
	}
        numbers_left = numbers_left - count;
    }
    fflush(out);
  }
/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_vector(Vector V, FILE *out, 
			  char separator, char begin_char, char end_char)
#else
void put_formatted_vector(V, out, separator, begin_char, end_char)
Vector V;
FILE *out;
char separator, begin_char, end_char;
#endif
/* Displays V on the given file with elements separated by separator    *
 * and with whole vector enclosed by begin_char and end_char.           *
 * To assign a null value to begin_char or end_char call this function  *
 * with '\0' (the null character) in the appropriate argument position. *
 * DO NOT call the function with "" instead of '\0'.                    */

{
    int  i;

    if (separator=='\0') separator=' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<V.N-1; i++)
            fprintf(out, "%g%c", V.el[i+COLSTART], separator);
    fprintf(out, "%g", V.el[V.N-1+COLSTART]);
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_vector(Vector V, FILE *out)
#else
void put_vector(V, out)
Vector V; 
FILE *out;
#endif
/* displays V on the given file with spaces as separators and no *
 * begin_char or end_char					 */
 
{
    put_formatted_vector(V, out, ' ', '\0', '\n');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_HTML_vector(Vector V, FILE *out)
#else
void put_HTML_vector(V, out)
Vector V;
FILE *out;
#endif
/* Displays V on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i;

    fprintf(out, "<TABLE BORDER>\n");
    fprintf(out,"<TR ALIGN=RIGHT>\n    ");
    for(i=0; i<V.N; i++) fprintf(out, "<TD> %g</TD>", V.el[i+COLSTART]);
    fprintf(out, "\n</TR><P>\n");
    fprintf(out,"</TABLE>\n");
    fflush(out);
}


/*===========================================================================*/

#ifndef _NO_PROTO
void put_Mathematica_vector(Vector V, FILE *out)
#else
void put_Mathematica_vector(V, out)
Vector V;
FILE *out;
#endif
/* displays V on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char		   */

{
    put_formatted_vector(V, out, ',', '{', '}');
}

/*===========================================================================*/

Vector *create_default_vector()

/* Calls create_vector(0) to allocate block of space for data 	 	*
 * using defaults specified by the definition of INITIAL_DATA_SPACE 	*/
{
	return create_vector(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
Vector *create_vector(int N)
#else
Vector *create_vector(N)
int N;
#endif

/* Allocates space for a vector */
{
   int i;
   Vector *B;

   B = (Vector *) malloc((unsigned) sizeof(Vector));
   if (!B) error("malloc of vector storage failed in create_vector");

   B->N = N;

/* Allocate block of space for data */

   if (B->N > 0)
   {
	B->el=(double *) malloc((unsigned) B->N*sizeof(double))-COLSTART;
	B->data_reserved = B->N;
   }
   else
   {
	B->el=(double *) malloc((unsigned) INITIAL_DATA_SPACE*sizeof(double))-COLSTART;
	B->data_reserved = INITIAL_DATA_SPACE;
   }
	
   if (!(B->el)) error("malloc of data space failed in create_vector");

   return B;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_vector(Vector* B)
#else
void free_vector(B)
Vector* B;
#endif

/* frees all pointers associated with the Vector structure *B               */
{
   free((void*)B->el);
   free((void*)B);
}

/*==========================================================================*/

#ifndef _NO_PROTO
int get_binary_vector(Vector *B, FILE *in)
#else
int get_binary_vector(B, in)
Vector *B;
FILE *in;
#endif
/****************************************************************
 * reads a vector of doubles in binary format from in           *
 * into vector B. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied vectors, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of vector */

    if( (read_numbers=fread((char *)&(B->N),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (N)\n",
			read_numbers);
            error("fread got too many numbers in get_binary_vector!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for N", read_numbers);
            error("Unexpected negative value in get_binary_vector");
        }
    }


/* Try to determine if B->el has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the vector does not initialize the elements 	*
 * to zero.  Use *M = *create_vector(0) to set up space if you don't 	*
 * know	the size of your vector, and create_vector(N) if you do.	*
 * Alternatively, set V.el = NULL.					*
 *									*/

    if (B->el==NULL || B->data_reserved <=0) *B = *create_vector(B->N);

/* If needed, expand block of space for data */

    if(B->N > B->data_reserved)
    {
	B->el = (double *) 
		realloc((char*) B->el+COLSTART*sizeof(double)/sizeof(char), 
				(unsigned) B->N*sizeof(double))-COLSTART;
	if (!(B->el)) 
		error("realloc of data space failed in get_binary_vector");
	B->data_reserved = B->N;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (B->N);
    start_read = (char *) B->el + (COLSTART*sizeof(double)/sizeof(char));
    while((read_numbers = fread(start_read, sizeof(double), numbers_left, in )) 
		!= numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_binary_vector!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    fprintf(stderr,"in get_binary_vector");
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(double) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
	    error("Unexpected negative value in get_binary_vector");
	}
    }
    return(0);
}
#ifndef _NO_PROTO
int get_binary_int_vector(Vector *B, FILE *in)
#else
int get_binary_int_vector(B, in)
Vector *B;
FILE *in;
#endif
/****************************************************************
 * reads a vector of ints    in binary format from in           *
 * into vector B. 		                                *
 ****************************************************************/
{

    int i, j,count,read_numbers, numbers_left;
    int tbuf[101];
    char *start_read;

/* Read size of vector */

    if( (read_numbers=fread((char *)&(B->N),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (N)\n",
			read_numbers);
            error("fread got too many numbers in get_binary_vector!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for N", read_numbers);
            error("Unexpected negative value in get_binary_vector");
        }
    }


/* Try to determine if B->el has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the vector does not initialize the elements 	*
 * to zero.  Use *M = *create_vector(0) to set up space if you don't 	*
 * know	the size of your vector, and create_vector(N) if you do.	*
 * Alternatively, set V.el = NULL.					*
 *									*/

    if (B->el==NULL || B->data_reserved <=0) *B = *create_vector(B->N);

/* If needed, expand block of space for data */

    if(B->N > B->data_reserved)
    {
	B->el = (double *) 
		realloc((char*) B->el+COLSTART*sizeof(double)/sizeof(char), 
				(unsigned) B->N*sizeof(double))-COLSTART;
	if (!(B->el)) 
		error("realloc of data space failed in get_binary_vector");
	B->data_reserved = B->N;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (B->N);
    j = COLSTART;
    while (numbers_left > 0) {
       if (numbers_left > 100)
	  count = 100;
       else
	  count = numbers_left;

       start_read = (char *) &tbuf[0];
       read_numbers = fread(start_read, sizeof(int), count, in);
       if (read_numbers != count) {
	   if(read_numbers == 0) return (EOF);
	   if(read_numbers > count) {
	     fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, count);
	     error("Too many numbers read in get_binary_int_vector!");
	   } 
	   if (read_numbers < count) {
	     fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, count);
	     error("Too few numbers read in get_binary_int_vector!");
           }
       }
       for (i=0;i<count;i++) {
	  B->el[j++] = (double)tbuf[i];
       }
       numbers_left = numbers_left - count;
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_vector(Vector *B, FILE *in)
#else
int get_vector(B, in)
Vector *B;
FILE *in;
#endif
/****************************************************************
 * constructs a vector of doubles from a data source which is a *
 * stream of numbers.		                                *
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character for the vector.  The end characters supported 	*
 * are carriage returns (the default) and close brackets of all	*
 * types (used if the first non--blank character is an open 	*
 * bracket of that type.)					*
 * This row is stored into a data block V.el, which is          *
 * enlarged whenever it fills up.                               *
 * B->el is set up to look like a C-style vector (list of       *
 *  pointers to vectors), except that if COLSTART  		*
 *  isn't zero, the vector starts at B->el[COLSTART] (Numerical *
 *  Recipes perversion from Fortran) 				*
 ****************************************************************/
 
 
{
    int  data_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that	*
 * might be between vectors.  Check if file/pipe has ended, return EOF 	*
 * without changing vector if so.					*/

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if B->el has been allocated.  If not, allocate      *
 * space. 	 							*
 *									*
 * WARNING: Declaring the vector does not reserve space, but can start  *
 * with a garbage pointer which will fool our routines.                 *
 * Either set V.el = NULL, or use *V = *create_default_vector(0) to set	*
 * up space if you don't know the size of your vector, and		*
 * create_vector(N) if you do.						*/

    if (B->el==NULL || B->data_reserved <=0) *B = *create_vector(0);

    B->N=0;

/* find out first character: number, or open bracket?  Use to determine	*
 * character which ends rows of vector 					*/

    end_char = '\n';
    if ((c=skip_white(end_char,in))=='{') end_char = '}';
    else if (c=='[') end_char = ']';
    else if (c==EOF) return(EOF);
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  When we	*
 * hit an end_chars, end the vector and return.				*/

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the vector is too large, double the allocated space.              */

    data_count=0;

    while((c=skip_punctuation(end_char, in)) != end_char && c != EOF)
    {
       ungetc(c,in);
       if (data_count == B->data_reserved)
       {
            B->data_reserved *=2;
            B->el = (double *) 
		realloc((char*) B->el+COLSTART*sizeof(double)/sizeof(char),
                        (unsigned) B->data_reserved*sizeof(double))-COLSTART;
            if (!B->el) error("realloc of el failed in get_vector");
       }
       if( fscanf(in,"%lf",&B->el[data_count+COLSTART])==0 )
          error("get_vector: non-numeric input");
       else data_count++;
    }
    B->N = data_count;
    return 0;
}

/***************************** COMPLEX DOUBLE  ******************************/

/*==========================================================================*/

#ifndef _NO_PROTO
int get_complex_double_binary_vector(ComplexDoubleVector *B, FILE *in)
#else
int get_complex_double_binary_vector(B, in)
ComplexDoubleVector *B;
FILE *in;
#endif
/****************************************************************
 * reads a vector of ComplexDoubles in binary format from in    *
 * into vector B. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied vectors, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of vector */

    if( (read_numbers=fread((char *)&(B->N),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (N)\n",
			read_numbers);
            error("fread got too many numbers in get_complex_double_binary_vector!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for N", read_numbers);
            error("Unexpected negative value in get_complex_double_binary_vector");
        }
    }


/* Try to determine if B->el has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the vector does not initialize the elements 	*
 * to zero.  Use *M = *create_vector(0) to set up space if you don't 	*
 * know	the size of your vector, and create_vector(N) if you do.	*
 * Alternatively, set V.el = NULL.					*
 *									*/

    if (B->el==NULL || B->data_reserved <=0) 
				*B = *create_complex_double_vector(B->N);

/* If needed, expand block of space for data */

    if(B->N > B->data_reserved)
    {
	B->el = (ComplexDouble *) realloc((char*) B->el +
				COLSTART*sizeof(ComplexDouble)/sizeof(char), 
			(unsigned) B->N*sizeof(ComplexDouble))-COLSTART;
	if (!(B->el)) 
		error("realloc of data space failed in get_complex_double_binary_vector");
	B->data_reserved = B->N;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (B->N);
    start_read = (char *) B->el + 
				(COLSTART*sizeof(ComplexDouble)/sizeof(char));
    while((read_numbers = fread(start_read, 
		sizeof(ComplexDouble), numbers_left, in )) != numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_complex_double_binary_vector!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    fprintf(stderr,"in get_complex_double_binary_vector");
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(ComplexDouble) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
	    error("Unexpected negative value in get_complex_double_binary_vector");
	}
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int input_Re_or_Im_double(int end_char, FILE *in, double* num)
#else
int input_Re_or_Im_double(end_char, in, num)
int end_char;
FILE *in;
double *num;
#endif

/* This routine is used to read the real or imaginary part of a complex *
 * number.  The routine returns 1 if I, i, J, or j is found in the      *
 * number read and 0 otherwise.                                         *
 * The value of the number read (once I's and J's have been stripped) is*
 * placed in *num.  The sign is taken into account within this routine. *

 * The first character read from the input should be a + (plus) or      *
 * - (minus), an I or i or J or j, part of a number, or white space; so,*
 * skip_punctuation should be used (if necessary) before calling this   *
 * routine.                                                             */
{
    int I=0;  /* indicates whether number is real (0) or imaginary (1)  */
    int sign=1; /* holds the sign of the number being read */
    int c;

/* Check for   + vs. -   and   real vs. imaginary.                      */
    if((c=skip_white(end_char, in))!='-' && c!='+' && c!='I'
                                 && c!='i' && c!='J' && c!='j') ungetc(c,in);
    if (c == '-' || c == '+') {
        if (c == '-') sign *=-1;
        if ((c=skip_white(end_char, in))!='-' && c!='+' && c!='I'
                                && c!='i' && c!='J' && c!='j') ungetc(c,in);
        if (c == '-' || c == '+') {
                if (c == '-') sign *=-1;
                if ((c=skip_white(end_char, in))!='I' && c!='i' && c!='J'
                                                && c!='j') ungetc(c,in);
	}
    }
    if (c=='I' || c=='i' || c=='J' || c=='j') I=1;

/* Watch out for an entry I (or i or J or j) without a coefficient!     */
    if ((c=skip_white(end_char, in))!=',' && c!= ';' && c!=':' && c!=')'
                        && c!=13 	/* ^M before \n in Mosaic */
			&& c!=end_char && c!='+' && c!='-')
    {
        ungetc(c,in);

      /* Read in the number in the proper format                        */
        if (fscanf(in,"%lf", num)==0)
                           error("non-numeric input in input_Re_or_Im_double");
        if ((c=skip_white(end_char,in))!='I' && c!='i' 
                                        && c!='J' && c!='j') ungetc(c,in);
        else I=1;
    }
    else {
        ungetc(c,in);
        (*num)=1;
        if (I==0)
       error("Real number expected, but no coefficient found in input_Re_or_Im_double");
    }
    (*num) *= (double) sign;
    return I;
}

/*===========================================================================*/

#ifndef _NO_PROTO
void get_complex_double_number(int end_char, FILE *in, double *real, 
								double *imag)
#else
void get_complex_double_number(end_char, in, real, imag)
int end_char;
FILE *in;
double *real, *imag;
#endif
/* This routine reads in a complex number in any of a number of formats *
 * and determines the real and imaginary parts.  The real and imaginary *
 * parts are placed in the double precision variables *real and *imag,  *
 * respectively.                                                        *

 * Formats recognized include: a+bI; a+Ib; bI+a; Ib+a; a b; (a,b);      *
 * (a,bI); and (a,Ib)  where 'I' can also be replaced by 'i', 'J', or   *
 * 'j'.  In addition, a-cI (etc.) is interpreted as a+(-c)I, bI-d is    *
 * interpreted as (-d)+bI, and an I (or i or J or j) without a          *
 * coefficient is interpreted as 1I.  Moreover, a by itself is          *
 * interpreted as a+0I while bI or Ib by itself is interpreted as 0+bI. *

 ************************************************************************
 *NOTE: if the format of the entries is a+bI and the separator between  *
 * elements is either a space or a tab, then MAKE SURE that both the    *
 * real and imaginary parts are explicitly shown even when one of them  *
 * is 0 (e.g. write 5+0I rather than just 5).  If this is not done then *
 * wherever you have an entry of the form *a* or *bI*, this routine     *
 * will misinterpret the entry as the real or imaginary part of an entry*
 * of the form a b.  For example,   1+I 2-3.7I 12.2 2I-3   will be      *
 * interpreted as     1+1I 2-3.7I 12.2+2I 1I-3.                         *
 * In many cases data in this form will generate an error message;      *
 * however, in some unfortunate cases (e.g. the example above) this     *
 * routine will think that it's doing the right thing and plug right    *
 * along.                                                               *
 *    If the entries are separated by commas instead of spaces, then    *
 * there is no need to write 'a' as 'a+0I' since the comma tells this   *
 * routine that the end of the element as been reached.                 *
 ************************************************************************/

{
    int c;
    int real_found=0;  /* used for *a b* format */
    double *temp;

    *real = 0;
    *imag = 0;
    temp = (double *) malloc((unsigned) sizeof(double));
    if ((c=skip_white(end_char, in)) == '(') /* format is some form of (a,b) */
    {
      /* get first entry, which should be real part                     */
        if (input_Re_or_Im_double(end_char, in, temp)==0) *real = *temp;
        else
       error("Imaginary entry found in first entry of number in (a,b) format");

      /* Look for comma or other separator between entries              */
        if ((c=skip_white(end_char,in)) != ',')
                error("No comma found where expected in (a,b) format");

      /* Get second entry, assumed to be imaginary part                 */
        input_Re_or_Im_double(end_char, in, imag);

      /* Check for unmatched parentheses. */
        if (skip_white(end_char, in) !=')')
            error("Unmatched parentheses found by get_complex_double_number");
    }
    else        /* format is some variant of a+bI or is  a b */
    {
        ungetc(c,in);

      /* Get first entry and assign it to real or imaginary part        */
        if (input_Re_or_Im_double(end_char, in, temp)==0) {
                *real = *temp;
                real_found=1;
	}
        else *imag = *temp;

      /* Check for a comma or other separator or end_char, indicating   *
       * that the end of the element has been reached.                  */
        if ((c=skip_white(end_char, in))!=',' && c!=';' && c!=':'
			&& c!=13 	/* ^M before \n in Mosaic */
			&& c!=end_char) {
          /* Get second entry and assign it to real or imaginary part   */
            ungetc(c, in);
            if (input_Re_or_Im_double(end_char, in, temp)==1 && (!real_found))
		error("number of the form aI bI read by get_complex_double_number");
	    if (real_found) *imag = *temp;
            else *real = *temp;
	}
        else ungetc(c,in);
    }
    free ((void *) temp);
}

#ifndef _NO_PROTO
void put_complex_double_binary_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_binary_vector(V, out)
ComplexDoubleVector V;
FILE *out;
#endif
/****************************************************************
 * outputs a vector of ComplexDoubles in binary format into	*
 * file out from vector V. 		                        *
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&V.N),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	 error("fwrite failed to write N in put_complex_double_binary_vector");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (N)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_double_binary_vector!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for N", written_numbers);
            error("Unexpected negative value in put_complex_double_binary_vector");
	}
    }

    numbers_left = V.N;
    start_write = (char *)(&V.el[COLSTART]);

    if((written_numbers = fwrite((char *) start_write, 
			   (int)sizeof(ComplexDouble), numbers_left, out))
		!= numbers_left)
    {
	if(written_numbers == 0) 
		error("fwrite failed in put_complex_double_binary_vector");
        if(written_numbers > numbers_left)
	{
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_complex_double_binary_vector!");
	}
	if(written_numbers > 0)
	{
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += 
			written_numbers * sizeof(ComplexDouble)/sizeof(char);
	}
	if(written_numbers < 0)
	{
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
                error("Unexpected negative value in put_complex_double_binary_vector");
	}
    }
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_element(double real, double imag, FILE *out,
                         char imaginary_char, char complex_sep,
                         char complex_begin, char complex_end)
#else
void put_complex_double_element(real, imag, out, imaginary_char, complex_sep,
                                        complex_begin, complex_end)
double real, imag;
FILE *out;
char imaginary_char, complex_sep, complex_begin, complex_end;
#endif
/* Displays element with real part *real* and imaginary part *imag*     *
 * using imaginary_char, complex_sep, complex_begin, and complex_end as *
 * described below in put_formatted_complex_double_vector.		*/

{
    if (complex_sep=='\0') complex_sep=' ';
    if (complex_begin!='\0') fprintf(out, "%c", complex_begin);
    fprintf(out, "%g", real);
    if (complex_sep=='+' && imag<0)
        fprintf(out, "%g", imag);
    else fprintf(out, "%c%g", complex_sep, imag);
    if (imaginary_char!='\0') fprintf(out, "%c", imaginary_char);
    if (complex_end!='\0') fprintf(out, "%c", complex_end);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_complex_double_vector(ComplexDoubleVector V, FILE *out, 
	char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end)
#else
void put_formatted_complex_double_vector(V, out, separator, begin_char, 
	end_char, imaginary_char, complex_sep, complex_begin, complex_end)
ComplexDoubleVector V;
FILE *out;
char separator, begin_char, end_char, imaginary_char, complex_sep;
char complex_begin, complex_end;
#endif
/* Displays V on the given file with elements separated by separator    *
 * and with whole vector enclosed by begin_char and end_char.           *

 * The character *imaginary_char* is used as the symbol for the square  *
 * root of -1.  The character *complex_sep* determines how the real     *
 * and imaginary parts of an element are separated:                     *
 *    complex_sep='+' causes the elements to be written as a+bI or a-bI *
 *    complex_sep=' ' causes the elements to be written as a b          *
 *    complex_sep=',' causes the elements to be written as a,b          *
 * The elements are enclosed by complex_begin and complex_end.  For     *
 * example, complex_begin='(', complex_end=')', and complex_sep=','     *
 * causes the elements to be written as (a,b).                          *
 * NOTE: If it is desired to suppress the I's (or J's) in the output,   *
 *    set imaginary_char to null.  e.g., if you want the elements to be *
 * written as (a,b) but set imaginary_char to 'I', then the elements    *
 * will be written as (a,bI).

 * To assign a null value to begin_char, end_char, or imaginary_char,   *
 * call this function with '\0' (the null character) in the appropriate *
 * argument position.                                                   *
 * DO NOT call the function with "" instead of '\0'.                    */

{
    int  i;

    if (separator=='\0') separator=' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<V.N-1; i++){
        put_complex_double_element(V.el[i+COLSTART].real, 
			V.el[i+COLSTART].imaginary, out,imaginary_char, 
			complex_sep, complex_begin, complex_end);
        fprintf(out, "%c", separator);
    }
    put_complex_double_element(V.el[i+COLSTART].real, 
			V.el[i+COLSTART].imaginary, out, imaginary_char, 
			complex_sep, complex_begin, complex_end);
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_vector(V, out)
ComplexDoubleVector V; 
FILE *out;
#endif
/* displays V on the given file with spaces as separators and no *
 * begin_char or end_char                                        *
 * 'I' is the imaginary_char and there are no complex_begin or	*
 * complex_end.							*/
 
{
    put_formatted_complex_double_vector(V, out, ' ', '\0', '\n', 'I',
					'+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_HTML_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_HTML_vector(V, out)
ComplexDoubleVector V;
FILE *out;
#endif
/* Displays V on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i;

    fprintf(out, "<TABLE BORDER>\n");
    fprintf(out,"<TR ALIGN=RIGHT>\n    ");

    for(i=0; i<V.N; i++){
	fprintf(out, "<TD> ");
        put_complex_double_element(V.el[i+COLSTART].real,
               		           V.el[i+COLSTART].imaginary, out,
				   'I', '+', '\0', '\0');
	fprintf(out, "</TD>");
    }
    fprintf(out, "\n</TR><P>\n");
    fprintf(out,"</TABLE>\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_Mathematica_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_Mathematica_vector(V, out)
ComplexDoubleVector V;
FILE *out;
#endif
/* Displays V on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char             *
 * 'I' is the imaginary_char and there are no complex_begin or		*
 * complex_end.								*/

{
    put_formatted_complex_double_vector(V, out, ',', '{', '}', 'I',
					'+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_ordered_pair_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_ordered_pair_vector(V, out)
ComplexDoubleVector V;
FILE *out;
#endif
/* Displays V on the given file in ordered pair (a,b) format with commas   *
 * as separators, '{' as the begin_char and '}' as the end_char.	   *
 * There is no imaginary_char, complex_begin is '(', and complex_end is ')'*/

{
    put_formatted_complex_double_vector(V, out, ',', '\0', '\0', '\0',
                                        ',', '(', ')');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_simple_vector(ComplexDoubleVector V, FILE *out)
#else
void put_complex_double_simple_vector(V, out)
ComplexDoubleVector V;
FILE *out;
#endif
/* displays V on the given file in simple a b format with elements	*
 * separated by spaces.							*/

{
    put_formatted_complex_double_vector(V, out, ' ', '\0', '\0', '\0',
                                        ' ', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexDoubleVector *create_default_complex_double_vector()
#else
ComplexDoubleVector *create_default_complex_double_vector()
#endif

/* Calls create_vector(0) to allocate block of space for data           *
 * using defaults specified by the definition of INITIAL_DATA_SPACE     */
{
        return create_complex_double_vector(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexDoubleVector *create_complex_double_vector(int N)
#else
ComplexDoubleVector *create_complex_double_vector(N)
int N;
#endif

/* Allocates space for a vector */
{
   int i;
   ComplexDoubleVector *B;

   B = (ComplexDoubleVector *) malloc((unsigned) sizeof(ComplexDoubleVector));
   if (!B) 
      error("malloc of vector storage failed in create_complex_double_vector");

   B->N = N;

/* Allocate block of space for data */

   if (B->N > 0)
   {
        B->el=(ComplexDouble *) malloc((unsigned) B->N*sizeof(ComplexDouble))-COLSTART;
        B->data_reserved = B->N;
   }
   else
   {
        B->el=(ComplexDouble *)
              malloc((unsigned) INITIAL_DATA_SPACE*sizeof(ComplexDouble))-COLSTART;
        B->data_reserved = INITIAL_DATA_SPACE;
   }

   if (!(B->el)) 
         error("malloc of data space failed in create_complex_double_vector");

   return B;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_complex_double_vector(ComplexDoubleVector* B)
#else
void free_complex_double_vector(B)
ComplexDoubleVector* B;
#endif

/* frees all pointers associated with the *
 * ComplexDoubleVector structure *B       */
{
   free((void*)B->el);
   free((void*)B);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_complex_double_vector(ComplexDoubleVector *B, FILE *in)
#else
int get_complex_double_vector(B, in)
ComplexDoubleVector *B;
FILE *in;
#endif
/****************************************************************
 * constructs a vector of ComplexDoubles from a data source	*
 * which is a stream of numbers.				*
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character for the vector.  The end characters supported  *
 * are carriage returns (the default) and close brackets of all *
 * types (used if the first non--blank character is an open     *
 * bracket of that type.)                                       *
 * This row is stored into a data block V.el, which is          *
 * enlarged whenever it fills up.                               *
 * B->el is set up to look like a C-style vector (list of       *
 *  pointers to vectors), except that if COLSTART               *
 *  isn't zero, the vector starts at B->el[COLSTART] (Numerical *
 *  Recipes perversion from Fortran)                            *
 ****************************************************************/


{
    int  data_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that   *
 * might be between vectors.  Check if file/pipe has ended, return EOF  *
 * without changing vector if so.					*/

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if B->el has been allocated.  If not, allocate      *
 * space.                                                               *
 *                                                                      *
 * WARNING: Declaring the vector does not reserve space, but can start  *
 * with a garbage pointer which will fool our routines.                 *
 * Either set V.el = NULL, or use					*
 *		*V = *create_default_complex_double_vector(0)		*
 * to set up space if you don't know the size of your vector, and	*
 *		create_complex_double_vector(N)     if you do.		*/

    if (B->el==NULL || B->data_reserved <=0) 
				*B = *create_complex_double_vector(0);

    B->N=0;

/* find out first character: number, or open bracket?  Use to determine *
 * character which ends rows of vector                                  */

    end_char = '\n';
    if ((c=skip_white(end_char,in))=='{') end_char = '}';
    else if (c=='[') end_char = ']';
    else if (c == EOF) return(EOF);
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  When we       *
 * hit an end_char, end the vector and return.                         */

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the vector is too large, double the allocated space.              */

/* Call real_vector or complex_vector, depending on B->type             */

    data_count=0;

    while((c=skip_punctuation(end_char, in)) != end_char && c != EOF)
    {
       ungetc(c,in);
       if (data_count == B->data_reserved)
       {
            B->data_reserved *=2;
            B->el = (ComplexDouble *) 
	     realloc((char*) B->el+COLSTART*sizeof(ComplexDouble)/sizeof(char),
                  (unsigned) B->data_reserved*sizeof(ComplexDouble))-COLSTART;
            if (!B->el) error("realloc of el failed in get_vector");
       }
        get_complex_double_number(end_char, in, 
				  &B->el[data_count+COLSTART].real,
				  &B->el[data_count+COLSTART].imaginary);
        data_count++;
    }
    B->N = data_count;
    return 0;
}

/*==========================================================================*/

/************************* COMPLEX FLOAT (SINGLE)  **************************/

#ifndef _NO_PROTO
int get_complex_float_binary_vector(ComplexFloatVector *B, FILE *in)
#else
int get_complex_float_binary_vector(B, in)
ComplexFloatVector *B;
FILE *in;
#endif
/****************************************************************
 * reads a vector of ComplexFloats in binary format from in    *
 * into vector B. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied vectors, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of vector */

    if( (read_numbers=fread((char *)&(B->N),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (N)\n",
			read_numbers);
            error("fread got too many numbers in get_complex_float_binary_vector!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for N", read_numbers);
            error("Unexpected negative value in get_complex_float_binary_vector");
        }
    }


/* Try to determine if B->el has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the vector does not initialize the elements 	*
 * to zero.  Use *M = *create_vector(0) to set up space if you don't 	*
 * know	the size of your vector, and create_vector(N) if you do.	*
 * Alternatively, set V.el = NULL.					*
 *									*/

    if (B->el==NULL || B->data_reserved <=0) 
				*B = *create_complex_float_vector(B->N);

/* If needed, expand block of space for data */

    if(B->N > B->data_reserved)
    {
	B->el = (ComplexFloat *) realloc((char*) B->el +
				COLSTART*sizeof(ComplexFloat)/sizeof(char), 
			(unsigned) B->N*sizeof(ComplexFloat))-COLSTART;
	if (!(B->el)) 
		error("realloc of data space failed in get_complex_float_binary_vector");
	B->data_reserved = B->N;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (B->N);
    start_read = (char *) B->el + 
				(COLSTART*sizeof(ComplexFloat)/sizeof(char));
    while((read_numbers = fread(start_read, 
		sizeof(ComplexFloat), numbers_left, in )) != numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_complex_float_binary_vector!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    fprintf(stderr,"in get_complex_float_binary_vector");
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(ComplexFloat) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
	    error("Unexpected negative value in get_complex_float_binary_vector");
	}
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int input_Re_or_Im_float(int end_char, FILE *in, float* num)
#else
int input_Re_or_Im_float(end_char, in, num)
int end_char;
FILE *in;
float *num;
#endif

/* This routine is used to read the real or imaginary part of a complex *
 * number.  The routine returns 1 if I, i, J, or j is found in the      *
 * number read and 0 otherwise.                                         *
 * The value of the number read (once I's and J's have been stripped) is*
 * placed in *num.  The sign is taken into account within this routine. *

 * The first character read from the input should be a + (plus) or      *
 * - (minus), an I or i or J or j, part of a number, or white space; so,*
 * skip_punctuation should be used (if necessary) before calling this   *
 * routine.                                                             */
{
    int I=0;  /* indicates whether number is real (0) or imaginary (1)  */
    int sign=1; /* holds the sign of the number being read */
    int c;

/* Check for   + vs. -   and   real vs. imaginary.                      */
    if((c=skip_white(end_char, in))!='-' && c!='+' && c!='I'
                                 && c!='i' && c!='J' && c!='j') ungetc(c,in);
    if (c == '-' || c == '+') {
        if (c == '-') sign *=-1;
        if ((c=skip_white(end_char, in))!='-' && c!='+' && c!='I'
                                && c!='i' && c!='J' && c!='j') ungetc(c,in);
        if (c == '-' || c == '+') {
                if (c == '-') sign *=-1;
                if ((c=skip_white(end_char, in))!='I' && c!='i' && c!='J'
                                                && c!='j') ungetc(c,in);
	}
    }
    if (c=='I' || c=='i' || c=='J' || c=='j') I=1;

/* Watch out for an entry I (or i or J or j) without a coefficient!     */
    if ((c=skip_white(end_char, in))!=',' && c!= ';' && c!=':' && c!=')'
                        && c!=end_char && c!='+' && c!='-')
    {
        ungetc(c,in);

      /* Read in the number in the proper format                        */
        if (fscanf(in,"%f", num)==0)
                           error("non-numeric input in input_Re_or_Im_float");
        if ((c=skip_white(end_char,in))!='I' && c!='i' 
                                        && c!='J' && c!='j') ungetc(c,in);
        else I=1;
    }
    else {
        ungetc(c,in);
        (*num)=1;
        if (I==0)
       error("Real number expected, but no coefficient found in input_Re_or_Im_float");
    }
    (*num) *= (float) sign;
    return I;
}

/*===========================================================================*/

#ifndef _NO_PROTO
void get_complex_float_number(int end_char, FILE *in, float *real, 
								float *imag)
#else
void get_complex_float_number(end_char, in, real, imag)
int end_char;
FILE *in;
float *real, *imag;
#endif
/* This routine reads in a complex number in any of a number of formats *
 * and determines the real and imaginary parts.  The real and imaginary *
 * parts are placed in the single precision variables *real and *imag,  *
 * respectively.                                                        *

 * Formats recognized include: a+bI; a+Ib; bI+a; Ib+a; a b; (a,b);      *
 * (a,bI); and (a,Ib)  where 'I' can also be replaced by 'i', 'J', or   *
 * 'j'.  In addition, a-cI (etc.) is interpreted as a+(-c)I, bI-d is    *
 * interpreted as (-d)+bI, and an I (or i or J or j) without a          *
 * coefficient is interpreted as 1I.  Moreover, a by itself is          *
 * interpreted as a+0I while bI or Ib by itself is interpreted as 0+bI. *

 ************************************************************************
 *NOTE: if the format of the entries is a+bI and the separator between  *
 * elements is either a space or a tab, then MAKE SURE that both the    *
 * real and imaginary parts are explicitly shown even when one of them  *
 * is 0 (e.g. write 5+0I rather than just 5).  If this is not done then *
 * wherever you have an entry of the form *a* or *bI*, this routine     *
 * will misinterpret the entry as the real or imaginary part of an entry*
 * of the form a b.  For example,   1+I 2-3.7I 12.2 2I-3   will be      *
 * interpreted as     1+1I 2-3.7I 12.2+2I 1I-3.                         *
 * In many cases data in this form will generate an error message;      *
 * however, in some unfortunate cases (e.g. the example above) this     *
 * routine will think that it's doing the right thing and plug right    *
 * along.                                                               *
 *    If the entries are separated by commas instead of spaces, then    *
 * there is no need to write 'a' as 'a+0I' since the comma tells this   *
 * routine that the end of the element as been reached.                 *
 ************************************************************************/

{
    int c;
    int real_found=0;  /* used for *a b* format */
    float *temp;

    *real = 0;
    *imag = 0;
    temp = (float *) malloc((unsigned) sizeof(float));
    if ((c=skip_white(end_char, in)) == '(') /* format is some form of (a,b) */
    {
      /* get first entry, which should be real part                     */
        if (input_Re_or_Im_float(end_char, in, temp)==0) *real = *temp;
        else
       error("Imaginary entry found in first entry of number in (a,b) format");

      /* Look for comma or other separator between entries              */
        if ((c=skip_white(end_char,in)) != ',')
                error("No comma found where expected in (a,b) format");

      /* Get second entry, assumed to be imaginary part                 */
        input_Re_or_Im_float(end_char, in, imag);

      /* Check for unmatched parentheses. */
        if (skip_white(end_char, in) !=')')
            error("Unmatched parentheses found by get_complex_float_number");
    }
    else        /* format is some variant of a+bI or is  a b */
    {
        ungetc(c,in);

      /* Get first entry and assign it to real or imaginary part        */
        if (input_Re_or_Im_float(end_char, in, temp)==0) {
                *real = *temp;
                real_found=1;
	}
        else *imag = *temp;

      /* Check for a comma or other separator or end_char, indicating   *
       * that the end of the element has been reached.                  */
        if ((c=skip_white(end_char, in))!=',' && c!=';' && c!=':'
			&& c!=13 	/* ^M before \n in Mosaic */
                                        && c!=end_char) {
          /* Get second entry and assign it to real or imaginary part   */
            ungetc(c, in);
            if (input_Re_or_Im_float(end_char, in, temp)==1 && (!real_found))
		error("number of the form aI bI read by get_complex_float_number");
	    if (real_found) *imag = *temp;
            else *real = *temp;
	}
        else ungetc(c,in);
    }
    free ((void *) temp);
}

#ifndef _NO_PROTO
void put_complex_float_binary_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_binary_vector(V, out)
ComplexFloatVector V;
FILE *out;
#endif
/****************************************************************
 * outputs a vector of ComplexFloats in binary format into	*
 * file out from vector V. 		                        *
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&V.N),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	 error("fwrite failed to write N in put_complex_float_binary_vector");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (N)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_float_binary_vector!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for N", written_numbers);
            error("Unexpected negative value in put_complex_float_binary_vector");
	}
    }

    numbers_left = V.N;
    start_write = (char *)(&V.el[COLSTART]);

    if((written_numbers = fwrite((char *) start_write, 
			   (int)sizeof(ComplexFloat), numbers_left, out))
		!= numbers_left)
    {
	if(written_numbers == 0) 
		error("fwrite failed in put_complex_float_binary_vector");
        if(written_numbers > numbers_left)
	{
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_complex_float_binary_vector!");
	}
	if(written_numbers > 0)
	{
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += 
			written_numbers * sizeof(ComplexFloat)/sizeof(char);
	}
	if(written_numbers < 0)
	{
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
                error("Unexpected negative value in put_complex_float_binary_vector");
	}
    }
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_element(float real, float imag, FILE *out,
                         char imaginary_char, char complex_sep,
                         char complex_begin, char complex_end)
#else
void put_complex_float_element(real, imag, out, imaginary_char, complex_sep,
                                        complex_begin, complex_end)
float real, imag;
FILE *out;
char imaginary_char, complex_sep, complex_begin, complex_end;
#endif
/* Displays element with real part *real* and imaginary part *imag*     *
 * using imaginary_char, complex_sep, complex_begin, and complex_end as *
 * described below in put_formatted_complex_float_vector.		*/

{
    if (complex_sep=='\0') complex_sep=' ';
    if (complex_begin!='\0') fprintf(out, "%c", complex_begin);
    fprintf(out, "%g", real);
    if (complex_sep=='+' && imag<0)
        fprintf(out, "%g", imag);
    else fprintf(out, "%c%g", complex_sep, imag);
    if (imaginary_char!='\0') fprintf(out, "%c", imaginary_char);
    if (complex_end!='\0') fprintf(out, "%c", complex_end);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_complex_float_vector(ComplexFloatVector V, FILE *out, 
	char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end)
#else
void put_formatted_complex_float_vector(V, out, separator, begin_char, 
	end_char, imaginary_char, complex_sep, complex_begin, complex_end)
ComplexFloatVector V;
FILE *out;
char separator, begin_char, end_char, imaginary_char, complex_sep;
char complex_begin, complex_end;
#endif
/* Displays V on the given file with elements separated by separator    *
 * and with whole vector enclosed by begin_char and end_char.           *

 * The character *imaginary_char* is used as the symbol for the square  *
 * root of -1.  The character *complex_sep* determines how the real     *
 * and imaginary parts of an element are separated:                     *
 *    complex_sep='+' causes the elements to be written as a+bI or a-bI *
 *    complex_sep=' ' causes the elements to be written as a b          *
 *    complex_sep=',' causes the elements to be written as a,b          *
 * The elements are enclosed by complex_begin and complex_end.  For     *
 * example, complex_begin='(', complex_end=')', and complex_sep=','     *
 * causes the elements to be written as (a,b).                          *
 * NOTE: If it is desired to suppress the I's (or J's) in the output,   *
 *    set imaginary_char to null.  e.g., if you want the elements to be *
 * written as (a,b) but set imaginary_char to 'I', then the elements    *
 * will be written as (a,bI).

 * To assign a null value to begin_char, end_char, or imaginary_char,   *
 * call this function with '\0' (the null character) in the appropriate *
 * argument position.                                                   *
 * DO NOT call the function with "" instead of '\0'.                    */

{
    int  i;

    if (separator=='\0') separator=' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<V.N-1; i++){
        put_complex_float_element(V.el[i+COLSTART].real, 
			V.el[i+COLSTART].imaginary, out,imaginary_char, 
			complex_sep, complex_begin, complex_end);
        fprintf(out, "%c", separator);
    }
    put_complex_float_element(V.el[i+COLSTART].real, 
			V.el[i+COLSTART].imaginary, out, imaginary_char, 
			complex_sep, complex_begin, complex_end);
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_vector(V, out)
ComplexFloatVector V; 
FILE *out;
#endif
/* displays V on the given file with spaces as separators and no *
 * begin_char or end_char                                        *
 * 'I' is the imaginary_char and there are no complex_begin or	*
 * complex_end.							*/
 
{
    put_formatted_complex_float_vector(V, out, ' ', '\0', '\n', 'I',
					'+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_HTML_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_HTML_vector(V, out)
ComplexFloatVector V;
FILE *out;
#endif
/* Displays V on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i;

    fprintf(out, "<TABLE BORDER>\n");
    fprintf(out,"<TR ALIGN=RIGHT>\n    ");

    for(i=0; i<V.N; i++){
	fprintf(out, "<TD> ");
        put_complex_float_element(V.el[i+COLSTART].real,
               		           V.el[i+COLSTART].imaginary, out,
				   'I', '+', '\0', '\0');
	fprintf(out, "</TD>");
    }
    fprintf(out, "\n</TR><P>\n");
    fprintf(out,"</TABLE>\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_Mathematica_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_Mathematica_vector(V, out)
ComplexFloatVector V;
FILE *out;
#endif
/* Displays V on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char             *
 * 'I' is the imaginary_char and there are no complex_begin or		*
 * complex_end.								*/

{
    put_formatted_complex_float_vector(V, out, ',', '{', '}', 'I',
					'+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_ordered_pair_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_ordered_pair_vector(V, out)
ComplexFloatVector V;
FILE *out;
#endif
/* Displays V on the given file in ordered pair (a,b) format with commas   *
 * as separators, '{' as the begin_char and '}' as the end_char.	   *
 * There is no imaginary_char, complex_begin is '(', and complex_end is ')'*/

{
    put_formatted_complex_float_vector(V, out, ',', '\0', '\0', '\0',
                                        ',', '(', ')');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_simple_vector(ComplexFloatVector V, FILE *out)
#else
void put_complex_float_simple_vector(V, out)
ComplexFloatVector V;
FILE *out;
#endif
/* displays V on the given file in simple a b format with elements	*
 * separated by spaces.							*/

{
    put_formatted_complex_float_vector(V, out, ' ', '\0', '\0', '\0',
                                        ' ', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexFloatVector *create_default_complex_float_vector()
#else
ComplexFloatVector *create_default_complex_float_vector()
#endif

/* Calls create_vector(0) to allocate block of space for data           *
 * using defaults specified by the definition of INITIAL_DATA_SPACE     */
{
        return create_complex_float_vector(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexFloatVector *create_complex_float_vector(int N)
#else
ComplexFloatVector *create_complex_float_vector(N)
int N;
#endif

/* Allocates space for a vector */
{
   int i;
   ComplexFloatVector *B;

   B = (ComplexFloatVector *) malloc((unsigned) sizeof(ComplexFloatVector));
   if (!B) 
      error("malloc of vector storage failed in create_complex_float_vector");

   B->N = N;

/* Allocate block of space for data */

   if (B->N > 0)
   {
        B->el=(ComplexFloat *) malloc((unsigned) B->N*sizeof(ComplexFloat))-COLSTART;
        B->data_reserved = B->N;
   }
   else
   {
        B->el=(ComplexFloat *)
              malloc((unsigned) INITIAL_DATA_SPACE*sizeof(ComplexFloat))-COLSTART;
        B->data_reserved = INITIAL_DATA_SPACE;
   }

   if (!(B->el)) 
         error("malloc of data space failed in create_complex_float_vector");

   return B;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_complex_float_vector(ComplexFloatVector* B)
#else
void free_complex_float_vector(B)
ComplexFloatVector* B;
#endif

/* frees all pointers associated with the *
 * ComplexFloatVector structure *B       */
{
   free((void*)B->el);
   free((void*)B);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_complex_float_vector(ComplexFloatVector *B, FILE *in)
#else
int get_complex_float_vector(B, in)
ComplexFloatVector *B;
FILE *in;
#endif
/****************************************************************
 * constructs a vector of ComplexFloats from a data source	*
 * which is a stream of numbers.				*
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character for the vector.  The end characters supported  *
 * are carriage returns (the default) and close brackets of all *
 * types (used if the first non--blank character is an open     *
 * bracket of that type.)                                       *
 * This row is stored into a data block V.el, which is          *
 * enlarged whenever it fills up.                               *
 * B->el is set up to look like a C-style vector (list of       *
 *  pointers to vectors), except that if COLSTART               *
 *  isn't zero, the vector starts at B->el[COLSTART] (Numerical *
 *  Recipes perversion from Fortran)                            *
 ****************************************************************/


{
    int  data_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that   *
 * might be between vectors.  Check if file/pipe has ended, return EOF  *
 * without changing vector if so.					*/

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if B->el has been allocated.  If not, allocate      *
 * space.                                                               *
 *                                                                      *
 * WARNING: Declaring the vector does not reserve space, but can start  *
 * with a garbage pointer which will fool our routines.                 *
 * Either set V.el = NULL, or use					*
 *		*V = *create_default_complex_float_vector(0)		*
 * to set up space if you don't know the size of your vector, and	*
 *		create_complex_float_vector(N)     if you do.		*/

    if (B->el==NULL || B->data_reserved <=0) 
				*B = *create_complex_float_vector(0);

    B->N=0;

/* find out first character: number, or open bracket?  Use to determine *
 * character which ends rows of vector                                  */

    end_char = '\n';
    if ((c=skip_white(end_char,in))=='{') end_char = '}';
    else if (c=='[') end_char = ']';
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  When we       *
 * hit an end_char, end the vector and return.                         */

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the vector is too large, double the allocated space.              */

/* Call real_vector or complex_vector, depending on B->type             */

    data_count=0;

    while((c=skip_punctuation(end_char, in)) != end_char && c != EOF)
    {
       ungetc(c,in);
       if (data_count == B->data_reserved)
       {
            B->data_reserved *=2;
            B->el = (ComplexFloat *) 
	     realloc((char*) B->el+COLSTART*sizeof(ComplexFloat)/sizeof(char),
                  (unsigned) B->data_reserved*sizeof(ComplexFloat))-COLSTART;
            if (!B->el) error("realloc of el failed in get_vector");
       }
        get_complex_float_number(end_char, in, 
				  &B->el[data_count+COLSTART].real,
				  &B->el[data_count+COLSTART].imaginary);
        data_count++;
    }
    B->N = data_count;
    return 0;
}

/*===========================================================================*/


/*******************************  MATRICES  *********************************/


#ifndef _NO_PROTO
void put_binary_int_matrix(Matrix M, FILE *out)
#else
void put_binary_int_matrix(M, out)
Matrix M;
FILE *out;
#endif
/****************************************************************
 * outputs a matrix of ints in binary format into file out   *
 * from matrix M. 		                                *
 ****************************************************************/
{
    int i, j, k, count, written_numbers, numbers_left;
    int tbuf[101];
    char *start_write;

    if((written_numbers=fwrite((char *)(&M.rows),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write rows in put_binary_int_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (rows)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_int_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for rows", written_numbers);
            error("Unexpected negative value in put_binary_int_matrix");
	}
    }

    if((written_numbers=fwrite((char *)(&M.cols),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write cols in put_binary_int_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (cols)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_int_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for cols", written_numbers);
            error("Unexpected negative value in put_binary_int_matrix");
	}
    }

    for(i=0;i<M.rows;i++){
        numbers_left = M.cols;
        j = COLSTART;
        while (numbers_left > 0) {
          if (numbers_left > 100)
	     count = 100;
	  else
	     count = numbers_left;
	  for (k=0;k<count;k++) 
	     tbuf[k] = (int) M.el[i+ROWSTART][j++];
          start_write = (char *)(&tbuf[0]);

          if((written_numbers = fwrite((char *) start_write, 
			   (int)sizeof(int), count, out)) != count) {
	     if(written_numbers == 0) 
		  error("fwrite failed in put_binary_int_matrix");
             if(written_numbers != count) {
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, count);
		    error("Too many (or few) numbers written in put_binary_int_matrix!");
	     }
	  }
          numbers_left = numbers_left - count;
	}
    }

    fflush(out);
}

#ifndef _NO_PROTO
void put_binary_matrix(Matrix M, FILE *out)
#else
void put_binary_matrix(M, out)
Matrix M;
FILE *out;
#endif
/****************************************************************
 * outputs a matrix of doubles in binary format into file out   *
 * from matrix M. 		                                *
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&M.rows),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write rows in put_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (rows)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for rows", written_numbers);
            error("Unexpected negative value in put_binary_matrix");
	}
    }

    if((written_numbers=fwrite((char *)(&M.cols),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
		error("fwrite failed to write cols in put_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (cols)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for cols", written_numbers);
            error("Unexpected negative value in put_binary_matrix");
	}
    }

    for(i=0;i<M.rows;i++){
        numbers_left = M.cols;
        start_write = (char *)(M.el[i+ROWSTART]+COLSTART);
        while((written_numbers = fwrite((char *) start_write, 
				(int)sizeof(double), numbers_left, out))
		!= numbers_left)
	{
	    if(written_numbers == 0) 
		error("fwrite failed in put_binary_matrix");
            if(written_numbers > numbers_left)
	    {
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_binary_matrix!");
	    }
	    if(written_numbers > 0)
	    {
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += written_numbers * sizeof(double)/sizeof(char);
	    }
	    if(written_numbers < 0)
	    {
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
		error("Unexpected negative value in put_binary_matrix");
	    }
	}
    }
    fflush(out);
}


/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_matrix(Matrix M, FILE *out, 
			  char separator, char begin_char, char end_char)
#else
void put_formatted_matrix(M, out, separator, begin_char, end_char)
Matrix M;
FILE *out;
char separator, begin_char, end_char;
#endif
/* Displays M on the given file with elements separated by *separator* and  *
 * with each row, and the whole matrix, enclosed by begin_char and          *
 * end_char.  To assign a null value to begin_char or end_char call this    *
 * function with '\0' (the null character) in the appropriate argument      *
 * position. DO NOT call the function with "" instead of '\0'.              */

{
    int  i,j;

    if (separator=='\0') separator = ' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<M.rows; i++){
        if (i>0 && begin_char!='\0') fprintf(out," ");
        if (begin_char!='\0') fprintf(out,"%c",begin_char);
        for(j=0; j<M.cols-1; j++)
            fprintf(out, "%g%c", M.el[i+ROWSTART][j+COLSTART],separator);
        fprintf(out, "%g", M.el[i+ROWSTART][j+COLSTART]);
        if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
        if (i<M.rows-1) fprintf(out,"%c\n",separator);
    }
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_matrix(Matrix M, FILE *out)
#else
void put_matrix(M, out)
Matrix M; 
FILE *out;
#endif
/* displays M on the given file with spaces as separators and no *
 * begin_char or end_char					 */
 
{
    put_formatted_matrix(M,out,' ','\0','\n');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_HTML_matrix(Matrix M, FILE *out) 
#else
void put_HTML_matrix(M, out)
Matrix M;
FILE *out;
#endif
/* Displays M on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i,j;

    fprintf(out, "<TABLE BORDER>\n");

    for(i=0; i<M.rows; i++){
        fprintf(out,"<TR ALIGN=RIGHT>\n    ");
        for(j=0; j<M.cols; j++)
            fprintf(out, "<TD> %g</TD>", M.el[i+ROWSTART][j+COLSTART]);
        fprintf(out, "\n</TR><P>\n");
    }
    fprintf(out,"</TABLE>\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_Mathematica_matrix(Matrix M, FILE *out)
#else
void put_Mathematica_matrix(M, out)
Matrix M;
FILE *out;
#endif
/* displays M on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char		   */

{
    put_formatted_matrix(M,out,',','{','}');
}


/*===========================================================================*/

#ifndef _NO_PROTO
void check_symmetric(Matrix M)
#else
void check_symmetric(M)
Matrix M;
#endif
/* check if matrix M is symmetric */
{
    int i,j;
 
    for(i=0; i<= M.rows; i++){
        for(j=0; j<i; j++)
            if(M.el[i+ROWSTART][j+COLSTART]!=M.el[j+ROWSTART][i+COLSTART])
                error("symmetric: input matrix is not symmetric");
    }
}

/*===========================================================================*/

#ifndef _NO_PROTO
void transpose_square(Matrix M)
#else
void transpose_square(M)
Matrix M;
#endif
/* transpose return M^T in M */
{
    int i, j;
    double tmp;

    if(M.rows!=M.cols) error("Asking for transpose of non--square matrix");    
 
    for(i=0; i<M.rows; i++){
        for(j=0; j<i; j++){
            tmp=M.el[j+ROWSTART][i+COLSTART];
            M.el[j+ROWSTART][i+COLSTART]=M.el[i+ROWSTART][j+COLSTART];
            M.el[i+ROWSTART][j+COLSTART]=tmp;
        }
    }
}

/*===========================================================================*/
Matrix * create_default_matrix()

/* Calls create_matrix(0,0) to allocate block of space for data and          *
 * for pointers to rows using defaults specified by the definitions          *
 * of INITIAL_DATA_SPACE and INITIAL_EL_SPACE                                */
{
        return create_matrix(0,0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
Matrix * create_matrix(int rows, int cols)
#else
Matrix * create_matrix(rows, cols)
int rows;
int cols;
#endif

/* Allocates space for a matrix */
{
   int i;
   Matrix *A;

   A = (Matrix *) malloc((unsigned) sizeof(Matrix));
   if (!A) error("malloc of matrix storage failed in create_matrix");

   A->rows = rows;
   A->cols = cols;

/* Allocate block of space for data */

   if (A->rows * A->cols > 0)
   {
	A->data=(double *) malloc((unsigned) A->rows*A->cols*sizeof(double));
	A->data_reserved = A->rows*A->cols;
   }
   else
   {
	A->data=(double*) malloc((unsigned) INITIAL_DATA_SPACE*sizeof(double));
	A->data_reserved = INITIAL_DATA_SPACE;
   }
	
   if (!(A->data)) error("malloc of data space failed in create_matrix");

/* Allocate space for pointers to rows: set up pointers */

   if (A->rows > 0)
   {
	A->el = (double **) malloc((unsigned)A->rows*sizeof(double*))-ROWSTART;
	A->pointers_reserved = A->rows;
   }
   else
   {
	A->el = (double **) 
		malloc((unsigned)INITIAL_EL_SPACE*sizeof(double*))-ROWSTART;
	A->pointers_reserved = INITIAL_EL_SPACE;
   }

   if (!(A->el)) error("malloc of element pointers failed in create_matrix");

/* Fill in row pointers */

   for(i=0; i<A->rows; i++)
   {
        A->el[i+ROWSTART] = (double *) A->data+i*A->cols-COLSTART;
   }

return A;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_matrix(Matrix* A)
#else
void free_matrix(A)
Matrix* A;
#endif

/* frees all pointers associated with the Matrix structure *A               */
{
   free((void*)A->data);
   free((void*)A->el);
   free((void*)A);
}

/*==========================================================================*/

#ifndef _NO_PROTO
int get_binary_matrix(Matrix *A, FILE *in)
#else
int get_binary_matrix(A, in)
Matrix *A;
FILE *in;
#endif
/****************************************************************
 * reads a matrix of doubles in binary format from in           *
 * into matrix A. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of matrix */

    if( (read_numbers=fread((char *)&(A->rows),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (rows)\n",
		read_numbers);
            error("fread got too many numbers in get_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for rows", read_numbers);
            error("Unexpected negative value in get_binary_matrix");
        }
    }

    if( (read_numbers=fread((char *)&(A->cols),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (cols)\n",
		read_numbers);
            error("fread got too many numbers in get_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for cols", read_numbers);
            error("Unexpected negative value in get_binary_matrix");
        }
    }

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_matrix(0,0) to set up space if you don't 	*
 * know	the size of your matrix, and create_matrix(rows,cols) if you do.	*
 * Alternatively, set M.data = NULL.					*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_matrix(A->rows,A->cols);

/* If needed, expand block of space for data */

    if(A->rows * A->cols > A->data_reserved)
    {
	A->data = (double *) realloc((char*) A->data, 
				(unsigned) A->rows*A->cols*sizeof(double));
	if (!(A->data)) 
		error("realloc of data space failed in get_binary_matrix");
	A->data_reserved = A->rows * A->cols;
    }

/* If needed, expand space for pointers to rows */

    if (A->rows > A->pointers_reserved)
    {
	A->el = (double **) realloc((char*) A->el,
				(unsigned)A->rows*sizeof(double*))-ROWSTART;
        if (!(A->el))
		error("realloc of el pointers failed in get_binary_matrix");
	A->pointers_reserved = A->rows;
    }

/* Set up C-style pointers to rows */

    for(i=0; i<A->rows; i++) 
    {
        A->el[i+ROWSTART] = (double *) A->data+i*A->cols-COLSTART;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (A->rows)*(A->cols);
    start_read = (char *) A->data;
    while((read_numbers = fread(start_read, sizeof(double), numbers_left, in )) 
		!= numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_binary_matrix!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(double) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
            error("Unexpected negative value in get_binary_matrix");
	}
    }
    return(0);
}
#ifndef _NO_PROTO
int get_binary_int_matrix(Matrix *A, FILE *in)
#else
int get_binary_int_matrix(A, in)
Matrix *A;
FILE *in;
#endif
/****************************************************************
 * reads a matrix of ints    in binary format from in           *
 * into matrix A. 		                                *
 ****************************************************************/
{

    int i, j,k,count,read_numbers, numbers_left;
    int tbuf[101];
    char *start_read;

/* Read size of matrix */

    if( (read_numbers=fread((char *)&(A->rows),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (rows)\n",
		read_numbers);
            error("fread got too many numbers in get_binary_int_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for rows", read_numbers);
            error("Unexpected negative value in get_binary_int_matrix");
        }
    }

    if( (read_numbers=fread((char *)&(A->cols),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (cols)\n",
		read_numbers);
            error("fread got too many numbers in get_binary_int_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for cols", read_numbers);
            error("Unexpected negative value in get_binary_int_matrix");
        }
    }

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_matrix(0,0) to set up space if you don't 	*
 * know	the size of your matrix, and create_matrix(rows,cols) if you do.	*
 * Alternatively, set M.data = NULL.					*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_matrix(A->rows,A->cols);

/* If needed, expand block of space for data */

    if(A->rows * A->cols > A->data_reserved)
    {
	A->data = (double *) realloc((char*) A->data, 
				(unsigned) A->rows*A->cols*sizeof(double));
	if (!(A->data)) 
		error("realloc of data space failed in get_binary_int_matrix");
	A->data_reserved = A->rows * A->cols;
    }

/* If needed, expand space for pointers to rows */

    if (A->rows > A->pointers_reserved)
    {
	A->el = (double **) realloc((char*) A->el,
				(unsigned)A->rows*sizeof(double*))-ROWSTART;
        if (!(A->el))
		error("realloc of el pointers failed in get_binary_int_matrix");
	A->pointers_reserved = A->rows;
    }

/* Set up C-style pointers to rows */

    for(i=0; i<A->rows; i++) 
    {
        A->el[i+ROWSTART] = (double *) A->data+i*A->cols-COLSTART;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/


    for (i=0;i<A->rows;i++) {
       numbers_left = A->cols;        
       j = COLSTART;
       while (numbers_left > 0) {
          if (numbers_left > 100)
	     count = 100;
          else
	     count = numbers_left;

          start_read = (char *) &tbuf[0];
          read_numbers = fread(start_read, sizeof(int), count, in);
          if (read_numbers != count) {
	     if(read_numbers == 0) return (EOF);
	     if(read_numbers != count) {
	        fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		   read_numbers, count);
	        error("Too many (or few) numbers read in get_binary_int_matrix!");
	      }
          }
          for (k=0;k<count;k++) 
	     A->el[i+ROWSTART][j++] = (double)tbuf[k];
          numbers_left = numbers_left - count;
       }
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_matrix(Matrix *A, FILE *in)
#else
int get_matrix(A, in)
Matrix *A;
FILE *in;
#endif
/****************************************************************
 * constructs a matrix of doubles from a data source which is a *
 * stream of rows of numbers.                                   *
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character.  The end characters supported are carriage    *
 * returns (the default) and close brackets of all types (used  *
 * if the first non--blank character is an open bracket of that *
 * type.)  This row is stored into a data block A.data, which   *
 * is enlarged whenever it fills up.                            *
 * A->data looks like a Fortran matrix.                         *
 * A->el is set up to look like a C-style matrix (list of       *
 *  pointers to vectors), except that if ROWSTART and COLSTART  *
 *  aren't zero, the matrix starts at A->el[ROWSTART,COLSTART]  *
 ****************************************************************/
 
 
{
    int  data_count,row_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that   *
 * might be between matrices.  Check if file/pipe has ended, return EOF *
 * without changing matrix if so.                                       */

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space.                                                               *
 *                                                                      *
 * WARNING: Declaring the matrix does not initialize the elements       *
 * to zero.  Use *M = *create_default_matrix() to set up space if you   *
 * don't know the size of your matrix, and create_matrix(rows,cols)     *
 * you do.  Alternatively, set M.data = NULL.                           *
 *                                                                      */

    if (A->data==NULL || A->el==NULL ||
                A->data_reserved <=0 || A->pointers_reserved <= 0)
        *A = *create_matrix(0,0);

    A->rows=0;
    A->cols=0;

/* find out first character: number, or open bracket?  Use to determine *
 * character which ends rows of matrix                                  */

    end_char = '\n';
    if((c=skip_white(end_char,in))=='{') end_char = '}';
    else if(c=='[') end_char = ']';
    else if (c == EOF) return(EOF);
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  The first     *
 * time we hit an end_char, store the length.  When we hit two          *
 * end_chars, end the matrix and return.                                */

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the matrix is too large, double the allocated space.              */

    data_count=0;

    while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
    {
        ungetc(c,in);
        while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
        {
           ungetc(c,in);
           if (data_count == A->data_reserved)
           {
            A->data_reserved *=2;
            A->data = (double *) realloc((char*) A->data,
                        (unsigned) A->data_reserved*sizeof(double));
            if (!A->data) error("realloc of data failed in get_matrix");
	   }
           if( fscanf(in,"%lf",&A->data[data_count])==0 )
           {
              fprintf(stderr,
                "Character read as beginning of number ascii %d = char %c\n",
                                (int) c, c);
              fprintf(stderr, "EOF = %d\n", (int) EOF);
              error("get_matrix: non-numeric input");
	   }
           else data_count++;
	}
        if(A->cols==0)  A->cols = data_count;
    }


/* Set up C-style pointers to rows */

    A->rows = data_count / A->cols;
    if (A->rows * A->cols != data_count)
         error("get_matrix: cols in row one doesn't divide # of elements");
    else
    {
         if (A->rows>=A->pointers_reserved)
         {
           A->el = (double **)
              realloc((char*) A->el, (unsigned) A->rows*sizeof(double*));
           if (!A->el) error("realloc of el pointers failed in get_matrix");
           A->pointers_reserved = A->rows;
	 }
         for(row_count=0; row_count<A->rows; row_count++)
              A->el[row_count+ROWSTART] =
                                A->data + A->cols * row_count - COLSTART;
    }
    return 0;
}

/*===========================================================================*/

/***************************** COMPLEX DOUBLE  ******************************/

/*==========================================================================*/

#ifndef _NO_PROTO
int get_complex_double_binary_matrix(ComplexDoubleMatrix *A, FILE *in)
#else
int get_complex_double_binary_matrix(A, in)
ComplexDoubleMatrix *A;
FILE *in;
#endif
/****************************************************************
 * reads a matrix of ComplexDoubles in binary format from in	*
 * into matrix A. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of matrix */

    if( (read_numbers=fread((char *)&(A->rows),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (rows)\n",
		read_numbers);
            error("fread got too many numbers in get_complex_double_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for rows", read_numbers);
            error("Unexpected negative value in get_complex_double_binary_matrix");
        }
    }

    if( (read_numbers=fread((char *)&(A->cols),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (cols)\n",
		read_numbers);
            error("fread got too many numbers in get_complex_double_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for cols", read_numbers);
            error("Unexpected negative value in get_complex_double_binary_matrix");
        }
    }

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_matrix(0,0) to set up space if you don't 	*
 * know	the size of your matrix, and create_matrix(rows,cols) if you do.	*
 * Alternatively, set M.data = NULL.					*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_complex_double_matrix(A->rows,A->cols);

/* If needed, expand block of space for data */

    if(A->rows * A->cols > A->data_reserved)
    {
	A->data = (ComplexDouble *) realloc((char*) A->data, 
			(unsigned) A->rows*A->cols*sizeof(ComplexDouble));
	if (!(A->data)) 
	    error("realloc of data space failed in get_complex_double_binary_matrix");
	A->data_reserved = A->rows * A->cols;
    }

/* If needed, expand space for pointers to rows */

    if (A->rows > A->pointers_reserved)
    {
	A->el = (ComplexDouble **) realloc((char*) A->el,
			(unsigned)A->rows*sizeof(ComplexDouble*))-ROWSTART;
        if (!(A->el))
	    error("realloc of el pointers failed in get_complex_double_binary_matrix");
	A->pointers_reserved = A->rows;
    }

/* Set up C-style pointers to rows */

    for(i=0; i<A->rows; i++) 
    {
        A->el[i+ROWSTART] = (ComplexDouble *) A->data+i*A->cols-COLSTART;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (A->rows)*(A->cols);
    start_read = (char *) A->data;
    while((read_numbers = fread(start_read, sizeof(ComplexDouble), 
				numbers_left, in )) != numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_complex_double_binary_matrix!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(ComplexDouble) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
            error("Unexpected negative value in get_complex_double_binary_matrix");
	}
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_binary_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_binary_matrix(M, out)
ComplexDoubleMatrix M;
FILE *out;
#endif
/****************************************************************
 * outputs a matrix of ComplexDoubles in binary format into	*
 * file out from matrix M.					*
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&M.rows),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	    error("fwrite failed to write rows in put_complex_double_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (rows)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_double_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for rows", written_numbers);
            error("Unexpected negative value in put_complex_double_binary_matrix");
	}
    }

    if((written_numbers=fwrite((char *)(&M.cols),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	    error("fwrite failed to write cols in put_complex_double_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (cols)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_double_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for cols", written_numbers);
            error("Unexpected negative value in put_complex_double_binary_matrix");
	}
    }

    for(i=0;i<M.rows;i++){
        numbers_left = M.cols;
        start_write = (char *) (M.el[i+ROWSTART]+COLSTART);
        while((written_numbers = fwrite((char *) start_write, 
	      (int)sizeof(ComplexDouble), numbers_left, out)) != numbers_left)
	{
	    if(written_numbers == 0) 
		error("fwrite failed in put_complex_double_binary_matrix");
            if(written_numbers > numbers_left)
	    {
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_complex_double_binary_matrix!");
	    }
	    if(written_numbers > 0)
	    {
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += 
			written_numbers * sizeof(ComplexDouble)/sizeof(char);
	    }
	    if(written_numbers < 0)
	    {
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
		error("Unexpected negative value in put_complex_double_binary_matrix");
	    }
	}
    }
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_complex_double_matrix(ComplexDoubleMatrix M, FILE *out,
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end)
#else
void put_formatted_complex_double_matrix(M, out, separator, begin_char,
        end_char, imaginary_char, complex_sep, complex_begin, complex_end)
ComplexDoubleMatrix M;
FILE *out;
char separator, begin_char, end_char, imaginary_char, complex_sep;
char complex_begin, complex_end;
#endif
/* Displays M on the given file with elements separated by separator    *
 * and with rows and whole matrix enclosed by begin_char and end_char.	*

 * The character *imaginary_char* is used as the symbol for the square  *
 * root of -1.  The character *complex_sep* determines how the real     *
 * and imaginary parts of an element are separated:                     *
 *    complex_sep='+' causes the elements to be written as a+bI or a-bI *
 *    complex_sep=' ' causes the elements to be written as a b          *
 *    complex_sep=',' causes the elements to be written as a,b          *
 * The elements are enclosed by complex_begin and complex_end.  For     *
 * example, complex_begin='(', complex_end=')', and complex_sep=','     *
 * causes the elements to be written as (a,b).                          *
 * NOTE: If it is desired to suppress the I's (or J's) in the output,   *
 *    set imaginary_char to null.  e.g., if you want the elements to be *
 * written as (a,b) but set imaginary_char to 'I', then the elements    *
 * will be written as (a,bI).

 * To assign a null value to begin_char, end_char, or imaginary_char,   *
 * call this function with '\0' (the null character) in the appropriate *
 * argument position.                                                   *
 * DO NOT call the function with "" instead of '\0'.                    */

{
    int  i, j;

    if (separator=='\0') separator=' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<M.rows; i++){
        if (i>0 && begin_char!='\0') fprintf(out," ");
        if (begin_char!='\0') fprintf(out,"%c",begin_char);
        for(j=0; j<M.cols-1; j++){
            put_complex_double_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
		imaginary_char, complex_sep, complex_begin, complex_end);
	    fprintf(out, "%c", separator);
	}
        put_complex_double_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
                imaginary_char, complex_sep, complex_begin, complex_end);
        if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
        if (i<M.rows-1) fprintf(out,"%c\n",separator);
    }
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n\n");
    fflush(out);
}


/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_matrix(M, out)
ComplexDoubleMatrix M; 
FILE *out;
#endif
/* displays M on the given file with spaces as separators and no *
 * begin_char or end_char                                        *
 * 'I' is the imaginary_char and there are no complex_begin or  *
 * complex_end.                                                 */
 
{
    put_formatted_complex_double_matrix(M, out, ' ', '\0', '\n', 'I',
                                        '+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_HTML_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_HTML_matrix(M, out)
ComplexDoubleMatrix M;
FILE *out;
#endif
/* Displays M on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i, j;

    fprintf(out, "<TABLE BORDER>\n");

    for(i=0; i<M.rows; i++){
	fprintf(out,"<TR ALIGN=RIGHT>\n    ");
        for(j=0; j<M.cols; j++){
	    fprintf(out, "<TD> ");
            put_complex_double_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
		'I', '+', '\0', '\0');
	    fprintf(out, "</TD>");
	}
        fprintf(out, "\n</TR><P>\n");
    }
    fprintf(out,"</TABLE>\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_Mathematica_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_Mathematica_matrix(M, out)
ComplexDoubleMatrix M;
FILE *out;
#endif
/* Displays M on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char             *
 * 'I' is the imaginary_char and there are no complex_begin or          *
 * complex_end.                                                         */

{
    put_formatted_complex_double_matrix(M, out, ',', '{', '}', 'I',
                                        '+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_ordered_pair_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_ordered_pair_matrix(M, out)
ComplexDoubleMatrix M;
FILE *out;
#endif
/* Displays M on the given file in ordered pair (a,b) format with commas   *
 * as separators, '{' as the begin_char and '}' as the end_char.           *
 * There is no imaginary_char, complex_begin is '(', and complex_end is ')'*/

{
    put_formatted_complex_double_matrix(M, out, ',', '\0', '\0', '\0',
                                        ',', '(', ')');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_double_simple_matrix(ComplexDoubleMatrix M, FILE *out)
#else
void put_complex_double_simple_matrix(M, out)
ComplexDoubleMatrix M;
FILE *out;
#endif
/* displays M on the given file in simple a b format with elements      *
 * separated by spaces.                                                 */

{
    put_formatted_complex_double_matrix(M, out, ' ', '\0', '\0', '\0',
                                        ' ', '\0', '\0');
}

/*===========================================================================*/
ComplexDoubleMatrix * create_default_complex_double_matrix()

/* Calls create_complex_double_matrix(0,0) to allocate block of space for    *
 * data and for pointers to rows using defaults specified by the definitions *
 * of INITIAL_DATA_SPACE and INITIAL_EL_SPACE                                */
{
        return create_complex_double_matrix(0,0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexDoubleMatrix * create_complex_double_matrix(int rows, int cols)
#else
ComplexDoubleMatrix * create_complex_double_matrix(rows, cols)
int rows;
int cols;
#endif

/* Allocates space for a matrix */
{
   int i;
   ComplexDoubleMatrix *A;

   A = (ComplexDoubleMatrix *) malloc((unsigned) sizeof(ComplexDoubleMatrix));
   if (!A) 
      error("malloc of matrix storage failed in create_complex_double_matrix");

   A->rows = rows;
   A->cols = cols;

/* Allocate block of space for data */

   if (A->rows * A->cols > 0)
   {
	A->data=(ComplexDouble *) 
		malloc((unsigned) A->rows*A->cols*sizeof(ComplexDouble));
	A->data_reserved = A->rows*A->cols;
   }
   else
   {
	A->data=(ComplexDouble*) 
		malloc((unsigned) INITIAL_DATA_SPACE*sizeof(ComplexDouble));
	A->data_reserved = INITIAL_DATA_SPACE;
   }
	
   if (!(A->data)) 
	error("malloc of data space failed in create_complex_double_matrix");

/* Allocate space for pointers to rows: set up pointers */

   if (A->rows > 0)
   {
	A->el = (ComplexDouble **) 
		malloc((unsigned)A->rows*sizeof(ComplexDouble*))-ROWSTART;
	A->pointers_reserved = A->rows;
   }
   else
   {
	A->el = (ComplexDouble **) 
		malloc((unsigned)INITIAL_EL_SPACE*sizeof(ComplexDouble*))
			-ROWSTART;
	A->pointers_reserved = INITIAL_EL_SPACE;
   }

   if (!(A->el)) 
    error("malloc of element pointers failed in create_complex_double_matrix");

/* Fill in row pointers */

   for(i=0; i<A->rows; i++)
   {
        A->el[i+ROWSTART] = (ComplexDouble *) A->data+i*A->cols-COLSTART;
   }

return A;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_complex_double_matrix(ComplexDoubleMatrix* A)
#else
void free_complex_double_matrix(A)
ComplexDoubleMatrix* A;
#endif

/* frees all pointers associated with the ComplexDoubleMatrix structure *A  */
{
   free((void*)A->data);
   free((void*)A->el);
   free((void*)A);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_complex_double_matrix(ComplexDoubleMatrix *A, FILE *in)
#else
int get_complex_double_matrix(A, in)
ComplexDoubleMatrix *A;
FILE *in;
#endif
/****************************************************************
 * constructs a matrix of ComplexDoubles from a data source	*
 * which is a stream of rows of numbers.			*
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character.  The end characters supported are carriage 	*
 * returns (the default) and close brackets of all types (used	*
 * if the first non--blank character is an open bracket of that *
 * type.)  This row is stored into a data block A.data, which   *
 * is enlarged whenever it fills up.                            *
 * A->data looks like a Fortran matrix.                         *
 * A->el is set up to look like a C-style matrix (list of       *
 *  pointers to vectors), except that if ROWSTART and COLSTART  *
 *  aren't zero, the matrix starts at A->el[ROWSTART,COLSTART]  *
 ****************************************************************/
 
 
{
    int  data_count,row_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that	*
 * might be between matrices.  Check if file/pipe has ended, return EOF *
 * without changing matrix if so.					*/

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_default_matrix() to set up space if you  	*
 * don't know the size of your matrix, and create_matrix(rows,cols) 	*
 * you do.  Alternatively, set M.data = NULL.				*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_complex_double_matrix(0,0);

    A->rows=0;
    A->cols=0;

/* find out first character: number, or open bracket?  Use to determine	*
 * character which ends rows of matrix 					*/

    end_char = '\n';
    if((c=skip_white(end_char,in))=='{') end_char = '}';
    else if(c=='[') end_char = ']';
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  The first     *
 * time we hit an end_char, store the length.  When we hit two          *
 * end_chars, end the matrix and return.                                */

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the matrix is too large, double the allocated space.              */

    data_count=0;

    while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
    {
        ungetc(c,in);
        while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
        {
           ungetc(c,in);
           if (data_count == A->data_reserved) 
           {
            A->data_reserved *=2;
            A->data = (ComplexDouble *) realloc((char*) A->data, 
			(unsigned) A->data_reserved*sizeof(ComplexDouble));
	    if (!A->data) 
		error("realloc of data failed in get_complex_double_matrix");
	   }
	get_complex_double_number(end_char, in,
                                  &A->data[data_count].real,
                                  &A->data[data_count].imaginary);
        data_count++;
	}
	if(A->cols==0) A->cols = data_count;
    }

/* Set up C-style pointers to rows */

    A->rows = data_count / A->cols;
    if (A->rows * A->cols != data_count) 
       error("get_complex_double_matrix: cols in row one doesn't divide # of elements");
    else
    {
         if (A->rows>=A->pointers_reserved)
	 {
           A->el = (ComplexDouble **)
             realloc((char*) A->el, (unsigned) A->rows*sizeof(ComplexDouble*));
	   if (!A->el) 
	     error("realloc of el pointers failed in get_complex_double_matrix");
	   A->pointers_reserved = A->rows;
	 }
         for(row_count=0; row_count<A->rows; row_count++)
              A->el[row_count+ROWSTART] = 
				A->data + A->cols * row_count - COLSTART;
    }
    return 0;
} 

/*==========================================================================*/

/************************** COMPLEX FLOAT (SINGLE) **************************/

/*==========================================================================*/

#ifndef _NO_PROTO
int get_complex_float_binary_matrix(ComplexFloatMatrix *A, FILE *in)
#else
int get_complex_float_binary_matrix(A, in)
ComplexFloatMatrix *A;
FILE *in;
#endif
/****************************************************************
 * reads a matrix of ComplexFloats in binary format from in	*
 * into matrix A. 		                                *
 ****************************************************************/

/* fread is ANSI, so will be more portable than read.  The error 	*
 * checking routines are probably not necessary and haven't been 	*
 * thoroughly debugged, because fread so far has not failed us.		*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/


{

    int i, read_numbers, numbers_left;
    char *start_read;

/* Read size of matrix */

    if( (read_numbers=fread((char *)&(A->rows),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (rows)\n",
		read_numbers);
            error("fread got too many numbers in get_complex_float_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for rows", read_numbers);
            error("Unexpected negative value in get_complex_float_binary_matrix");
        }
    }

    if( (read_numbers=fread((char *)&(A->cols),(int)sizeof(int),1,in)) != 1) 
    {
	if(read_numbers == 0) return (EOF);
        if(read_numbers > 1)
        {
            fprintf(stderr,"%d numbers read; expected only one (cols)\n",
		read_numbers);
            error("fread got too many numbers in get_complex_float_binary_matrix!");
        }
        if(read_numbers < 0)
        {
            fprintf(stderr, "fread returned %d for cols", read_numbers);
            error("Unexpected negative value in get_complex_float_binary_matrix");
        }
    }

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_matrix(0,0) to set up space if you don't 	*
 * know	the size of your matrix, and create_matrix(rows,cols) if you do.	*
 * Alternatively, set M.data = NULL.					*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_complex_float_matrix(A->rows,A->cols);

/* If needed, expand block of space for data */

    if(A->rows * A->cols > A->data_reserved)
    {
	A->data = (ComplexFloat *) realloc((char*) A->data, 
			(unsigned) A->rows*A->cols*sizeof(ComplexFloat));
	if (!(A->data)) 
	    error("realloc of data space failed in get_complex_float_binary_matrix");
	A->data_reserved = A->rows * A->cols;
    }

/* If needed, expand space for pointers to rows */

    if (A->rows > A->pointers_reserved)
    {
	A->el = (ComplexFloat **) realloc((char*) A->el,
			(unsigned)A->rows*sizeof(ComplexFloat*))-ROWSTART;
        if (!(A->el))
	    error("realloc of el pointers failed in get_complex_float_binary_matrix");
	A->pointers_reserved = A->rows;
    }

/* Set up C-style pointers to rows */

    for(i=0; i<A->rows; i++) 
    {
        A->el[i+ROWSTART] = (ComplexFloat *) A->data+i*A->cols-COLSTART;
    }

/* Read in data.  According to the manual, if fread returns zero,	*
 * usually we've hit the end-of-file or the pipe has been closed:	*
 * return EOF.  If fread returns a positive number, it is the number	*
 * of items read.  If this is less than the number needed, we print a 	*
 * message to stderr and try some more.  				*
 * We don't know of conditions which fread would return negative	*
 * numbers or more than bytes_left, so we write errors and stop.	*/

    numbers_left = (A->rows)*(A->cols);
    start_read = (char *) A->data;
    while((read_numbers = fread(start_read, sizeof(ComplexFloat), 
				numbers_left, in )) != numbers_left)
    {
	if(read_numbers == 0) return (EOF);
	if(read_numbers > numbers_left) 
	{
	    fprintf(stderr,"Numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    error("Too many numbers read in get_complex_float_binary_matrix!");
	}
	if(read_numbers > 0) 
	{
	    fprintf(stderr,"Try more: numbers read = %d, numbers left = %d\n", 
		read_numbers, numbers_left);
	    numbers_left -= read_numbers;
	    start_read += read_numbers * sizeof(ComplexFloat) / sizeof(char);
	}
	if(read_numbers < 0) 
	{
	    fprintf(stderr, "fread returned %d numbers read", read_numbers);
            error("Unexpected negative value in get_complex_float_binary_matrix");
	}
    }
    return(0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_binary_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_binary_matrix(M, out)
ComplexFloatMatrix M;
FILE *out;
#endif
/****************************************************************
 * outputs a matrix of ComplexFloats in binary format into file	*
 * out from matrix M. 		                                *
 ****************************************************************/

/* fwrite is buffered, so it can be much faster than write, which calls	*
 * a system routine for each row.  It's also ANSI, so will be more 	*
 * portable.  The error checking routines are probably not necessary	*
 * and haven't been thoroughly debugged, because fwrite so far has not	*
 * failed us.								*/

/* I passed large, binary files through several pipes, using the 	*
 * fread and fwrite routines, and compared them with routines written	*
 * with read and write.  Rough averages follow:				*
 *									*
 *		     fwrite			   write		*
 *		  CPU	  Elapsed 	  	CPU	Elapsed		*
 *									*
 * Varied	 0.42	    9.0			0.31	 13.4	 	*
 *									*
 * Long, Thin	 0.62	    7.4			0.62	 39.8		*
 *									*
 * Fat		 0.28	    5.7			0.17	  8.6		*
 *									*
 * Although write is a bit faster on CPU time, the system calls kill 	*
 * it.  A comparison of routines differing only in the read command	*
 * showed no important difference for varied matrices, and the		*
 * fread command was a lot easier to use.				*/

{
    int i, written_numbers, numbers_left;
    char *start_write;

    if((written_numbers=fwrite((char *)(&M.rows),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	    error("fwrite failed to write rows in put_complex_float_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (rows)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_float_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for rows", written_numbers);
            error("Unexpected negative value in put_complex_float_binary_matrix");
	}
    }

    if((written_numbers=fwrite((char *)(&M.cols),(int)sizeof(int),1,out))!= 1) 
    {
        if(written_numbers == 0) 
	    error("fwrite failed to write cols in put_complex_float_binary_matrix");
        if(written_numbers > 1)
	{
            fprintf(stderr,"%d numbers written; expected only one (cols)\n",
		written_numbers);
            error("fwrite wrote too many numbers in put_complex_float_binary_matrix!");
	}
        if(written_numbers < 0)
	{
            fprintf(stderr, "fwrite returned %d for cols", written_numbers);
            error("Unexpected negative value in put_complex_float_binary_matrix");
	}
    }

    for(i=0;i<M.rows;i++){
        numbers_left = M.cols;
        start_write = (char *) (M.el[i+ROWSTART]+COLSTART);
        while((written_numbers = fwrite((char *) start_write, 
	      (int)sizeof(ComplexFloat), numbers_left, out)) != numbers_left)
	{
	    if(written_numbers == 0) 
		error("fwrite failed in put_complex_float_binary_matrix");
            if(written_numbers > numbers_left)
	    {
		fprintf(stderr,"Numbers written = %d, numbers left = %d\n",
		    written_numbers, numbers_left);
		    error("Too many numbers written in put_complex_float_binary_matrix!");
	    }
	    if(written_numbers > 0)
	    {
		fprintf(stderr,
			"Try more: numbers written = %d, numbers left = %d\n",
			   written_numbers, numbers_left);
		numbers_left -= written_numbers;
		start_write += 
			written_numbers * sizeof(ComplexFloat)/sizeof(char);
	    }
	    if(written_numbers < 0)
	    {
		fprintf(stderr, 
			"fwrite returned %d numbers written", written_numbers);
		error("Unexpected negative value in put_complex_float_binary_matrix");
	    }
	}
    }
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_formatted_complex_float_matrix(ComplexFloatMatrix M, FILE *out,
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end)
#else
void put_formatted_complex_float_matrix(M, out, separator, begin_char,
        end_char, imaginary_char, complex_sep, complex_begin, complex_end)
ComplexFloatMatrix M;
FILE *out;
char separator, begin_char, end_char, imaginary_char, complex_sep;
char complex_begin, complex_end;
#endif
/* Displays M on the given file with elements separated by separator    *
 * and with rows and whole matrix enclosed by begin_char and end_char.	*

 * The character *imaginary_char* is used as the symbol for the square  *
 * root of -1.  The character *complex_sep* determines how the real     *
 * and imaginary parts of an element are separated:                     *
 *    complex_sep='+' causes the elements to be written as a+bI or a-bI *
 *    complex_sep=' ' causes the elements to be written as a b          *
 *    complex_sep=',' causes the elements to be written as a,b          *
 * The elements are enclosed by complex_begin and complex_end.  For     *
 * example, complex_begin='(', complex_end=')', and complex_sep=','     *
 * causes the elements to be written as (a,b).                          *
 * NOTE: If it is desired to suppress the I's (or J's) in the output,   *
 *    set imaginary_char to null.  e.g., if you want the elements to be *
 * written as (a,b) but set imaginary_char to 'I', then the elements    *
 * will be written as (a,bI).

 * To assign a null value to begin_char, end_char, or imaginary_char,   *
 * call this function with '\0' (the null character) in the appropriate *
 * argument position.                                                   *
 * DO NOT call the function with "" instead of '\0'.                    */

{
    int  i, j;

    if (separator=='\0') separator=' ';
    if (begin_char!='\0') fprintf(out,"%c",begin_char);
    for(i=0; i<M.rows; i++){
        if (i>0 && begin_char!='\0') fprintf(out," ");
        if (begin_char!='\0') fprintf(out,"%c",begin_char);
        for(j=0; j<M.cols-1; j++){
            put_complex_float_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
		imaginary_char, complex_sep, complex_begin, complex_end);
	    fprintf(out, "%c", separator);
	}
        put_complex_float_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
                imaginary_char, complex_sep, complex_begin, complex_end);
        if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
        if (i<M.rows-1) fprintf(out,"%c\n",separator);
    }
    if (end_char!='\0' && end_char!='\n') fprintf(out,"%c",end_char);
    fprintf(out,"\n\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_matrix(M, out)
ComplexFloatMatrix M; 
FILE *out;
#endif
/* displays M on the given file with spaces as separators and no *
 * begin_char or end_char                                        *
 * 'I' is the imaginary_char and there are no complex_begin or  *
 * complex_end.                                                 */
 
{
    put_formatted_complex_float_matrix(M, out, ' ', '\0', '\n', 'I',
                                        '+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_HTML_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_HTML_matrix(M, out)
ComplexFloatMatrix M;
FILE *out;
#endif
/* Displays M on the given file in HTML Table format (PROPOSED HTML3.0 SPEC)*/

{
    int  i, j;

    fprintf(out, "<TABLE BORDER>\n");

    for(i=0; i<M.rows; i++){
	fprintf(out,"<TR ALIGN=RIGHT>\n    ");
        for(j=0; j<M.cols; j++){
	    fprintf(out, "<TD> ");
            put_complex_float_element(M.el[i+ROWSTART][j+COLSTART].real,
                        M.el[i+ROWSTART][j+COLSTART].imaginary, out,
		'I', '+', '\0', '\0');
	    fprintf(out, "</TD>");
	}
        fprintf(out, "\n</TR><P>\n");
    }
    fprintf(out,"</TABLE>\n");
    fflush(out);
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_Mathematica_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_Mathematica_matrix(M, out)
ComplexFloatMatrix M;
FILE *out;
#endif
/* Displays M on the given file in standard Mathematica format with commas *
 * as separators '{' as the begin_char and '}' as the end_char             *
 * 'I' is the imaginary_char and there are no complex_begin or          *
 * complex_end.                                                         */

{
    put_formatted_complex_float_matrix(M, out, ',', '{', '}', 'I',
                                        '+', '\0', '\0');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_ordered_pair_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_ordered_pair_matrix(M, out)
ComplexFloatMatrix M;
FILE *out;
#endif
/* Displays M on the given file in ordered pair (a,b) format with commas   *
 * as separators, '{' as the begin_char and '}' as the end_char.           *
 * There is no imaginary_char, complex_begin is '(', and complex_end is ')'*/

{
    put_formatted_complex_float_matrix(M, out, ',', '\0', '\0', '\0',
                                        ',', '(', ')');
}

/*===========================================================================*/

#ifndef _NO_PROTO
void put_complex_float_simple_matrix(ComplexFloatMatrix M, FILE *out)
#else
void put_complex_float_simple_matrix(M, out)
ComplexFloatMatrix M;
FILE *out;
#endif
/* displays M on the given file in simple a b format with elements      *
 * separated by spaces.                                                 */

{
    put_formatted_complex_float_matrix(M, out, ' ', '\0', '\0', '\0',
                                        ' ', '\0', '\0');
}

/*===========================================================================*/
ComplexFloatMatrix * create_default_complex_float_matrix()

/* Calls create_complex_float_matrix(0,0) to allocate block of space for    *
 * data and for pointers to rows using defaults specified by the definitions *
 * of INITIAL_DATA_SPACE and INITIAL_EL_SPACE                                */
{
        return create_complex_float_matrix(0,0);
}

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexFloatMatrix * create_complex_float_matrix(int rows, int cols)
#else
ComplexFloatMatrix * create_complex_float_matrix(rows, cols)
int rows;
int cols;
#endif

/* Allocates space for a matrix */
{
   int i;
   ComplexFloatMatrix *A;

   A = (ComplexFloatMatrix *) malloc((unsigned) sizeof(ComplexFloatMatrix));
   if (!A) 
      error("malloc of matrix storage failed in create_complex_float_matrix");

   A->rows = rows;
   A->cols = cols;

/* Allocate block of space for data */

   if (A->rows * A->cols > 0)
   {
	A->data=(ComplexFloat *) 
		malloc((unsigned) A->rows*A->cols*sizeof(ComplexFloat));
	A->data_reserved = A->rows*A->cols;
   }
   else
   {
	A->data=(ComplexFloat*) 
		malloc((unsigned) INITIAL_DATA_SPACE*sizeof(ComplexFloat));
	A->data_reserved = INITIAL_DATA_SPACE;
   }
	
   if (!(A->data)) 
	error("malloc of data space failed in create_complex_float_matrix");

/* Allocate space for pointers to rows: set up pointers */

   if (A->rows > 0)
   {
	A->el = (ComplexFloat **) 
		malloc((unsigned)A->rows*sizeof(ComplexFloat*))-ROWSTART;
	A->pointers_reserved = A->rows;
   }
   else
   {
	A->el = (ComplexFloat **) 
		malloc((unsigned)INITIAL_EL_SPACE*sizeof(ComplexFloat*))
			-ROWSTART;
	A->pointers_reserved = INITIAL_EL_SPACE;
   }

   if (!(A->el)) 
    error("malloc of element pointers failed in create_complex_float_matrix");

/* Fill in row pointers */

   for(i=0; i<A->rows; i++)
   {
        A->el[i+ROWSTART] = (ComplexFloat *) A->data+i*A->cols-COLSTART;
   }

return A;
}

/*==========================================================================*/

#ifndef _NO_PROTO
void free_complex_float_matrix(ComplexFloatMatrix* A)
#else
void free_complex_float_matrix(A)
ComplexFloatMatrix* A;
#endif

/* frees all pointers associated with the ComplexFloatMatrix structure *A  */
{
   free((void*)A->data);
   free((void*)A->el);
   free((void*)A);
}

/*===========================================================================*/

#ifndef _NO_PROTO
int get_complex_float_matrix(ComplexFloatMatrix *A, FILE *in)
#else
int get_complex_float_matrix(A, in)
ComplexFloatMatrix *A;
FILE *in;
#endif
/****************************************************************
 * constructs a matrix of ComplexFloats from a data source	*
 * which is a stream of rows of numbers.			*
 * First preceding blank lines are removed until a row          *
 * of data is encountered.                                      *
 * Then the first character is examined to determine the        *
 * end character.  The end characters supported are carriage 	*
 * returns (the default) and close brackets of all types (used	*
 * if the first non--blank character is an open bracket of that *
 * type.)  This row is stored into a data block A.data, which   *
 * is enlarged whenever it fills up.                            *
 * A->data looks like a Fortran matrix.                         *
 * A->el is set up to look like a C-style matrix (list of       *
 *  pointers to vectors), except that if ROWSTART and COLSTART  *
 *  aren't zero, the matrix starts at A->el[ROWSTART,COLSTART]  *
 ****************************************************************/
 
 
{
    int  data_count,row_count;
    int c, end_char;

/* Skip blank lines, comments at the beginning.  Also skip stuff that	*
 * might be between matrices.  Check if file/pipe has ended, return EOF *
 * without changing matrix if so.					*/

    if(skip_comments(in)==EOF) return(EOF);

/* Try to determine if A->data has been allocated.  If not, allocate    *
 * space. 	 							*
 *									*
 * WARNING: Declaring the matrix does not initialize the elements 	*
 * to zero.  Use *M = *create_default_matrix() to set up space if you  	*
 * don't know the size of your matrix, and create_matrix(rows,cols) 	*
 * you do.  Alternatively, set M.data = NULL.				*
 *									*/

    if (A->data==NULL || A->el==NULL || 
		A->data_reserved <=0 || A->pointers_reserved <= 0) 
	*A = *create_complex_float_matrix(0,0);

    A->rows=0;
    A->cols=0;

/* find out first character: number, or open bracket?  Use to determine	*
 * character which ends rows of matrix 					*/

    end_char = '\n';
    if((c=skip_white(end_char,in))=='{') end_char = '}';
    else if(c=='[') end_char = ']';
    else ungetc(c,in);

/* Read numbers one at a time, counting with data_count.  The first     *
 * time we hit an end_char, store the length.  When we hit two          *
 * end_chars, end the matrix and return.                                */

/* Skip commas, semicolons, colons, and white spaces between numbers.   *
 * Skip open brackets.                                                  *
 * If the matrix is too large, double the allocated space.              */

    data_count=0;

    while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
    {
        ungetc(c,in);
        while(((c=skip_punctuation(end_char, in)) != end_char) && (c != EOF))
        {
           ungetc(c,in);
           if (data_count == A->data_reserved) 
           {
            A->data_reserved *=2;
            A->data = (ComplexFloat *) realloc((char*) A->data, 
			(unsigned) A->data_reserved*sizeof(ComplexFloat));
	    if (!A->data) 
		error("realloc of data failed in get_complex_float_matrix");
	   }
	get_complex_float_number(end_char, in,
                                  &A->data[data_count].real,
                                  &A->data[data_count].imaginary);
        data_count++;
	}
	if(A->cols==0) A->cols = data_count;
    }

/* Set up C-style pointers to rows */

    A->rows = data_count / A->cols;
    if (A->rows * A->cols != data_count) 
       error("get_complex_float_matrix: cols in row one doesn't divide # of elements");
    else
    {
         if (A->rows>=A->pointers_reserved)
	 {
           A->el = (ComplexFloat **)
             realloc((char*) A->el, (unsigned) A->rows*sizeof(ComplexFloat*));
	   if (!A->el) 
	     error("realloc of el pointers failed in get_complex_float_matrix");
	   A->pointers_reserved = A->rows;
	 }
         for(row_count=0; row_count<A->rows; row_count++)
              A->el[row_count+ROWSTART] = 
				A->data + A->cols * row_count - COLSTART;
    }
    return 0;
} 

/*===========================================================================*/

/***********************  COMPLEX NUMBER ROUTINES  ***************************/
/****************************  COMPLEX DOUBLE  *******************************/

/*===========================================================================*/

#ifndef _NO_PROTO
ComplexDouble polar(double modulus, double argument)
#else
ComplexDouble polar(modulus, argument)
double modulus, argument;
#endif
/* Takes as arguments the polar form of a complex number and returns	*
 * the corresponding ComplexDouble.					*/
{
    ComplexDouble z;

    z.real = modulus * cos(argument);
    z.imaginary = modulus * sin(argument);
    return z;
}

/*===========================================================================*/

#ifndef _NO_PROTO
double mod(ComplexDouble z)
#else
double mod(z)
ComplexDouble z;
#endif
/* Gives the modulus or absolute value of the complex number z.		*/
{
    return sqrt((z.real)*(z.real) + (z.imaginary)*(z.imaginary));
}

/*===========================================================================*/

#ifndef _NO_PROTO
double arg(ComplexDouble z)
#else
double arg(z)
ComplexDouble z;
#endif
/* Gives the arg or phase of the complex number z.			*/
{
    if (z.real != 0) {
	if (z.real>0) return atan(z.imaginary/z.real);
	else return (acos(-1) + atan(z.imaginary/z.real));
    }
    else if (z.imaginary > 0) return asin(1);
    else return asin(-1);
}

/*===========================================================================*/
