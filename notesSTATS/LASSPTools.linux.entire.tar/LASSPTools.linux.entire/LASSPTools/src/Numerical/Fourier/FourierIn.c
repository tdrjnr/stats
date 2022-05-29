
/*
** FourierIn.c
**
** This file contains functions for reading in the data to be FFT'd.
**
** The data may be real or complex and may be in either ASCII or binary
** format.
**
*/
#include "Fourier.h"
/*
** real_fft_binary_in()
**
** This function reads in a binary real matrix.
**
** M - Matrix where data is to be stored.
** in - File descriptor that data is to read from
** vector - contains flags for -ordinate_in, -ordinate_out, -inverse (not used)
** L - returns length of input (not used)
** x0 - Returns first postion. (not used)
**
*/
#ifndef _NO_PROTO
int real_fft_binary_in(Matrix **M, FILE *in, int vector, double *L, double *x0)
#else
int real_fft_binary_in(M, in, vector, L, x0)
Matrix **M;
FILE *in;
int vector;
double *L;
double *x0;
#endif
{
  Vector V;
  int c;

  V.N = 0;
  V.el = (*M)->data;
  V.data_reserved = (*M)->data_reserved;
  c = get_binary_vector(&V, in);
  (*M)->data_reserved = V.data_reserved;
  (*M)->data = V.el;
  if((*M)->pointers_reserved == 0) {
    (*M)->el = (double**)malloc((size_t)40*sizeof(double*));
    (*M)->pointers_reserved = 40;
  }
  (*M)->el[0] = V.el;
  (*M)->rows = 1;
  (*M)->cols = V.N;
  return(c);
}

/*
** complex_fft_binary_in()
**
** This function reads in a complex binary matrix. The matrix is read into a 
** vector and the data portion of M is adjusted to point to the elements.
**
** M - Matrix where data is to be stored.
** in - File descriptor that data is to read from
** vector - contains flags for -ordinate_in, -ordinate_out, -inverse (not used)
** L - returns length of input (not used)
** x0 - Returns first postion. (not used)
**
*/
#ifndef _NO_PROTO
int complex_fft_binary_in(ComplexDoubleMatrix **M, FILE *in, int vector, double
*L, double *x0)
#else
int complex_fft_binary_in(M, in, vector, L, x0)
ComplexDoubleMatrix **M;
FILE *in;
int vector;
double *L;
double *x0;
#endif
{
  ComplexDoubleVector V;
  int c;

  V.N = 0;
  V.el = (*M)->data;
  V.data_reserved = (*M)->data_reserved;
  c = get_complex_double_binary_vector(&V, in);
  (*M)->data_reserved = V.data_reserved;
  (*M)->data = V.el;
  if((*M)->pointers_reserved == 0) {
    (*M)->el = (ComplexDouble**)malloc((size_t)40*sizeof(ComplexDouble*));
    (*M)->pointers_reserved = 40;
  }
  (*M)->el[0] = V.el;
  (*M)->rows = 1;
  (*M)->cols = V.N;
  return(c);
}

