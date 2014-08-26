#include "ball.h"

// Constructor
BALL::BALL(float centerX, float centerY, float rad, float spd)
{
	// Set center of the BALL
	x = centerX;
	y = centerY;

	radius = rad; // Set radius
	speed = spd; // Set the speed of the BALL (the speed it's traveling at)
}

// Set the center of the BALL
void BALL::setCenter(float cX, float cY)
{
	x = cX;
	y = cY;
}

// Set the "direction vector" of the BALL (ie what direction is the BALL heading in)
void BALL::setDirection(VECTOR new_dir)
{
	dir = new_dir; // Set the new direction of the BALL

	dir.normalize(); // Make sure new direction is normalized
}

// Sets the direction of the BALL to the vector formed from the center of the BALL (x,y) 
// to POINT p1
void BALL::setDirection(POINT p1)
{
	dir.setX(p1.x - x);
	dir.setY(p1.y - y);

	dir.normalize(); // Make sure direction is normalized
}

// Updates the BALL's position
void BALL::moveBall()
{
	/*
		We know where the center of	the BALL is (x,y) -- 
		We we want to move it in the direction of vector "dir"
		We know how fast we want to move it in that direction "speed"

		So to calculate the NEW center of the BALL we take to old center (x,y) and ADD
		to it the direction we're traveling MULTIPLIED by the speed
	*/
	x += dir.getX() * speed;
	y += dir.getY() * speed;
}

// Draw the BALL to the HDC passed in
void BALL::drawBall(HDC hdc)
{
	// Okay Ellipse takes the upper left corner and the lower right corner of the
	// bounding box of the ellipse (BALL) we want to draw
	Ellipse(hdc,int(x + radius + 0.5f),int(y + radius + 0.5f),
			int(x - radius + 0.5f),int(y - radius + 0.5f));

	// So by adding/subtracting the radius from x and y we correctly calculate the bounding
	// box of the ellipse we want to draw

	// So what is the "+ 0.5f" all about?  Well the int() will cast our result to an integer
	// which always rounds down.  We add the 0.5f so we get an more accurate rounding.
	// To demonstrate this "rounding error" look at the following.

	// int someNumber = int(3.9f); // This will round 3.9 down to 3 even though it's much
								  // closer to 4

	// int someNumber = int(3.9f + 0.5f) // Well 3.9f + 0.5 = 4.4f and that rounded down
										// is 4 which is what we want since 4 is closer 
									   // to 3.9 than 3.  
}
