#include "Fourier.h"

/*
** FourierOut.c
**
** This file contains functions for output of Fourier transforms.
** See the file Fourier.doc for information on how the data is
** stored after the FFT is done.
**
** There are five functions to handle ASCII or binary data:
**
**    complex_fft_binary_out()
**    complex_fft_out()
**    real_fft_binary_out()
**    real_fft_out()
**    real_fft_packed_out()
**    
*/

/*
** real_fft_binary_out()
**
** This function outputs an FFT in binary form, the input matrix was a 
** real matrix. The number of input parameters is the same the other 
** output functions even though some parameters are not used.
**
** M - The FFT matrix to output.
** n - THe number of elements in the FFT.
** norm - The normalization to be applied to each element (see Fourier.c)
** center - TRUE if the output is to be centered around the 0 wave vector (not used)
** vector - contains flags for ordinate_in, ordinate_out and inverse (not used)
** out - The File descriptor where the output is to be sent to.
** L - The length of the input spectrum (not used).
** x0 - The value of the first time (or frequency) index from the input (not used).
** 
*/
#ifndef _NO_PROTO
void real_fft_binary_out(Matrix *M, int n, double norm,
			 int center, int vector, FILE *out, double L, double x0, int nozero)
#else
void real_fft_binary_out(M, n, norm, center, vector, out, L, x0,nozero)
Matrix *M;
int n;
double norm;
int center;
int vector;
FILE *out;
double L;
double x0;
int nozero;
#endif
{
  Vector V;
  int i;

  for(i=0;i<n;i++)
    M->data[i] = M->data[i] / norm;
  V.N = n;
  V.el = M->data;
  V.data_reserved = M->data_reserved;
  put_binary_vector(V, out);
}


/*
** complex_fft_binary_out()
**
** This function handles binary output of a complex FFT.
**
** C - The FFT matrix to output.
** n - THe number of elements in the FFT.
** norm - The normalization to be applied to each element (see Fourier.c)
** center - TRUE if the output is to be centered around the 0 wave vector (not used)
** vector - contains flags for ordinate_in, ordinate_out and inverse (not used)
** out - The File descriptor where the output is to be sent to.
** L - The length of the input spectrum (not used).
** x0 - The value of the first time (or frequency) index from the input (not used).
** nozero - do not output the k=0 point
*/

#ifndef _NO_PROTO
void complex_fft_binary_out(ComplexDoubleMatrix *M, int n, double norm,
			     int center, int vector, FILE *out, double L, double x0,int nozero)
#else
void complex_fft_binary_out(M, n, norm, center, vector, out, L, x0, nozero)
ComplexDoubleMatrix *M;
int n;
double norm;
int center;
int vector;
FILE *out;
double L;
double x0;
int nozero;
#endif
{
  ComplexDoubleVector V;
  int i;

  for(i=0;i<n;i++) {
    M->data[i].real = M->data[i].real / norm;
    M->data[i].imaginary = M->data[i].imaginary / norm;
  }
  V.N = n;
  V.el = M->data;
  V.data_reserved = M->data_reserved;
  put_complex_double_binary_vector(V, out);
}


/*
** real_fft_packed_out()
**
** This function dumps out the real FFT.
**
** M - The FFT matrix to output.
** n - THe number of elements in the FFT.
** norm - The normalization to be applied to each element (see Fourier.c)
** center - TRUE if the output is to be centered around the 0 wave vector (not used)
** vector - contains flags for ordinate_in, ordinate_out and inverse (not used)
** out - The File descriptor where the output is to be sent to.
** L - The length of the input spectrum (not used).
** x0 - The value of the first time (or frequency) index from the input (not used).
** type - The particular format of the output FFT (not used).
** 
** 
*/
#ifndef _NO_PROTO
void real_fft_packed_out(Matrix *M, int n, double norm,
                         int center, int vector, FILE *out, double L, double x0, OutputType type, int nozero)
#else
void real_fft_packed_out(M, n, norm, center, vector, out, L, x0, type, nozero)
Matrix *M;
int n;
double norm;
int center;
int vector;
FILE *out;
double L;
double x0;
OutputType type;
int nozero;
#endif
{
  int i,start;

  if (nozero)
     start = 1;
  else
     start = 0;

  for(i=start;i<n;i++) {
    M->data[i] = M->data[i] / norm;
    fprintf(out, "%g\n" , M->data[i]);
  }

  fflush(out);
}



