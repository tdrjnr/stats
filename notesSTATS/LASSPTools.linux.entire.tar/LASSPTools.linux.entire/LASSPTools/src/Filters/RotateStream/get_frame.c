#include "RotateStream.h"

#define FREE(a,b,c,d,e,f,i) \
  if((a)[(i)]!=NULL) free((a)[(i)]); \
  if((b)[(i)]!=NULL) free((b)[(i)]); \
  if((c)[(i)]!=NULL) free((c)[(i)]); \
  if((d)[(i)]!=NULL) free((d)[(i)]); \
  if((e)[(i)]!=NULL) free((e)[(i)]); \
  if((f)[(i)]!=NULL) free((f)[(i)])

extern char binary;
extern char long_remains;
extern char movieaxis_mode;
extern char pass_comments;
extern char pass_text;

#ifndef _NO_PROTO
char empty_file(int);
#else
char empty_file();
#endif

char input_buffer[BUFSIZ + 1];
char *next_char = NULL;
char buffer_eof = False;
int  chars_left = 0;


/*---------------------------------------------------------------------------
 * fatal(char *message);
 *
 */

#ifndef _NO_PROTO
void fatal(char *message)
#else
void fatal(message)
char *message;
#endif
{
  fprintf(stderr,"RotateStream : fatal %s\n", message);
  exit(1);
}

/*---------------------------------------------------------------------------
 * expand_frame(frame *F);
 *
 */

#ifndef _NO_PROTO
char expand_frame(frame *F)
#else
char expand_frame(F)
frame *F;
#endif
{
  size_t i, max_lines;

  max_lines = F->maxpoly;
  max_lines *= 2;
  i = max_lines * sizeof(int);
  F->num = (int *)realloc((void *)F->num, i);
  F->ordering = (int *)realloc((void *)F->ordering, i);
  i = max_lines * sizeof(double *);
  F->Act[X] = (double **)realloc((void *)F->Act[X], i);
  F->Act[Y] = (double **)realloc((void *)F->Act[Y], i);
  F->Act[Z] = (double **)realloc((void *)F->Act[Z], i);
  F->Rot[X] = (double **)realloc((void *)F->Rot[X], i);
  F->Rot[Y] = (double **)realloc((void *)F->Rot[Y], i);
  F->Rot[Z] = (double **)realloc((void *)F->Rot[Z], i);
  if(binary)
    F->color = (XColor *)realloc((void *)F->color,((size_t)max_lines)*sizeof(XColor));
  else
    F->remains = (char **)realloc((void *)F->remains,
				  ((size_t)max_lines)*sizeof(char *));

  if(F->num == NULL || F->ordering == NULL ||
     F->Act[X] == NULL || F->Act[Y] == NULL || F->Act[Z] == NULL ||
     F->Rot[X] == NULL || F->Rot[Y] == NULL || F->Rot[Z] == NULL ||
     (binary && F->color == NULL) || (!binary && F->remains == NULL))
    {
      return(False);
    }
  if(!binary) {
    if(long_remains || !pass_text)
      for (i=max_lines/2; i<max_lines; i++)
	F->remains[i] = NULL;
    else {
      i = max_lines * (size_t)INITIALTEXT;
      F->remains[0] = (char *)realloc((void *)F->remains[0],(i*sizeof(char)));
      if(F->remains[0] == NULL) return(False);
      for (i=1; i<max_lines; i++)
	F->remains[i] = F->remains[i-1] + 30;
    }
  }
  F->maxpoly = max_lines;
  return(True);
}

/*---------------------------------------------------------------------------
 * fill(char *line)
 *
 */

#ifndef _NO_PROTO
unsigned fill(int fd, char *buffer, unsigned buff_size)
#else
unsigned fill(fd, buffer, buff_size)
int fd;
char *buffer;
unsigned buff_size;
#endif
{
  int num_read;

  if(buffer_eof) return(0);
  if((num_read = read(fd, buffer, buff_size)) < 0)
    fatal("system error reading data");
  if(num_read == 0) buffer_eof = True;
  return((unsigned)num_read);
}

/*---------------------------------------------------------------------------
 * safe_move(char *line)
 *
 */

#ifdef _NO_PROTO
char safe_move(to, from, num_move)
char *to;
char *from;
size_t num_move;
{
  size_t empty_space;

  if(to < from) {
    empty_space = from - to;
    while(empty_space < num_move) {
      memcpy(to, from, empty_space);
      to += empty_space;
      from += empty_space;
      num_move -= empty_space;
    }
    if(num_move) memcpy(to, from, num_move);
  }
  else {
    empty_space = from - to;
    to += num_move;
    from += num_move;
    while(empty_space < num_move) {
      to -= empty_space;
      from -= empty_space;
      memcpy(to, from, empty_space);
      num_move -= empty_space;
    }
    if(num_move) {
      to -= num_move;
      from -= num_move;
      memcpy(to, from, num_move);
    }
  }
}
#endif

