/*-----------------------------------------------------------------------------
 * blend.c
 * this program takes from stdin duples (or triples with the -t option) 
 * and outputs a set of interpolated points, inserting a constant number
 * of additional duples (or triples) in between each pair of duples (or
 * triples).  The method of parabolic blending is used on the internal
 * intervals and the first and last intervals are simply interpolated
 * by parabolas.
 * options are:
 * 	-i #       where # is the number of points to insert, default 4
 * 	-t         for points with three dimensions
 * this is based on the algorithm in Rogers' Elements of Mathematics for
 * Computer Graphics and the variable names (as much as possible) and the
 * comments use his notation. 
 */

#include <math.h>
#include <stdio.h>
#define INSERT_DEF 4
#define TRUE 1

#ifndef _NO_PROTO
int readone(double *pf)
#else
int readone(pf)
double *pf;
#endif
{
        int fs;

        fs = fscanf(stdin,"%lf",pf);

        return(fs);
}

#ifndef _NO_PROTO
int getpoint(double thepoint[3], int thedim)
#else
int getpoint(thepoint, thedim)
double thepoint[3];
int thedim;
#endif
{
        int i, stat[3];

	stat[2] = TRUE;

	for (i=0; i<thedim; i++) 	
        	stat[i] = readone(&thepoint[i]);

        if (stat[0]==EOF || stat[1]==EOF || stat[2] == EOF)
                return(EOF);
        else
                return(stat[0] && stat[1] && stat[2]);
}

#ifndef _NO_PROTO
void printpoint(double thepoint[3], int thedim)
#else
void printpoint(thepoint, thedim)
double thepoint[3];
int thedim;
#endif
{
        
	if (thedim==2)
		printf("%f %f\n",thepoint[0],thepoint[1]);
	else 
		printf("%f %f %f\n",thepoint[0],thepoint[1],thepoint[2]);
}

/*----------------------------------------------------------------------------
 * scalevect()
 * scales a vector stored as an array of doubles
 */
#ifndef _NO_PROTO
void scalevect(double a[3], double factor, int dim, double ans[3])
#else
void scalevect(a, factor, dim, ans)
double a[3];
double factor;
int dim;
double ans[3];
#endif
{
	int i;

	for (i=0; i<dim; i++)
		ans[i] = factor * a[i];
}

/*----------------------------------------------------------------------------
 * addvects()
 * vectorwise computes ans = a-b
 */
#ifndef _NO_PROTO
void addvects(double a[3], double b[3], int dim, double ans[3])
#else
void addvects(a,b,dim,ans)
double a[3];
double b[3];
int dim;
double ans[3];
#endif
{
        int i;

        for (i=0; i<dim; i++)
                ans[i] = a[i] + b[i];
}

/*----------------------------------------------------------------------------
 * subvects()
 * vectorwise computes ans = a-b
 */
#ifndef _NO_PROTO
void subvects(double a[3], double b[3], int dim, double ans[3])
#else
void subvects(a,b,dim,ans)
double a[3];
double b[3];
int dim;
double ans[3];
#endif
{
        double bp[3];

        scalevect(b,-1.0,dim,bp);
        addvects(a,bp,dim,ans);
}


/*----------------------------------------------------------------------------
 * dotvects()
 * computes vector dot product a*b
 */
#ifndef _NO_PROTO
double dotvects(double a[3], double b[3], int dim)
#else
double dotvects(a, b, dim)
double a[3];
double b[3];
int dim;
#endif
{
	int i;
	double ans = 0.0;

	for (i=0; i<dim; i++) 
		ans += a[i]*b[i];
	
	return(ans);
}

/*-----------------------------------------------------------------------------
 * makeQcoeffs()
 * this routine makes the coefficients necessary to evaluate the Q polynmial
 * through p4, p5, and p6.  the interval for interpolation is between p4
 * and p5.  the coefficients are -
 *       e = the distance from p4 to p6
 *      xp = the distance along p4p6 to the perp through p3
 *      sslope = the coefficient of (t/t0) in s as fn of t/t0
 */
