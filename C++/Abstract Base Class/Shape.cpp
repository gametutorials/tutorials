#include "Shape.h"

// Notice we only implemented the CTriangle and CSquare class.  The CShape class is
// an ABC, and the only thing in it is a pure virtual function which DOES NOT require 
// implementation.

// Also remember that once you've put the keyword "virtual" in a base class,
// you do not need to repeat it.  These methods become "virtual" by default.

// The CTriangle draw method draws an ASCII triangle
void CTriangle::draw()
{
	cout << "  *  " << endl;
	cout << " * * " << endl;
	cout << "*****" << endl;

	cout << endl;
}

// The CSquare draw method draws an ASCII square
void CSquare::draw()
{
	cout << "******" << endl;
	cout << "*    *" << endl;
	cout << "*    *" << endl;
	cout << "******" << endl;

	cout << endl;
}
