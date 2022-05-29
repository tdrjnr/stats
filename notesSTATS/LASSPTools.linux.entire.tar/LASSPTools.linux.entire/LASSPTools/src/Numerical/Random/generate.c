
/*
** Generate.c
**
** Various random number generator function interfaces.
**
*/
#include <stdio.h>
#include <stdlib.h>
#include "Boolean.h"
#include "math_io.h"
#include "random.h"
#include "ranlib.h"


static double total = 0;
static ComplexFloat total_complex = {0,0};

/*
** random_number();
**
** This function will return a single random number based on what 
** distribution is active.
**
*/
double random_number()
{

    double temp_rand;
    float temp_float;
    long templ;
   
    /* See what type of distribution we should do
    ** The internal generator in each function will call the
    ** function (*get_number) ()
    */
    switch (distribution) {
        case FLAT:
           temp_rand = (*get_number) (); 
           if (adjust_range) {
               /* Adjust for a different width */
               temp_rand = (flat_max - flat_min) * temp_rand;
               /* Next adjust for any distribution not starting at zero */
               temp_rand += flat_min;
           }
           break;

        case NORMAL:
           temp_rand =  (double) gennor(norm_av,norm_sigma);
           
           break;

        case EXPONENTIAL:
           temp_rand = (double) genexp(exp_av);
           break;

	case F:
           break;
 
	case POISSON:
           temp_rand = (double)ignpoi(pos_av);
           break;

	case BINOMIAL:
           temp_rand = (double) ignbin(bin_trials,bin_prob);
           break;
   
    }
          
    return(temp_rand);

}

/*
** do_scaler()
**
** Will produce single random numbers and optionally accumulate them
**
*/
#ifndef _NO_PROTO
do_scaler(ACCUMULATE_TYPE accumulate, boolean put_index, int index)
#else
do_scaler(accumulate, put_index,index)
ACCUMULATE_TYPE accumulate;
boolean put_index;
int index;
#endif

{
    double temp_rand, temp_complex;
    float fl_rand,sigma;
    ComplexDouble complex_val;
      
    temp_rand = random_number();

    if (complex_out) {
       complex_val.imaginary = random_number();
       complex_val.real = temp_rand;
    }

    /* 
    ** The following options are valid for a scaler random number:
    **    accumulate -> keep a running total of each random number and
    **                  output this each time instead.
    **    square_it -> squares the random number (or the total if accumulate
    **                 is specified.
    **    adjust_range -> adjust the width of the flat range.
    **    put_index -> put out the current object number
    **
    */ 

    if (accumulate) {
       if (complex_out) {
          total_complex.real += complex_val.real;
          total_complex.imaginary += complex_val.imaginary;
          complex_val.real = total_complex.real;
          complex_val.imaginary = total_complex.imaginary;   
          if (square_it) 
             square_complex(&complex_val,&complex_val);
                             
       } else {
          total+=temp_rand;
          if (square_it) 
             temp_rand  = total * total;
          else
             temp_rand = total;
       }
              
    } else
       if (square_it)
          if (complex_out) 
             
             square_complex(&complex_val,&complex_val);
          else
             temp_rand = temp_rand * temp_rand;
    
    if (!average) {
       if (put_index) {
         
          if (complex_out)
             fprintf(out,"%d %G+%GI\n",index,complex_val.real,complex_val.imaginary);
          else
             fprintf(out,"%d %G\n",index,temp_rand);
       
       } else {
          if (complex_out)
             fprintf(out,"%G+%GI\n",complex_val.real,complex_val.imaginary);
          else
             fprintf(out,"%G\n",temp_rand);
      
       }
    }    
    
}    

/*
** square_complex()
**
** Squares a complex number.
**
** source - The complex number to square.
** dest - The complex number where the result will be stored.
**
*/
#ifndef _NO_PROTO
void square_complex(ComplexDouble *source, ComplexDouble *dest)
#else
void square_complex(source, dest)
ComplexDouble *source;
ComplexDouble *dest;
#endif
{
    /* This is not what we want
    double temp = source->real;

    
    dest->real = (source->real * source->real) - (source->imaginary * source->imaginary);
    dest->imaginary = 2 * (temp * source->imaginary);
    */

    /* For now just square each part, this should probably be changed to
    ** the above for squaring a complex number 
    */
    dest->real = source->real * source->real;
    dest->imaginary = source->imaginary * source->imaginary;
      
}

