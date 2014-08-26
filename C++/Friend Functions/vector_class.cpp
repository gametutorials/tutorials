#include "vector_class.h"

// Overloaded Constructor
CVector::CVector(float x_val, float y_val, float z_val)
{
	// Set our x, y and z to the values passed in
	x = x_val;
	y = y_val;
	z = z_val;
}

// Here we implement our friend function.  Notice the keyword friend is NO WHERE to be 
// found.  Also notice we're NOT inside the scope of the class (no CVector:: whatever).
float dotProduct(CVector vec1, CVector vec2)
{
	// The dot product is a pretty simple function to write, hence, such little 
	// implementation.  However, take notice that we are DIRECTLY accessing
	// PRIVATE variable members.  Normally this is strictly forbidden.  If you
	// don't believe me, try setting x, y, or z in main.cpp and see what happens %)

	// So to reiterate, what friend functions give you is the ability to directly
	// access private OR protected variable members of a class inside of the 
	// friend function
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}