#ifndef _NO_PROTO
void makeQcoeffs(double *p4, double *p5, double *p6, int dim,
			double *e, double *xp, double *sslope)
#else
void makeQcoeffs(p4, p5, p6,dim, e, xp, sslope)
double *p4;
double *p5;
double *p6;
int dim;
double *e;
double *xp;
double *sslope;
#endif


{
	double diffvect64[3], diffvect54[3];

	subvects(p6,p4,dim, diffvect64);

	*e = sqrt(dotvects(diffvect64, diffvect64,dim));

	subvects(p5,p4,dim, diffvect54);

	*sslope = dotvects(diffvect54, diffvect64, dim)/(*e);

	*xp = (*sslope)/(*e);
}

/*-----------------------------------------------------------------------------
 * makePcoeffs()
 * this routine makes the coefficients necessary to evaluate the P polynmial
 * through p3, p4, and p5.  the interval for interpolation is between p4
 * and p5.  the coefficients are -
 *       d = the distance from p3 to p5
 *       x = the distance along p4p6 to the perp through p3
 *      rslope = the coefficient of (t/t0) in r as fn of t/t0
 *	rint = the constant in r as a fn of t/t0
 */
#ifndef _NO_PROTO
void makePcoeffs(double *p3, double *p4, double *p5, int dim,
                        double *d, double *x, double *rslope, double *rint)
#else
void makePcoeffs( p3, p4, p5,  dim, d, x, rslope, rint)
double *p3;
double *p4;
double *p5;
int dim;
double *d;
double *x;
double *rslope;
double *rint;
#endif
{
        double diffvect43[3], diffvect53[3], diffvect54[3];
	double dsquared;

        subvects(p5,p3,dim,diffvect53);
	
	dsquared = dotvects(diffvect53, diffvect53, dim);
        *d = sqrt(dsquared);

        subvects(p5,p4,dim,diffvect54);
	subvects(p4,p3,dim,diffvect43);
	
	*x = dotvects(diffvect43,diffvect53,dim)/dsquared;

        *rslope = dotvects(diffvect54, diffvect53, dim)/(*d);

	*rint = (*x) * (*d);
}

/*-----------------------------------------------------------------------------
 * genpoly()
 * this routine evaluates a generalized belnding parabola (either P or Q) based on
 * the parameters passed
 */
#ifndef _NO_PROTO
void genpoly(double gamma, double x0[3], double x1[3], double x2[3], int dim, double c,
			double w, double m, double b, double *ans) 
#else
void genpoly(gamma, x0, x1, x2, dim, c, w, m, b, ans)
double gamma;
double x0[3];
double x1[3];
double x2[3];
int dim;
double c;
double w;
double m;
double b;
double *ans;
#endif

{

	double l,factor;
	double temp1[3], temp2[3];
	double first2terms[3], thirdterm[3];
	double diffvect10[3], diffvect20[3];


	l = (m*gamma + b);
	subvects(x2,x0,dim,diffvect20);
	scalevect(diffvect20,l/c,dim,temp1);
	addvects(x0,temp1,dim,first2terms);

	factor = l*(c-l)/(c*c*w*(1-w));
	subvects(x1,x0,dim,diffvect10);

	scalevect(diffvect10,factor,dim,temp1);

	factor*=(-w);
	subvects(x2,x0,dim,diffvect20);
	scalevect(diffvect20,factor,dim,temp2);

	addvects(temp1,temp2,dim,thirdterm);

	addvects(first2terms,thirdterm,dim,ans);
	
}	

