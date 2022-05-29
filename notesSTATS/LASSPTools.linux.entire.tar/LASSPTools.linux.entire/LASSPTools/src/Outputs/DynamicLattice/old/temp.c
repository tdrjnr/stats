#include <stdio.h>
#include <math.h>

int nrows = 100, ncols = 100;
char line[64];

main(argc, argv)
int argc;
char **argv;
{
    int i,j,n;
    double **mat;

    mat = (double **) malloc (nrows * sizeof(double *));
    for (i = 0; i < nrows; i++)
    {
	 mat[i] = (double *) malloc (ncols * sizeof(double));
    }

    for (i = 0; i < nrows; i++) 
    {
	 for (j = 0; j < ncols; j++) 
	 {
	      mat[i][j] = 10.*i + 23.*j;
	 }
    }

/*
    sprintf(line, "#string blah blah = %d", 12);
    fwrite((char *) line, (int)sizeof(line), 1, stdout);
*/

    for(n=0; n < 10; n++)
    {
    fwrite((char *) &nrows, (int)sizeof(int), 1, stdout);
    fwrite((char *) &ncols, (int)sizeof(int), 1, stdout);
    for (i = 0; i < nrows; i++)
    {
	      mat[i][j] = (n+1)*10.*i + (n-1)*23.*j;
	 fwrite((char *) mat[i], (int) sizeof(double), ncols, stdout);
    }
    fflush(stdout);
    }
}

/*
        fwrite((char *) (&M.rows),(int)sizeof(int),1,out));
        fwrite((char *)(&M.cols),(int)sizeof(int),1,out));
        for(i=0;i<M.rows;i++) {
        fwrite((char *) start_write, (int)sizeof(double), numbers_left, out)

        fwrite((char *)(&rows),(int)sizeof(int),1,stdout));
        fwrite((char *)(&cols),(int)sizeof(int),1,stdout));
        for(i=0;i<rows;i++) {
        fwrite((char *) M[i], (int)sizeof(double), columns, stdout)}

*/


/*
        fwrite((char *)(&M.rows),(int)sizeof(int),1,out));
        fwrite((char *)(&M.cols),(int)sizeof(int),1,out));
        for(i=0;i<M.rows;i++) {
        fwrite((char *) start_write, (int)sizeof(double), numbers_left, out)

        fwrite((char *)(&rows),(int)sizeof(int),1,stdout));
        fwrite((char *)(&cols),(int)sizeof(int),1,stdout));
        for(i=0;i<rows;i++) {
        fwrite((char *) M[i], (int)sizeof(double), columns, stdout)}

        fflush(stdout);

*/
