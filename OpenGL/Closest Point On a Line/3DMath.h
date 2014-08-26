#ifndef _3DMATH_H
#define _3DMATH_H

#define PI 3.1415926535897932					// This is our famous PI


// This is our 3D point and vector class which has the +, -, and * operators overloaded
struct CVector3
{
public:
	
	// A default constructor
	CVector3() {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Here we overload the + operator so we can add vectors together 
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result.
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator*(float num)
	{
		// Return the subtracted vectors result
		return CVector3(x * num, y * num, z * num);
	}

	float x, y, z;						
};


//	This returns the magnitude of a normal (or any other vector)
float Magnitude(CVector3 vNormal);

//	This returns a normalize vector (A vector exactly of length 1)
CVector3 Normalize(CVector3 vNormal);

// This returns the dot product between 2 vectors
float Dot(CVector3 vVector1, CVector3 vVector2);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This returns the distance between 2 3D points
float Distance(CVector3 vPoint1, CVector3 vPoint2);

// This returns the point on the line segment vA_vB that is closest to point vPoint
CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added to new functions to our math library (and took some out to cut out
// uneeded code for this tutorial).  We can now find the distance from 2 3D points,
// along with finding the point on a line segment that is closest to another point.
// This comes into play when doing sphere polygon collision.  Check out our tutorial
// on line at www.GameTutorials.com for the implementation of this technique.
//
// Check out 3DMath.cpp for the explanations of how these new functions work.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//


