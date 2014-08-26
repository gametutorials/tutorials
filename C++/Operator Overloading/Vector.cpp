#include "Vector.h"

// Okay the assignment operator -- What we want to do is be able to set two VECTORs equal
// to one another
CVector& CVector::operator =(const CVector &vec)
{
	// So simply do that -- There they are equal now
	x = vec.x;
	y = vec.y;

		return *this; // Okay quick lesson on the "this" pointer
					 // The "this" pointer is a pointer YOU ALWAYS get with a class
					// the "this" pointer is a pointer that points to all the 
				   // functions/variables associated with the class
}

// Okay here the addition operator is taking a reference to a CVector so in the code
// if A and B are both VECTORS
// A + B -- Means A overloaded '+' operator is called and B is passed in 
CVector CVector::operator +(const CVector &vec)
{
	// Okay getting a little tricky here -- We want to return a new CVector so we
	// are creating one here -- AND since we're doing addition, ideally the vector we create
	// should have the result of A.x + B.x (see explanation directly above function) for it's x value
	// and A.y + B.y (again see above) for it's y value
	CVector result(x + vec.x, y + vec.y);

	// Well our vector is made with the correct values so simply return it
	return result;
}

// Okay here the subtraction operator is taking a reference to a CVector so in the code
// if A and B are both VECTORS
// A - B -- Means A overloaded '-' operator is called and B is passed in 
CVector CVector::operator -(const CVector &vec)
{
	// Make a result vector
	CVector result;

	// Set the corresponding X and Y -- Not as tricky looking eh?  Just wanted to show a 
	// slightly different way of accomplishing the exact same thing
	result.x = x - vec.x;
	result.y = y - vec.y;

	// Well our vector is made with the correct values so simply return it
	return result;
}


// Last but not least the overloaded '<<' operator -- Notice no CVector::
// That's because "friend" functions are NOT implemented within a classes scope
// Also you DO NOT repeat the keyword "friend" when implementing the function
ostream& operator << (ostream& out, const CVector &vec)
{
	// Here treat "out" just as if it we "cout"
	// Simply format the text as whatever you want to see when a CVector is printed to the screen
	out << "(" << vec.x << "," << vec.y << ")";
	return out;
}

// So if the this was the line of code in main()
// cout << vec1; -- "cout" would be passed in as "out", "vec1" would be passed in as "vec"
// Make sense ???
