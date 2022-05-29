/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 
/*

   DynamicLattice: X-windows program to animate dynamics on square 
   or rectangular lattices

   see enclosed man page (DynamicLattice.l) and online help 
   via "DynamicLattice -h"

   Copyright (C) 1990 Christopher R. Myers (with the exception of
   math_io.c, from the LASSPTools distribution).  All rights reserved.
   Feel free to copy, modify and redistribute this program, but do not
   sell it.  Include this notice in any copy.

   written: 10/90; latest revision: 11/92 (v. 3.0)

*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 

#ifdef MAIN
#define EXTERN
#else 
#define EXTERN extern
#endif

typedef struct
{
    int D;
    int nx;
    int ny;
    int offset;
} LATTICE;

EXTERN LATTICE lattice;

typedef struct
{
    int mode;
    int grid;
    int defc;
    int pix;
    int gray;
    char mapf[64];
    FILE *mapfd;
    int printcolor;
    int textc;
    char title[128];
    int writez;
} GRAPHICS;

EXTERN GRAPHICS graphics;

typedef struct 
{
    int mode;
    int pause;
    int initpause;
    int interrupt;
    int frills;
} DYNAMICS;

EXTERN DYNAMICS dynamics;

typedef struct 
{
    int binary;
} INPUT;

EXTERN INPUT input;

typedef struct 
{
    double zmin;
    double zmax;
    double zrange;
} ZSCALE;

EXTERN ZSCALE zscale, z2scale;

typedef struct
{
    int nx;
    int ny;
    int defc;
    int textc;
    int pix;
    int map;
    int zscale;
    int z2scale;
    int **z;
} SET;

EXTERN SET set;


