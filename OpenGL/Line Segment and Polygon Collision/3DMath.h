#ifndef _3DMATH_H
#define _3DMATH_H

#define PI 3.1415926535897932					// This is our famous PI

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

// This returns the distance the plane is from the origin (0, 0, 0)
// It takes the normal to the plane, along with ANY point that lies on the plane (any corner)
float PlaneDistance(CVector3 Normal, CVector3 Point);

// This takes a triangle (plane) and line and returns true if they intersected
bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This returns the dot product between 2 vectors
float Dot(CVector3 vVector1, CVector3 vVector2);

// This returns the angle between 2 vectors
double AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2);

// This returns an intersection point of a polygon and a line (assuming intersects the plane)
CVector3 IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance);

// This returns true if the intersection point is inside of the polygon
bool InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount);

// Use this function to test collision between a line and polygon
bool IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added 4 new math functions to our library.  A few of these functions will 
// be used constantly throughout your 3D programming careers, so make sure you know them.
// They are :  Dot()	AngleBetweenVectors()	IntersectionPoint()   InsidePolygon()
//
// We also added a helper function called IntersectedPolygon(), which calls
// IntersectionPoint() and InsidePolygon().  This is the function that the client
// will call.  We don't need to call the other 2 functions by themselves anywhere else.
//
// From now on, the dot product will be one of the biggest math functions you will use.
// It's importance is right behind vectors and normals.
//
// Read more about these functions in 3DMath.cpp
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//