/*
** real_fft_out()
**
** This function handles data that was read in having only real components. To output the
** data a complex number is built and then passed to the routine print_point_complex()
** which handles any normalization and scaling. This function will optionally center
** the data.
**
** The incoming data is stored in the following format:
**     data(0)  - the sum from i=1 to n of r(i)
**     data(1)  - real part of first k point value
**     data(2)  - imaginary part of first k point value
**     data(3)  - real part of second k point value
**     data(4)  - imaginary part of second k point value
**     :
**     :
**     data(last-2) - real part of last k point value
**     data(last-1) - imaginary part of last k point value
**                 (last = n/2 if n is even OR last = (n+1)/2 if n is odd)
**
**     The sum is actually output twice, once at the beginning of the period and then
**     again at the end of the period, thus actually n+1 points are output.
**
** M - The FFT matrix to output.
** n - The number of elements in the FFT.
** norm - The normalization to be applied to each element (see Fourier.c)
** center - TRUE if the output is to be centered around the 0 wave vector
** vector - contains flags for ordinate_in, ordinate_out and inverse
** out - The File descriptor where the output is to be sent to.
** L - The length of the input spectrum.
** x0 - The value of the first time (or frequency) index from the input.
** type - The particular format of the output FFT.
** nozero - TRUE if the k=0 point should not be printed.
*/

#ifndef _NO_PROTO
void real_fft_out(Matrix *M, int n, double norm, int center,
                          int vector, FILE *out, double L, double x0, OutputType type, int nozero)
#else
void real_fft_out(M, n, norm, center, vector, out, L, x0, type, nozero)
Matrix *M;
int n;
double norm;
int center;
int vector;
FILE *out;
double L;
double x0;
OutputType type;
int nozero;
#endif
{
  int i,m,end_pt,l,start;
  ComplexDouble cdata;
  
  double k, kstart, scale, index, temp;
  cdata.imaginary = 0.0;

  /* Figure out the distance that is to be between each output value */  
  k = 2*(PI/L);

  /* Check if the output should be scaled to account for a time (or freq) shift */
  if (x0 != 0.0) { 
     scale = -(x0*2*PI/L);
     if (vector & INVERSE)
        scale = -scale;
  } else
     scale = 0.0;

  /* Check if the output data should be centered */
  if (center) 
    /* Determine the index of the first point output */
    kstart = -(PI*(n-2))/L;
  else {
    /* Since it is not centered always start at zero */
    kstart = 0;
  }
   /* Figure out the endpoint of the data in the transform */
  if (IsOdd(n))
     end_pt = n-2;
  else
     end_pt = n-3;


  /* 
  ** If the data is centered then the most negative value is at the end of
  ** the array, start from there and move to the beginning.
  */
  if (center) {

     /* This for loop prints out the negative frequencies (or time). 
     ** note that 'l' starts out large and approaches zero, this is
     ** used to get the correct value in order to shift. 'i' is
     ** decremented by 2 because the data is stored with a real and
     ** imaginary part.
     */

     for (i=end_pt,l=((end_pt/2))+1,m=0;i>0;i-=2,m++,l--) {
         /* If an index is being output then figure out what it
         ** should be from the number of steps taken (m) and the 
         ** size of each step (k) added to the initial position (kstart).
         */
         if (vector & ORDINATE_OUT) 
            index = kstart+(m*k);

         cdata.real = M->data[i];
         /* For negative frequencies the value is the complex conjugate 
         ** so reverse the sign on the imaginary part of the number 
         */
         cdata.imaginary = -(M->data[i+1]);
         print_point_complex(vector,index,&cdata, type, norm, out, scale*l,TRUE);
     }

     /* Print out the value that is the zero point if nozero is not set */
     if (!nozero) {
        cdata.real = M->data[0];
        cdata.imaginary = 0.0;
        print_point_complex(vector, 0.0 , &cdata, type, norm, out, scale*0,FALSE);
     }
     m++;

     /* This for loop prints out the positive frequencies, the FFT array is
     ** ordered from least positive to most positive 
     */ 
     for(i=1,l=1;i<=end_pt;i+=2,m++,l++) { /* changed <=endpoint EB 1-18-94 */
      if (vector & ORDINATE_OUT)
         index = kstart+(m*k);
      cdata.real = M->data[i];
      cdata.imaginary = (M->data[i+1]);
      print_point_complex(vector,index,&cdata, type, norm, out, scale*l,FALSE);
    }

    /* For an even number of points output the last, an FFT from an odd
    ** number of points does not have this extra entry see Fourier.Doc
    ** Removed 1-19-94 to get rid of extra point - jwf
    if (IsOdd(n))
    ** Added again 9-29-94: should have been even -jps
    */
    if (IsEven(n)) { 
      cdata.real = M->data[i];
      cdata.imaginary = 0.0;
      print_point_complex(vector,kstart+(m*k),&cdata, type, norm, out, scale*l,FALSE);
    } 


  } else {  

     /* This section prints out an FFT that is not centered around zero,
     ** for a real FFT this represents putting out the values twice 
     ** using the complex conjugate for half the values.
     */

     if (!nozero) {
        /* Put out the zero point */
        cdata.real = M->data[0];
        cdata.imaginary = 0.0;
        print_point_complex(vector,kstart, &cdata, type, norm,out, scale*0,FALSE);
     }

     /* Make a pass through the FFT array and put out the values. */
     for(i=1,l=1,m=1;i<=end_pt;i+=2,m++,l+=2) {
      if (vector & ORDINATE_OUT)
         index = kstart+(m*k);
      
      cdata.real = M->data[l];
      cdata.imaginary = (M->data[i+1]); 
      print_point_complex(vector,index,&cdata, type, norm, out, scale*m,FALSE);
 
    }

    /* For an even number of points there is an extra point in the FFT 
    ** See Fourier.Doc for the data layout.
    */
    if (IsEven(n)) {
      cdata.real = M->data[l];
      cdata.imaginary = 0.0;
      print_point_complex(vector,kstart+(m*k),&cdata, type, norm, out, scale*m,FALSE);  
      m++;     
    } 

    /*
    ** Since the transform is symetric put out the same points over again with a new set of indexes 
    ** Got through the data array from high to low which corresponds to putting out the least positive
    ** to the most postive value. Reverse the sign of the imaginary component, when all the elements are
    ** summed the imaginary component will be zero and the real components will sum to data(0) which
    ** was the first point we put out.
    */
    l = m;
    for(i=end_pt,m=(end_pt/2)+1;i>0;i-=2,m--,l++) {
      if (vector & ORDINATE_OUT)
         index = kstart+(l*k);
      cdata.real = M->data[i];
      cdata.imaginary = -(M->data[i+1]);
      print_point_complex(vector, index, &cdata, type, norm, out, scale*m,TRUE);
    }
    
  }

  /* Make sure that all data is output in case Fourier is in a pipe */
  fflush(out);
}

