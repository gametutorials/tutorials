#ifndef POINT_H
#define POINT_H

// Here's our SIMPLE point struct -- It contains an (x,y)
struct SPoint
{
	float x;
	float y;
};

// Here's some simple functions to interact with our point struct:
float GetX(const SPoint &pt); // Returns the 'x' of the SPoint passed in
float GetY(const SPoint &pt); // Returns the 'y' of the SPoint passed in

// Here we declare an EXTERNAL instance of our sweet SPoint struct:
extern SPoint externedPoint;

// *** WARNING!  COMMENT IN BELOW CODE TO SEE A COMPILER ERROR! ***
// Yep, you read that above comment correctly, this seemingly harmless 
// piece of code below would cause a compiler error.  This is because point.h is 
// included in point.cpp and main.cpp, which effectively creates this variable twice, giving
// us a dual declaration of the same variable which is bad (i.e. you can't declare two 
// variables with the same name in the same scope).  This is just one
// illustration of where using extern would allow you to declare a variable in a header
// file and allow it to be used by other multiple files.
//SPoint notExternedPoint; // *** UNCOMMENT THIS TO SEE A COMPILER ERROR ***

#endif