/*
** real_fft_in_fctn()
**
** This routine reads is a real matrix, if -ordinate_in was specified then 
** the first element of each row is the position and the second and third
** elements are the real and optional imaginary part of the number.
** Since the FFT routines do not want the position the matrix is stored as
** an Mx1 matrix or 1xM matrix, the position (if present) is removed.
**
** A - The matrix where the values are to be stored.
** fp - The input file or stdin where the matrix is being read from
** vector - A flag that contains bits set if -ordinate_in or -ordinate_out
**      is specified on the command line.
** L - The range of the spectrum, this is calculated from the minumum and
**     maximum position elements. This is returned.
** x00 - The value of the first position 
** 
*/
#ifndef _NO_PROTO
real_fft_in_fctn(Matrix **A, FILE *fp, int vector, double *L, double *x00)
#else
real_fft_in_fctn(A,fp,vector,L, x00)
Matrix **A;
FILE *fp;
int vector;
double *L;
double *x00;
#endif
{
      int ret_val,i,sign_pt;
      double x0,x1,xn,temp,min,max;
      Matrix *scratch;

     /* Check to see if -ordinate_in was specified, this means that the 
     ** incoming data matrix has an extra column of data indicating the
     ** position which must be removed before the FFT is done.
     */
     if (vector & ORDINATE_IN) {

        /* This gets a little tricky because we have to read the data in as an
        ** Nx2 (or Nx3 if complex) but what is actually useful in the fft is
        ** only the second column and third columns not the position.
        */

        /* Create a scratch matrix, from libmath_io library */
        scratch = create_default_matrix();

        /* Read in the data */
        if ((ret_val = get_matrix(scratch,fp)) == EOF) return(EOF);

        /* Get out x(0,0) this is the first position  */
        x0 = *x00 = scratch->el[0][0];

        /* Save the second and last elements                              */
        if (scratch->rows > 1) {
           x1 = scratch->el[1][0];
           xn = scratch->el[scratch->rows-1][0];
        } else {
           /* This is misleading, the -ordinate option was specified but there 
           ** is only 1 row of data, assume the positions increase by 1
           */
           x1 = x0 + 1;
           xn = scratch->cols - 1;
        }

        /* Find the min and max in case the input is not ordered */
        if (scratch->rows > 1) {
           min = max = x0;

           /* Keep track of where the input position (or freq) changes
	   ** sign, this is used to order the postions before the transform
	   ** is done. This could be accounted for after the transform is complete
	   ** but would require a similar method of finding the Zero point and
	   ** outputing from there instead of always starting from a fixed point
	   ** in the transform.
           */
           sign_pt = -1;
  
           /* Look at each position (or freq) value */
           for (i=1;i<scratch->rows;i++) {
      
              /* Check for a minimum           */
              if (scratch->el[i][0] < min)
                 min = scratch->el[i][0];
              else
                 /* Check for a maximum   */
                 if (scratch->el[i][0] > max)
                    max = scratch->el[i][0];
       
              /* Check for a sign change (zero point) */
              if ((scratch->el[i-1][0] < 0.0) && (scratch->el[i][0] >= 0.0)) 
                 sign_pt = i;

           }
           /* Make sure that x1 > x0 because we want only the distance */
           if (x1 < x0)
	   {
/* XXX */
		fprintf(stderr,"Probable error in ordinate in FourierIn: x1=-x1\n");
		fprintf(stderr,"I can't figure out why he did this ... JPS\n");
              x1 = -x1;
	    }

           /* Set xn and x0 */
           xn = max;
           x0 = min;        /* This is not the x0 that is returned it is only used
                            ** in computing L (the length of spectrum)
                            */

           /* Reorder the frequencies before doing the inverse if they do not 
	   ** start with the 0 point
           */
           if ((sign_pt > 0) && (vector & INVERSE))
              sort_freq(scratch,sign_pt);
       
	}

        /* Compute how long the spectrum is */
        *L = (xn - x0) + (x1 - x0);

        /* Get rid of the old matrix */
        free_matrix(*A);

        /* Create a new matrix to replace it */
        *A = create_matrix(scratch->rows,1);

        /* Copy the elements from the scratch matrix into the new matrix */
        for (i=0;i<scratch->rows;i++)
            (*A)->el[i][0] = scratch->el[i][1];

        /* Free the scratch matrix */
        free_matrix(scratch);

     } else {

       /* Since there is no ordinate make the starting position 0 */
       *x00 = 0.0;

       /* Just input the matrix as is */
       ret_val = get_matrix(*A,fp);

       /* Set L to the number of elements */
       if ((*A)->rows == 1)
          *L = (*A)->cols;
       else
          if ((*A)->cols == 1)
             *L = (*A)->rows;
     }

     return(ret_val);


}

