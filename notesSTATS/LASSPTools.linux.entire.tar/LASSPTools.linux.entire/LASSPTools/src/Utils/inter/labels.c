#include "menudef.h"
#include "iodefs.h"
#include "interdef.h"
#include <strings.h>

extern CURVE **curve;
extern int ncols, xcol;

static char *argptr;
char string[100];
extern int moveflag;
extern double atof();
char *stripquotes();

labelx() {
    if((argptr = argline()) == NULL) {  /* get label from terminal */
        fprintf(stderr,"*Enter x label: ");
        while(gets(string) == NULL);
        printf("#lx \"%s\"\n",stripquotes(string));
    }
    else
        printf("#lx \"%s\"\n",stripquotes(argptr));
    return 1;
}

labely() {
    if((argptr = argline()) == NULL) {  /* get label from terminal */
        fprintf(stderr,"*Enter y label: ");
        while(gets(string) == NULL);
        printf("#ly \"%s\"\n",stripquotes(string));
    }
    else
        printf("#ly \"%s\"\n",stripquotes(argptr));

    /*if(!moveflag) printf("#r 0.1\n");*/       /* Move over if necessary */
    return 1;
}

title() {
    if((argptr = argline()) == NULL) {  /* get label from terminal */
        fprintf(stderr,"*Enter title: ");
        while(gets(string) == NULL);
        printf("#lt \"%s\"\n",stripquotes(string));
    }
    else
        printf("#lt \"%s\"\n",stripquotes(argptr));
    return 1;
}

labelxsize() {
    double size;
    if((argptr=arg()) == NULL) 
        size = dgetd("x label size",1.5);
    else
        size = atof(argptr);

    printf("#lxs %e\n",size);
    return 1;
}

labelysize() {
    double size;
    if((argptr=arg()) == NULL) 
        size = dgetd("y label size",1.5);
    else
        size = atof(argptr);

    printf("#lys %e\n",size);
    return 1;
}

titlesize() {
    double size;
    if((argptr=arg()) == NULL) 
        size = dgetd("title size",2.);
    else
        size = atof(argptr);

    printf("#lts %e\n",size);
    return 1;
}

numbersize() {
    double size;
    if((argptr=arg()) == NULL) 
        size = dgetd("number size",1.);
    else
        size = atof(argptr);

    printf("#ns %e\n",size);
    return 1;
}

labelxpos() {
    double posx, posy;
    if((argptr =arg()) == NULL)
       posx = dgetd("x position of x label",0.5);
    else
       posx = atof(argptr);

    if((argptr =arg()) == NULL)
       posy = dgetd("y position of x label",-.1);
    else
       posy = atof(argptr);

    printf("#lxx %e\n",posx);
    printf("#lxy %e\n",posy);
    return 1;
}

labelypos() {
    double posx, posy;
    if((argptr =arg()) == NULL)
       posx = dgetd("x position of y label",-.15);
    else
       posx = atof(argptr);

    if((argptr =arg()) == NULL)
       posy = dgetd("y position of y label",0.5);
    else
       posy = atof(argptr);

    printf("#lyx %e\n",posx);
    printf("#lyy %e\n",posy);
    return 1;
}

titlepos() {
    double posx, posy;
    posx = dgetd("x position of title",0.5);
    posy = dgetd("y position of title",0.9);
    printf("#ltx %e\n",posx);
    printf("#lty %e\n",posy);
    return 1;
}

pointlabel() {
    if ((argptr=arg()) == NULL) {
        fprintf(stderr,"*Enter label for points: ");
        gets(string);
        printf("#c \"%s\"\n",stripquotes(string));
    }
    else
        printf("#c \"%s\"\n",stripquotes(argptr));
    return 1;
}

pointoff() {
    printf("#c0\n");
    return 1;
}

pointsize() {
    double size;
    if((argptr=arg()) == NULL) 
        size = dgetd("point label size",1.5);
    else
        size = atof(argptr);

    printf("#cs %e\n",size);
    return 1;
}

arblabel() {

    static double axpos = 0.;
    static double aypos = 0.;
    int noargs = 0;

    if((argptr=arg()) == NULL) {
       axpos = dgetd("x position of label (in data units)",axpos);
       noargs = 1;
    }
    else
        axpos = atof(argptr);

    if(noargs || (argptr=arg()) == NULL) {
       aypos = dgetd("y position of label (in data units)",aypos);
       noargs = 1;
    }
    else
        aypos = atof(argptr);

    if(noargs || (argptr = argline()) == NULL) {  /* get label from terminal */
        fprintf(stderr,"*Enter label: ");
        while(gets(string) == NULL);
        printf("#\n%e %e \"%s\"\n",axpos,aypos,stripquotes(string));
    }
    else
        printf("#\n%e %e \"%s\"\n",axpos,aypos,stripquotes(argptr));
    return 1;
}

curvelabel() {
    int col;
    int point, i;
    DATAPT *xptr, *yptr;
    char *lstring;

    /* Label which curve? */
    if((argptr = arg()) == NULL)
        col = igetd("column to label", ncols) - 1;
    else
        col = atoi(argptr) - 1;

    if(col >= ncols || col < 0) {
        fprintf(stderr," ** There is no column %d!\n", col+1);
        trashline();
        return 1;
    }

    /* Label which point? */
    if((argptr = arg()) == NULL) {
       fprintf(stderr,"Curve has %d points.\n", curve[col]->npts);
       point = igetd("which point to label", curve[col]->npts);
    }
    else {
      if(*argptr == '$')
         point = curve[col]->npts;
      else
         point = atoi(argptr);
   }

   if(point > curve[col]->npts || point <= 0) {
      fprintf(stderr, "Bad point! There are %d points in the curve\n",
         curve[col]->npts);
      return 1;
   }

    /* Find the point */
    xptr = curve[xcol]->firstpt;
    yptr = curve[col]->firstpt;
    for(i=0; i<point-1; i++) {
        xptr = xptr->next;
        yptr = yptr->next;
    }

    /* Get label */
    if((argptr = argline()) == NULL) {
       fprintf(stderr,"*Enter label: ");
       while(gets(string) == NULL);
       lstring = string;
    }
    else
      lstring = argptr;

    printf("#\n%e %e \"%s\"\n", xptr->x, yptr->x, stripquotes(lstring));
    return 1;
}

helpchars() {           /* Print possible characters */
    fprintf(stderr,"The special characters are\n\
   \\pl    plus sign   \\fp    fancy plus\n\
   \\cr    cross       \\fx    fancy cross\n\
   \\di    diamond     \\fd    fancy diamond\n\
   \\sq    square      \\fs    fancy square\n\
   \\oc    octagon     \\bu    burst\n");
   return 1;
}

/**************************************************/

/* Strip quotation marks from a string, return pointer to new string. */
/* Only looks for quotes in first and last elements */

char *stripquotes(str)
char *str;
{
   char *last;
   if(str[0] == '"') {
      if((last = rindex(str, '"')) == str) {/* Only one quote ?!?? */
         fputs("Unmatched quotation marks!\n", stderr);
         return str;
      }
      *last = '\0';  /* Terminate at quotation mark */
      return str+1;  /* Skip quotation mark */
   }
   else
      return str;   /* Doesn't begin with quotation mark */
}