/*
** print_point_complex()
**
** Outputs a complex number with optional index. Can output the real part, imaginary
** part or the whole complex number. The power and absolute options are also handled. 
** This function has three main parts:
**    - scale the real and imaginary parts.
**    - preform normalization on the real and imaginary parts.
**    - print out the data, with or without indexes
**
** vector - contains flags for ordinate_in, ordinate_out and inverse
** index - The value to put out if -ordinate_out was specified
** data - A pointer to a complex number holding the real and imaginary parts to output
** type - The particular format of the output FFT (not used).
** norm - The normalization that is to be applied to each value.
** fp  - The File descriptor where the output is to be sent to.
** scale - The value to multiply the transform by if -ordinate_in was specified
**        and the first index was not zero. This accounts for shifting.
**         
** 
*/
#ifndef _NO_PROTO
print_point_complex(int vector, double index, ComplexDouble *data, OutputType type,
                  double norm,FILE *fp, double scale,int con)
#else
print_point_complex(vector, index, data, type, norm, fp, scale,con)
int vector;
double index;
ComplexDouble *data;
OutputType type;
double norm;
FILE *fp;
double scale;
int con;
#endif
{

      double temp, power, realp,imagp;

      /* Copy the values because they will be overwritten */
      realp = data->real;
      imagp = data->imaginary;

      /* Check if we need to multiply each element by to account for time shifting 
      ** this is equivalent to multiplying by exp(2*PI*i*f*t0)
      */
      if (scale != 0.0) {
         if (con) imagp = -imagp; 
         switch (type) {
            case Real_Out:
               /* Only deal with the real part */
               /* realp = (realp * cos(scale)); */
               realp = (realp * cos(scale)) - (imagp * sin(scale));
               break;
            case Imag_Out:
               /* Only deal with the imaginary part */
               /* imagp = (imagp * cos(scale)); */
               imagp = (imagp * cos(scale)) + (realp *sin(scale));
               break;
            case Complex_Out:
               /* Scale the whole number */
               temp = realp;
               realp = (realp * cos(scale)) - (imagp * sin(scale));
               imagp = (temp * sin(scale)) + (imagp * cos(scale));
               break;
	    case Absolute_Out:
	    case Power_Out:
               /* No scaling for Power or Absolute */
               break;
	  }
        if (con) imagp = -imagp; 
      }

      /* Normalize the output  */
      if (norm != 1.0) {
         realp = realp / norm;
         imagp = imagp / norm;
      }

      /* Check if we should output an index and the value */
      if (vector & ORDINATE_OUT) 
         switch (type) {
	    case Real_Out:
               fprintf(fp,"%lg %lg\n",index,realp);
               break;
	    case Imag_Out:
               fprintf(fp,"%lg %lg\n",index,imagp);
               break;
	    case Power_Out:
               power = (realp * realp) + (imagp * imagp);
               fprintf(fp,"%lg %lg\n", index, power);
               break;
	    case Absolute_Out:
               power = (realp * realp) + (imagp * imagp);
               power = sqrt(power);
               fprintf(fp,"%lg %lg\n", index, power);
               break;
	    case Complex_Out:
               fprintf(fp,"%lg %lg %lg\n",index,realp, imagp);
               break;
	  }
      else
         /* Only output the value, no index */ 
         switch (type) {
	     case Real_Out:
                fprintf(fp,"%lg\n",realp);
                break;
	     case Imag_Out:
                fprintf(fp,"%lg\n",imagp);
                break;
	     case Power_Out:
               power = (realp * realp) + (imagp * imagp);
               fprintf(fp,"%lg\n",power);
               break;
 	     case Absolute_Out:
               power = (realp * realp) + (imagp * imagp);
               power = sqrt(power);
               fprintf(fp,"%lg\n",  power);
               break;
             case Complex_Out:
               fprintf(fp,"%lg %lg\n",realp, imagp);            
               break;
	 }
     
}

