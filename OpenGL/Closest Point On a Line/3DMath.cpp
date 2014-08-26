//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		ClosestPointOnLine								 //
//																		 //
//		$Description:	Finds closest point on a line to another point	 //
//																		 //
//***********************************************************************//

#include "main.h"
#include <math.h>	// We include math.h so we can use the sqrt() function


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This tutorial was sort of built from the first collision tutorial of a line and plane.
// Functions were take out that had to deal with collision because they were not used
// in this tutorial.  We added 2 more important math functions to our math library:
// 
// // This returns the distance from 2 3D points 
// float Distance(CVector3 vPoint1, CVector3 vPoint2);
//
// // This returns the point on a line segment that is closest to a given 3D point
// CVector3 ClosetPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint)
//
// These functions allow us to find the closest point on a line segment from a given point.
// This is used in collision detection and a few other cases.
//
//


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a normal (or any other vector)
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// This will give us the magnitude or "Norm" as some say, of our normal.
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)  Where V is the vector

	return (float)sqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
}


/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a normalize vector (A vector exactly of length 1)
/////
/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normalize(CVector3 vNormal)
{
	float magnitude = Magnitude(vNormal);				// Get the magnitude of our normal

	// Now that we have the magnitude, we can divide our normal by that magnitude.
	// That will make our normal a total length of 1.  This makes it easier to work with too.

	vNormal.x /= magnitude;								// Divide the X value of our normal by it's magnitude
	vNormal.y /= magnitude;								// Divide the Y value of our normal by it's magnitude
	vNormal.z /= magnitude;								// Divide the Z value of our normal by it's magnitude

	// Finally, return our normalized normal.

	return vNormal;										// Return the new normal of length 1.
}

