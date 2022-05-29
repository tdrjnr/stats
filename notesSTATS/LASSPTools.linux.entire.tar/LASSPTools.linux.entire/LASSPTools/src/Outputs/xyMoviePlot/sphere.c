#include 	<stdio.h>
#include	<math.h>
void main ()
{ 
	double angle;
	double city;
	angle = 0;
	while (17)	
	{
		fprintf(stdout,"fyellow\n");
                if ( sin(angle + 0.2) > 0 )
                        {
                        fprintf(stdout,"%f 25 %f\n", 50 - 43 * cos(angle + 0.2),
					sin(angle + 0.2));
                        }
                if ( sin(angle) > 0.0 )
                        {
                        fprintf(stdout,"%f 58.68 %f\n", 50 - 49.24 * cos(angle),
					0.6 * sin(angle));
                        }
                if ( sin(angle + 4.9) > 0 )
                        {
                        fprintf(stdout,"%f 93 %f\n", 50 - 25 * cos(angle + 4.9),
					0.3 * sin(angle + 4.9));
                        }
                if ( sin(angle + 2.4) > 0 )
                        {
                        fprintf(stdout,"%f 14.64 %f\n", 50 - 35.36 *
					 cos(angle + 2.4), 0.7 * sin(angle + 2.4));
                        }
		fprintf(stdout,"fwhite\n");
	 	fprintf(stdout,"50 50 %f 50 0\n", fabs( 50.0 * cos (angle)));
                fprintf(stdout,"50 50 %f 50 0\n", fabs( 50.0 * cos (angle+1.044)));
                fprintf(stdout,"50 50 %f 50 0\n", fabs( 50.0 * cos (angle+2.088)));
		fprintf(stdout,"50 50 50 2 0\n\n");
		angle = angle+0.05;
	}
}	
