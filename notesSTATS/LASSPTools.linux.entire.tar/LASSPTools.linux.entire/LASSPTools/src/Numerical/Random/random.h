/*
** Definitions, prototypes and externs.
**
*/

extern double drand48();
extern void srand48();
extern double aix_random();
extern float ranf();
extern double local_ranf();
extern void square_complex();
extern double local_uni();
extern double local_apst();
extern double local_acm();
extern double local_marin();
extern double local_rcarry();


#ifdef UNDERSCOREFORTRAN
#define UNI_RAND uni_
#define APST_SET setrandom_
#define APST_RAND apstrand_
#define ACM_RAND uniform_
#define ACM_SET setuniform_
#define MAR_SET rmarin_
#define MAR_RAND ranmar_
#define RCARRY_SET rcarryset_
#define RCARRY_RAND rcarry_
#else
#define UNI_RAND uni
#define APST_SET setrandom
#define APST_RAND apstrand
#define ACM_RAND uniform
#define ACM_SET setuniform
#define MAR_SET rmarin
#define MAR_RAND ranmar
#define RCARRY_SET rcarryset
#define RCARRY_RAND rcarry
#endif

extern float UNI_RAND();
extern void  APST_SET();
extern float APST_RAND();
extern void ACM_SET();
extern float ACM_RAND();
extern void MAR_SET();
extern float MAR_RAND();
extern void RCARRY_SET();
extern float RCARRY_RAND();


#define MAX_NUM 2147483561L
#define MAX_NUM_INV 4.656613057E-10

typedef enum RAN_TYPE {DRAND48,RANF,AIX_RANDOM,UNI,GRIFF,ACM,MARIN,RCARRY} RANDOM_TYPE;
typedef enum OBJ_TYPE {SCALER,VECTOR,MATRIX} OBJECT_TYPE;
typedef enum ACC_TYPE {NONE=0,ADD_ELEMENTS=1,ADD_ROWS=2,ADD_COLS=4,ADD_OBJECTS=8} ACCUMULATE_TYPE;

/*
** This defines the kinds of random distributions that can be generated
*/
typedef enum DIST_TYPE {FLAT,
                        NORMAL,
                        EXPONENTIAL,
                        CHISQUARE,
                        POISSON,
                        BINOMIAL,
                        F } DISTRIBUTION_TYPE;

/*
** These are prototypes for functions in random 
*/ 
extern double (*get_number) ();


/*
** The following are global data items.
*/
extern FILE *out;
extern boolean adjust_range;
extern float gaussian_width;
extern float flat_min;
extern float flat_max;
extern int vector_size;
extern int matrix_rows;
extern int  matrix_cols;
extern boolean adjust_range;
extern boolean square_it;
extern int start_col;
extern float exp_av;
extern boolean complex_out;
extern DISTRIBUTION_TYPE distribution;
extern Matrix *M,*M_total,*M_sqtotal,*M_global;
extern Vector *V,*V_total,*V_sqtotal;
extern ComplexFloatVector *CV,*CV_total,*CV_sqtotal;
extern ComplexFloatMatrix *CM,*CM_total,*CM_sqtotal,*CM_global;
extern boolean mathematica_out;
extern boolean html_out;
extern boolean average;
extern float norm_av;
extern float norm_sigma;
extern OBJECT_TYPE object_type;
extern float bin_prob;
extern long bin_trials;
extern float pos_av;
#define DEBUG yes








