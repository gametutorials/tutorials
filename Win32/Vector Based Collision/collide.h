#ifndef COLLIDE_H
#define COLLIDE_H

#include "ball.h"

/*  Okay collide.h and collide.cpp are "the meat and potatoes" of this tutorial.
	This is where will make the decision if collision has happened with the edges of
	the window or not.  If it has we will change the direction of the ball according to
	some laws of physics (angle of refraction equals the angle of incidence)
	BUT we'll be disregarding some other concepts of physics (like gravity and friction)
	so our ball will always maintain the speed it's traveling at.
*/

#define MAX_WALLS 4 // Number of "walls" (window edges) to check

// This is our wall struct -- It defines a WALL (ie an edge of the window)
struct WALL
{
	POINT pt1; // Starting point of line segment
	POINT pt2; // Ending point of line segment
};

void Init(HWND hwnd); // Initializes the app

void RunApp(HDC hdc); // Runs the app

bool CheckCollision(int &which_wall); // Checks to see if the ball collided with
								     // ANY of the sides of the window -- If it
								    // has it returns true and fills "which_wall"
								   // with the index of the wall that was 
								  // collided with, otherwise returns false

void ChangeDirection(int wall_index); // Changes direction of the ball 
									 // so that it "refracts" off of the wall
									// specified by "wall_index"

#endif			
