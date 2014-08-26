#ifndef _3DMATH_H
#define _3DMATH_H

#define PI 3.1415926535897932					// This is our famous PI

#define BEHIND		0
#define INTERSECTS	1
#define FRONT		2


// This is our basic 3D point/vector class
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
	
	// Here we overload the * operator so we can multiply by scalars
	CVector3 operator*(float num)
	{
		// Return the scaled vector
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	CVector3 operator/(float num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};


// This returns the absolute value of "num"
float Absolute(float num);

//	This returns a perpendicular vector from 2 given vectors by taking the cross product.
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

//	This returns the magnitude of a normal (or any other vector)
float Magnitude(CVector3 vNormal);

//	This returns a normalize vector (A vector exactly of length 1)
CVector3 Normalize(CVector3 vNormal);

//	This returns the normal of a polygon (The direction the polygon is facing)
CVector3 Normal(CVector3 vPolygon[]);

// This returns the distance between 2 3D points
float Distance(CVector3 vPoint1, CVector3 vPoint2);

// This returns the point on the line segment vA_vB that is closest to point vPoint
CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint);

// This returns the distance the plane is from the origin (0, 0, 0)
// It takes the normal to the plane, along with ANY point that lies on the plane (any corner)
float PlaneDistance(CVector3 Normal, CVector3 Point);

// This takes a triangle (plane) and line and returns true if they intersected
bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance);

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

// This function classifies a sphere according to a plane. (BEHIND, in FRONT, or INTERSECTS)
int ClassifySphere(CVector3 &vCenter, 
				   CVector3 &vNormal, CVector3 &vPoint, float radius, float &distance);

// This takes in the sphere center, radius, polygon vertices and vertex count.
// This function is only called if the intersection point failed.  The sphere
// could still possibly be intersecting the polygon, but on it's edges.
bool EdgeSphereCollision(CVector3 &vCenter, 
						 CVector3 vPolygon[], int vertexCount, float radius);

// This returns true if the sphere is intersecting with the polygon.
bool SpherePolygonCollision(CVector3 vPolygon[], 
							CVector3 &vCenter, int vertexCount, float radius);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This returns the offset the sphere needs to move in order to not intersect the plane
CVector3 GetCollisionOffset(CVector3 &vNormal, float radius, float distance);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing was added to this header file except GetCollisionOffset().  This allows
// us to determine the offset that the sphere needs to move away from the plane.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//


