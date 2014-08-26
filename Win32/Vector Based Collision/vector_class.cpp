#include "vector_class.h"

// Constructor that sets the VECTOR to the values passed in
VECTOR::VECTOR(float x_val, float y_val)
{
	x = x_val;
	y = y_val;
}

// Constructor that takes two POINT's and creates a VECTOR from p1 to p2
VECTOR::VECTOR(POINT p1, POINT p2)
{
	setVector(p1,p2);
}

// Creates a VECTOR from p1 to p2
void VECTOR::setVector(POINT p1, POINT p2)
{
	x = float(p2.x - p1.x);
	y = float(p2.y - p1.y);
}

// Makes VECTOR a unit vector (ie magnitude == 1)
void VECTOR::normalize()
{
	float mag = magnitude(); // Get the magnitude of the vector

	// Divide (x,y) by magnitude
	x /= mag;
	y /= mag;
}

// Flips the VECTOR so it's orientated in the opposite direction
void VECTOR::flip()
{
	x *= -1;
	y *= -1;
}

// Returns the magnitude (length) of the vector
float VECTOR::magnitude()
{
	return sqrtf((x * x) + (y * y)); // sqrtf() returns the square root in "float" form
}

// Overloading of the '*' operator
// Returns the dot product of "this VECTOR" and VECTOR vvv
float VECTOR::operator *(VECTOR vvv)
{
	return (x * vvv.x) + (y * vvv.y);
}

