/*----------------------------------------------------------------------*/
/* ComplexDoubleVectors.c
 * This program uses the various get_complex_double_vector and		*
 * put_complex_double_vector routines to convert ascii to and from	*
 * binary, and files to and from pipes					*/

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
   ComplexDoubleVector V;

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
        /* Ordered pair output (a,b) */
        if(strcmp(argv[i],"-ordered_pair_out") == 0){
                binary_out = 3;
                continue;
	}
        /* Simple output a b */
        if(strcmp(argv[i],"-simple_out") == 0){
                binary_out = 4;
                continue;
	}
        /* HTML output */
        if(strcmp(argv[i],"-HTML_out") == 0){
                binary_out = 5;
                continue;
	}
/* garbage */
        else fprintf(stderr,"bad argument %s\n",argv[i]);
    }


   V = *create_default_complex_double_vector();

   if (!binary_in) 
   {
   	while(get_complex_double_vector(&V, in) != EOF)
	{
	      if (!binary_out) put_complex_double_vector(V, out);
	      else if (binary_out==1) put_complex_double_binary_vector(V, out);
	      else if (binary_out==2) 
			put_complex_double_Mathematica_vector(V, out);
	      else if (binary_out==3)
			put_complex_double_ordered_pair_vector(V,out);
	      else if (binary_out==4)
			put_complex_double_simple_vector(V,out);
	      else if (binary_out==5) 
			put_complex_double_HTML_vector(V, out);
	      else fprintf(stderr, "Unknown output format\n");
   	}
   }
   else
   {
   	while(get_complex_double_binary_vector(&V,in) != EOF)
	{
	    if (!binary_out) put_complex_double_vector(V, out);
	    else if (binary_out==1) put_complex_double_binary_vector(V, out);
	    else if (binary_out==2)
                        put_complex_double_Mathematica_vector(V, out);
            else if (binary_out==3)
                        put_complex_double_ordered_pair_vector(V,out);
	    else if (binary_out==4)
			put_complex_double_simple_vector(V,out);
	    else if (binary_out==5) 
			put_complex_double_HTML_vector(V, out);
	    else fprintf(stderr, "Unknown output format\n");
   	}
   }
}

help()
{
   fprintf(stderr, "\nConvertVector uses the various get_complex_double_vector and put_complex_double_vector routines to\n");
   fprintf(stderr, "convert ascii to and from binary, and files to and from pipes\n");
   fprintf(stderr, "\n-binary_in gives binary double precision format for input  \n");
   fprintf(stderr, "    (starting with binary integer for columns)\n");
   fprintf(stderr, "\n-binary_out gives binary double precision format for output\n");
   fprintf(stderr, "    (starting with binary integer)\n");
   fprintf(stderr, "\n-ascii_in (default) gives ascii input, no size information\n");
   fprintf(stderr, "	(columns determined by formatting: either one vector\n");
   fprintf(stderr, "	 per line or parentheses, brackets, or braces)\n");
   fprintf(stderr, "\n-ascii_out (default) gives ascii output, one vector per line\n");
   fprintf(stderr, "\n-Mathematica_out gives ascii output in Mathematica format\n");
   fprintf(stderr, "\n-HTML_out gives ascii output in HTML Table format\n");
   fprintf(stderr, "\n-ordered_pair_out gives ascii output in the form (a,b) with elements\n");
   fprintf(stderr, "     separated by commas\n");
   fprintf(stderr, "\n-simple_out gives ascii output in the form a b with elements\n");
   fprintf(stderr, "     separated by spaces\n");
   fprintf(stderr, "\n-file_in filename reads vector from file\n");
   fprintf(stderr, "\n-file_out filename writes vector to file\n");
   fprintf(stderr, "\n-fixed_size_in (not yet supported)\n");
   fprintf(stderr, "\n-fixed_size_out (not yet supported)\n");
   fprintf(stderr, "\n-specified_size_in (not yet supported)\n");
   fprintf(stderr, "\n-specified_size_out (not yet supported)\n\n");

   exit(-1);
}