/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This computers the dot product of 2 vectors
/////
/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Dot(CVector3 vVector1, CVector3 vVector2) 
{
	// The dot product is this equation: V1.V2 = (V1.x * V2.x  +  V1.y * V2.y  +  V1.z * V2.z)
	// In math terms, it looks like this:  V1.V2 = ||V1|| ||V2|| cos(theta).
	// It returns the distance of the projected vector, vVector2, from the start of vVector1.
	
			 //    (V1.x * V2.x        +        V1.y * V2.y        +        V1.z * V2.z)
	return ( (vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z) );
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

/////////////////////////////////// DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the distance between 2 3D points
/////
/////////////////////////////////// DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Distance(CVector3 vPoint1, CVector3 vPoint2)
{
	// This is the classic formula used in beginning algebra to return the
	// distance between 2 points.  Since it's 3D, we just add the z dimension:
	// 
	// Distance = sqrt(  (P2.x - P1.x)^2 + (P2.y - P1.y)^2 + (P2.z - P1.z)^2 )
	//
	double distance = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
						    (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
						    (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

	// Return the distance between the 2 points
	return (float)distance;
}


////////////////////////////// CLOSET POINT ON LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the point on the line vA_vB that is closest to the point vPoint
/////
////////////////////////////// CLOSET POINT ON LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint)
{
	// This function takes a line segment, vA to vB, then a point, vPoint.
	// We want to find the closet point on the line segment to our vPoint
	// out in space.  Either it is going to be one of the end points of the line,
	// or it is going to be somewhere between vA and vB.  This is a important
	// function when dealing with collision detection.

	// Here is how it works, it's a bit confusing at first, so you will need
	// to contemplate it a bit.  First, we want to grab a vector from "vA" to the point.
	// Then we want to get a normalized vector from "vA" to "vB".  We don't need the
	// full length of the line segment vector, we just want a direction.  That is why 
	// we normalize it.  Remember, this is important because we are going to be using 
	// the dot product coming up next.  So, now we have 2 vectors that form a pseudo corner
	// of a triangle on the plane of the line segment and the point.
	//
	// Next, we want to find the distance or "magnitude" of the line segment.  This is
	// done with a simple distance formula.  Then we use the dot "vVector2" with "vVector1".
	// By using the dot product, we can essentially project vVector1 onto the
	// line segments normalized vector, "vVector2".  If the result of the dot product is
	// 0, that means the vectors were perpendicular and had a 90 degree angle between them.
	// The 0 part is the distance the new projected vector is from the starting of vVector2.
	// If the result is a negative number, that means the angle between the 2 vectors
	// is greater than 90 degrees, which means that the closest point must be "vA" because
	// it's projected vector is on the outside of the line.  So, if the result is a positive
	// number, the projected vector is on the right side of "vA", but could be past the right
	// side of vB.  To test this, we make sure that the result of the dot product is NOT
	// greater than the distance "d".  If it is, then the closest point on the plane is
	// obviously vB.  
	//
	// So, we can find the closest point easily if it's one of the end points of the line
	// segment, but how do we find the point between the 2 end points?  This is simple.
	// Since we have the distance "t" from point "vA" (given to us from the dot product 
	// of the 2 vectors), we just use our vector that is going the direction of the
	// line segment, "vVector2", and multiply it by the distance scalar "t".  This will
	// create a vector going in the direction of the line segment, with a distance
	// (or magnitude) of the projected vector, "vVector1", is from from "vA".  We then add
	// this vector to "vA", which gives us the point on the line that is closest to our
	// point out in space, vPoint!  
	//
	// This is probably pretty hard to visualize with just comments, unless you have a good 
	// grasp of linear algebra.  
	
	// Create the vector from end point vA to our point vPoint.
	CVector3 vVector1 = vPoint - vA;

	// Create a normalized direction vector from end point vA to end point vB
    CVector3 vVector2 = Normalize(vB - vA);

	// Use the distance formula to find the distance of the line segment (or magnitude)
    float d = Distance(vA, vB);

	// Using the dot product, we project the vVector1 onto the vector vVector2.
	// This essentially gives us the distance from our projected vector from vA.
    float t = Dot(vVector2, vVector1);

	// If our projected distance from vA, "t", is less than or equal to 0, it must
	// be closest to the end point vA.  We want to return this end point.
    if (t <= 0) 
		return vA;

	// If our projected distance from vA, "t", is greater than or equal to the magnitude
	// or distance of the line segment, it must be closest to the end point vB.  So, return vB.
    if (t >= d) 
		return vB;
 
	// Here we create a vector that is of length t and in the direction of vVector2
    CVector3 vVector3 = vVector2 * t;

	// To find the closest point on the line segment, we just add vVector3 to the original
	// end point vA.  
    CVector3 vClosestPoint = vA + vVector3;

	// Return the closest point on the line segment
	return vClosestPoint;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This is one of the more simple math tutorials to code, but it's incredibly
// important to have when doing collision detection.  In the Sphere-Polygon collision
// tutorial, we will use this technique to determine the collision on the edges.
// 
// Let me go over the basics of how we find the closest point on a line segment, to a
// given point.
// 
// The dot product is the key function that makes all this magic work.  Basically,
// if we get a vector between an end point of the line segment and the point, plus
// a normalized directional vector of that end point to the other end point, we can
// use the dot product to project the first vector onto the directional vector.  This
// gives us a distance from the new project vector to the end point.  If that distance
// is 0, the 2 vectors were perpendicular and the projected vector is directory on top
// of the end point, which would make the distance 0.  If the distance is negative, the
// point is on the outside of the line segment and closest to that end point.  If the
// distance is positive; we know that the point is on the other side of the end point.
// If we find the distance of the line segment, we can then check to see if the point
// is closest to a point on the line, or it is past the other end point, which makes that
// other end point the closest point to our given point.  To check the closest point
// is inside the line and not an end point, we just make sure that the original dot product
// result is less than the distance of the line segment. This distance can be calculated 
// using the normal distance formula you use in algebra.  
// 
// So to sum it all up... if the distance "t" is less than 0, our first end point is the
// closest point on the line, where if the distance "t" is greater than the distance of
// the line, distance "d", then it's the other end point.  If both of those fail, it must
// be on the line between the 2 end points.  To calculate where, we multiply our directional
// vector times the result of the dot product.  Then add that result to our original end point.
// That will give us the closest point on the line, between the 2 end points.
// 
// Let us know at www.GameTutorials.com if this was helpful to you.  
// Any feedback is welcome.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