/*
** complex_fft_in_fctn()
**
** This function reads is a complex matrix, if -ordinate_in was specified
** then the first element of each row contains the posistion. Since the
** FFT routines do not use the position the matrix is stored as a Mx2
** matrix.
**
** C - The matrix where the values are to be stored.
** fp - The input file or stdin where the matrix is being read from
** vector - A flag that contains bits set if -ordinate_in or -ordinate_out
**      is specified on the command line.
** L - The range of the spectrum, this is calculated from the minumum and
**     maximum position elements. This is returned.
** x00 - The value of the first position 
** 

*/
#ifndef _NO_PROTO
complex_fft_in_fctn(ComplexDoubleMatrix **C, FILE *fp, int vector, double *L, double *x00)
#else
complex_fft_in_fctn(C, fp, vector, L, x00)
ComplexDoubleMatrix **C;
FILE *fp;
int vector;
double *L;
double *x00;
#endif
{
      int ret_val,i,sign_pt;
      double x0,x1,xn,min,max;
      Matrix *scratch;

     /* Check to see if -ordinate_in was specified */
     if (vector & ORDINATE_IN) {

        /* This gets a little tricky because we have to read the data in as an
        ** Nx2 but what is actually useful in the fft is only the second column.
        */

        /* Create a scratch matrix */
        scratch = create_default_matrix();

        /* Read in the data */
        ret_val = get_matrix(scratch,fp);

        /* Get out x(0,0) and x(1,0) in order to compute L */
        x0 = *x00 = scratch->el[0][0];

        if (scratch->rows > 1) {
           x1 = scratch->el[1][0];
           xn = scratch->el[scratch->rows-1][0];
        } else {
           x1 = x0 + 1;
           xn = scratch->cols - 1;
        }
       
        sign_pt = -1;

        /* Find the min and max in case the input is not ordered */
        if (scratch->rows > 1) {
           min = max = x0;
           for (i=1;i<scratch->rows;i++) {
              if (scratch->el[i][0] < min)
                 min = scratch->el[i][0];
              else
                 if (scratch->el[i][0] > max)
                    max = scratch->el[i][0];
              if ((scratch->el[i-1][0] < 0.0) && (scratch->el[i][0] >= 0.0)) 
                 sign_pt = i;
           }
           /* Make sure that x1 > x0 */
           if (x1 < x0)
              x1 = -x1;

           /* Set xn and x0 */
           xn = max;
           x0 = min;        /* This is not the x0 that is returned it is only used
                            ** in computing L (the length of spectrum)
                            */

           if ((vector & INVERSE) && (sign_pt > 0))
              sort_freq(scratch,sign_pt);     

	}

        *L = (xn - x0) + (x1 - x0);

        /* Get rid of the old matrix */
        free_complex_double_matrix(*C);

        /* Create a new matrix to replace it */
        *C = create_complex_double_matrix(scratch->rows,1);

        /* Copy the elements from the scratch matrix into the new matrix */
        for (i=0;i<scratch->rows;i++) {
            (*C)->el[i][0].real = scratch->el[i][1];
            (*C)->el[i][0].imaginary = scratch->el[i][2];
        }

        /* Free the scratch matrix */
        free_matrix(scratch);

     } else {


       *x00 = 0.0;

       /* Just input the matrix as is */
       ret_val = get_complex_double_matrix(*C,fp);

       /* Set L to the number of elements */
       if ((*C)->rows == 1)
          *L = (*C)->cols;
       else
          if ((*C)->cols == 1)
             *L = (*C)->rows;
     }

     return(ret_val);

}

/*
** sort_freq()
**
** The new frequency array is ordered as follows:
**      least positive H(i) .... most positive H(i) most negative H(i) ..... least negative H(i)
**
** This allows the output functions to not have to start at various places in the
** transformed array, instead the output functions can always start at the beginning.
**
** This function should only be called if there is negative indexes for H(i), this could be the
** result of a previous center operation.
**
** This function assumes that the data is only shifted and not totally random frequencies.
**
** scratch - Matrix to order.
** sign_pt - the index in the array where the frequency changes from - to +.
*/
#ifndef _NO_PROTO
sort_freq(Matrix *scratch, int sign_pt)
#else
sort_freq(scratch, sign_pt)
Matrix *scratch;
int sign_pt;
#endif
{
        int i,j,k;
        Matrix *temp_m;
        int cplx;

        /* From finding the min and max we already know the point
        ** in the array where the freq changes sign
        */
        temp_m = create_matrix(scratch->rows, scratch->cols);

        if (scratch->cols > 2)
           cplx = TRUE;
        else  
           cplx = FALSE;

        j = 0;
        i = sign_pt;
        while ((i < scratch->rows) && (scratch->el[i][0] >= 0.0)) {
           for (k=0;k<scratch->cols;k++) 
               temp_m->el[j][k] = scratch->el[i][k];
           if (cplx)
              temp_m->el[j][k-1] = -temp_m->el[j][k-1];
           j++; i++;
	}
        
        /* Assume that sign_pt - 1 is the least negative and H(0) is most negative */
        for (i=0;i<sign_pt;i++,j++) {
            for (k=0;k<scratch->cols;k++)
                temp_m->el[j][k] = scratch->el[i][k];
            if (cplx)
               temp_m->el[j][k-1] = -temp_m->el[j][k-1];
	}

       /* Now we just have to copy back the fixed up array */
       for (i=0;i<scratch->rows;i++)
           for (j=0;j<scratch->cols;j++)
               scratch->el[i][j] = temp_m->el[i][j];
          
       /* Get rid of the matrix that was used for rearranging */  
       free_matrix(temp_m);            
}









