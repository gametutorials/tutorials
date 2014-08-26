#include "collide.h"

/*	The FIRST thing we're going to do is go over a parametric equation.

	You've probably seen this before:

	Point P1 = (x1, y1)
	Point P2 = (x2, y2)

	The LINE SEGEMENT (where 0 <= t <= 1) between P1 and P2 equals 

	X = (1-t) * x1 + t * x2
	Y = (1-t) * y1 + t * y2

	Where 't' is the time it takes to travel from P1 to P2
	Notice if you put in 0 for t, you get P1 -- If you put in 1 for t, you get P2

	Now with a little algebra (I'll go through all the steps)

	X = x1 - t*x1 + t*x2
	X = x1 + t * (-x1 + x2)
	X = x1 + t * (x2 - x1)

	Of course the same algebra would apply to the 'Y' part so the parametric equation
	form we are going to use is

	X = x1 + t * (x2 - x1)
	Y = y1 + t * (y2 - y1)

	Okay so now that your memory has been jolted you know that a parametric equation (where
	0 <= t <= 1) defines a LINE SEGMENT between two points.  And the following two sets
	of parametric equations (where (X,Y) is a point on the line segment at time 't')
	are equivalent:

	X = (1-t) * x1 + t * x2  is the same as  X = x1 + t * (x2 - x1)
	Y = (1-t) * y1 + t * y2  is the same as  Y = y1 + t * (y2 - y1)
*/

// Indexes into our "wall" array
#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3

BALL ball; // Our ball
WALL wall[MAX_WALLS]; // "Walls" (edges of the window)

// This initializes our "ball" and our "wall" array
void Init(HWND hwnd)
{
	RECT win_rect; // We'll fill this with the RECT of the window

	GetClientRect(hwnd,&win_rect); // Fill "win_rect"

	// Init our ball
	ball.setCenter(win_rect.right / 2.0f, win_rect.bottom / 2.0f); // Center of window
	ball.setRadius(10.0f);
	ball.setSpeed(5.0f);
		
		// Make a random point in the window
		POINT rand_point;

		rand_point.x = rand()%win_rect.right;
		rand_point.y = rand()%win_rect.bottom;

	// Set the direction the ball is traveling in to the vector made from the center of
	// the ball to the random point that was generated
	ball.setDirection(rand_point);

	long rad = long(ball.getRadius());

	// Make four points that define our window
	// Now we're going to take the TRUE window rect and offset all those positions 
	// by "rad" -- The reason we do this is we'll be checking for collision with the
	// the CENTER of our ball, but visually we want to see the ball "collide" when the
	// edge of a ball hits (any point "radius" units from the center of the ball)
	// See Diagram.bmp for a visual of what I mean :)
	POINT leftTop = {rad,rad};
	POINT rightTop = {win_rect.right - rad,rad};
	POINT leftBottom = {rad,win_rect.bottom - rad};
	POINT rightBottom = {win_rect.right - rad,win_rect.bottom - rad};

	// Fill in the starting and ending points of our "walls"
	wall[TOP].pt1 = leftTop;
	wall[TOP].pt2 = rightTop;

	wall[RIGHT].pt1 = rightTop;
	wall[RIGHT].pt2 = rightBottom;
	
	wall[BOTTOM].pt1 = rightBottom;
	wall[BOTTOM].pt2 = leftBottom;

	wall[LEFT].pt1 = leftBottom;
	wall[LEFT].pt2 = leftTop;
}

// This is our "main loop"
void RunApp(HDC hdc)
{
	int wallIndex;

	// If we've collided with a wall
	if(CheckCollision(wallIndex))
	{
		ChangeDirection(wallIndex); // Change the ball's direction
	}
	else
		ball.moveBall(); // Otherwise just move in the same direction we've been going

	ball.drawBall(hdc); // Draw the ball
}

