/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 
/*
   DynamicLattice: 
   X-windows program to animate dynamics on square or rectangular lattices

   see enclosed man page and online help via "DynamicLattice -h"

   Copyright (C) 1990 Christopher R. Myers (with the exception of
   math_io.c, from the LASSPTools distribution).  All rights reserved.
   Feel free to copy, modify and redistribute this program, but do not
   sell it.  Include this notice in any copy.

   written: 10/90; latest revision: 2/93 (v. 3.0)
*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/ 

/* dlat.c: main routine for DynamicLattice */

#define MAIN
#define XINIT
#include <stdio.h>
/* Needed for Linux; didn't work for something, but I've forgotten what */
/* Without, gives error because atof is treated as an int */
/* #ifdef LINUX */
#include <stdlib.h>
/* #endif */
#include <math.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "DynamicLattice.h"
#include "Xwindowdefs.h"
#include "math_io.h"

#define STRCMP0(arg,str) (strcmp(arg,str)==0)


int ninp, pcnt=1;
char line[128], **inp;
Matrix *zmat; 

main(argc, argv)
int argc;
char **argv;
{
     int i, j, c;
     double z, d;
     
     /* print basic information */
     fprintf(stderr, 
	     "DynamicLattice (version 3.0)\n");
     
     /* allocate input strings */
     inp = (char **) malloc (10 * sizeof (char *));
     for (i = 0; i < 10; i++) 
     {
	  inp[i] = (char *) malloc (32*sizeof(char));
     }
     
     /* set default values */
     lattice.D          = 2;
     lattice.offset     = 0; 
     graphics.grid      = 0; 
     graphics.defc      = 1;
     graphics.textc     = 1;
     graphics.writez    = 1;
     graphics.mode      = 0; 
     graphics.gray      = 0;
     dynamics.mode      = 0;
     dynamics.initpause = 2;
     dynamics.interrupt = 0;
     dynamics.frills    = 1;
     input.binary       = 0;
     zscale.zmin        = 0.; 
     zscale.zmax        = 1.; 
     zscale.zrange      = 1.;
     set.defc           = 0;
     set.nx             = 0;
     set.ny             = 0;
     set.pix            = 0;
     set.zscale         = 0;
     map.position       = 0;
     strcpy(graphics.title, "");
     
     /* provide online help */
     if (argc > 1) 
       if (STRCMP0(argv[1],"-h") || 
	   STRCMP0(argv[1],"-help") || 
	   STRCMP0(argv[1],"-usage")) help();
     
     /* read command line arguments */
     parse_argv(argc, argv);
     
     /* test if nx and ny set */
     if (!set.nx || !set.ny) 
     { 
	  fprintf(stderr, 
		  "Error: -nx and -ny not set; run 'DynamicLattice -h' for help.\n");
	  exit(-1);
     }
     zmat  = create_matrix(lattice.ny, lattice.nx);
     set.z = (int **) malloc (lattice.ny * sizeof(int *));
     for (j = 0; j < lattice.ny; j++) 
       set.z[j] = (int *) malloc (lattice.nx * sizeof(int));
     
     /* preliminary X graphics */
     initialize_display();
     
     /* read .colormaprc/.graymaprc file; get overriding colors or grays */
     input_map_info();
     
     /* initiate X graphics */
     open_Xwindow();    
     
     /* sleep for initpause seconds to allow window to open */
     sleep(dynamics.initpause);
     
     /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
     /* start reading and drawing */
     /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
     
     while (!feof(stdin))
     {
	  dynamics.interrupt = 0;
	  switch (dynamics.frills)
	  {
	     case (0):
	       while (!feof(stdin))
	       {
		    load_new_matrix(zmat);
		    for (j = 0; j < lattice.ny; j++) 
		    {
			 for (i = 0; i < lattice.nx; i++)
			 {
			      z = zmat->el[j][i];
			      set.z[j][i] = 1;
			      plot_updated_site(g1_pixwdw, i, j, z);
			 }
		    }
		    copy_pix2win(g1_pixwdw); 
		    XFlush (display);
	       }
	       break;
	       
	     case (1):
	       
	       switch (dynamics.mode)  /* unbuffered update */
	       {
		  case (0):
		    
		    /* shaded sites */
		    if (graphics.mode == 0)
		    {
			 while (!dynamics.interrupt) 
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line,"%s %s %s \n",
					    inp[0],inp[1],inp[2]);
			      
			      if (STRCMP0(inp[2], "erase")) 
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j))
				      erase_site(g1_pixwdw, i, j);
			      }
			      else 
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = z = atof(inp[2]);
				      set.z[j][i] = 1;
				      plot_updated_site(g1_pixwdw, i, j, z);
				   }
			      }
			      XFlush(display);
			 }
		    }
		    
		    /*--------------------------------------------------*/
		    
		    /* arrow graphics */
		    if (graphics.mode == 1)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line, "%s %s %s %s %s", 
					    inp[0], inp[1], inp[2], 
					    inp[3], inp[4]);
			      
			      if (STRCMP0(inp[2], "erase"))
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				       erase_arrow(g1_pixwdw, i, j);
				       strcpy(inp[2], "");
				    }
			      }
			      else if (ninp == 3)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      set.z[j][i] = 1;
				      /* z = 1.0 */
				      plot_updated_arrow(g1_pixwdw,i,j,d,
						      zscale.zmax,
						      graphics.defc);
				    }
			      }
			      else if (ninp == 4)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      set.z[j][i] = 1;
				      z = atof(inp[3]);
				      plot_updated_arrow(g1_pixwdw,i,j,d,z,
						      graphics.defc);
				   }
			      }
			      else if (ninp == 5)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      set.z[j][i] = 1;
				      z = atof(inp[3]); 
				      c = atoi(inp[4]);
				      plot_updated_arrow(g1_pixwdw,i,j,d,z,c);
				   }
			      }
			      XFlush(display);
			 }
		    }
		    
		    /*--------------------------------------------------*/
		    
		    /* bond graphics */
		    if (graphics.mode == 2)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line, "%s %s %s %s %s", 
					    inp[0], inp[1], inp[2], 
					    inp[3], inp[4]);
			      
			      if (STRCMP0(inp[3], "erase"))
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      d = atof(inp[2]);
				      erase_bond(g1_pixwdw, i, j, d);
				      strcpy(inp[3], "");
				   }
			      }
			      else if (ninp == 3)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]);
				      set.z[j][i] = 1;
				      plot_updated_bond(g1_pixwdw,i,j,d,
						     zscale.zmax,
						     graphics.defc);
				   }
			      }
			      else if (ninp == 4)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      set.z[j][i] = 1;
				      z = atoi(inp[3]);
				      plot_updated_bond(g1_pixwdw,i,j,d,z,
						     graphics.defc);
				   }
			      }
			      else if (ninp == 5)
			      {
				   i = atoi(inp[0])-lattice.offset;
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]);
				      set.z[j][i] = 1;
				      z = atof(inp[3]);
				      c = atoi(inp[4]);
				      plot_updated_bond(g1_pixwdw,i,j,d,z,c);
				   }
			      }
			      XFlush(display);
			 }
		    }
		    
		    continue;
		    
		    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		    
		  case (1): /* buffered update */
		    
		    /* shaded sites */
		    if (graphics.mode == 0)
		    {
			 while (!dynamics.interrupt) 
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line,"%s %s %s\n",
					    inp[0],inp[1],inp[2]);
			      
			      if (STRCMP0(inp[2], "erase"))
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j))
				      erase_site(g1_pixwdw, i, j);
			      }
			      else
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = z = atof(inp[2]);
				      set.z[j][i] = 1;
				      plot_updated_site(g1_pixwdw, i, j, z);
				   }
			      }
			 }
		    }
		    
		    /*----------------------------------------------------*/
		    
		    /* arrow graphics */
		    if (graphics.mode == 1)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line,"%s %s %s\n",
					    inp[0],inp[1],inp[2]);
			      
			      if (STRCMP0(inp[2], "erase"))
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j))
				      erase_arrow(g1_pixwdw, i, j);
			      }
			      else if (ninp == 3)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]);
				      set.z[j][i] = 1;
				      plot_updated_arrow(g1_pixwdw,i,j,d,
						      zscale.zmax, graphics.defc);
				   }
				   
			      }
			      else if (ninp == 4)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      z = atof(inp[3]);
				      set.z[j][i] = 1;
				      plot_updated_arrow(g1_pixwdw,i,j,d,
						      z,graphics.defc);
				   }
			      }
			      else if (ninp == 5)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      z = atof(inp[3]);
				      set.z[j][i] = 1;
				      c = atoi(inp[4]);
				      plot_updated_arrow(g1_pixwdw,i,j,d,z,c);
				   }
			      }
			 }
		    }
		    
		    /*-----------------------------------------------------*/
		    
		    /* bond graphics */
		    if (graphics.mode == 2)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      gets(line);
			      ninp = sscanf(line,"%s %s %s\n",
					    inp[0],inp[1],inp[2]);
			      
			      if (STRCMP0(inp[2], "erase"))
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   d = atof(inp[2]);
				   if (in_bounds(i,j))
				      erase_bond(g1_pixwdw, i, j);
			      }
			      else if (ninp == 3)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset; 
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]);
				      set.z[j][i] = 1;
				      plot_updated_bond(g1_pixwdw,i,j,d,
						     zscale.zmax,graphics.defc);
				   }
			      }
			      else if (ninp == 4)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      set.z[j][i] = 1;
				      c = atoi(inp[3]);
				      plot_updated_bond(g1_pixwdw,i,j,d,z,c);
				   }
			      }
			      else if (ninp == 5)
			      {
				   i = atoi(inp[0])-lattice.offset; 
				   j = atoi(inp[1])-lattice.offset;
				   if (in_bounds(i,j)) {
				      zmat->el[j][i] = d = atof(inp[2]); 
				      z = atof(inp[3]);
				      set.z[j][i] = 1;
				      c = atoi(inp[4]);
				      plot_updated_arrow(g1_pixwdw,i,j,d,z,c);
				   }
			      }
			 }
		    }
		    continue;
		    
		    /*::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		    
		    /* matrix update */
		  case (2):
		    
		    /* shaded sites */
		    if (graphics.mode == 0)
		    {
			 while (!dynamics.interrupt) 
			 {
			      if (test_for_command()) continue;
			      load_new_matrix(zmat);
			      for (j = 0; j < lattice.ny; j++) 
			      {
				   for (i = 0; i < lattice.nx; i++)
				   {
					z = zmat->el[j][i];
					set.z[j][i] = 1;
					plot_updated_site(g1_pixwdw, i, j, z);
				   }
			      }
			      copy_pix2win(g1_pixwdw); 
			      XFlush (display);
/*			      clear_graphics_window(g1_pixwdw); */
			 }
		    }
		    
		    /*-----------------------------------------------------*/
		    
		    /* arrow graphics */
		    if (graphics.mode == 1)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      load_new_matrix(zmat);
			      for (j = 0; j < lattice.ny; j++) 
			      {
				   for (i = 0; i < lattice.nx; i++)
				   {
					d = zmat->el[j][i];
					set.z[j][i] = 1;
					plot_updated_arrow(g1_pixwdw,i,j,d,
							   zscale.zmax,
							   graphics.defc);
				   }
			      }
			      copy_pix2win(g1_pixwdw); 
			      XFlush (display);
			      clear_graphics_window(g1_pixwdw);
			 }
		    }
		    
		    /*-----------------------------------------------------*/
		    
		    /* bond graphics */
		    if (graphics.mode == 2)
		    {
			 while (!dynamics.interrupt)
			 {
			      if (test_for_command()) continue;
			      load_new_matrix(zmat);
			      for (j = 0; j < lattice.ny; j++) 
			      {
				   for (i = 0; i < lattice.nx; i++)
				   {
					d = zmat->el[j][i];
					set.z[j][i] = 1;
					plot_updated_bond(g1_pixwdw,i,j,d,
							  graphics.defc);
				   }
			      }
			      copy_pix2win(g1_pixwdw); 
			      XFlush (display);
			      /* clear_graphics_window(g1_pixwdw); */
			 }
		    }
		    continue;
	       }
	   }
	  break;
      }
	  
     while(1) 
     {
	 XMaskEvent(display, ButtonPressMask, &event2);
       {
	   switch(event2.type) 
	   {
	     case ButtonPress:
	       exit(-1);
	       break;
	     default:
	       break;
	   }
       }
     }
 }