/*
** do_matrix()
**
** This function will fill a matrix with random numbers and then
** put it out. 
**
** rows - number of rows in the matrix
** cols - number of cols in the matrix
** accumulate - What (if any) accumulation should be done
*/
#ifndef _NO_PROTO
do_matrix(int rows, int cols, ACCUMULATE_TYPE accumulate)
#else
do_matrix(rows,cols,accumulate)
int rows,cols;
ACCUMULATE_TYPE accumulate;
#endif

{
    int i,j;
    double temp_rand, temp_complex;
    Matrix *M_total_objs = M;
    ComplexFloatMatrix *CM_total_objs = CM;
    ComplexFloatMatrix *CM_temp = CM;
    Matrix *M_temp = M;
    ComplexDouble complex_val;

    /* Check if the accumulate is only within an object */
    if (accumulate) {
       if (~accumulate & ADD_OBJECTS) {
          /* We are only accumulating within objects */
          CM_temp = CM_total;
          M_temp = M_total;
       
          if (square_it) {
            CM_temp = CM_sqtotal;
            M_temp = M_sqtotal;
          }
       } else {
          /* Objects are being added */
          CM_temp = CM_global;
          M_temp = M_global;
       }   
    } /* end if accumulate */

    
    /* This loop fills each element of the object with a random number
    ** by looping over the rows and cols. The cols start at start_col
    ** because the first col (col[0]) may be filled with an index if
    ** the -index option was specified
    */
       
    for (i=0;i<rows;i++) {
       for (j=start_col;j<cols;j++) {

          /* Get a random number, this is the generic funtion that will
          ** check for the type of distribution which in turn will invoke
          ** one of the core generators
           */
          temp_rand = random_number();
          
          /* If this is a complex matrix get the imag part */
          if (complex_out)
             temp_complex = random_number();
           
          /* If not accumulating just square the value (if -square) */
          if ((square_it) && (!accumulate)) {
             if (complex_out) { 
                complex_val.real = temp_rand;
                complex_val.imaginary = temp_complex;
                square_complex(&complex_val,&complex_val);
             } else
               temp_rand = temp_rand * temp_rand;
	  }
           
          /*                     
          ** Determine what to accumulate
          */
          if (accumulate) {
             switch (accumulate & (ADD_COLS | ADD_ROWS)) {
             
               /*
               ** This will add up elements that have the same column
               */
               case ADD_COLS:
                 
                  /* Check if this is the first row, also check if indexes are present in 
                  ** the first column, if neither of these is true then the entry can
                  ** be accumulated
                  */ 
                  if ((i != 0) && ((j != 0) || (!start_col)))
                    
                     /* Accumulate by adding the current random number to the
                     ** sum of the previous random numbers in the same column.
                     */
                     if (complex_out) {
                        CM_total->el[i][j].real = CM_total->el[i-1][j].real + temp_rand;
                        CM_total->el[i][j].imaginary = CM_total->el[i-1][j].imaginary + temp_complex;

                        /* See if we should square the accumulated value */
                        if (square_it) {
                           square_complex(&CM_total->el[i][j],&CM_sqtotal->el[i][j]);
                         
			}
		     } else {
                        /* Just add a normal number */
                        M_total->el[i][j] = M_total->el[i-1][j] + temp_rand;
                        if (square_it) 
                           M_sqtotal->el[i][j] = M_total->el[i][j] * M_total->el[i][j];
		     }
                  else
                     /* Do not accumulate this element, it is either an index
                     ** or the first row of an object, so just assign it to
		     ** the matrix.
                     */
                     if (complex_out) {
                        CM_total->el[i][j].real = temp_rand;
                        CM_total->el[i][j].imaginary = temp_complex;
                        
                        if (square_it) {
                           square_complex(&CM_total->el[i][j],&CM_sqtotal->el[i][j]);
			                             
			}
		     } else {
                        M_total->el[i][j] = temp_rand;
                        if (square_it)
                           M_sqtotal->el[i][j] = temp_rand * temp_rand;
		     }
                  break;

             case ADD_ROWS:
                  /* This will add up each element in a row, this involves adding
                  ** up the previous columns.
                  */
                  if (j != start_col)
                     if (complex_out) {
                        CM_total->el[i][j].real = CM_total->el[i][j-1].real + temp_rand;
                        CM_total->el[i][j].imaginary = CM_total->el[i][j-1].imaginary + temp_complex;
                        if (square_it) {
                           square_complex(&CM_total->el[i][j],&CM_sqtotal->el[i][j]);
               
                        }
		     } else {
                        M_total->el[i][j] = M_total->el[i][j-1] + temp_rand;
                        if (square_it)
                           M_sqtotal->el[i][j] = M_total->el[i][j] * M_total->el[i][j];
		     }
                  else
                     /* Since this is the first column then just assign the
                     ** value.
                     */
                     if (complex_out) {
                        CM_total->el[i][j].real = temp_rand;
                        CM_total->el[i][j].imaginary = temp_complex;
                        if (square_it) {
                           square_complex(&CM_total->el[i][j],&CM_sqtotal->el[i][j]);
                           
			}
		     } else {
                        M_total->el[i][j] = temp_rand;
                        if (square_it)
                           M_sqtotal->el[i][j] = temp_rand * temp_rand;
		     }
                     
                  break;

             default: 
                  /* Accumulate was set but neither rows nor cols specified */
                  if (complex_out) {
                        CM_total->el[i][j].real = temp_rand;
                        CM_total->el[i][j].imaginary = temp_complex;
		   } else
                        M_total->el[i][j] = temp_rand;
                  break;

          } /* end switch */

	} else { /* Not accumulating */
              
            if (complex_out) {
               CM->el[i][j].real = temp_rand;
               CM->el[i][j].imaginary = temp_complex;
            } else
               M->el[i][j] = temp_rand;

	} /* Endif accumulate */ 

        /* Check we should accumulate across objects, this
        ** means that we have to add the _total object which contains
        ** the sum for that object to the _global object which is the
        ** grand total for all objects so far. As above there are
        ** seperate cases for squared entries.
        */
        if (accumulate & ADD_OBJECTS) 
           if (complex_out) {
              if (square_it) {
                 CM_global->el[i][j].real += CM_sqtotal->el[i][j].real;
                 CM_global->el[i][j].imaginary += CM_sqtotal->el[i][j].imaginary;
	      } else {
                 CM_global->el[i][j].real += CM_total->el[i][j].real;
                 CM_global->el[i][j].imaginary += CM_total->el[i][j].imaginary;
	      }
	   } else
              if (square_it)
                 M_global->el[i][j] += M_sqtotal->el[i][j];
              else
                 M_global->el[i][j] += M_total->el[i][j];
           
       } /* end col for loop */
       
    } /* end row for loop */

   /* The only difference between a vector and a matrix is in
	mathematica and html output */
   if (!((mathematica_out || html_out) && (object_type == VECTOR)))  {
      /* If we are doing an average only output 1 object */
      if (!average) {
        if (mathematica_out)
           if (complex_out)
              put_complex_float_Mathematica_matrix(*CM_temp,out);
           else
              put_Mathematica_matrix(*M_temp,out);
        else if (html_out)
           if (complex_out)
              put_complex_float_HTML_matrix(*CM_temp,out);
           else
              put_HTML_matrix(*M_temp,out);
        else
           if (complex_out)
              put_complex_float_matrix(*CM_temp,out);
           else
              put_matrix(*M_temp,out);
      }
   } else {
      /* Since this object is really a vector and we want to print it in
      ** mathematica or HTML format then copy it to a vector object before
      ** doing the output to get the braces right.
      */
      if (!average) {
         if (complex_out) {
            for (i=0;i<matrix_cols;i++) {
                CV->el[i].real = CM_temp->el[0][i].real;
                CV->el[i].imaginary = CM_temp->el[0][i].imaginary;
            } 
	    if (mathematica_out) put_complex_float_Mathematica_vector(*CV,out);
	    else if (html_out) put_complex_float_HTML_vector(*CV,out);
         } else {
           for (i=0;i<matrix_cols;i++)
               V->el[i] = M_temp->el[0][i];
	    if (mathematica_out) put_Mathematica_vector(*V,out);
	    else if (html_out) put_HTML_vector(*V,out);
	 }
      }         
  }    
   
} /* end do_matrix */

