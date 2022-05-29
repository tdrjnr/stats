/* This is a set of routines, designed to be general support for 	*
 * matrix and vector data structures.  At this point, the routines	*
 * primarily create, read, write, and destroy matrices in various 	*
 * ways.  We support ascii and binary modes, and automatically resize	*
 * the matrix to fit the input.						*/

/*****************************  VECTORS  ********************************/

/* The data structure for vectors is pretty obvious, except that	*
 * for now we store the allocated length as well as the data length.	*
 * This will probably be important for later matrix routines, when	*
 * we support matrix allocated sizes different from their data length	*
 * to avoid cache misses (IBM's "stride").				*
 * Remember that the same vector will start with V[0] in a Fortran 	*
 * routine and V[1] in a C routine.					*
 * WARNING: Either use alloc_vector, or initialize 			*
 *   V.data_reserved = NULL.						*
 *   We automatically reallocate space if there isn't enough, but since	*
 *   the vector will be initialized to garbage when it is declared,	*
 *   sometimes the garbage will fool the routines, which will then	*
 *   write to garbage pointers.						*/

/* In Numerical Recipes in C, the vectors point (perversely) to the     *
 * element before the vector begins, so that v[0] is illegal and v[1]   *
 * is the first element of the vector.                                  *
 * To support Numerical Recipes in C (probably a mistake), we include   *
 * COLSTART, which should be set to one to make the vectors start in    *
 * the wrong place.                                                     */
/* NOTICE: if you leave COLSTART zero, and pass a vector to             *
 * a Fortran subroutine, it will be perfectly OK: Fortran will          *
 * read as M.el[i] what the C routine thinks is M.el[i-1].              */


/*****************************  MATRICES  *******************************/

/* The matrix data type supports both the C and Fortran conventions:    *
 *                                                                      *
 * In C, the preferred representation of a matrix is as a list of       *
 * pointers to vectors.                                                 *
 * In Fortran, the representation is as a pointer to the first element  *
 * of a block of data.                                                  *
 *                                                                      *
 * WARNINGS: (1) Fortran matrix indices are the transpose of C indices  *
 *               so a Fortran program M.data(i,j) will be the C data    *
 *               element M[j][i].  The C element M.data[i,j] doesn't    *
 *               exist, because C doesn't know the size of the matrix.  *
 *           (2) In C, one can manipulate rows by shifting pointers.    *
 *               This will not change M.data at all!  Fortran--style    *
 *               manipulations will change the C matrix.  The printing  *
 *               routines use the C conventions.                        */


/* In mathematics and in Fortran, rows and columns of matrices start    *
 * at 1: M[1,1], M[1,2], ... .  (Actually, in Fortran they are actually *
 * stored as M[1,1], M[2,1], ... I think.)                              *
 * In C, rows and columns conventionally start at zero:                 *
 * M[0,0], M[0,1], ... .  Some users (i.e. the authors of Numerical     *
 * Recipes) prefer to stick with the math and Fortran conventions.  We  *
 * support both, through the variables ROWSTART and COLSTART, defined   *
 * to be the starting element of rows and columns.			*
 *                                                                      *
 * NOTICE: if you leave ROWSTART and COLSTART zero, and pass a matrix to*
 * a Fortran subroutine, the matrix will be perfectly OK: Fortran will  *
 * read as M.data[i,j] what the C routine thinks is M.data[j-1][i-1].   *
 * Inversion, determinants, and so forth are unchanged.  Right          *
 * eigenvectors become left eigenvectors, of course.                    * 
 *                                                                      *
 * Define ROWSTART and COLSTART to be zero for C convention, and one for*
 * Fortran.                                                             */

# define ROWSTART    0
# define COLSTART    0

#ifndef _NO_PROTO
void    error(char *error_text);
#else
void    error();
#endif

/*************************  ENUMERATE TYPES  ****************************/

enum   Type   {REAL, COMPLEX_DOUBLE, COMPLEX_FLOAT};

/***********************  COMPLEX STRUCTURES  ***************************/

typedef    struct {float real;
		   float imaginary;
} ComplexFloat;

typedef    struct {double real;
                   double imaginary;
} ComplexDouble;

/************************************************************************/
/*****************************  VECTORS  ********************************/
/************************************************************************/

/***************************  REAL DOUBLE  ******************************/

typedef    struct { int N;
                    double   *el;  /* Pointer to the elements */

/* Private: Space allocated */

                int data_reserved;
} Vector;

#ifndef _NO_PROTO
Vector *create_vector(int N);
int     get_vector(Vector *B, FILE *in);
int     get_binary_vector(Vector *B, FILE *in);
int     get_binary_int_vector(Vector *B, FILE *in);
void    put_vector(Vector V, FILE *out);
void    put_binary_vector(Vector V, FILE *out);
void    put_binary_int_vector(Vector V, FILE *out);
void    put_Mathematica_vector(Vector V, FILE *out);
#else
Vector *create_vector();
int     get_vector();
int     get_binary_vector();
int     get_binary_int_vector();
void    put_vector();
void    put_binary_vector();
void    put_binary_int_vector();
void	put_Mathematica_vector();
#endif
Vector *create_default_vector();

