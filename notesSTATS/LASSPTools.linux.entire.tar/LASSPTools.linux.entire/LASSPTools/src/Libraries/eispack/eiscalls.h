#include <stdio.h>
#include <stdlib.h>
#include "math_io.h"

typedef struct {
  int     Wsize;
  int     Isize;
  int     *ipvt;
  double  *work;

} WorkSpace;

#ifndef UNDERSCOREFORTRAN

void balanc();
void elmhes();
void eltran();
void hqr2();
void balbak();

void hqr();

void tred2();
void tql2();

void tred1();
void tqlrat();

void cbal();
void corth();
void comqr2();
void cbabk2();

void comqr();

void htridi();
void htribk();

#else

void balanc_();
void elmhes_();
void eltran_();
void hqr2_();
void balbak_();

void hqr_();

void tred2_();
void tql2_();

void tred1_();
void tqlrat_();

void cbal_();
void corth_();
void comqr2_();
void cbabk2_();

void comqr_();

void htridi_();
void htribk_();

#endif


void real_eigen();
void real_values();
void real_vectors();

void symmetric_eigen();
void symmetric_values();
void symmetric_vectors();

void complex_eigen();
void complex_values();
void complex_vectors();

void hermitian_eigen();
void hermitian_values();
void hermitian_vectors();

#ifdef _NO_PROTO
#define TRUE 1
#define FALSE 0
#define CLOCKS_PER_SEC 1e6
#endif