/*------------------------------------------------------------------------- */
/*------------------------------------------------------------------------- */

int test_for_command()
{
     int nc = getc(stdin);
     
     if (nc == EOF)
     {
	  dynamics.interrupt = 1;
	  return (EOF);
     }	 
     if (nc == '\n')
     {
	  return (1);
     }	 
     if (nc == '#')
     {
	  parse_command_string();
	  return (1);
     }
     if (isalpha(nc)) 
     {
	  gets(line);
	  return (1);
     }
     ungetc(nc,stdin);
     return (0);
}

/*------------------------------------------------------------------------- */

parse_command_string()
{
     int i, nc = getc(stdin);
     char textstr[128];
     
     if (nc=='s')
     {
	  for (i=0; i<6; i++) getchar();   /* discard word "string" */
	  gets(line);
	  write_text(line);
     }
     
     else
     {
	  ungetc(nc, stdin);
	  gets(line);
	  ninp = sscanf(line,"%s %s %s \n",inp[0],inp[1],inp[2]);
	  
	  if (STRCMP0(inp[0], "draw") && dynamics.mode == 1) 
	  {
	       copy_pix2win(g1_pixwdw); 
	       XFlush (display);
	  }		 
	  else if (STRCMP0(inp[0], "nobuffer"))
	  {
	       dynamics.mode = 0;
	       dynamics.interrupt = 1;
	  }
	  else if (STRCMP0(inp[0], "buffer"))
	  {
	       dynamics.mode = 1;
	       dynamics.interrupt = 1;
	       copy_win2pix(g1_pixwdw);
	  }
	  else if (STRCMP0(inp[0], "matrix"))
	  {
	       dynamics.mode = 2;
	       dynamics.interrupt = 1;
	  }
	  else if (STRCMP0(inp[0], "print"))
	  {
	       print_lattice(pcnt);
	       pcnt++;
	  }
	  else if (STRCMP0(inp[0], "z"))
	  {
	       zscale.zmin = atof(inp[1]); 
	       zscale.zmax = atof(inp[2]);
	       zscale.zrange = zscale.zmax - zscale.zmin;
	       set.zscale  = 1;
	       if (graphics.writez) write_zrange();
	  }
	  else if (STRCMP0(inp[0], "erase"))
	  {
	       clear_graphics_window(g1_pixwdw);
	  }
	  else if (STRCMP0(inp[0], "pause"))
	  {
	       sleep(atoi(inp[1]));
	  }
	  else if (STRCMP0(inp[0], "quit"))
	  {
	       exit(-1);
	  }
     }
}

