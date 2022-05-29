/* sort by z-coordinate, giving smallest first */

#ifndef _NO_PROTO
#include<stdlib.h>
#endif

#include<math.h>
#include<stdio.h>
#include<string.h>

#define ALLWHITE      1
#define NOTWHITE      0
#define NEWFRAME      1
#define LINE_LENGTH   100
#define MAX_ATOMS     5000

#ifndef _NO_PROTO
  int whitespace(char xypositions[]); 
  void sort(int n,double arr[MAX_ATOMS][3], int nums[MAX_ATOMS]);
#else
  int whitespace();
  void sort();
#endif

main()
{
  char xypositions[LINE_LENGTH];
  double xvec[MAX_ATOMS][3];
  int atom_no[MAX_ATOMS];
  int n=0,i, number, flag=0;
  int index;

  while( (fgets(xypositions,LINE_LENGTH,stdin))!= NULL) {
    if(whitespace(xypositions)!=NEWFRAME) {
      number = sscanf(xypositions,"%lf %lf %lf %i",&(xvec[n][0]),
		      &(xvec[n][1]), &(xvec[n][2]),&(atom_no[n]) );
      if(number==3) flag = 1;
      if (n >= MAX_ATOMS) {
         fprintf(stderr,"\nsortit: Error too many elements, max is 5000\n");
         exit(0);
      }
      ++n;
    }
    else {
      sort(n,xvec,atom_no);
      for(i=0;i<n;++i) {
	if(flag != 1) fprintf(stdout,"%g %g %g %i\n",xvec[i][0],
			      xvec[i][1], xvec[i][2],
			      atom_no[i]);
	else fprintf(stdout,"%g %g %g\n",xvec[i][0],xvec[i][1],
		     xvec[i][2]);
	
      }
      fprintf(stdout,"\n");
      fflush(stdout);
      n=0;
    }
  }
}

/*
** This uses a quicksort algorithm slightly modified to be non-recursize.
** See Horiwitz and Sahni - " Fundamentals of Data Structures" or any
** comp sci data structures book for a description of the basic idea of
** splitting the set and then ordering each half - the end result is that
** the whole list is ordered.
** 
*/

#ifndef _NO_PROTO
  void sort(int n,double arr[MAX_ATOMS][3], int nums[MAX_ATOMS])
#else
  void sort( n, arr, nums)
    int n;
    double arr[MAX_ATOMS][3]; 
    int nums[MAX_ATOMS];
#endif
{
       
        int stack[MAX_ATOMS*2];
	int i,j,index,top,m;
	double a,b,c,K;
	int t;
	int keep_going;

        m = 0;
	top = 0;
	n--;
	while (top >= 0) {
	   while (m < n) {
	     i = m;
	     j = n + 1;
	     K = arr[m][2];
	     keep_going = 1;
	     
	     while (keep_going) {
	        do {
		  i++;
		} while (arr[i][2] < K);
		do {
		  j--;
		} while (arr[j][2] > K);
		if (i<j) {
		  /* Interchange Items */
		  a = arr[i][0];
		  b = arr[i][1];
		  c = arr[i][2];
		  t = nums[i];
		  arr[i][0] = arr[j][0];
		  arr[i][1] = arr[j][1];
		  arr[i][2] = arr[j][2];
		  nums[i] = nums[j];
		  arr[j][0] = a;
		  arr[j][1] = b;
		  arr[j][2] = c;
		  nums[j] = t;
		} else
		  keep_going = 0;
	      } /* end while keep_going */

	  a = arr[m][0];
	  b = arr[m][1];
	  c = arr[m][2];
	  t = nums[m];
	  arr[m][0] = arr[j][0];
	  arr[m][1] = arr[j][1];
	  arr[m][2] = arr[j][2];
	  nums[m] = nums[j];
	  arr[j][0] = a;
	  arr[j][1] = b;
	  arr[j][2] = c;
	  nums[j] = t;

	  stack[top++] = n; stack[top++] = j;
	  n = j - 1;
       } /* end while m < n */
      
       if (top > 0) {
          m = stack[--top] + 1;
          n = stack[--top];
       } else
	  top--;
    } /* end while top >= 0 */

}


