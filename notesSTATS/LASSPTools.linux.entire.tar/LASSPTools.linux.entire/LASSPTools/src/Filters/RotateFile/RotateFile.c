/* Reads xyz triples from a file; reads rotation matrix from
   standard in; puts rotated positions to standard out */

#include <stdio.h>

#define MAXLINELENGTH 256

#ifndef _NO_PROTO
  main(int argc, char **argv)
#else
  main( argc, argv)
    int argc;
    char **argv;
#endif
{
   double R[3][3];
   int i,n;
   FILE *fp1,*fp2;
   char line1[MAXLINELENGTH];
   char line2[MAXLINELENGTH];
   double x[MAXLINELENGTH];
   double y[MAXLINELENGTH];
   double z[MAXLINELENGTH];
   int number[MAXLINELENGTH];

   if(argc==1) {
     strcpy(line1,"atoms");
     strcpy(line2,"atoms.no");
   } else {
     strcpy(line1,*(argv+1));
     strcpy(line2,*(argv+2));
   }


   if( (fp1 = fopen(line1,"r")) == NULL ) {
     fprintf(stderr,"Error opening file %s\n",line1);
     exit(-1);
   }
   if( (fp2 = fopen(line2,"r")) == NULL ) {
     fprintf(stderr,"Error opening file %s\n",line2);
     exit(-1);
   }

   i=0;
   while(fscanf(fp1, "%lf %lf %lf", &x[i], &y[i], &z[i]) != EOF) {
     fscanf(fp2,"%i",&number[i]);
     i++;
   }

   n = i;
   fclose(fp1);
   fclose(fp2);

   while((scanf("%lf %lf %lf",&R[0][0],&R[0][1],&R[0][2])) != EOF &&
         (scanf("%lf %lf %lf",&R[1][0],&R[1][1],&R[1][2])) != EOF &&
         (scanf("%lf %lf %lf",&R[2][0],&R[2][1],&R[2][2])) != EOF) { 

     for(i=0;i<n;++i) {
       fprintf(stdout,"%f %f %f %i\n",
	       R[0][0] * x[i] + R[0][1] * y[i] + R[0][2] * z[i],
	       R[1][0] * x[i] + R[1][1] * y[i] + R[1][2] * z[i],
	       R[2][0] * x[i] + R[2][1] * y[i] + R[2][2] * z[i], number[i]);
     }
     printf("\n");
     fflush(stdout);
   }
 }