/*---------------------------------------------------------------------------
 * read_line(char **line, char Blocking)
 * 
 * read_line is a version of gets that allows both blocking and non_blocking
 * input.  it returns the number of characters in the current line and 
 * copies the data to the space pointed to by *line.  in a future version it 
 * will make *line point to the start of the new line in the input buffer.  
 * it replaces the '\n' (new line) character with a '\0' (null) character.
 *
 */

#ifndef _NO_PROTO
size_t read_line(char **line_ptr, char Blocking, int space_in_line)
#else
size_t read_line(line_ptr, Blocking, space_in_line)
char **line_ptr;
char Blocking;
int space_in_line;
#endif
{
  char *copy_to;
  size_t num_read;
  static char old_char;

  if(!chars_left) { /* if the buffer is empty try to fill it */
    if(Blocking || (!buffer_eof && !empty_file(0)))
      chars_left = fill(0,input_buffer, BUFSIZ);
    if(chars_left) /* fill traps errors internally and exits */
      next_char = input_buffer;
    else { /* no data is available */
      *line_ptr = NULL; /* non-copy version only */
      return(0);
    }
  }
/* this is the non-copy version */
  else
    *next_char = old_char;
  
  *line_ptr = next_char;
  num_read = chars_left;
  while(*(next_char++) != '\n') {
    if(!(--chars_left)) { /* checks to see if there is another char */ 
      if(num_read < BUFSIZ) { 
	if(*line_ptr != input_buffer) {
	  if((*line_ptr - input_buffer) < num_read)
#ifndef _NO_PROTO
	    memmove(input_buffer, *line_ptr, num_read);
#else
	    safe_move(input_buffer, *line_ptr, num_read);
#endif
	  else
	    memcpy(input_buffer,*line_ptr,num_read);
	  *line_ptr = input_buffer;
	}
	chars_left = fill(0,(input_buffer + num_read), (BUFSIZ - num_read));
      }
      else /* input line too big */
	fatal("error, input line too big");

      next_char = input_buffer + num_read;
      if(chars_left) /* fill traps errors internally and exits */
	num_read += chars_left;
      else break;  /* fill returns 0 at EOF */
    }
  }
  old_char = *next_char;
  *(next_char) = '\0'; /* this adds the '\0' to the end of the string */
  if(chars_left) num_read = num_read - (--chars_left);
  return(num_read); /* length does not count the '\0' */
}

/*---------------------------------------------------------------------------
 * get_row(char *line, double **x, double **y, double **z, char **rem)
 *
 * get_row returns the number of triples it found, 0 if it did not find any
 * triples and -1 on error;
 */

#ifndef _NO_PROTO
int get_row(char *line, double **x, double **y, double **z,
	     char **remains, size_t length)
#else
int get_row(line, x, y, z, remains, length)
char *line;
double **x;
double **y;
double **z;
char **remains;
size_t length;
#endif
{
  int num;
  char *rem;

  num = 0;
  if(!read_triples(line, x, y, z, &num, &rem))
    return(-1); /* read_triples returns False only on a memory error */

  if(num ==0) /* if there are no triples then the line is text */
    return(0);

  length = length - (size_t)(rem - line);
  length++; /* this is necessary since the length does not count the '\0' */

  if(long_remains) {
    /* get space for the remaining text at the end of the line */
    *remains = (char *)malloc(length * sizeof(char));
    if(*remains == NULL) /* check for memory error */
      return(-1);   /* mem error true for malloc of space for remains */
  }
  else /* short remains.  truncate anything that won't fit */
      if(length > INITIALTEXT) {
	rem[INITIALTEXT-1] = '\0';
	rem[INITIALTEXT-2] = '\n';
	length = INITIALTEXT;
      }
  /* copy the remaining string rem to remains */
  if(pass_text)
    memcpy(*remains, rem, length);
  else **remains = '\n';
  return(num);
}

/*---------------------------------------------------------------------------
 * get_frame(frame *F, char *last_line, char Blocking)
 *
 * get_frame() returns the number of lines in the frame it
 * read or EOF for EOF.  Frames are lines beginning with triples but may
 * end with text.  The end of a frame is a line that begins with text.
 * get_frame returns the line of text immediately following new frame in
 * last_line.  If EOF marks the end of the frame, then get_frame returns
 * "\n" in last_line by default, or "#Q" in the movieaxis mode.
 */