/*------------------------------------------------------------------------- */

int load_new_matrix(mat)
Matrix *mat;
{
     int i, j, done;
     
     if (input.binary) done = get_binary_matrix(mat, stdin);
     else              done = get_matrix(mat, stdin);
     
     lattice.ny = zmat->rows;
     lattice.nx = zmat->cols;
     
     if (done == EOF) return done; /* exit at the end of the file */
     
     /*  Need to find min and max values in matrix.  */
     if (!set.zscale) 
     {
	  zscale.zmin = zscale.zmax = zmat->el[0][0];
	  for (j = 0; j < zmat->rows; j++)
	  {
	       for (i = 0; i < zmat->cols; i++)
	       {
		    if (zmat->el[j][i] < zscale.zmin) 
		      zscale.zmin = zmat->el[j][i];
		    if (zmat->el[j][i] > zscale.zmax) 
		      zscale.zmax = zmat->el[j][i];
	       }
	  }
	  zscale.zrange = zscale.zmax - zscale.zmin;
	  if (graphics.writez) write_zrange();
     }
}

/*------------------------------------------------------------------------- */

print_lattice(printcnt)
int printcnt;
{
     int i, j;
     char line[256];
     FILE *printstream;
     
     if (graphics.printcolor)
       sprintf(line, "lattice2ps -nx %d -ny %d -color > dlat.%d.ps\n",
	       lattice.nx, lattice.ny, printcnt); 
     else
       sprintf(line, "lattice2ps -nx %d -ny %d -gray 0.1 0.9 > dlat.%d.ps\n",
	       lattice.nx, lattice.ny, printcnt);
     printstream = popen(line, "w");
     
     for (j = 0; j < lattice.ny; j++)
     {
	  for (i = 0; i < lattice.nx; i++)
	  {
	       if (set.z[j][i]) 
		 fprintf(printstream, "%d %d %e\n", i,j,zmat->el[j][i]);
	  }
     }
     fflush (printstream);
     fclose (printstream);
}

