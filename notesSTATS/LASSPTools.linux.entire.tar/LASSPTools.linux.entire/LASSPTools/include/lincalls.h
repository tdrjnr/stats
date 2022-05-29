/* This is the header file for the det program that calls Linpack */

typedef struct {
  int     Wsize;
  int     Isize;
  int     *ipvt;
  double  *work;

} WorkSpace;

#ifndef UNDERSCOREFORTRAN
/* These are the BLAS fortran routine */
void dcopy();
void zcopy();

/* These are the Linpack fortran routines */

/* for general real matrices */
void dgefa();
void dgeco();
void dgesl();
void dgedi();

/* for symetric real matrices */
void dsifa();
void dsico();
void dsisl();
void dsidi();

/* for triagular real matrices */
void dtrco();
void dtrsl();
void dtrdi();

/* for tridiagonal real matrices */
/* void dgtsl(); */

/* for general complex matrices */
void zgefa();
void zgeco();
void zgesl();
void zgedi();

/* for symetric complex matrices */
void zsifa();
void zsico();
void zsisl();
void zsidi();

/* for hermitian matrices */
void zhifa();
void zhico();
void zhisl();
void zhidi();

/* for triangular complex matrices */
void ztrco();
void ztrsl();
void ztrdi();

/* for tridiagonal complex matrices */
/* void zgtsl(); */

#else
/* These are the BLAS fortran routine */
void dcopy_();
void zcopy_();

/* These are the Linpack fortran routines */

/* for general real matrices */
void dgefa_();
void dgeco_();
void dgesl_();
void dgedi_();

/* for symetric real matrices */
void dsifa_();
void dsico_();
void dsisl_();
void dsidi_();

/* for triagular real matrices */
void dtrco_();
void dtrsl_();
void dtrdi_();

/* for tridiagonal real matrices */
/* void dgtsl_(); */

/* for general complex matrices */
void zgefa_();
void zgeco_();
void zgesl_();
void zgedi_();

/* for symetric complex matrices */
void zsifa_();
void zsico_();
void zsisl_();
void zsidi_();

/* for hermitian matrices */
void zhifa_();
void zhico_();
void zhisl_();
void zhidi_();

/* for triangular complex matrices */
void ztrco_();
void ztrsl_();
void ztrdi_();

/* for tridiagonal complex matrices */
/* void zgtsl_(); */
#endif

#ifdef _NO_PROTO
#define CLOCKS_PER_SEC 1e6
#endif
