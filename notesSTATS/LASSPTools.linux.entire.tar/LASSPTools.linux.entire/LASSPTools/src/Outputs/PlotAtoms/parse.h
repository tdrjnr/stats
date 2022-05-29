#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xresource.h>
#include<stdio.h>

/* command line options to parse */

static XrmOptionDescRec opTable[] = {
/* 1*/  {"-h","*help",XrmoptionNoArg,"True"},
/* 2*/  {"-help","*help",XrmoptionNoArg,"True"},
/* 3*/  {"-v","*version",XrmoptionNoArg,"True"},
/* 4*/  {"-version","*version",XrmoptionNoArg,"True"},
/* 5*/  {"-e","*entire",XrmoptionNoArg,"True"},
/* 6*/  {"-entire","*entire",XrmoptionNoArg,"True"},
/* 7*/  {"-shade","*shade",XrmoptionNoArg,"True"}, 
/* 8*/  {"-noshade","*shade",XrmoptionNoArg,"False"},
/* 9*/  {"-scale","*scale",XrmoptionNoArg,"True"},
/*10*/  {"-n","*natoms",XrmoptionSepArg,(caddr_t)NULL},
/*11*/  {"-natoms","*natoms",XrmoptionSepArg,(caddr_t)NULL},
/*12*/  {"-xmin","*xmin",XrmoptionSepArg,(caddr_t)NULL},
/*13*/  {"-xmax","*xmax",XrmoptionSepArg,(caddr_t)NULL},
/*14*/  {"-ymin","*ymin",XrmoptionSepArg,(caddr_t)NULL},
/*15*/  {"-ymax","*ymax",XrmoptionSepArg,(caddr_t)NULL},
/*16*/  {"-g","*geometry",XrmoptionSepArg,(caddr_t)NULL},
/*17*/  {"-geometry","*geometry",XrmoptionSepArg,(caddr_t)NULL},
/*18*/  {"-fg","*foreground",XrmoptionSepArg, (caddr_t)NULL},  
/*19*/  {"-bg","*background",XrmoptionSepArg, (caddr_t)NULL},  
/*20*/  {"-border","*border",XrmoptionSepArg,(caddr_t)NULL},
/*21*/  {"-r","*radius",XrmoptionSepArg,(caddr_t)NULL},
/*22*/  {"-radius","*radius",XrmoptionSepArg,(caddr_t)NULL},
/*23*/  {"-c","*color",XrmoptionSepArg,(caddr_t)NULL},
/*24*/  {"-color","*color",XrmoptionSepArg,(caddr_t)NULL},
/*25*/  {"-r1","*atom1.radius",XrmoptionSepArg,(caddr_t)NULL},
/*26*/  {"-r2","*atom2.radius",XrmoptionSepArg,(caddr_t)NULL},
/*27*/  {"-r3","*atom3.radius",XrmoptionSepArg,(caddr_t)NULL},
/*28*/  {"-r4","*atom4.radius",XrmoptionSepArg,(caddr_t)NULL},
/*29*/  {"-r5","*atom5.radius",XrmoptionSepArg,(caddr_t)NULL},
/*30*/  {"-r6","*atom6.radius",XrmoptionSepArg,(caddr_t)NULL},
/*31*/  {"-r7","*atom7.radius",XrmoptionSepArg,(caddr_t)NULL},
/*32*/  {"-r8","*atom8.radius",XrmoptionSepArg,(caddr_t)NULL},
/*33*/  {"-r9","*atom9.radius",XrmoptionSepArg,(caddr_t)NULL},
/*34*/  {"-r10","*atom10.radius",XrmoptionSepArg,(caddr_t)NULL},
/*35*/  {"-r11","*atom11.radius",XrmoptionSepArg,(caddr_t)NULL},
/*36*/  {"-r12","*atom12.radius",XrmoptionSepArg,(caddr_t)NULL},
/*37*/  {"-r13","*atom13.radius",XrmoptionSepArg,(caddr_t)NULL},
/*38*/  {"-r14","*atom14.radius",XrmoptionSepArg,(caddr_t)NULL},
/*39*/  {"-r15","*atom15.radius",XrmoptionSepArg,(caddr_t)NULL},
/*40*/  {"-r16","*atom16.radius",XrmoptionSepArg,(caddr_t)NULL},
/*41*/  {"-r17","*atom17.radius",XrmoptionSepArg,(caddr_t)NULL},
/*42*/  {"-r18","*atom18.radius",XrmoptionSepArg,(caddr_t)NULL},
/*43*/  {"-r19","*atom19.radius",XrmoptionSepArg,(caddr_t)NULL},
/*44*/  {"-r20","*atom20.radius",XrmoptionSepArg,(caddr_t)NULL},
/*45*/  {"-c1","*atom1.color",XrmoptionSepArg,(caddr_t)NULL},
/*46*/  {"-c2","*atom2.color",XrmoptionSepArg,(caddr_t)NULL},
/*47*/  {"-c3","*atom3.color",XrmoptionSepArg,(caddr_t)NULL},
/*48*/  {"-c4","*atom4.color",XrmoptionSepArg,(caddr_t)NULL},
/*49*/  {"-c5","*atom5.color",XrmoptionSepArg,(caddr_t)NULL},
/*50*/  {"-c6","*atom6.color",XrmoptionSepArg,(caddr_t)NULL},
/*51*/  {"-c7","*atom7.color",XrmoptionSepArg,(caddr_t)NULL},
/*52*/  {"-c8","*atom8.color",XrmoptionSepArg,(caddr_t)NULL},
/*53*/  {"-c9","*atom9.color",XrmoptionSepArg,(caddr_t)NULL},
/*54*/  {"-c10","*atom10.color",XrmoptionSepArg,(caddr_t)NULL},
/*55*/  {"-c11","*atom11.color",XrmoptionSepArg,(caddr_t)NULL},
/*56*/  {"-c12","*atom12.color",XrmoptionSepArg,(caddr_t)NULL},
/*57*/  {"-c13","*atom13.color",XrmoptionSepArg,(caddr_t)NULL},
/*58*/  {"-c14","*atom14.color",XrmoptionSepArg,(caddr_t)NULL},
/*59*/  {"-c15","*atom15.color",XrmoptionSepArg,(caddr_t)NULL},
/*60*/  {"-c16","*atom16.color",XrmoptionSepArg,(caddr_t)NULL},
/*61*/  {"-c17","*atom17.color",XrmoptionSepArg,(caddr_t)NULL},
/*62*/  {"-c18","*atom18.color",XrmoptionSepArg,(caddr_t)NULL},
/*63*/  {"-c19","*atom19.color",XrmoptionSepArg,(caddr_t)NULL},
/*64*/  {"-c20","*atom20.color",XrmoptionSepArg,(caddr_t)NULL},
/*65*/  {"-ang1","*atom1.angle",XrmoptionSepArg,(caddr_t)NULL},
/*66*/  {"-ang2","*atom2.angle",XrmoptionSepArg,(caddr_t)NULL},
/*67*/  {"-ang3","*atom3.angle",XrmoptionSepArg,(caddr_t)NULL},
/*68*/  {"-ang4","*atom4.angle",XrmoptionSepArg,(caddr_t)NULL},
/*69*/  {"-ang5","*atom5.angle",XrmoptionSepArg,(caddr_t)NULL},
/*70*/  {"-ang6","*atom6.angle",XrmoptionSepArg,(caddr_t)NULL},
/*71*/  {"-ang7","*atom7.angle",XrmoptionSepArg,(caddr_t)NULL},
/*72*/  {"-ang8","*atom8.angle",XrmoptionSepArg,(caddr_t)NULL},
/*73*/  {"-ang9","*atom9.angle",XrmoptionSepArg,(caddr_t)NULL},
/*74*/  {"-ang10","*atom10.angle",XrmoptionSepArg,(caddr_t)NULL},
/*75*/  {"-ang11","*atom11.angle",XrmoptionSepArg,(caddr_t)NULL},
/*76*/  {"-ang12","*atom12.angle",XrmoptionSepArg,(caddr_t)NULL},
/*77*/  {"-ang13","*atom13.angle",XrmoptionSepArg,(caddr_t)NULL},
/*78*/  {"-ang14","*atom14.angle",XrmoptionSepArg,(caddr_t)NULL},
/*79*/  {"-ang15","*atom15.angle",XrmoptionSepArg,(caddr_t)NULL},
/*80*/  {"-ang16","*atom16.angle",XrmoptionSepArg,(caddr_t)NULL},
/*81*/  {"-ang17","*atom17.angle",XrmoptionSepArg,(caddr_t)NULL},
/*82*/  {"-ang18","*atom18.angle",XrmoptionSepArg,(caddr_t)NULL},
/*83*/  {"-ang19","*atom19.angle",XrmoptionSepArg,(caddr_t)NULL},
/*84*/  {"-ang20","*atom20.angle",XrmoptionSepArg,(caddr_t)NULL},
/*85*/  {"-ar1","*atom1.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*86*/  {"-ar2","*atom2.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*87*/  {"-ar3","*atom3.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*88*/  {"-ar4","*atom4.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*89*/  {"-ar5","*atom5.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*90*/  {"-ar6","*atom6.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*91*/  {"-ar7","*atom7.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*92*/  {"-ar8","*atom8.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*93*/  {"-ar9","*atom9.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*94*/  {"-ar10","*atom10.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*95*/  {"-ar11","*atom11.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*96*/  {"-ar12","*atom12.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*97*/  {"-ar13","*atom13.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*98*/  {"-ar14","*atom14.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*99*/  {"-ar15","*atom15.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*100*/  {"-ar16","*atom16.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*101*/  {"-ar17","*atom17.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*102*/  {"-ar18","*atom18.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*103*/  {"-ar19","*atom19.aspect",XrmoptionSepArg,(caddr_t)NULL},
/*104*/  {"-ar20","*atom20.aspect",XrmoptionSepArg,(caddr_t)NULL}
};
static int table_count = 104;


/* default options */

static char *op_defaults = "PlotAtoms*help:False\n              \
                            PlotAtoms*version:False\n           \
                            PlotAtoms*entire:False\n            \
                            PlotAtoms*scale:False\n             \
                            PlotAtoms*shade:True\n              \
                            PlotAtoms*natoms:1\n                \
                            PlotAtoms*radius:16\n               \
                            PlotAtoms*color:grey\n              \
                            PlotAtoms*geometry:600x600+650+5\n  \
                            PlotAtoms*xmin:nil\n                \
                            PlotAtoms*xmax:nil\n                \
                            PlotAtoms*ymin:nil\n                \
                            PlotAtoms*ymax:nil\n                \
                            PlotAtoms*border:.25\n              \
                            PlotAtoms*foreground:white\n        \
                            PlotAtoms*background:black\n";

static char *file = "plotatoms.db";


/* PlotAtoms */
/* by Bruce W. Roberts */

/* last modified 9-24-1991 */