/************************  COMPLEX FLOAT (SINGLE)  ****************************/

typedef    struct { int N;
                    ComplexFloat   *el;  /* Pointer to the elements */

/* Private: Space allocated */

                int data_reserved;
} ComplexFloatVector;

#ifndef _NO_PROTO
int     get_complex_float_binary_vector(ComplexFloatVector *B, FILE *in);
int     input_Re_or_Im_float(int end_char, FILE *in, float *num);
void    get_complex_float_number(int end_char, FILE *in,
                                                float *real, float *imag);
void    put_complex_float_binary_vector(ComplexFloatVector V, FILE *out);
void    put_complex_float_element(float real, float imag, FILE *out,
                         char imaginary_char, char complex_sep,
                         char complex_begin, char complex_end);
void    put_formatted_complex_float_vector(ComplexFloatVector V, FILE *out,
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end);
void    put_complex_float_vector(ComplexFloatVector V, FILE *out);
void    put_complex_float_Mathematica_vector(ComplexFloatVector V,
                                                                FILE *out);
void    put_complex_float_ordered_pair_vector(ComplexFloatVector V,
                                                                FILE *out);
void    put_complex_float_simple_vector(ComplexFloatVector V, FILE *out);
ComplexFloatVector     *create_complex_float_vector(int N);
void    free_complex_float_vector(ComplexFloatVector* B);
int     get_complex_float_vector(ComplexFloatVector *B, FILE *in);
#else
int     get_complex_float_binary_vector();
int     input_Re_or_Im_float();
void    get_complex_float_number();
void    put_complex_float_binary_vector();
void    put_complex_float_element();
void    put_formatted_complex_float_vector();
void    put_complex_float_vector();
void    put_complex_float_Mathematica_vector();
void    put_complex_float_ordered_pair_vector();
void    put_complex_float_simple_vector();
ComplexFloatVector     *create_complex_float_vector();
void    free_complex_float_vector();
int     get_complex_float_vector();
#endif
ComplexFloatVector     *create_default_complex_float_vector();

/*************************  COMPLEX DOUBLE  *****************************/

typedef    struct { int N;
                    ComplexDouble   *el;  /* Pointer to the elements */

/* Private: Space allocated */

                int data_reserved;
} ComplexDoubleVector;

#ifndef _NO_PROTO
int	get_complex_double_binary_vector(ComplexDoubleVector *B, FILE *in);
int     input_Re_or_Im_double(int end_char, FILE *in, double *num);
void    get_complex_double_number(int end_char, FILE *in,
                                                double *real, double *imag);
void	put_complex_double_binary_vector(ComplexDoubleVector V, FILE *out);
void	put_complex_double_element(double real, double imag, FILE *out,
                         char imaginary_char, char complex_sep,
                         char complex_begin, char complex_end);
void	put_formatted_complex_double_vector(ComplexDoubleVector V, FILE *out, 
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end);
void	put_complex_double_vector(ComplexDoubleVector V, FILE *out);
void	put_complex_double_Mathematica_vector(ComplexDoubleVector V, 
								FILE *out);
void    put_complex_double_ordered_pair_vector(ComplexDoubleVector V,
								FILE *out);
void    put_complex_double_simple_vector(ComplexDoubleVector V, FILE *out);
ComplexDoubleVector	*create_complex_double_vector(int N);
void	free_complex_double_vector(ComplexDoubleVector* B);
int	get_complex_double_vector(ComplexDoubleVector *B, FILE *in);
#else
int	get_complex_double_binary_vector();
int     input_Re_or_Im_double();
void    get_complex_double_number();
void	put_complex_double_binary_vector();
void    put_complex_double_element();
void    put_formatted_complex_double_vector();
void	put_complex_double_vector();
void    put_complex_double_Mathematica_vector();
void    put_complex_double_ordered_pair_vector();
void    put_complex_double_simple_vector();
ComplexDoubleVector     *create_complex_double_vector();
void    free_complex_double_vector();
int     get_complex_double_vector();
#endif
ComplexDoubleVector	*create_default_complex_double_vector();

/*----------------------------------------------------------------------*/


/************************************************************************/
/*****************************  MATRICES  *******************************/
/************************************************************************/

/***************************  REAL DOUBLE  ******************************/

typedef    struct {
                int      rows;
                int      cols;

                double   **el;  /* C-style pointer to the elements */
                double   *data; /* Fortran-style pointer to the first element */

/* Private: Space allocated */

		int data_reserved;
		int pointers_reserved;

} Matrix;

/*----------------------------------------------------------------------*/


#ifndef _NO_PROTO
int     skip_white(int end_char,FILE *in);
void    error(char *error_text);
Matrix *create_matrix(int rows, int cols);
void    free_matrix(Matrix* A);
int     get_matrix(Matrix *A, FILE *in);
int     get_binary_matrix(Matrix *A, FILE *in);
int     get_binary_int_matrix(Matrix *A, FILE *in);
void    put_formatted_matrix(Matrix A, FILE *out,
			     char separator, char beg_char, char end_char);
