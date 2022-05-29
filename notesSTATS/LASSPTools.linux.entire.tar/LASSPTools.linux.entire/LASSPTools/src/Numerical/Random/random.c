/*
** Random
**
** This program generates random numbers which can be output as vectors, 
** matrices or scalers. The output objects may also be accumulated. The program
** uses core random number generators from documented sources and uses the
** distribution functions from the ranlib package. The ranlib package was modified
** slightly to support:
**      - old style K&R parameter passing
**      - indirectly calling the core random number generator so that 
**        other generators could be combined with various distributions.
**
** Orginally the ranlib package called a core generator based on the algorithm
** by L'Ecuyer. The package can now use one of several generators based on what
** the command line argument was.
**
** The following options are recognized:
**
** -objects n - Generate n objects
** -vector m - Specify that each object is to be a vector of length m
** -matrix j k - Specify that each object is to be a matrix of j rows by k cols.
** -binary_out - The data is to be output in binary format
** -ascii_out - The data is to be output as ascii (the default)
** -file_out filename - Send the data to the specified filename
** -h - print help message
** -help - print help message
** -Mathematica_out - Put data out in Mathematica format (curly braces)
** -HTML_out - Put data out in HTML Table format (curly braces)
** -index - Output an index number in front of each random row (if matrix or vector)
**     or in front of each number if the output is a scaler object.
** -square - 
** -complex - generate complex numbers in the form A+BI
** -accumulate cols - accumulate cols of a matrix.
** -accumulate objs - accumulates the same elements across objects
**
** The following options are recognized for various distribution types:
**
** -flat min max - Generate a uniform distribution between min and max
** -gaussian sd [av] - Generate a distribution with sd standard deviation and av average,
**     the average is assumed to be zero unless specified.
** -exp av - Generate an exponential distribution with av average
** -poisson av - Generate a poisson distribution with average av.
** -binomial prob [trials] - Generate a binomial distribution with the probability of 
**     success being prob. The optional number of trials may be specified.
**
** The following options are recognized for various random number generators:
**  
** -drand48 - Use the drand48 random number generator as the core generator.
** -aixrand - Use the Aix system supplied random number generator.
** -nbs - From National Bureau of Standards 
** -marin - From Marsaglia and Zaman
** -rcarry - F. James, from Marsaglia and Zaman
** -griff - From Applied statistics
** -ranf - C implementation of L'Ecuyer algorithm
** -acm - L'Ecuyer algorithm 
**
** 12-4-92 - Jim Frankenberger
**
*/

#include <stdio.h>
#include "math_io.h"
#include "Boolean.h"
#include <stdlib.h>
#include "random.h"
#include "ranlib.h"

FILE *out;
double (*get_number) (); 
float gaussian_width = 0;
float flat_min = 0;
float flat_max = 1;
int vector_size = 0;     
int matrix_rows = 0; 
int  matrix_cols = 0; 
int start_col = 0;
long bin_trials = 0;
float bin_prob = 0;
boolean adjust_range = false; 
boolean square_it = false;
float exp_av = 0;
float pos_av = 0;
boolean complex_out = false;
DISTRIBUTION_TYPE distribution = FLAT;
Vector *V,*V_total,*V_sqtotal;
Matrix *M,*M_total,*M_sqtotal,*M_global;
ComplexFloatMatrix *CM,*CM_total,*CM_sqtotal,*CM_global;
ComplexFloatVector *CV,*CV_total,*CV_sqtotal;
boolean mathematica_out = false;
boolean html_out = false;
boolean average = false;
float norm_av = 0.0;
float norm_sigma = 1.0;
OBJECT_TYPE object_type = SCALER;

