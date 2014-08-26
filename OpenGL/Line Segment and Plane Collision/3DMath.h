#ifndef _3DMATH_H
#define _3DMATH_H


// This is used to hold 3D points and vectors.  This will eventually be added upon
// to create a robut class with operator overloading.  For now we just need an x, y, z.
struct CVector3
{
public:
	float x, y, z;						
};


//	This returns a perpendicular vector from 2 given vectors by taking the cross product.
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

//	This returns a vector between 2 points
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2);

//	This returns the magnitude of a normal (or any other vector)
float Magnitude(CVector3 vNormal);

//	This returns a normalize vector (A vector exactly of length 1)
CVector3 Normalize(CVector3 vNormal);

//	This returns the normal of a polygon (The direction the polygon is facing)
CVector3 Normal(CVector3 vTriangle[]);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This returns the distance the plane is from the origin (0, 0, 0)
// It takes the normal to the plane, along with ANY point that lies on the plane (any corner)
float PlaneDistance(CVector3 Normal, CVector3 Point);

// This takes a triangle (plane) and line and returns true if they intersected
bool IntersectedPlane(CVector3 vTriangle[], CVector3 vLine[]);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added 2 new help functions to our math library we are building up.
// They are :  PlaneDistance() and IntersectedPlane()
// 
// Read more about these in 3DMath.cpp
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//


