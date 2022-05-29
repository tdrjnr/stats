/*----------------------------------------------------------------------------
 * 3D-2D.c
 * this is the source file for a filter which takes in triples of doubles from 
 * stdin and output pairs of doubles to stdout (stripping the third number 
 * of each triple).
 */

#include "3D-2D.h"

#ifndef _NO_PROTO
 int main(int argc, char **argv)
#else
 int main(argc,argv)
	int argc;
	char **argv;
#endif
{
	double *x, *y, *z;
	char *remains;
	char input[MAXLENGTH], output[MAXLENGTH];
	char binarymode;
	XColor acolor;
	int num, i;
	char *dummy_string = "";

	i = parse_argument(argc,argv,0,&dummy_string,"-b");
	if (i!=-1)
		binarymode = TRUE;
	else
		binarymode = FALSE;

	/* now set x and y and z to NULL so that read_triples ( or
	 * read_binary_triples) knows that they are initially unallocated
	 */
	x = NULL;
	y = NULL;
	z = NULL;

	if (!binarymode) {
		while (fgets(input,MAXLENGTH,stdin) != NULL) {
			if (read_triples(input, &x, &y, &z, &num, &remains)) {
				write_pairs(output, x, y, num);
				strcat(output, remains);
				if(num==0) fflush(stdout);
				printf("%s",output);
			} else {
				fprintf(stderr,"3D-2D: failure.\n");
			}
		}
	} else { /*binary mode*/
		while (read_binary_triples(stdin, &x, &y, &z, &num)) {
			read_binary_rgb_color(stdin, &acolor);
			if(num==0) fflush(stdout);
			write_binary_pairs(stdout, x, y, num);
			write_binary_rgb_color(stdout, &acolor);
		}
	}

	if (x!=NULL) free(x);
	if (y!=NULL) free(y);
	if (z!=NULL) free(z);

	fflush(stdout);
	exit(0);
}