#ifndef _NO_PROTO
int get_frame(frame *F, char *last_line, char Blocking)
#else
int get_frame(F, last_line, Blocking)
frame *F;
char *last_line;
char Blocking;
#endif
{
  int iNum;
  double *iX, *iY, *iZ;
  size_t i, length, num_triples, poly_cnt;
  char *input, *iRem, *error_message;

  poly_cnt = 0;
  num_triples = 0;
  iNum = 0;
  error_message = "RotateStream : Memory error in get_frame.  Line skipped";
  if(!long_remains) iRem = F->remains[poly_cnt];

  /* skip non-numeric iRem, passing it to stdout if pass_text == True */
  while((length = read_line(&input, Blocking, BUFSIZ)) != 0 &&
	(iNum = get_row(input, &iX, &iY, &iZ, &iRem, length)) < 1) {
    if(iNum == -1)
      fprintf(stderr,"%s\n", error_message);
    else /* iNum == 0 it is just text */
      if(pass_comments) {
/*	last_line[length--] = '\0';
	last_line[length] = '\n';
 * to be implemented latter */
	fputs(input, stdout);
      }
  }
  /* if there are no lines in the pipe then we are done */
  if(length == 0) {
    if(buffer_eof)
      return(EOF);
    else
      return(0);
  }  
  /* free up space for the new frame */
  for (i=0; i<F->numpoly; i++) {
    free(F->Act[X][i]);
    free(F->Act[Y][i]);
    free(F->Act[Z][i]);
  } 
  if(long_remains)
    for (i=0; i<F->numpoly; i++)
      free(F->remains[i]);
  free(F->Rot[X][0]);
  free(F->Rot[Y][0]);
  free(F->Rot[Z][0]);
  /* install the first line of the new frame */
  F->Act[X][poly_cnt] = iX; F->Act[Y][poly_cnt] = iY;
  F->Act[Z][poly_cnt] = iZ; F->remains[poly_cnt] = iRem;
  F->num[poly_cnt] = iNum;

  /* set num_triples and poly_cnt to the correct values */
  num_triples += F->num[poly_cnt];
  poly_cnt++;

  /* get the frame of triples */
  while ((length = read_line(&input, True, BUFSIZ)) != 0 &&
	 (F->num[poly_cnt]=get_row(input, &(F->Act[X][poly_cnt]),
				   &(F->Act[Y][poly_cnt]),
				   &(F->Act[Z][poly_cnt]),
				   &(F->remains[poly_cnt]), length)))
    {
      if(F->num[poly_cnt]>0) {
	num_triples += F->num[poly_cnt];
	poly_cnt++;
	/* check to see if there is enough lines allocated in the frame F */
	if(poly_cnt == F->maxpoly)
	  if(!expand_frame(F)) /* expand_frame returns True if it succeeds */
	    fatal("memory error in expand frame");
      }
      else {
	fprintf(stderr,"%s\n", error_message);
      }
    }
  /* fix things up before returning */
  if(poly_cnt) { /* if poly_cnt == 0 then leave things alone */
    F->numpoly = poly_cnt;
    /* allocate space to hold the rotated data */
    F->Rot[X][0]=(double *)malloc(num_triples * sizeof(double));
    F->Rot[Y][0]=(double *)malloc(num_triples * sizeof(double));
    F->Rot[Z][0]=(double *)malloc(num_triples * sizeof(double));
    error_message = "RotateStream : get_frame";
    if(check_for_mem_error((void *)F->Rot[X][0], error_message) ||
       check_for_mem_error((void *)F->Rot[Y][0], error_message) ||
       check_for_mem_error((void *)F->Rot[Z][0], error_message))
      {
	exit(1);
      }
    else {
      for(i=0; i<poly_cnt-1; i++) {
	F->Rot[X][i+1] = F->Rot[X][i] + F->num[i];
	F->Rot[Y][i+1] = F->Rot[Y][i] + F->num[i];
	F->Rot[Z][i+1] = F->Rot[Z][i] + F->num[i];    
      }
    }
    if(length) {
      length++; /* this is necessary since length does not count the '\0' */
      memcpy(last_line, input, length);
    }
    else {
      /* if length is False, then we have reached the end of the file */
      if(movieaxis_mode) {
	last_line[0] = '#';
	last_line[1] = 'Q';
	last_line[2] = '\n';
	last_line[3] = '\0';
      }
      else {
	last_line[0] = '\n';
	last_line[1] = '\0';
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr,"Read %d polygons.\n",poly_cnt);
#endif
  iNum = poly_cnt;
  if(iNum == EOF) iNum = 1;
  return(iNum);
}


/*---------------------------------------------------------------------------
 * get_binary_row(double **x, double **y, double **z, XColor *color)
 *
 */
#ifndef _NO_PROTO
int get_binary_row(double **x, double **y, double **z, XColor *color)
#else
int get_binary_row(x, y, z, color)
double **x;
double **y;
double **z;
XColor *color;
#endif
{
  int num;

  if(read_binary_triples(stdin, x, y, z, &num)) {
    if(num) {
      if(read_binary_rgb_color(stdin, color))
	return(num);
      else /* fread failed */
	fatal("error in get_binary row, unable to read binary rgb color");
    }
    else /* empty line */
      return(num);
  }
  else {
    if(num) /* memory error */
      fatal("memory error in get_binary_row");
    else /* fread failed reading the first int or EOF */
      if(feof(stdin))
	return(EOF); /* EOF */
      else
	fatal("error in get_row, unable to read input");
  }
}

/*---------------------------------------------------------------------------
 * get_binary_frame(frame *F, char Blocking)
 *
 * get_binary_frame() returns the number of lines in the frame it read or 
 * EOF for EOF.  Binary rames are lines begin with a positive integer that
 * tells how many triples are on that line.  The line must end in a binary
 * rgb color.  The end of a binary frame is a line that has 0.
 */
#ifndef _NO_PROTO
int get_binary_frame(frame *F, char Blocking)
#else
int get_binary_frame(F, Blocking)
frame *F;
char Blocking;
#endif
{
  int iNum;
  double *iX, *iY, *iZ;
  XColor iColor;
  size_t i, length, num_triples, poly_cnt;
  char *error_message;

  poly_cnt = 0;
  num_triples = 0;
  error_message = "RotateStream : Memory error in get_frame.  Line skipped";

  if((iNum = get_binary_row(&iX, &iY, &iZ, &iColor)) == EOF)
    return(EOF);

  if(!iNum) return(0);
  
  for (i=0; i<F->numpoly; i++) {
    free(F->Act[X][i]);
    free(F->Act[Y][i]);
    free(F->Act[Z][i]);
  }
  free(F->Rot[X][0]);
  free(F->Rot[Y][0]);
  free(F->Rot[Z][0]);

  /* install the first line of the new frame */
  F->Act[X][poly_cnt] = iX; F->Act[Y][poly_cnt] = iY;
  F->Act[Z][poly_cnt] = iZ; F->num[poly_cnt] = iNum;
  F->color[poly_cnt] = iColor;

  /* set num_triples and poly_cnt to the correct values */
  num_triples += F->num[poly_cnt];
  poly_cnt++;

  /* get the frame of triples */
  while((F->num[poly_cnt] = get_binary_row(&(F->Act[X][poly_cnt]),
					   &(F->Act[Y][poly_cnt]),
					   &(F->Act[Z][poly_cnt]),
					   &(F->color[poly_cnt]))) > 0)
    {
      num_triples += F->num[poly_cnt];
      poly_cnt++;
      if(poly_cnt == F->maxpoly)
	if(!expand_frame(F)) /* expand_frame returns True if it succeeds */
	  fatal("memory error in expand frame");
    }
  /* fix things up before returning */
  if(poly_cnt) { /* if poly_cnt == 0 then leave things alone */
    F->numpoly = poly_cnt;
    /* allocate space to hold the rotated data */
    F->Rot[X][0]=(double *)malloc(num_triples * sizeof(double));
    F->Rot[Y][0]=(double *)malloc(num_triples * sizeof(double));
    F->Rot[Z][0]=(double *)malloc(num_triples * sizeof(double));
    error_message = "RotateStream : get_frame";
    if(check_for_mem_error((void *)F->Rot[X][0], error_message) ||
       check_for_mem_error((void *)F->Rot[Y][0], error_message) ||
       check_for_mem_error((void *)F->Rot[Z][0], error_message))
      {
	exit(1);
      }
    else {
      poly_cnt--;
      for(i=0; i<poly_cnt; i++) {
	F->Rot[X][i+1] = F->Rot[X][i] + F->num[i];
	F->Rot[Y][i+1] = F->Rot[Y][i] + F->num[i];
	F->Rot[Z][i+1] = F->Rot[Z][i] + F->num[i];    
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr,"Read %d polygons.\n",poly_cnt);
#endif
  iNum = ++poly_cnt;
  if(iNum == EOF) iNum = 1;
  return(iNum);
}