/*
** main()
**
**
*/
#ifndef _NO_PROTO
void main(int argc, char **argv)
#else
void main(argc, argv)
int argc;
char **argv;
#endif
{
   int i, j, binary_in, binary_out,temp_size,temp;

   /* Some data for initialization of Aix random generator */
#ifdef AIX32
   char state[50] = "sgha;hehgigbvfmb34904534509345uyt4tklhfleher0yer"; 
#endif
   char *ch_ptr;

   float temp_fl,test_it;
 
   double temp_rand;
   
   boolean good_option = false;
   boolean put_index = false;      /* Set to true if an index is output before each number */    
   boolean bad_option = false;     /* Keep track of how the input parsing went */

   /* Set up some inital seeds, these may be cahnged from the command line */
   long seed = 8375629L;
   long seed1 = 178463L;
   long seed2 = 1234897L;
   long seed3 = 7853L;
   long seed4 = 98415L;


   RANDOM_TYPE generator = DRAND48;    /* Use the drand48 function call in unix */
   
   int object_count = 1;               /* Put out one object by default       */
   
   ACCUMULATE_TYPE accumulate = NONE; /* Do not accumulate anything   */

   out = stdout;              /* Default output is stdout             */
   binary_in = 0;             /* Output is in ascii                   */
   binary_out = 0;            /* Input is in ascii                    */

   get_number = drand48;      /* Set up the default generator to call */ 
   
   /* We at least need an object count */
   if (argc < 2) {
      object_count = 1;
   }      
      
    /* read arguments and set flags */
    i = 1;
    while (i < argc) {
   
        /* help */
        if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"-help")==0)) help();

     
        /* binary output */
        if(strcmp(argv[i],"-binary_out") == 0){
                binary_out = 1; good_option = true;
		
        }
        /* ascii output */
        if(strcmp(argv[i],"-ascii_out") == 0){
                binary_out = 0; good_option = true;
		
		}
        if(strcmp(argv[i],"-file_out") == 0){
		out = fopen(argv[++i], "w");
		if (out==NULL) 
		   fprintf(stderr,"bad output file %s\n", argv[i]);
		
		}
        /* Mathematica output */
        if(strcmp(argv[i],"-Mathematica_out") == 0){
                mathematica_out = true;
            
	}
        /* HTML output */
        if(strcmp(argv[i],"-HTML_out") == 0){
                html_out = true;

	}

        /* Average objects */
        if (strcmp(argv[i],"-average") == 0) {
           average = true;
           accumulate |= ADD_OBJECTS;
	}

        /* seed Number */
        if (strcmp(argv[i],"-seed") == 0) {
           sscanf(argv[++i],"%ld",&seed);
           
	 }

        /* accumulate */
        if (strcmp(argv[i],"-accumulate") == 0) {
           accumulate |= ADD_ELEMENTS;
          
           /* Check for rows being specified */
           if (strcmp(argv[i+1],"rows") == 0) 
              accumulate |= ADD_ROWS;
           else 
              /* Check for cols */
              if (strcmp(argv[i+1],"cols") == 0)
                 accumulate |= ADD_COLS;
              else
                 /* Check for objects */
                 if (strcmp(argv[i+1],"objects") == 0)
                    accumulate |= ADD_OBJECTS; 
         
        }
     
        /* gaussian */
        if (strcmp(argv[i],"-gaussian") == 0) {
           
           sscanf(argv[++i],"%f",&norm_sigma);
           
           /* Check if 2 arguments are specified */
           ch_ptr = argv[i+1];
           if (*ch_ptr == '-') {
              /* Check the next character */
              if ((*(ch_ptr+1) >= 0) && (*(ch_ptr+1) <= 9)) {
                 /* Assume it is another number */
                 sscanf(argv[++i],"%f",&norm_av);
	      } 
           } else {
              /* Assume there are two arguments */
              sscanf(argv[++i],"%f",&norm_av);
	   }
           distribution = NORMAL;
        }

        /* Exponential */
        if (strcmp(argv[i],"-exp") == 0) {
           sscanf(argv[++i],"%f",&exp_av);
           distribution = EXPONENTIAL;
        }

        /* Poisson */
        if (strcmp(argv[i],"-poisson") == 0) {
           sscanf(argv[++i],"%f",&pos_av);
           distribution = POISSON;
	}

        /* Binomial */
        if (strcmp(argv[i],"-binomial") == 0) {
           sscanf(argv[++i],"%f",&bin_prob);

           /* Check if 2 arguments are specified */
           ch_ptr = argv[i+1];
           if (*ch_ptr == '-') {
              /* Check the next character */
              if ((*(ch_ptr+1) >= 0) && (*(ch_ptr+1) <= 9)) {
                 /* Assume it is another number */
                 sscanf(argv[++i],"%ld",&bin_trials);
	      } 
           } else {
              /* Assume there are two arguments */
              sscanf(argv[++i],"%ld",&bin_trials);
	   }
           distribution = BINOMIAL;
	}
  
        /* flat */
        if (strcmp(argv[i],"-flat") == 0) {
           sscanf(argv[i+1],"%f",&flat_min);
           sscanf(argv[i+2],"%f",&flat_max);
           adjust_range = true;
           i+=2;
        } 

        /* vector */
        if (strcmp(argv[i],"-vector") == 0) {
           sscanf(argv[++i],"%d",&vector_size);
           object_type = VECTOR;

	 }

        /* matrix */
        if (strcmp(argv[i],"-matrix") == 0) {
           sscanf(argv[++i],"%d",&matrix_rows);
           sscanf(argv[++i],"%d",&matrix_cols);
           object_type = MATRIX;

	 }
        
         /* Use drand48() */
        if (strcmp(argv[i],"-drand48")==0) {
             get_number = drand48;
                      
	}

        /* Use algorithm from Griffith-Hill Applied Stat. */
        if (strcmp(argv[i],"-griff") == 0) {
             get_number = local_apst;
             generator = GRIFF; 
	}

        /* Use random number generator uni() from NBS */
        if (strcmp(argv[i],"-nbs")==0) {
             get_number = local_uni;
             generator = UNI;
	}

        /* Use random number generator described in James */
	if (strcmp(argv[i],"-marin")==0) {
	    get_number = local_marin;
	    generator = MARIN;
	}

        /* Use another random generator described in James */
        if (strcmp(argv[i],"-rcarry") == 0) {
            get_number = local_rcarry;
	    generator = RCARRY;
	  }

