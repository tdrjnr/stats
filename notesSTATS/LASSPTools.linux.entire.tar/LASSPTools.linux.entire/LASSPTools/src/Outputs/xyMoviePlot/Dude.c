/* This file makes the data for the funny little dude that walks along 
 * for our demonstration
 */
#include <stdio.h>
#include <math.h>
#define	foot_length   	4.0 
#define shin_length	8.0
#define thigh_length	8.0
#define shoulder_width	5.0
#define chest_height	10.0
#define forearm_length	6.0
#define bicep_length 	5.0
#define neck_length	3.0
#define hand_radius     2.0   
#define head_radius	4.0
#define G	   	8.0		/* distance from hip to outside
					   shirt tail   */
#define S		2.5

typedef struct _joint
{
 	float	X;
	float	Y;
} Joint;

void main()
{
        int i;
 	float	T;
 	float   X;
	float 	ankle_angle;
	float 	thigh_angle;
	float	elbow_angle;
	float 	neck_angle;
	Joint	left_toe;
	Joint	left_ankle;
	Joint 	left_knee;
	Joint	left_hip;
	Joint 	left_shirt_tail;
	Joint 	left_shoulder;
	Joint 	left_arm;
	Joint 	left_elbow;
	Joint 	left_wrist; 
	Joint 	right_toe;
	Joint 	right_ankle;
	Joint 	right_knee;  
	Joint 	right_hip;
	Joint 	right_shirt_tail;
	Joint	right_shoulder;
	Joint	right_arm;
	Joint	right_elbow;
	Joint	right_wrist;   	
	Joint	lower_neck;
	Joint 	upper_neck;
	Joint   head_center;
	Joint 	right_hand;
	Joint 	left_hand;
/*        set intitial values for the variables   */
	X = 20;
	ankle_angle = 0;
	elbow_angle = 45;
	thigh_angle = 45;
	T = 0;
	


/*  the following code takes a given timestep and determines the points
 *  of all the joints
 */
       
	for (i=0;i<=200;i++) {
		left_toe.X = (X - S + foot_length * cos(ankle_angle));
		left_toe.Y = 0;    
		left_ankle.X = X - S;
		left_ankle.Y = foot_length + sin(ankle_angle);
		left_knee.X = left_ankle.X;
		left_knee.Y = left_ankle.Y + shin_length;  
 		left_hip.X = left_knee.X + thigh_length * cos (thigh_angle);
		left_hip.Y = left_knee.Y + thigh_length * sin (thigh_angle);
		left_shirt_tail.X = left_hip.X - G;
		left_shirt_tail.Y = left_hip.Y;
 		left_shoulder.X = X - shoulder_width;
		left_shoulder.Y = left_hip.Y + chest_height;
		right_toe.X = (X + S + foot_length * cos(ankle_angle));
	        right_toe.Y = 0;
	        right_ankle.X = X+S;
       		right_ankle.Y = foot_length + sin(ankle_angle);
        	right_knee.X = right_ankle.X;
    	        right_knee.Y = right_ankle.Y + shin_length;
	        right_hip.X = right_knee.X - thigh_length * cos (thigh_angle);
		right_hip.Y = right_knee.Y + thigh_length * sin (thigh_angle);
	        right_shirt_tail.X = right_hip.X + G;
	        right_shirt_tail.Y = right_hip.Y;
	        right_shoulder.X = X + shoulder_width;
       		right_shoulder.Y = right_hip.Y + chest_height;
		lower_neck.X = X;
		lower_neck.Y = right_hip.Y + chest_height;
		upper_neck.X = lower_neck.X + neck_length * sin (neck_angle);
		upper_neck.Y = lower_neck.Y + neck_length * cos (neck_angle);
		right_arm.X = ( right_shoulder.X + right_shirt_tail.X ) / 2;
		right_arm.Y = ( right_shoulder.Y + right_shirt_tail.Y ) / 2; 
      		left_arm.X = ( left_shoulder.X + left_shirt_tail.X ) / 2;
		left_arm.Y = ( left_shoulder.Y + left_shirt_tail.Y ) / 2;
		left_elbow.X = left_arm.X - bicep_length;
		left_elbow.Y = left_arm.Y;
		left_wrist.X = left_arm.X - forearm_length * cos (elbow_angle);
		left_wrist.Y = left_arm.Y + forearm_length * sin (elbow_angle);
		right_elbow.X = right_arm.X + bicep_length;
       		right_elbow.Y = right_arm.Y;
        	right_wrist.X = right_arm.X + forearm_length * cos (elbow_angle);
        	right_wrist.Y = right_arm.Y + forearm_length * sin (elbow_angle);
		head_center.X = upper_neck.X;
		head_center.Y = upper_neck.Y + head_radius;
		left_hand.X = left_wrist.X - cos (elbow_angle) * hand_radius;
		left_hand.Y = left_wrist.Y + sin (elbow_angle) * hand_radius; 
		right_hand.X = right_wrist.X + cos (elbow_angle) * hand_radius;
		right_hand.Y = right_wrist.Y + sin (elbow_angle) * hand_radius;



/*  now we print each joint in the correct form */
		fprintf(stdout,"%f %f %f %f %f %f %f %f\n",left_toe.X, left_toe.Y, 
			left_ankle.X, left_ankle.Y, left_knee.X, left_knee.Y, 
			left_hip.X, left_hip.Y);
		fprintf(stdout,"%f %f %f %f %f %f %f %f\n",right_toe.X, right_toe.Y, 
                      right_ankle.X, right_ankle.Y, right_knee.X, right_knee.Y, 
                        right_hip.X, right_hip.Y);	
		fprintf(stdout,"%f %f %f %f %f %f %f %f %f %f\n", left_shoulder.X, 
			left_shoulder.Y, right_shoulder.X, right_shoulder.Y,
			right_shirt_tail.X, right_shirt_tail.Y, 
			left_shirt_tail.X, left_shirt_tail.Y, 
			left_shoulder.X, left_shoulder.Y);
		fprintf(stdout,"%f %f %f %f %f %f\n", right_arm.X, right_arm.Y, 
			right_elbow.X, right_elbow.Y, 
			right_wrist.X, right_wrist.Y);
		fprintf(stdout,"%f %f %f %f %f %f\n", left_arm.X, left_arm.Y, 
                        left_elbow.X, left_elbow.Y, 
                        left_wrist.X, left_wrist.Y);
		fprintf(stdout,"%f %f %f %f\n", lower_neck.X, lower_neck.Y, 
				      upper_neck.X, upper_neck.Y);
		fprintf(stdout,"%f %f %f\n", head_center.X, head_center.Y, head_radius);
		fprintf(stdout,"%f %f %f\n", right_hand.X, right_hand.Y, hand_radius);
		fprintf(stdout,"%f %f %f\n\n", left_hand.X, left_hand.Y, hand_radius);

/*  now we update the values of the variables so that it looks like he is moving */
		T = T + 0.01;	
		ankle_angle = 2.0 * T;
		elbow_angle = 1.0 * T;
		thigh_angle = 0.8 + cos(3.0 * T);	
		neck_angle = cos(T);
		X =  50 + 50 * sin(T); 
	}  /* end while statement */
}
