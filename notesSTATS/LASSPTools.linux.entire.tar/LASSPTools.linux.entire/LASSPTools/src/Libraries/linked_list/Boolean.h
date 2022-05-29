/***************************************************************************//*


	This is Boolean.H   .

	It allows for the eloquent use of type-checked boolean logic.


	This file was written by Dean Cirielli.
	Started:  2-6-90.
	Ended:    2-6-90.


*//***************************************************************************/

#ifndef BOOLEAN_H		/*  prevent multiple copies  */
#define BOOLEAN_H


/*  We define the eloquent boolean variable type similar to Pascal.
	Note that this type is compatible with the "int" type, while the
	converse is not true ( 5 is not boolean! ).
*/

typedef enum _boolean { false = 0, true = 1 } boolean;


#endif				/*  end of #ifndef BOOLEAN_H  */

/* The end of file do not add anything */