/*------------------------------------------------------------------------- */

parse_argv(argc,argv)
int argc;
char **argv;
{
     int i;
     
     /* read command line arguments */
     for(i = 1; i < argc; i++)
     {
	  /* specify number of x points */
	  if(STRCMP0(argv[i],"-nx"))
	  {
	       set.nx = 1;
	       if(sscanf(argv[++i],"%d",&lattice.nx) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* specify number of y points */
	  if(STRCMP0(argv[i],"-ny"))
	  {
	       set.ny = 1;
	       if(sscanf(argv[++i],"%d",&lattice.ny) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* set unit offset */
	  if(STRCMP0(argv[i],"-1"))
	  {
	       lattice.offset = 1;
	  }
	  /* set flag to draw arrows at each site */
	  if(STRCMP0(argv[i],"-a"))
	  {
	       graphics.mode = 1;
	       arrow.points = (XPoint *)malloc(6 * sizeof(short));
	  }
	  /* set flag to draw bonds rather than sites */
	  if(STRCMP0(argv[i],"-b"))
	  {
	       graphics.mode = 2;
	  }
	  if(STRCMP0(argv[i],"-bin"))
	  {
	       input.binary = 1;
	  }
	  /* set update mode */
	  if(STRCMP0(argv[i],"-buffer"))
	  {
	       dynamics.mode = 1;
	  }
	  /* set overriding colormap file */
	  if(STRCMP0(argv[i],"-cmap"))
	  {
	       set.map = 1;
	       strcpy(graphics.mapf, argv[++i]);
	  }
	  /* set geometry */
	  if(STRCMP0(argv[i],"-geometry"))
	  {
	       strcpy(geomstr, argv[++i]);
	  }
	  /* set flag for grayscale bitmaps (default on monochrome) */
	  if(STRCMP0(argv[i],"-gray"))
	  {
	       graphics.gray = 1;
	  }
	  /* set flag for grid */
	  if(STRCMP0(argv[i],"-grid"))
	  {
	       graphics.grid = 1;
	  }
	  /* set initial pause (in seconds) */
	  if(STRCMP0(argv[i],"-initpause"))
	  {
	       if(sscanf(argv[++i],"%d",&dynamics.initpause) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  if(STRCMP0(argv[i],"-matrix"))
	  {
	       dynamics.mode = 2;
	  }
	  /* set update mode */
	  if(STRCMP0(argv[i],"-nobuffer"))
	  {
	       dynamics.mode = 1;
	  }
	  /* specify number of pixels per linear edge of cell */
	  if(STRCMP0(argv[i],"-pix"))
	  {
	       set.pix = 1;
	       if(sscanf(argv[++i],"%d",&graphics.pix) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* specify if printed postscript is to be in color */
	  if(STRCMP0(argv[i],"-printcolor"))
	  {
	       graphics.printcolor = 1;
	  }
	  /* set default color for arrows/bonds */
	  if(STRCMP0(argv[i],"-defcolor"))
	  {
	       set.defc = 1;
	       if(sscanf(argv[++i],"%d",&graphics.defc) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* set default color for arrows/bonds */
	  if(STRCMP0(argv[i],"-title"))
	  {
	       set.defc = 1;
	       if(sscanf(argv[++i],"%s",graphics.title) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* specify nofrills mode: fast matrix mode w/o command parsing */
	  if(STRCMP0(argv[i],"-nofrills"))
	  {
	       dynamics.frills = 0;
	  }
	  /* specify if printed postscript is to be in color */
	  if(STRCMP0(argv[i],"-norange"))
	  {
	       graphics.writez = 0;
	  }
	  /* set color for text */
	  if(STRCMP0(argv[i],"-textcolor"))
	  {
	       set.textc = 1;
	       if(sscanf(argv[++i],"%d",&graphics.textc) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	  }
	  /* set colorbar position to vertical */
	  if(STRCMP0(argv[i],"-vert"))
	  {
	       map.position = 1;
	  }
	  /* replace zmin and zmax if desired */
	  if(STRCMP0(argv[i],"-z"))
	  {
	       if(sscanf(argv[++i],"%lf",&zscale.zmin) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	       if(sscanf(argv[++i],"%lf",&zscale.zmax) != 1)
		 fprintf(stderr,"bad argument %s\n",argv[i]);
	       zscale.zrange = zscale.zmax - zscale.zmin;
	       set.zscale = 1;
	  }
     }
}

/*------------------------------------------------------------------------- */

help()
{
     fprintf(stderr,
	     "\nDynamicLattice: real-time X-windows animation on two-dimensional lattices\n");
     fprintf(stderr,
	     "(C) Copyright 1990, 1991, 1993 Christopher R. Myers\n");
     fprintf(stderr,
	     "All rights reserved.\n");

     fprintf(stderr, 
	     "\nusage: DynamicLattice -nx (no. of x points ) -ny (no. of y points) \n");
     fprintf(stderr, 
	     "\t[-z zmin zmax][-1][-a][-b][-bin][-buffer][-cmap filename]\n");
     fprintf(stderr, 	    
	     "\t[-defcolor number][-geometry string][-gray][-grid][-initpause]\n");
     fprintf(stderr, 
	     "\t[-nobuffer][-nofrills][-norange][-pix pixels][-printcolor number]\n");
     fprintf(stderr, 	    
	     "\t[-textcolor number][-vert]\n\n");
     fprintf(stderr, 
	     
	     "-nx (no. of x points) and -ny (no. of y points) MUST appear on command line\n\n");
     fprintf(stderr, 
	     
	     "Possible inputs (to standard input):\n\n");
     fprintf(stderr, 
	     
	     "i j z        (i,j integer coordinates, i on [0,NX-1], j on [0,NY-1]; z real)\n");
     fprintf(stderr, 
	     
	     "i j erase    (clear site i,j)\n");
     fprintf(stderr, 
	     
	     "#erase        (clear entire window)\n");
     fprintf(stderr, 
	     
	     "i j d        (in arrow or bond mode, d = clockwise direction / 2 Pi) \n");
     fprintf(stderr, 
	     
	     "i j d l      (l = length of arrow, scaled in z range)\n");
     fprintf(stderr, 
	     
	     "i j d l c    (color arrow/bond, with c an integer entry number in colormap)\n");
     fprintf(stderr,
	     
	     "#z zmin zmax  (reset z range with minimum zmin and maximum zmax)\n");
     fprintf(stderr,
	     
	     "#draw         (force drawing in -buffer update mode)\n");
     fprintf(stderr,
	     
	     "#nobuffer     (toggle to unbuffered line-at-a-time update mode (default))\n");
     fprintf(stderr,
	     
	     "#buffer       (toggle to -buffer input mode)\n");
     fprintf(stderr,
	     
	     "#matrix       (toggle to -matrix input mode)\n");
     fprintf(stderr,
	     
	     "#print        (send current state of lattice to lattice2ps for ps hardcopy)\n");
     fprintf(stderr,
	     
	     "#string str   (write character string str to text window)\n");
     fprintf(stderr, 
	     
	     "\nAvailable options are:\n");
     fprintf(stderr, 
	     
	     "-z zmin zmax \t sets range of z values from zmin to zmax (default: [0.,1.])\n");
     fprintf(stderr, 
	     
	     "-1 \t\t for coordinates offset by 1 (x: [1,NX], y: [1,NY])\n");
     fprintf(stderr, 
	     
	     "-a \t\t draws arrows rather than filled sites\n");
     fprintf(stderr, 
	     
	     "-b \t\t draws bonds rather than filled sites\n");
     fprintf(stderr, 
	     
	     "-bin \t\t accepts binary data in matrix update mode\n");
     fprintf(stderr, 
	     
	     "-buffer \t sets update mode to be buffered\n");
     fprintf(stderr,
	     
	     "-cmap filename \t reads colormap from specified file\n");
     fprintf(stderr, 
	     
	     "-defcolor number sets default arrow/bond color to specified entry in colormap\n");
     fprintf(stderr, 
	     
	     "-geometry str \t sets geometry information from str (size ignored)\n");
     fprintf(stderr, 
	     
	     "-gray \t\t uses gray bitmaps to shade (default on monochrome)\n");
     fprintf(stderr, 
	     
	     "-grid \t\t draws a grid of lines between sites\n");
     fprintf(stderr, 
	     
	     "-initpause nsec   pauses initial drawing for nsec seconds\n");
     fprintf(stderr, 
	     
	     "-matrix \t sets update mode to be matrix \n\t\t (matrix specified in standard LASSPTools format)\n");
     fprintf(stderr, 
	     
	     "-nobuffer \t sets update mode to be unbuffered (default mode)\n");
     fprintf(stderr, 
	     
	     "-nofrills \t runs quickly in -matrix mode with no command strings allowed\n");
     fprintf(stderr, 
	     
	     "-norange \t turns off text display of z range\n");
     fprintf(stderr, 
	     
	     "-pix pixels \t allocates specified number of pixels per site\n");
     fprintf(stderr, 
	     
	     "-printcolor \t specifies printed postscript is to be in color (default gray)\n");
     fprintf(stderr, 
	     
	     "-textcolor number draws text in specified colormap entry\n");
     fprintf(stderr, 
	     
	     "-vert \t\t lays out colormap vertically at right\n");
     fprintf(stderr, "\n");
     exit(-1);
     
}

/* 
** in_bounds()
**
** Checks that a point specified is within the x,y range of the lattice.
**
** 6/8/93 jwf
*/
in_bounds(i,j)
int i,j;
{

     if ((j<lattice.ny)&&(i<lattice.nx)&&(j>=0)&&(i>=0))
        return(1);
     else {
	fprintf(stderr,"\nDynamicLattice: %d %d is outside lattice.\n",i+lattice.offset,j+lattice.offset);
 	return(0);
     }


}
     
/*------------------------------------------------------------------------- */

