#ifndef BALL_H
#define BALL_H

#include "vector_class.h"

// Okay what we have here is our "BALL class" -- Our application is going to consist of 
// a ball that will move around the window and bounce off the edge's of the window

class BALL
{
	public:

		// Default constructor
		BALL():x(0.0f),y(0.0f),radius(0.0f),speed(0.0f) { /* do nothing */ }
		
		// Constructor -- Sets center of ball, radius and speed
		BALL(float centerX, float centerY, float rad, float spd);
		
		void setRadius(float r) { radius = r; } // Set the radius of the BALL
		void setSpeed(float s) { speed = s; } // Set the speed of the BALL

		void setCenter(float cX, float cY); // Set the center of the BALL
		void setDirection(VECTOR new_dir); // Set the direction of the BALL to "new_dir"
		void setDirection(POINT p1); // Sets the direction of the BALL to the 
								    // vector from the center of the ball to "p1"

		void moveBall(); // Moves the ball in the current direction while 
					    // traveling at "speed"

		void drawBall(HDC hdc); // Draws the ball to the specified HDC

		// *** Data access

		float getX() { return x; }
		float getY() { return y; }
		float getRadius() { return radius; }
		float getSpeed() { return speed; }
		float getDirX() { return dir.getX(); }
		float getDirY() { return dir.getY(); }

		// End of data access ***

	private:

		float x; // Center x coordinate of ball
		float y; // Center y coordinate of ball

		float radius; // The radius of the ball
		float speed;

		VECTOR dir; // Direction ball is traveling in

};

#endif
