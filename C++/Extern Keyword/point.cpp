#include "point.h"

#include <iostream>
using namespace std;

/*
	It's very important to understand that when we're declaring a variable with "extern" what 
	we are saying is "Hey this variable DOES NOT exist yet, it will be declared (filled in)
	externally".  The code below is NOT creating a new SPoint, it is in fact filling 
	in the SPoint we externed from point.h
*/
SPoint externedPoint = { 5.0f, 5.0f }; // Even though the code looks like we're declaring
									   // a new variable, all we are really doing is 
									   // initializing the variable declared in point.h

float GetX(const SPoint &pt) { return pt.x; }
float GetY(const SPoint &pt) { return pt.y; }