void    put_matrix(Matrix A, FILE *out);
void    put_binary_matrix(Matrix A, FILE *out);
void    put_binary_int_matrix(Matrix A, FILE *out);
void    put_Mathematica_matrix(Matrix A, FILE *out);
void    check_symmetric(Matrix A);
void    transpose_square(Matrix A);
#else
int     skip_white();
void    error();
Matrix *create_matrix();
void	free_matrix();
int     get_matrix();
int     get_binary_matrix();
void	put_formatted_matrix();
void    put_matrix();
void    put_binary_matrix();
void	put_Mathematica_matrix();
void    check_symmetric();
void    transpose_square();
#endif
Matrix *create_default_matrix();

/************************  COMPLEX (SINGLE)  ****************************/

typedef    struct {
                int      rows;
                int      cols;

                ComplexFloat   **el;  /* C-style pointer to the elements */
                ComplexFloat   *data; /* Fortran-style pointer to the first element */

/* Private: Space allocated */

                int data_reserved;
                int pointers_reserved;

} ComplexFloatMatrix;

#ifndef _NO_PROTO
int     get_complex_float_binary_matrix(ComplexFloatMatrix *M, FILE *in);
void    put_complex_float_binary_matrix(ComplexFloatMatrix M, FILE *out);
void    put_formatted_complex_float_matrix(ComplexFloatMatrix M, FILE *out,
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end);
void    put_complex_float_matrix(ComplexFloatMatrix M, FILE *out);
void    put_complex_float_Mathematica_matrix(ComplexFloatMatrix M,
                                                                FILE *out);
void    put_complex_float_ordered_pair_matrix(ComplexFloatMatrix M,
                                                                FILE *out);
void    put_complex_float_simple_matrix(ComplexFloatMatrix M, FILE *out);
ComplexFloatMatrix     *create_complex_float_matrix(int rows, int cols);
void    free_complex_float_matrix(ComplexFloatMatrix *A);
int     get_complex_float_matrix(ComplexFloatMatrix *A, FILE *in);
#else
int     get_complex_float_binary_matrix();
void    put_complex_float_binary_matrix();
void    put_formatted_complex_float_matrix();
void    put_complex_float_matrix();
void    put_complex_float_Mathematica_matrix();
void    put_complex_float_ordered_pair_matrix();
void    put_complex_float_simple_matrix();
ComplexFloatMatrix     *create_complex_float_matrix();
void    free_complex_float_matrix();
int     get_complex_float_matrix();
#endif
ComplexFloatMatrix     *create_default_complex_float_matrix();

/*----------------------------------------------------------------------*/

/*************************  COMPLEX DOUBLE  *****************************/

typedef    struct {
                int      rows;
                int      cols;

                ComplexDouble   **el;  /* C-style pointer to the elements */
                ComplexDouble   *data; /* Fortran-style pointer to the first element */

/* Private: Space allocated */

                int data_reserved;
                int pointers_reserved;

} ComplexDoubleMatrix;

#ifndef _NO_PROTO
int     get_complex_double_binary_matrix(ComplexDoubleMatrix *M, FILE *in);
void    put_complex_double_binary_matrix(ComplexDoubleMatrix M, FILE *out);
void    put_formatted_complex_double_matrix(ComplexDoubleMatrix M, FILE *out,
        char separator, char begin_char, char end_char, char imaginary_char,
        char complex_sep, char complex_begin, char complex_end);
void    put_complex_double_matrix(ComplexDoubleMatrix M, FILE *out);
void    put_complex_double_Mathematica_matrix(ComplexDoubleMatrix M,
                                                                FILE *out);
void    put_complex_double_ordered_pair_matrix(ComplexDoubleMatrix M,
                                                                FILE *out);
void    put_complex_double_simple_matrix(ComplexDoubleMatrix M, FILE *out);
ComplexDoubleMatrix     *create_complex_double_matrix(int rows, int cols);
void    free_complex_double_matrix(ComplexDoubleMatrix *A);
int     get_complex_double_matrix(ComplexDoubleMatrix *A, FILE *in);
#else
int     get_complex_double_binary_matrix();
void    put_complex_double_binary_matrix();
void    put_formatted_complex_double_matrix();
void    put_complex_double_matrix();
void    put_complex_double_Mathematica_matrix();
void    put_complex_double_ordered_pair_matrix();
void    put_complex_double_simple_matrix();
ComplexDoubleMatrix     *create_complex_double_matrix();
void    free_complex_double_matrix();
int     get_complex_double_matrix();
#endif
ComplexDoubleMatrix     *create_default_complex_double_matrix();
/*----------------------------------------------------------------------*/

/************************************************************************/
/********************  COMPLEX NUMBER ROUTINES  *************************/
/************************************************************************/

/*************************  COMPLEX DOUBLE  *****************************/

#ifndef _NO_PROTO
ComplexDouble polar(double modulus, double argument);
double	mod(ComplexDouble z);
double	arg(ComplexDouble z);
#else
ComplexDouble polar();
double	mod();
double	arg();
#endif

/*----------------------------------------------------------------------*/
