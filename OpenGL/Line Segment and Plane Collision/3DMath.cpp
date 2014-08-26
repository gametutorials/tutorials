//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		PlaneCollision									 //
//																		 //
//		$Description:	This demonstrates line and plane collision.		 //
//																		 //
//***********************************************************************//

#include "main.h"
#include <math.h>	// We include math.h so we can use the sqrt() function


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Because in our future tutorials we will be reusing all of this code quite often,
// we will put it in a separate .cpp file.  Also this is nice because then we can
// just copy 3DMath.cpp and 3DMath.h into a new project without having to cut and paste code.
// Eventually, once this gets big enough you will probably want to make a library out of it.

// *FINDING THE INTERSECTION OF A LINE AND PLANE*


// We added 2 functions to this math file:  PlaneDistance() and IntersectPlane().
// This will enable us to check if a line intersects a plane.  The next tutorial
// go go farther in depth and check a line against a polygon, not just it's plane.
// The most important part of this tutorial is the plane equation, Ax + By + Cz + D = 0.

/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;									// The vector to hold the cross product

	// Once again, if we are given 2 vectors (directions of 2 sides of a polygon)
	// then we have a plane define.  The cross product finds a vector that is perpendicular
	// to that plane, which means it's point straight out of the plane at a 90 degree angle.
	
	// The X value for the vector is:  (V1.y * V2.z) - (V1.z * V2.y)													// Get the X value
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
														
	// The Y value for the vector is:  (V1.z * V2.x) - (V1.x * V2.z)
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
														
	// The Z value for the vector is:  (V1.x * V2.y) - (V1.y * V2.x)
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;										// Return the cross product (Direction the polygon is facing - Normal)
}


/////////////////////////////////////// VECTOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a vector between 2 points
/////
/////////////////////////////////////// VECTOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector = {0};								// Initialize our variable to zero

	// In order to get a vector from 2 points (a direction) we need to
	// subtract the second point from the first point.

	vVector.x = vPoint1.x - vPoint2.x;					// Get the X value of our new vector
	vVector.y = vPoint1.y - vPoint2.y;					// Get the Y value of our new vector
	vVector.z = vPoint1.z - vPoint2.z;					// Get the Z value of our new vector

	return vVector;										// Return our new vector
}


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a normal (or any other vector)
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// This will give us the magnitude or "Norm" as some say, of our normal.
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)  Where V is the vector

	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
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


/////////////////////////////////////// NORMAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the normal of a polygon (The direction the polygon is facing)
/////
/////////////////////////////////////// NORMAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normal(CVector3 vTriangle[])					
{														// Get 2 vectors from the polygon (2 sides), Remember the order!
	CVector3 vVector1 = Vector(vTriangle[2], vTriangle[0]);
	CVector3 vVector2 = Vector(vTriangle[1], vTriangle[0]);

	CVector3 vNormal = Cross(vVector1, vVector2);		// Take the cross product of our 2 vectors to get a perpendicular vector

	// Now we have a normal, but it's at a strange length, so let's make it length 1.

	vNormal = Normalize(vNormal);						// Use our function we created to normalize the normal (Makes it a length of one)

	return vNormal;										// Return our normal at our desired length
}



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the distance between a plane and the origin
/////
/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
									
float PlaneDistance(CVector3 Normal, CVector3 Point)
{	
	float distance = 0;									// This variable holds the distance from the plane tot he origin

	// Use the plane equation to find the distance (Ax + By + Cz + D = 0)  We want to find D.
	// For more information about the plane equation, read about it in the function below (IntersectedPlane())
	// Basically, A B C is the X Y Z value of our normal and the x y z is our x y z of our point.
	// D is the distance from the origin.  So, we need to move this equation around to find D.
	// We come up with D = -(Ax + By + Cz)
														// Basically, the negated dot product of the normal of the plane and the point. (More about the dot product in another tutorial)
	distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

	return distance;									// Return the distance
}

/////////////////////////////////// INTERSECTED PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks to see if a line intersects a plane
/////
/////////////////////////////////// INTERSECTED PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
											