/*
** output_average()
**
** This function will dump out the last object it
** is used when the -average option was specified.
**
** 
*/
#ifndef _NO_PROTO
output_average(int object_count, int matrix_rows, int matrix_cols)
#else
output_average(object_count, matrix_rows, matrix_cols)
int object_count,matrix_rows,matrix_cols;
#endif

{
    int i,j;

    switch (object_type) {

       case SCALER:
          if (complex_out)
             fprintf(out,"%f+%fI\n",(total_complex.real/object_count),
                                     (total_complex.imaginary/object_count));
          else
             fprintf(out,"%f\n",(total/object_count));
          break;

       case VECTOR:
       case MATRIX:
           
           /* Compute an average */
           for (i=0;i<matrix_rows;i++) {
               for (j=start_col;j<matrix_cols;j++) {
                   if (complex_out) {
                      CM_global->el[i][j].real = CM_global->el[i][j].real / object_count;
                      CM_global->el[i][j].imaginary = CM_global->el[i][j].imaginary / object_count;
		    } else
                      M_global->el[i][j] = M_global->el[i][j] / object_count;
		}
	   }
           
           /* Put it out */
           if (mathematica_out)
              if (complex_out)
                  if (object_type == VECTOR) { 
                     for (i=0;i<matrix_cols;i++) {
                         CV->el[i].real = CM_global->el[0][i].real;
                         CV->el[i].imaginary = CM_global->el[0][i].imaginary;
	             } 
                     put_complex_float_Mathematica_vector(*CV,out);
		  } else
                     put_complex_float_Mathematica_matrix(*CM_global,out);
              else 
                  if (object_type == VECTOR) {
                     for (i=0;i<matrix_cols;i++) 
                         V->el[i] = M_global->el[0][i];
                     put_Mathematica_vector(*V,out);
		  } else
                     put_Mathematica_matrix(*M_global,out);
           else if (html_out)
              if (complex_out)
                  if (object_type == VECTOR) { 
                     for (i=0;i<matrix_cols;i++) {
                         CV->el[i].real = CM_global->el[0][i].real;
                         CV->el[i].imaginary = CM_global->el[0][i].imaginary;
	             } 
                     put_complex_float_HTML_vector(*CV,out);
		  } else
                     put_complex_float_HTML_matrix(*CM_global,out);
              else 
                  if (object_type == VECTOR) {
                     for (i=0;i<matrix_cols;i++) 
                         V->el[i] = M_global->el[0][i];
                     put_HTML_vector(*V,out);
		  } else
                     put_HTML_matrix(*M_global,out);

           else
               if (complex_out)
                  put_complex_float_matrix(*CM_global,out);
               else
                  put_matrix(*M_global,out);

           break;
     }
}

