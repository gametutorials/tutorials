#ifndef VECTOR_CLASS_H
#define VECTOR_CLASS_H

// This is our 2 dimensional vector class -- This tutorial assumes you understand what 
// vectors are how they can be used, etc, etc

// If you are a little rusty on vector math, here's a site where you can brush up:  
// http://www.physicsclassroom.com/Class/vectors/U3L1a.html

#include <windows.h>
#include <math.h>

class VECTOR
{
	public:

		// Our default constructor
		VECTOR():x(0.0f),y(0.0f) { /* do nothing */ }

		// Constructor -- Sets the VECTOR to the values passed in
		VECTOR(float x_val, float y_val);

		// Constructor -- Creates the VECTOR from p1 to p2
		VECTOR(POINT p1, POINT p2);

		void setX(float x_val) { x = x_val; } // Sets x to value passed in
		void setY(float y_val) { y = y_val; } // Sets y to value passed in
		
		// Creates the VECTOR from p1 to p2
		void setVector(POINT p1, POINT p2);

		void normalize(); // Normalizes the VECTOR (makes it a unit vector)
		void flip(); // Flips the VECTOR's direction

		float magnitude(); // Returns the magnitude of the VECTOR

		// Overload the '*' operator
		// It will return the dot product between "this VECTOR" and VECTOR vvv
		float operator *(VECTOR vvv);

		// *** Data access

		float getX() { return x; }
		float getY() { return y; }

		// End of data access ***


	private:

		float x; // x coord of VECTOR
		float y; // y coord of VECTOR

};

#endif
