/*
 *
 *      Title: xyMoviePlot
 *
 * Written by: Michael G. Reed  (reed@Theory.TC.Cornell.Edu)
 *             Robert B. Pless  (robert@Theory.TC.Cornell.Edu)
 *
 *     Course: ComS 418 (Computer Graphics Practicum)
 *
 * Department: Computer Science and Material Science
 *
 * (c) Copyright 1992, Cornell University
 *
 *       File: file.c
 *
 *    Purpose: Handle all file I/O for the application.
 *
 *    History: 01/28/92 - MGR - Initial coding of application.
 *             01/29/92 - MGR - Added Next_Char and Write_Char.
 *             04/20/92 - RBP - Added comments for all procedures.
 *
 */


#include "globals.h"


void Load_Image ()

/*  Load an Image to be put into the background */

{


}


/* A Special THANKS to Freek Bomhof (F.W.Bomhof@research.ptt.nl for the
   following code fragment which prevents I/O bottlenecks */

int Is_Char ()

/*  Returns whether or not that is a character in the data stream */

{

	int	fd,oldstat,dummy,present = 0;
	char	ch;
	
	fd = fileno (INPUT);
	oldstat = fcntl (fd, F_GETFL, dummy);
	fcntl (fd, F_SETFL, oldstat | O_NDELAY);
	ch = getc (INPUT);
	if (ch != -1)
		{
		present = 1;
		ungetc (ch, INPUT);
		}
	fcntl (fd, F_SETFL, oldstat);
	return present;
}


int Next_Char ()

/*  return the next character in the input stream */

{

	int	c;

	c = fgetc (INPUT);
	if (PIPE)
		Write_Char (c);
	return c;
}


void Non_Space ()

/* skip ahead to the next non space or carraige return character. */

{

	int	c;

	while (((c=Next_Char()) == ' ') || (c == '\n'))
		{
		}
	ungetc (c,INPUT);
}


void Write_Char (c)

/*  Write a character.  This is used when we are recording the input stream */

	int	c;

{

	if (OUTPUT != NULL)
		{
		if ((fputc (c, OUTPUT)) == EOF)
			{
			Soft_Error (OUTPUTERR);
			}
		}
}


Parse Parse_Input ()
/*
 *  This reads one data value.
 *  It keeps reading characters until it finds a space, end-of-line, or comma
 *  at which point it traslates the string of numbers in a floating point 
 *  number.  Return.code.valid returns true for any valid data entry, on the
 *  last entry of the line, return.code.reason = /n  
 */


{
        char    temp[50];
        int     i=0,c;
        Parse   return_code;

        while (((c = Next_Char()) != ' ') && (c != ',') && (c != '\n'))
                {
                if ((c >= '0' && c <= '9') || (c == '-') || (c == '.'))
                        temp[i++] = c;
                else
                        {
                        while (Next_Char() != '\n')
                                {
                                }
                        Soft_Error (BADINPUT);
                        return_code.valid = FALSE;
                        return (return_code);
                        }
                }
        temp[i]='\0';
        return_code.value = atof(temp);
        return_code.valid = TRUE;
	return_code.reason = c;
        return (return_code);
}


char *Read_EOL ()

/*  read to the end of line... and return the string.. (for input-data 
 *  stream option that take a string as input   
 */

{

	char	temp[200];
	char	*new_str;
	int	pos = 0;

	while (((temp[pos] = Next_Char ()) != '\n') && (pos++ < 200))
		{
		}
	temp[pos] = '\0';
	new_str = (char *) malloc (sizeof(char) * (strlen (temp)+1));
	strcpy (new_str,temp);
	return (new_str);
}


void Dump_Screen ()

/*  Dump the screen to a standard pix-map file */

{
fprintf (stderr,"Dump_Screen\n");
}


void Start_Record ()

/*  start recording the input.  basically just pipe it to another file */

{

	char	*temp;

	temp = Read_EOL ();
	if ((strlen(temp) == 1) && (temp[0] == '-'))
		{
		OUTPUT = stdout;
		}
	else
		{
		if ((OUTPUT = fopen(temp, "a")) == NULL)
			Soft_Error (OUTPUTERR);
		}
	PIPE = TRUE;
}