#ifdef AIX32
/*
** aix_random()
**
** Use the AIX random() function to generate a random number.
**
** from -aixrand command line option.
*/
double aix_random()
{
       double dtemp;

       dtemp = (double)random();

          
       dtemp = (double)(dtemp * MAX_NUM_INV);

       return((double)dtemp);
}
#endif

/*
** local_ranf()
**
** This returns the correct type from the ranf function.
**
** ranf_really is the C implementation of L'Ecuyer (-ranf )
*/
double local_ranf()
{
     float dtemp;
     dtemp = ranf_really();

     return((double)dtemp);
}

/*
** local_uni()
**
** This returns the correct type from the uni function.
**
** 
*/
double local_uni()
{
             
     return((double)UNI_RAND(0));
           
}

/*
** local_apst()
**
** from the -griff option
*/
double local_apst()
{
     float tempd;
    
     
     tempd = APST_RAND();

     return(tempd);
}

/*
** local_acm()
**
** from the -acm option
*/
double local_acm()
{
     return((double)ACM_RAND());
}

/*
** ranf()
** 
** This is the function that gets called from each of the
** various distributions (except -flat).
*/
float ranf()
{
      float temp;

      temp = (float) (*get_number)();

      return(temp);
}

/*
** local_marin()
** 
** This is the function that gets called when the
** -marin option is specified.
**
*/
double local_marin()
{
  float buf;
  int temp = 1;

  MAR_RAND(&buf,&temp);

  return((double)buf);
}


/*
** local_rcarry_set()
**
** This sets the initial array for the rcarry 
** function by setting it to 24 random numbers.
** This is the setup for the -rcarry generator option.
*/
#ifndef _NO_PROTO
local_rcarry_set(long seed)
#else
local_rcarry_set(seed)
long seed;
#endif
{
       float seedv[24];
       int i;

       srand48(seed);

       for (i=0;i<24;i++) {
           seedv[i] = drand48();
	}

        RCARRY_SET(seedv);
}

/*
** local_rcarry()
**
** This is the function that gets called when the -rcarry
** option was specified.
**
*/
double local_rcarry()
{
       float buf;
       int temp = 1;

       RCARRY_RAND(&buf,&temp);

       return((double)buf);
}


