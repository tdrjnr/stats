#include<stdio.h>

#define ALLWHITE 1
#define NOTWHITE 0

#ifndef _NO_PROTO
int whitespace(char xypositions[])
#else
int whitespace(xypositions)
char xypositions[];
#endif
{
  int i=0,flag=ALLWHITE;

  while(xypositions[i]!='\0') {
    if((xypositions[i]!='\n')&&(xypositions[i]!=' ')&&(xypositions[i]!='\r')&&
       (xypositions[i]!='\t')&&(xypositions[i]!='\v')&&(xypositions[i]!='\f')){
      flag=NOTWHITE;
      return flag;
    }
    ++i;
  }
  return flag;
}

