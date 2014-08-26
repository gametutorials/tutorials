#include "Shape.h"

// You'll notice the implementation of these classes is just like usual,
// no "virtual" keywords or anything like that.

// Doesn't do a thing!
void CShape::draw() { return; }

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