// This returns true if we collided with a "wall" AND fills "which_wall" with the index of
// the wall that the collision took place with, otherwise returns false
bool CheckCollision(int &which_wall)
{
	// Remember our parametric equation summary up top?  Well this is where that's going
	// to come into use.  What we have is two line segments: ballVec (vector representing
	// the direction the ball is traveling) and each line segment that makes up a "window wall"
	// So we need to see if the ball collided with EACH "line segment" -- Now how do you
	// solve for the intersection point of two line segments?

	// It's quite simple really, you just set the equations (parametric equations in our case)
	// equal to one another and solve for "t" (the time of intersection) -- If "t" is between
	// 0 and 1, we've intersected

	// Okay, it's math time :)

	/*
		So what we have is this:

		Point P1 = (x1, y1) // Current center of the ball
		Point P2 = (x2, y2) // Center of the ball after the next frame

		Point P3 = (x3, y3) // Starting point of current wall we're checking
		Point P4 = (x4, y4) // Ending point of current wall we're checking

		BVx = x1 + t(x2 - x1) // Parametric x value of vector from P1 to P2
		BVy = y1 + t(y2 - y1) // Parametric y value of vector from P1 to P2

		Wx = x3 + u(x4 - x3) // Parametric x value of vector (wall) from P3 to P4
		Wy = y3 + u(y4 - y3) // Parametric y value of vector (wall) from P3 to P4

		Thus the line segments intersect when the following is true:

		x1 + t(x2 - x1) = x3 + u(x4 - x3)
		y1 + t(y2 - y1) = y3 + u(y4 - y3)

		So we need to solve for two unknowns 't' and 'u' -- Now I'm not going to do all the 
		math (too much typing :), but I'll recap how you solve for two unknowns.

		First put everything in terms of 'u' (ie u = some equation) for the "y part of the
		equation" -- Then take that mess and plug it in for 'u' in the "x part of 
		the equation" -- Then solve for 't' -- Using 't' plug back into "the y part of the
		equation" and out comes 'u'

		When it's all said and done, this is what we get:

		t = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) /
		    ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1))

		u = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) /
		    ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1))

		**NOTE** The denominators are EQUAL

		Now if we plug 't' into (BVx,BVy) we'll get the intersection point OR
		if we plug 'u' into (Wx,Wy) we'll get the intersection point.  If 't' and 'u' are 
		between 0 and 1, then the intersection has occured between the two LINE SEGMENTS
		defined by (BVx,BVy) and (Wx,Wy).

		All right, now lets code this puppy		

*/

	// The first thing we do is determine where the NEXT center point would be assuming
	// we keep traveling in the same direction (and at the same speed)
	float cX = ball.getX() + (ball.getDirX() * ball.getSpeed());
	float cY = ball.getY() + (ball.getDirY() * ball.getSpeed());

	// Check for collision against ALL the walls
	// To do this we're going to calculate the 't' and 'u' values from the equation above
	// If both 't' and 'u' are between 0 and 1, then we've collided, otherwise we have not
	for(int i = 0; i < MAX_WALLS; i++)
	{
		// *Remember* this comes from the notation above

		float ballVecX = cX - ball.getX(); // ballVecX == (x2 - x1)
		float ballVecY = cY - ball.getY(); // ballVecY == (y2 - y1)

		float wallX = (float)wall[i].pt2.x - wall[i].pt1.x; // wallX == (x4 - x3)
		float wallY = (float)wall[i].pt2.y - wall[i].pt1.y; // wallY == (y4 - y3)
	
		float numerator_t = wallX * (ball.getY() - wall[i].pt1.y)
						  - wallY * (ball.getX() - wall[i].pt1.x);

		float numerator_u = ballVecX * (ball.getY() - wall[i].pt1.y)
						  - ballVecY * (ball.getX() - wall[i].pt1.x);

		// Remember the denominator is the same for both the 't' and 'u' equation
		float denominator = wallY * (cX - ball.getX()) 
							- wallX * (cY - ball.getY());

		float t = numerator_t / denominator; // Calculate 't'
		float u = numerator_u / denominator; // Calculate 'u'

		// The two LINE SEGMENTS have collided if (0 < t < 1) AND  (0 < u < 1)
		if((t > 0.0f) && (t < 1.0f) && (u > 0.0f) && (u < 1.0f))
		{
			which_wall = i; // Record which wall we collided with
				return true;
		}		

	}

	return false; // No collision
}