#ifdef AIX32
        /* Use AIX random() function */
        if (strcmp(argv[i],"-aixrand")==0) {
             get_number = aix_random;
             generator = AIX_RANDOM;
	}

#endif
        /* Use ranf() function from ranlib */
        if (strcmp(argv[i],"-ranf")==0) {
             get_number = local_ranf;
             generator = RANF;
        } 

        /* Use the uniform() generator from ACM */
        if (strcmp(argv[i],"-acm") == 0) {
             get_number = local_acm;
             generator = ACM;
	}

        /* index */
        if (strcmp(argv[i],"-index")==0) {
             put_index = true;
         }

        /* Square */
        if (strcmp(argv[i],"-square")==0) {
           square_it = true;
        }

        /* number of objects to produce */
        if (strcmp(argv[i],"-objects")==0) {
           sscanf(argv[++i],"%d",&object_count);
	}

        /* if Complex numbers are output */
        if (strcmp(argv[i],"-complex")==0)
           complex_out = true;

        i++;
 
   }

    /*
    ** Check if we got through the command line arguments with any
    ** degree of success.
    */
    if (bad_option) {
       fprintf(stderr,"\nCheck Command line arguments...quitting\n");
       exit(-1);
    }
    
    /* Start off with the seed value, depending on the generator selected
    ** the setup many take one or more seeds.
    */
    switch (generator) {
       case DRAND48:
          srand48(seed);
          break;
       case RANF:
          setall(seed,seed1);
          break;
       case MARIN:
          MAR_SET(&seed);
          break;

       case RCARRY:
	  local_rcarry_set(seed);
          break;

#ifdef AIX32
       /* The state array defined at the beginning of this function is
       ** used as a default, this could be changed although there is
       ** no command line interface to set the state array.
       */
       case AIX_RANDOM:
          initstate((unsigned)seed,state,48);
          srandom((int)seed);
          break;
#endif
       /* Taking the mod of the seeds will insure that the
       ** numbers are in the correct range for the Fortran
       ** functions.
       */
       case UNI:
          seed = seed % 30000;
          UNI_RAND(&seed);
          break;
       case ACM:
          seed = seed % 30000;
          seed1 = seed1 % 30000;
          ACM_SET(&seed,&seed1);
          break;
       case GRIFF:
          seed = seed % 30000;
          seed1 = seed1 % 30000;
          seed2 = seed2 % 30000;
          
          APST_SET(&seed,&seed1,&seed2);
          break;
    }   
   

    /* Initialize any accumulating structures */
    if (object_type == VECTOR) { 
       
       /* Check if the first element should be an index */
              
       matrix_rows = 1;
       matrix_cols = temp_size = vector_size;

       if (put_index)
          temp_size = vector_size + 1;

       if (complex_out) 
          CV = create_complex_float_vector(temp_size);
       else 
          V = create_vector(temp_size);
    }

    /* 
    ** Set up any data structures to hold the matrix information
    ** A vector is just treated as a matrix with 1 row
    */
    if ((object_type == MATRIX) || (object_type == VECTOR)) {
       
       /* Check if the first element should be an index, this adds an extra column
       ** to each row, start_col indicates where the actual random numbers will 
       ** begin, col[0] becomes the index 1...N
       */
       if (put_index) {
          start_col = 1; matrix_cols++;
          
	}
     
       /* Allocate either a complex object or just a plain matrix */
       if (complex_out) 
          CM = create_complex_float_matrix(matrix_rows,matrix_cols);
       else
          M = create_matrix(matrix_rows,matrix_cols);
       
       
       /* Initialize all the elements to zero */
       for (i=0;i<matrix_rows;i++)
          for (j=0;j<matrix_cols;j++)
              if (complex_out) {
                 CM->el[i][j].real = 0;
                 CM->el[i][j].imaginary = 0;
              } else
                 M->el[i][j] = 0;

       /* If index is specified then fill them in once */
       if (put_index)
          for (i=0;i<matrix_rows;i++)
              if (complex_out) 
                 CM->el[i][0].real = i + 1;
              else
    
              M->el[i][0] = i+1;

       /* Accumulating needs another set of objects, the _total object holds
       ** the accumulted values within one objects, _sqtotal holds the squared
       ** version and _global holds the total across all objects.
       */
       if (accumulate) {
          if (complex_out) {
             CM_total = create_complex_float_matrix(matrix_rows,matrix_cols);
             if (square_it)
                CM_sqtotal = create_complex_float_matrix(matrix_rows,matrix_cols);
             if (accumulate & ADD_OBJECTS)
                CM_global = create_complex_float_matrix(matrix_rows,matrix_cols);
          } else {
             M_total = create_matrix(matrix_rows,matrix_cols);
             if (square_it)
                M_sqtotal = create_matrix(matrix_rows,matrix_cols);
             if (accumulate & ADD_OBJECTS)
                M_global = create_matrix(matrix_rows,matrix_cols);
	  }

          /* This section just initalizes the various accumulating 
          ** matrix elements to zero.
          */
          for (i=0;i<matrix_rows;i++)
              for (j=0;j<matrix_cols;j++)
                  if (complex_out) {
                     CM_total->el[i][j].real = 0;
                     CM_total->el[i][j].imaginary = 0;
                     if (square_it) {
                        CM_sqtotal->el[i][j].real = 0;
                        CM_sqtotal->el[i][j].imaginary = 0;
		     }
                     if (accumulate & ADD_OBJECTS) {
                        CM_global->el[i][j].real = 0;
                        CM_global->el[i][j].imaginary = 0;
                     }
		   } else {
                     M_total->el[i][j] = 0;
                     if (square_it)
                        M_sqtotal->el[i][j] = 0;
                     if (accumulate & ADD_OBJECTS)
                        M_global->el[i][j] = 0;
		   }
                             
          /* It would be possible to have an index + accumulate, this
          ** fills in the index in each of the accumulating matrices
          */
          if (put_index)
             for (i=0;i<matrix_rows;i++)
                 if (complex_out) {
                    CM_total->el[i][0].real = i + 1;
                    if (square_it)
                       CM_sqtotal->el[i][0].real = i+1;
                    if (accumulate & ADD_OBJECTS)
                       CM_global->el[i][0].real = i+1;
                 } else {
                    M_total->el[i][0] = i+1;
                    if (square_it)
                       M_sqtotal->el[i][0] = i+1;
                    if (accumulate & ADD_OBJECTS)
                       M_global->el[i][0] = i+1;
		 }
       }
    }
    

    /* This is the main loop of the program, each time
    ** through will produce one object of the requested type
    ** followed by a blank line.
    */
    for (i=0;i<object_count;i++) {
          switch (object_type) {
	     case SCALER:
                do_scaler(accumulate,put_index,i+1);
                break;
	     case VECTOR:
             case MATRIX:
                do_matrix(matrix_rows,matrix_cols,accumulate);
                break;
          }
                   
    }

    /* If we are doing an average then dump the last object */
    if (average)
       output_average(object_count,matrix_rows,matrix_cols);
    
       
}