/*
** complex_fft_out()
**
** This function handles outputting an FFT if the original data was read in 
** in complex format. 
**
** M - The FFT matrix to output.
** n - The number of elements in the FFT.
** norm - The normalization to be applied to each element (see Fourier.c)
** center - TRUE if the output is to be centered around the 0 wave vector (not used)
** vector - contains flags for ordinate_in, ordinate_out and inverse (not used)
** out - The File descriptor where the output is to be sent to.
** L - The length of the input spectrum (not used).
** x0 - The value of the first time (or frequency) index from the input (not used).
** type - The particular format of the output FFT (not used).
**
*/ 
#ifndef _NO_PROTO
void complex_fft_out(ComplexDoubleMatrix *M, int n, double norm,
                     int center, int vector, FILE *out, double L, double x0, 
                     OutputType type, int nozero)
#else
void complex_fft_out(M, n, norm, center, vector, out, L, x0, type,nozero)
ComplexDoubleMatrix *M;
int n;
double norm;
int center;
int vector;
FILE *out;
double L;
double x0;
OutputType type;
int nozero;
#endif
{
  int i,m,start,l,istart;
  double k,kstart,index,scale;

  /* This is how big each step should be in the output */
  k = (2*PI)/L;

  /* If -ordinate_in was specified this handles adjusting for data that
  ** is shifted
  */
  if (x0 != 0.0) {
     scale = -(x0*2*PI/L);   /* This goes from time to frequency */
     if (vector & INVERSE)
        scale = -scale;      /* Opposite sign for inverse freq to time */
  } else
     scale = 0.0;

  /* Compute the offset for centering the data */
  if (center) 
    /* Start at a point below zero and center the output around 0 */
    kstart = -(PI*(n-1))/L;
  else
    kstart = 0;

  /* Check if the data should be centered around zero */
  if (center) {
     
     /* Pick the middle so that it ends up at position zero */
     if (IsOdd(n))
        start = (n/2)+1;
     else
        start = n/2;

     /* Print out in the order most negative to least negative */     
     for (i=start,m=0,index=0; i<n; i++,m++) {
        if (vector & ORDINATE_OUT)
           index = kstart + (m*k);
        else 
           index++;
        print_point_complex(vector, index, &M->data[i], type, norm, out, scale*i,FALSE);         
     }

     /* if -nozero was an option then skip the 0 point */
     if (nozero) {
        istart=1; m++;
      } else
        istart=0;
     /* Print out in the order least postive to most positive */
     for (i=istart;i<=start;i++,m++) {
        if (vector & ORDINATE_OUT)
           index = kstart + (m*k);
        else
           index++;
        print_point_complex(vector, index, &M->data[i], type, norm, out, scale*i,FALSE);
     } 

     
  } else {

    /* This handles data that is not centered, which is just a matter of dumping the
    ** FFT out.
    */
    if (nozero)
      istart=1;
    else
      istart=0;
    for(i= l= istart,m=istart;i<n;i++,m++) {
     if (vector & ORDINATE_OUT)
        index = (k*m)+kstart;
  
     print_point_complex(vector, index, &M->data[l++], type, norm, out, scale*m,FALSE);

    } /* End for loop */
  }

  /* Make sure no data is left in the pipe */
  fflush(out);
}