// Changes the direction of the ball -- This is a hard thing to explain with out
// a diagram, thus I HIGHLY suggest looking at Diagram.bmp -- I'll be referring to the 
// "names" of vectors that I used in the bitmap
void ChangeDirection(int wallIndex)
{
	float vecX, vecY; // Used to store the vector components made by two wall points
	float dotProduct; // Used to hold the dot product between two vectors
	
	VECTOR normal; // This corresponds to 'n' in Diagram.bmp
				  // It will be used to store the normal to the "wall" (ie a vector that
			      // is 90° to the wall represented by (vecX, vecY))

	VECTOR vPrime(ball.getDirX(), ball.getDirY()); // This corresponds to v' in Diagram.bmp
												  // It is the direction the ball is
											     // traveling flipped (we'll flip it
												// in the next line of code)
	vPrime.flip(); // There it's flipped :)

	VECTOR a; // This corresponds to vector 'a' on the diagram -- What it will hold
			 // is the FLIPPED traveling vector of the ball PROJECTED onto the
		    // normal of the wall we've collided with (see Diagram.bmp for clearer picture :)

	VECTOR newDir; // This will hold the NEW DIRECTION the ball should travel in
				  // This will correspond to 'd' in Diagram.bmp

	// Determine which wall we collided with
	switch(wallIndex)
	{
		case TOP: // We've collided with the top of the window

			// Get the vector (x,y) for the top edge
			vecX = float(wall[TOP].pt2.x - wall[TOP].pt1.x);
			vecY = float(wall[TOP].pt2.y - wall[TOP].pt1.y);
				break;

		case BOTTOM: // We've collided with the bottom of the window

			// Get the vector (x,y) for the bottom edge
			vecX = float(wall[BOTTOM].pt2.x - wall[BOTTOM].pt1.x);
			vecY = float(wall[BOTTOM].pt2.y - wall[BOTTOM].pt1.y);
				break;

		case LEFT: // We've collided with the left of the window

			// Get the vector (x,y) for the left edge
			vecX = float(wall[LEFT].pt2.x - wall[LEFT].pt1.x);
			vecY = float(wall[LEFT].pt2.y - wall[LEFT].pt1.y);
				break;

		case RIGHT: // We've collided with the right of the window

			// Get the vector (x,y) for the right edge
			vecX = float(wall[RIGHT].pt2.x - wall[RIGHT].pt1.x);
			vecY = float(wall[RIGHT].pt2.y - wall[RIGHT].pt1.y);
				break;

	}

	// Calculate the normal to this vector (ie the vector that is perpendicular
	// to the "wall" from (vecX, vecY))
	// This corresponds to 'n' in Diagram.bmp
	normal.setX(-1 * vecY);
	normal.setY(vecX);
	
	normal.normalize(); // Make "normal" be a unit vector

	dotProduct = vPrime * normal; // Calculate the dot product between
			    				 // the ball's flipped direction vector
								// and a normal to the wall we've collided with

	// Calculate the vector 'a' -- This is simply using the linear algebra (vector math)
	// definition of "projection of a vector unto another" -- This corresponds to 'a' in 
	// Diagram.bmp
	a.setX(dotProduct * normal.getX());
	a.setY(dotProduct * normal.getY());

	/*	
		Now this is not tricky, but it calls for a little thinking and visualizing.
		If you look at Diagram.bmp you see 'a' is the vector projected onto 'n'
		Well visualize that you take 'a' double it's length (hence the multiply by 2)
		Now if 'a' is doubled and you moved v' so that the head of v' was touching the 
		head of "doubled a", you would see that 'd' (the direction we want to head) would
		make a triangle with these two vectors (ie it's the result of subtracting the two
		vectors -- Recall "How to subtract two vectors" :).  Thus, simply subtract 2 * a 
		from v' and presto, you have you new direction to travel in.
	*/
	newDir.setX(2 * a.getX() - vPrime.getX());
	newDir.setY(2 * a.getY() - vPrime.getY());

	ball.setDirection(newDir); // Set our new direction

}

