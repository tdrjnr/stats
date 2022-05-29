#include"plotatoms.h"

#ifndef _NO_PROTO
 void set_options(XrmDatabase *database, unsigned long *option_mask, 
		 Options *options)
#else
 void set_options(database, option_mask, options)
 XrmDatabase *database; 
 unsigned long *option_mask; 
 Options *options;
#endif
{
  extern char name[];
  XrmValue value;
  char line[100];
  char atom_line1[100];
  char atom_line2[100];
  char number[10];
  char *str_type[20];
  int i;

/* help option */
  if ( XrmGetResource(*database,"PlotAtoms.help","Plotatoms.Help",str_type, 
		      &value) == False )  option_error("help");
  strncpy(line,value.addr,(int)value.size);
  if( !strcmp(line,"True") || !strcmp(line,"true") || !strcmp(line,"TRUE")) {
    help();
    exit(0);
  }

/* version option */
  if ( XrmGetResource(*database,"PlotAtoms.version","Plotatoms.Version", 
		      str_type, &value) == False) option_error("version");
  strncpy(line,value.addr,(int)value.size);
  if( !strcmp(line,"True") || !strcmp(line,"true") || !strcmp(line,"TRUE")) {
    fprintf(stderr,"Version 1.0. Last modified 12/10/1990.\n");
    exit(0);
  }

/* entire option */
  if ( XrmGetResource(*database,"PlotAtoms.entire","Plotatoms.Entire", 
		      str_type, &value) == False) option_error("entire");
  strncpy(line,value.addr,(int)value.size);
  if( !strcmp(line,"True") || !strcmp(line,"true") || !strcmp(line,"TRUE")) {
    *option_mask = *option_mask | PAEntire;
  }

/* shading option */
  if ( XrmGetResource(*database,"PlotAtoms.shade","Plotatoms.Shade", 
		      str_type, &value) == False) option_error("shade");
  strncpy(line,value.addr,(int)value.size);
  if( !strcmp(line,"True") || !strcmp(line,"true") || !strcmp(line,"TRUE")) {
    *option_mask = *option_mask | PAShade;
  }

/* scaling option */
  if ( XrmGetResource(*database,"PlotAtoms.scale","Plotatoms.Scale", 
		      str_type, &value) == False) option_error("scale");
  strncpy(line,value.addr,(int)value.size);
  if( !strcmp(line,"True") || !strcmp(line,"true") || !strcmp(line,"TRUE")) {
    *option_mask = *option_mask | PAScale;
  }

/* number of atoms */
  if ( XrmGetResource(*database,"PlotAtoms.natoms","Plotatoms.Natoms", 
		      str_type, &value) == False ) option_error("atoms");
  strncpy(line,value.addr,(int)value.size);
  sscanf(line,"%i", &(options->natoms) );
  if(options->natoms < 1) {
    fprintf(stderr,"%s: Error. Number of Atoms is %i.  Must be > 0.\n",
	    name,options->natoms);
    fprintf(stderr,"%s: Using natoms = 1.\n",name);
    fflush(stderr);
    options->natoms = 1;
  }

/* minimum x value */
  if ( XrmGetResource(*database,"PlotAtoms.xmin","Plotatoms.Xmin", 
		      str_type, &value) == False ) option_error("xmin");
  strncpy(line,value.addr,(int)value.size);
  if( strcmp(line,"nil") != 0 ) {
    sscanf(line,"%lf", &(options->limits.xmin) );
    *option_mask = *option_mask | PAXMinSet;
  }

/* maximum x value */
  if ( XrmGetResource(*database,"PlotAtoms.xmax","Plotatoms.Xmax", 
		      str_type, &value) == False ) option_error("xmax");
  strncpy(line,value.addr,(int)value.size);
  if( strcmp(line,"nil") != 0 ) {
    sscanf(line,"%lf", &(options->limits.xmax) );
    *option_mask = *option_mask | PAXMaxSet;
  }

/* minimum y value */
  if ( XrmGetResource(*database,"PlotAtoms.ymin","Plotatoms.Ymin", 
		      str_type, &value) == False ) option_error("ymin");
  strncpy(line,value.addr,(int)value.size);
  if( strcmp(line,"nil") != 0 ) {
    sscanf(line,"%lf", &(options->limits.ymin) );
    *option_mask = *option_mask | PAYMinSet;
  }

/* maximum y value */
  if ( XrmGetResource(*database,"PlotAtoms.ymax","Plotatoms.Ymax", 
		      str_type, &value) == False ) option_error("ymax");
  strncpy(line,value.addr,(int)value.size);
  if( strcmp(line,"nil") != 0 ) {
    sscanf(line,"%lf", &(options->limits.ymax) );
    *option_mask = *option_mask | PAYMaxSet;
  }

/* geometry value */
  if ( XrmGetResource(*database,"PlotAtoms.geometry","Plotatoms.Geometry", 
		      str_type, &value) == False ) option_error("geometry");
  strncpy(line,value.addr,(int)value.size);
  sscanf(line,"%ix%i+%i+%i",&(options->geometry.width),
	 &(options->geometry.height),&(options->geometry.xpos),
	 &(options->geometry.ypos) );

/* atom colors and radii */
  if ( XrmGetResource(*database,"PlotAtoms.radius","Plotatoms.Radius", 
		      str_type, &value) == False ) option_error("radius");
  strncpy(line,value.addr,(int)value.size);
  sscanf(line,"%i",&(options->default_atom.radius) );
  if ( XrmGetResource(*database,"PlotAtoms.color","Plotatoms.Color", 
		      str_type, &value) == False ) option_error("color");
  strncpy(line,value.addr,(int)value.size);
  options->default_atom.color=(char *)malloc((value.size+1)*sizeof(char));  
  strcpy((options->default_atom.color), line );
/* JPS added to avoid errors on Linux: default aspect ratio zero! */
  if( XrmGetResource(*database,"PlotAtoms.aspect","Plotatoms.Aspect",str_type, 
			 &value) == False ) {
	options->default_atom.aspect_ratio = 1;
  } else {
	strncpy(line,value.addr,(int)value.size);
	sscanf(line,"%f",&(options->default_atom.aspect_ratio) );
  }
/* End modification */


/* multiple atoms */
  if( options->natoms != 1) {
    options->atoms = (Sphere *) malloc((options->natoms)*sizeof(Sphere));
    for(i=0;i<options->natoms;++i) {
      sprintf(number,"%i",i+1);

/* radii for atoms */
      strcpy(atom_line1,"PlotAtoms.atom");
      strcat(atom_line1,number);
      strcat(atom_line1,".radius");
      strcpy(atom_line2,"PlotAtoms.Atom");
      strcat(atom_line2,number);
      strcat(atom_line2,".Radius");
      if( XrmGetResource(*database,atom_line1,atom_line2,str_type, 
			 &value) == False ) {
	options->atoms[i].radius = options->default_atom.radius;
      } else {
	strncpy(line,value.addr,(int)value.size);
	sscanf(line,"%i",&(options->atoms[i].radius) );
      }

/* color for atoms */
      strcpy(atom_line1,"PlotAtoms.atom");
      strcat(atom_line1,number);
      strcat(atom_line1,".color");
      strcpy(atom_line2,"PlotAtoms.Atom");
      strcat(atom_line2,number);
      strcat(atom_line2,".Color");
      if( XrmGetResource(*database,atom_line1,atom_line2,str_type, 
			 &value) == False ) {
	options->atoms[i].color = options->default_atom.color;
      } else {
	strncpy(line,value.addr,(int)value.size);
	options->atoms[i].color = (char *)malloc((value.size+1)*sizeof(char));
	strcpy( options->atoms[i].color, line );
      }

/* aspect ratio for atoms */
      strcpy(atom_line1,"PlotAtoms.atom");
      strcat(atom_line1,number);
      strcat(atom_line1,".aspect");
      strcpy(atom_line2,"PlotAtoms.Atom");
      strcat(atom_line2,number);
      strcat(atom_line2,".Aspect");
      if( XrmGetResource(*database,atom_line1,atom_line2,str_type, 
			 &value) == False ) {
	options->atoms[i].aspect_ratio = 1;
      } else {
	strncpy(line,value.addr,(int)value.size);
	sscanf(line,"%f",&(options->atoms[i].aspect_ratio) );
      }

/* angle for ellipse of the atom */
      strcpy(atom_line1,"PlotAtoms.atom");
      strcat(atom_line1,number);
      strcat(atom_line1,".angle");
      strcpy(atom_line2,"PlotAtoms.Atom");
      strcat(atom_line2,number);
      strcat(atom_line2,".Angle");
      if( XrmGetResource(*database,atom_line1,atom_line2,str_type, 
			 &value) == False ) {
	options->atoms[i].angle = 0;
      } else {
	strncpy(line,value.addr,(int)value.size);
	sscanf(line,"%i",&(options->atoms[i].angle) );
      }

    } /* ends for(i=0;i<options->natoms ... */

  } else {

    options->atoms = &(options->default_atom);

  }

/* fix malloc stuff here */

/* border fraction */
  if( XrmGetResource(*database,"PlotAtoms.border","Plotatoms.Border", 
		     str_type, &value) == False ) option_error("border");
  strncpy(line,value.addr,(int)value.size);
  sscanf(line,"%lf",&(options->border) );
  if( (options->border >= 0.5) || (options->border < 0.0) ) {
    fprintf(stderr,"Border value should be between 0.0 and 0.5.\n");
    fflush(stderr);
    exit(-1);
  }

/* foreground color */
  if ( XrmGetResource(*database,"PlotAtoms.foreground","Plotatoms.Foreground", 
		      str_type, &value) == False ) option_error("foreground"); 
  strncpy(line,value.addr,(int)value.size);
  options->foreground = (char *) malloc((value.size+1)*sizeof(char));
  strcpy(options->foreground,line);

/* background color */
  if ( XrmGetResource(*database,"PlotAtoms.background","Plotatoms.Background", 
		      str_type, &value) == False ) option_error("background");
  strncpy(line,value.addr,(int)value.size);
  options->background = (char *) malloc((value.size+1)*sizeof(char));
  strcpy(options->background,line);

/* add any more gems you may have */

#ifdef DEBUG
  XrmPutFileDatabase(*database,"debug.db");
#endif
     
  return;
}

#ifndef _NO_PROTO
 void option_error(char option[])
#else
 void option_error(option)
 char option[];
#endif
{
  extern char name[];

  fprintf(stderr,"%s: Error in set_options %s.\n",name,option);
  fflush(stderr);
  exit(-1);
}



/* last modified 5-7-91 */