bool IntersectedPlane(CVector3 vTriangle[], CVector3 vLine[])
{
	float distance1=0, distance2=0;						// The distances from the 2 points of the line from the plane
			
	CVector3 vNormal = Normal(vTriangle);				 // We need to get the normal of our plane to go any further

	// Now that we have the normal, we need to calculate the distance our triangle is from the origin.
	// Since we would have the same triangle, but -10 down the z axis, we need to know
	// how far our plane is to the origin.  The origin is (0, 0, 0), so we need to find
	// the shortest distance our plane is from (0, 0, 0).  This way we can test the collision.
	// The direction the plane is facing is important (We know this by the normal), but it's
	// also important WHERE that plane is in our 3D space.  I hope this makes sense.

	// We created a function to calculate the distance for us.  All we need is the normal
	// of the plane, and then ANY point located on that plane.  Well, we have 3 points.  Each
	// point of the triangle is on the plane, so we just pass in one of our points.  It doesn't
	// matter which one, so we will just pass in the first one.  We get a single value back.
	// That is the distance.  Just like our normalized normal is of length 1, our distance
	// is a single value too.  It's like if you were to measure something with a ruler,
	// you don't measure it according to the X Y and Z of our world, you just want ONE number.

	float originDistance = PlaneDistance(vNormal, vTriangle[0]);

	// Now the next step is simple, but hard to understand at first.  What we need to
	// do is get the distance of EACH point from out plane.  Above we got the distance of the
	// plane to the point (0, 0, 0) which happens to be the origin, now we need to get a distance
	// for each point.  If the distance is a negative number, then the point is BEHIND the plane.
	// If the distance is positive, then the point is in front of the plane.  Basically, if the
	// line collides with the plane, there should be a negative and positive distance.  make sense?
	// If the line pierces the plane, it will have a negative distance and a positive distance,
	// meaning that a point will be on one side of the plane, and one point on the other.  But we
	// will do the check after this, first we need to get the distance of each point to the plane.

	// Now, we need to use something called the plane equation to get the distance from each point.
	// Here is the plane Equation:  (Ax + By + Cz + D = The distance from the plane)
	// If "The distance from the plane" is 0, that means that the point is ON the plane, which all the polygon points should be.
	// A, B and C is the Normal's X Y and Z values.  x y and z is the Point's x y and z values.
	// "the Point" meaning one of the points of our line.  D is the distance that the plane
	// is from the origin.  We just calculated that and stored it in "originDistance".
	// Let's fill in the equation with our data:

	// Get the distance from point1 from the plane using: Ax + By + Cz + D = (The distance from the plane)

	distance1 = ((vNormal.x * vLine[0].x)  +					// Ax +
		         (vNormal.y * vLine[0].y)  +					// Bx +
				 (vNormal.z * vLine[0].z)) + originDistance;	// Cz + D

	// We just got the first distance from the first point to the plane, now let's get the second.
	
	// Get the distance from point2 from the plane using Ax + By + Cz + D = (The distance from the plane)
	
	distance2 = ((vNormal.x * vLine[1].x)  +					// Ax +
		         (vNormal.y * vLine[1].y)  +					// Bx +
				 (vNormal.z * vLine[1].z)) + originDistance;	// Cz + D

	// Ok, we should have 2 distances from the plane, from each point of our line.
	// Remember what I said about an intersection?  If one is negative and one is positive,
	// that means that they are both on either side of the plane.  So, all we need to do
	// is multiply the 2 distances together, and if the result is less than 0, we intersection.
	// This works because, any number times a negative number is always negative, IE (-1 * 1 = -1)
	// If they are both positive or negative values then it will be above zero.

	if(distance1 * distance2 >= 0)			// Check to see if both point's distances are both negative or both positive
	   return false;						// Return false if each point has the same sign.  -1 and 1 would mean each point is on either side of the plane.  -1 -2 or 3 4 wouldn't...
					
	return true;							// The line intersected the plane, Return TRUE
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// That's all we need to find if a ray (line) and plane intersect.  Pretty simple eh?
// I think it's not too complicated.  Once you understand the a cool math function
// called the dot product, it starts to make sense.  We will learn more about this in
// the next collision tutorial.  That is when stuff actually start getting complicated.
// Right now, all we can collide with is a plane.  In the next tutorial we will be
// able to just collide with the polygon, not just the plane the polygon is on.
//  
// Here are some of the more important steps to finding the intersection of a line and plane:
// 
// 1) First we need a polygon (at least 3 points) and a line segment.  We then find
//    the normal of the polygon.
// 
// 2) Once we have the normal, we want to find the distance the plane is from the origin.
//    We use the plane equation (Ax + By + Cz + D = 0) to find the distance.
//	  We want D, so we turn that into D = -(Ax + By + Cz)
// 
// 3) With the distance, we can now fill in the rest of the plane equation to find
//    the distance a point is from the plane.  So (Ax + By + Cz + D = The distance from the plane)
//    A B C is the plane's normal.X normal.Y normal.Z, and x y z is the point's x y z value.
//    We get the first points distance, then get the second points distance.
// 
// 4) If we times the 2 distances of each point together, we either get a positive or negative number.
//    If the number is negative, that means we collided because each distance had to be the opposite value.
//    If the result is positive, that means that the 2 points were on the same side of the plane as each other.
// 
// That's it!
// 
// Let us know at www.GameTutorials.com if this was helpfull to you.  Any feedback is welcome.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
