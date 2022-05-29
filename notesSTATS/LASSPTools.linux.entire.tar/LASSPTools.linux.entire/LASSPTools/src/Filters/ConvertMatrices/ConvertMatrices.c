
/*----------------------------------------------------------------------*/
/* ConvertMatrix.c
 * This program uses the various get_matrix and put_matrix routines to	*
 * convert ascii to and from binary, and files to and from pipes	*/

#include <stdio.h>
#include "math_io.h"

#ifndef _NO_PROTO
void main(int argc, char **argv)
#else
void main(argc, argv)
int argc;
char **argv;
#endif
{
   int i, binary_in, binary_out;
   FILE *in, *out;
   Matrix M;

/* Defaults */
   in = stdin;
   out = stdout;
   binary_in = 0;
   binary_out = 0;

/* provide online help */
    if (argc > 1) 
    {
       if (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-help") == 0) help();
    }

/* read arguments and compute parameters */

    for(i = 1; i < argc; i++)
    {
        /* binary input */
        if(strcmp(argv[i],"-binary_in") == 0){
                binary_in = 1;
		continue;
        }
        /* ascii input (default) */
        if(strcmp(argv[i],"-ascii_in") == 0){
                binary_in = 0;
		continue;
        }
        /* binary output */
        if(strcmp(argv[i],"-binary_out") == 0){
                binary_out = 1;
		continue;
        }
        /* ascii input */
        if(strcmp(argv[i],"-ascii_out") == 0){
                binary_out = 0;
		continue;
		}
        if(strcmp(argv[i],"-file_in") == 0){
		in = fopen(argv[++i], "r");
		if (in==NULL) 
		   fprintf(stderr,"bad input file %s\n", argv[i]);
		continue;
		}
        if(strcmp(argv[i],"-file_out") == 0){
		out = fopen(argv[++i], "w");
		if (out==NULL) 
		   fprintf(stderr,"bad output file %s\n", argv[i]);
		continue;
		}
        /* Mathematica output */
        if(strcmp(argv[i],"-Mathematica_out") == 0){
                binary_out = 2;
                continue;
		}
        /* HTML output */
        if(strcmp(argv[i],"-HTML_out") == 0){
                binary_out = 3;
                continue;
		}
/* garbage */
        else fprintf(stderr,"bad argument %s\n",argv[i]);
    }

/* fprintf(stderr,"Debug: Inputs cow\n");
   binary_in = 1;
   in = fopen("cow","r"); */

/* fprintf(stderr,"Debug: Binary output to cow\n");
   binary_out = 1;
   out = fopen("cow","w");
   in = fopen("testone","r"); */


   M = *create_default_matrix();

   if (!binary_in) 
   {
   	while(get_matrix(&M, in) != EOF)
	{
	      if (!binary_out) put_matrix(M, out);
	      else if (binary_out==1) put_binary_matrix(M, out);
	      else if (binary_out==2) put_Mathematica_matrix(M, out);
	      else if (binary_out==3) put_HTML_matrix(M, out);
	      else fprintf(stderr, "Unknown output format\n");
   	}
   }
   else
   {
   	while(get_binary_matrix(&M,in) != EOF)
	{
	    if (!binary_out) put_matrix(M, out);
	    else if (binary_out==1) put_binary_matrix(M, out);
	    else if (binary_out==2) put_Mathematica_matrix(M, out);
	    else if (binary_out==3) put_HTML_matrix(M, out);
	    else fprintf(stderr, "Unknown output format\n");
   	}
   }
}

help()
{
   fprintf(stderr, "\nConvertMatrix uses the various get_matrix and put_matrix routines to\n");
   fprintf(stderr, "convert ascii to and from binary, and files to and from pipes\n");
   fprintf(stderr, "\n-binary_in gives binary double precision format for input  \n");
   fprintf(stderr, "	(starting with two binary integers for rows and columns)\n");
   fprintf(stderr, "\n-binary_out gives binary double precision format for output\n");
   fprintf(stderr, "	(starting with two binary integers)\n");
   fprintf(stderr, "\n-ascii_in (default) gives ascii input, no size information\n");
   fprintf(stderr, "	(rows and columns determined by formatting: either one row\n");
   fprintf(stderr, "	 per line or parentheses, brackets, or braces)\n");
   fprintf(stderr, "\n-ascii_out (default) gives ascii output, one row per line\n");
   fprintf(stderr, "	blank line terminating matrix\n");
   fprintf(stderr, "\n-Mathematica_out gives ascii output in Mathematica format\n");
   fprintf(stderr, "\n-HTML_out gives ascii output in HTML Table format\n");
   fprintf(stderr, "\n-file_in filename reads matrix from file\n");
   fprintf(stderr, "\n-file_out filename writes matrix to file\n");
   fprintf(stderr, "\n-fixed_size_in (not yet supported)\n");
   fprintf(stderr, "\n-fixed_size_out (not yet supported)\n");
   fprintf(stderr, "\n-specified_size_in (not yet supported)\n");
   fprintf(stderr, "\n-specified_size_out (not yet supported)\n\n");

   exit(-1);
}