/*
** help()
** 
** This function just prints outs the program options to standard Out
**
*/
help()
{
   
 fprintf(stderr,"\nUsage: random -objects n [-flat min max] [-matrix m n]\n");
 fprintf(stderr,"\t[-gaussian width] [-exp av] [-help] [-seed n] [-binary]\n");
 fprintf(stderr,"\t[-Mathematica_out] [-complex] [-fileout File]\n");
 fprintf(stderr,"\t[-HTML_out] [-complex] [-fileout File]\n");
 fprintf(stderr,"\t[-accumulate cols | rows] [-accumulate objects]\n");
 fprintf(stderr,"\t[-square] [-poisson av] [-binomial prob trials]\n");
 fprintf(stderr,"\t[-ranf] [-nbs] [-griff] [-random]\n\n");
 fprintf(stderr,"\t-objects n : How many random numbers,vectors or matrices to generate\n");
 fprintf(stderr,"\t-flat min max : Produce an even distribution between min and max\n");
 fprintf(stderr,"\t-square : Square each random number generated\n");
 fprintf(stderr,"\t-accumulate cols | rows : Accumulate all elements with the same\n");
 fprintf(stderr,"\t\t row or col within a matrix.\n");
 fprintf(stderr,"\t-vector n : Produce an vector object of size n\n");
 fprintf(stderr,"\t-matrix m n: Produce a matrix object with m rows and n columns\n");
 fprintf(stderr,"\t-gaussian sd width : Use random numbers from a normal distribution.\n");
 fprintf(stderr,"\t-exp av : Use random numbers from the exponential distribution\n");
 fprintf(stderr,"\t-poisson av : Generate numbers based on a poisson distribution\n");
 fprintf(stderr,"\t-binomial prob trials : Generate the number of successful events in a\n");
 fprintf(stderr,"\t\t distribution where the probability of success is prob for the indicated number\n");
 fprintf(stderr,"\t\tof trials.\n");
 fprintf(stderr,"\t-ranf : Use the random generator from ranlib package.\n");
 fprintf(stderr,"\t-nbs : Use the random genertator developed at National Bureau of Standards\n");
 fprintf(stderr,"\t-griff : Use the random generator in Griffith-Hill Applied Statistics\n");
 fprintf(stderr,"\t-random : Use the AIX random() function\n");
 fprintf(stderr,"\t-help : This message.\n");
 fprintf(stderr,"\t-seed n m: Supply a startup seed(s) to the random number generator.\n");
 fprintf(stderr,"\t-binary : Produce binary output\n");
 fprintf(stderr,"\t-Mathematica_out : Produce matrices and vectors with brackets.\n");
 fprintf(stderr,"\t-HTML_out : Produce matrices and vectors in HTML Table format.\n");
 fprintf(stderr,"\t-complex : Produce objects made up of complex elements\n");
 fprintf(stderr,"\t-fileout File : Send output to the specified file.\n");
 fprintf(stderr,"\t-accumulate objects : Sum the same elements in each matrix object.\n");
 fprintf(stderr,"\t-average : Print one object that is the average of all\n");


  /* Since the user asked for help lets not do anything else */
  exit(-1);
}





