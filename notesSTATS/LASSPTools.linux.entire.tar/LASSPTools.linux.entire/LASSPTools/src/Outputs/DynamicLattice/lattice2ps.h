#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

typedef struct
{
    int nx;
    int ny;
    int offset;
} LATTICE;

EXTERN LATTICE lattice;

typedef struct
{
    double originx;
    double originy;
    double scalef;
    double scalefx;
    double scalefy;
    double size;
    double sizex;
    double sizey;
    double rotate;
} IMAGE;

EXTERN IMAGE image;

typedef struct
{
    int mode;
    int grid;
    int defc;
    int pix;
    int gray;
    double gmin;
    double gmax;
    double gridg;
    int Ncolors;
    double bgd;
    double *rcolor;
    double *gcolor;
    double *bcolor;
    char mapf[64];
    FILE *mapfd;
    int border;
} GRAPHICS;

EXTERN GRAPHICS graphics;

typedef struct
{
    int binary;
    int mode;
} INPUT;

EXTERN INPUT input;

typedef struct
{
    double zmin;
    double zmax;
    double zrange;
} ZSCALE;

EXTERN ZSCALE zscale;

typedef struct
{
    int nx;
    int ny;
    int defc;
    int pix;
    int map;
    int **z;
    int zscale;
    int trans;
} SET;

EXTERN SET set;