#ifndef _NO_PROTO 
int main (int argc, char **argv) 
#else
int main(argc, argv)
int argc;
char **argv;
#endif
{
	int i,dimension, status;
	char c;
	long numpoints, n;
	double gamma, dgamma, ansq[3], ansp[3], ans[3], term1[3], term2[3];
	double pA[3], pB[3], pC[3], pD[3];
	double *p3=pA;
	double *p4=pB;
	double *p5=pC;
	double *p6=pD;
	double *temp;

	double d,e,rslope,rint,sslope,x,xp;

        /*
         * initialize
         */
	numpoints = 0;
	n = INSERT_DEF;
	dimension = 2;

        /*
         * now read the options off the command line
         */
        while (--argc>0) {
                argv++;
		c = *argv[0];
		if (c=='-')
			c = *(++argv[0]);
		switch (c) {
			case 'i':
				if (*(argv[0]+1)!=0) {
					fprintf(stderr,"blend: found extra stuff after the -i option\n");
					return 1;
				}
				if (--argc>0) {
					status = sscanf((++argv)[0],"%d",&n);
					if (status!=1) {
						fprintf(stderr,"blend: couldn't read number\n");	
						return 1;
					}
				} else {
					fprintf(stderr,"blend: couldn't find the number in the -i option\n");
					return 1;
				};
				break;
			case 't':
				dimension = 3;
				break;
			case 'h':
				printf("\n");
				printf("blend interpolates a smooth curve through data points from standard-in\n");
				printf("  using the technique of parabolic blending (see Rogers and Adams 'Elements\n");
				printf("  of Mathematics for Computer Graphics')\n");
				printf("  the interpolated points (lagging behind by one interval) are written to\n");
				printf("  standard-out\n");
				printf("Options are:	-t	..	use triples rather than the default duples\n"); 
				printf("		-i #	..	change number of inserted points per interval\n");
				printf("				to # (default is 4)\n");
				return 0;
			default:
				fprintf(stderr,"blend: illegal option %c\n",c);
				return 1;
		}
        }
	
	dgamma = 1.0/(n+1);

	while ((status=getpoint(p6,dimension)) != EOF) {
		numpoints++;	
		if (status == TRUE) {
			if (numpoints>3) {
				/*blend*/
				makePcoeffs(p3,p4,p5,dimension,&d,&x,&rslope,&rint);
				makeQcoeffs(p4,p5,p6,dimension,&e,&xp,&sslope);
				printpoint(p4,dimension);
				gamma = 0.0;
				for (i=0; i<n; i++) {
					gamma+=dgamma;
					genpoly(gamma,p3,p4,p5,dimension,d,x,rslope,rint,ansp);
					genpoly(gamma,p4,p5,p6,dimension,e,xp,sslope,0.,ansq);
					/* perform the blend! */
					scalevect(ansp,1.0-gamma,dimension,term1);
					scalevect(ansq,gamma,dimension,term2);
					addvects(term1,term2,dimension,ans);
				
					/* output result */	
					printpoint(ans,dimension);
				}
			} else if (numpoints == 3) {
				/* use Q to interpolate for the first interval */
				makeQcoeffs(p4,p5,p6,dimension,&e,&xp,&sslope);
				printpoint(p4,dimension);
				gamma = 0.0;
				for (i=0; i<n; i++) {
					gamma+=dgamma;
					genpoly(gamma,p4,p5,p6,dimension,e,xp,sslope,0.,ansq);
					printpoint(ansq,dimension);
				}
			}	
			
			/* cycle pointers */
			temp=p3;
			p3 = p4;
			p4 = p5;
			p5 = p6;
			p6 = temp;
		} else {
                        printf("blend: found something unacceptable in the input!\n");
                        break;
                }
	}

	if (numpoints>3) {
		/*use P to interpolate on the final interval */
		makePcoeffs(p3,p4,p5,dimension,&d,&x,&rslope,&rint);
		printpoint(p4,dimension);
		gamma = 0.0;
		for (i=0; i<n; i++) {
			gamma+=dgamma;
			genpoly(gamma,p3,p4,p5,dimension,d,x,rslope,rint,ansp);
			printpoint(ansp,dimension);
		}
		printpoint(p5,dimension);
	}

	return 0;
}
