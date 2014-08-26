//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Normals											 //
//																		 //
//		$Description:	This shows how to find the normal of a polygon.  //
//																		 //
//***********************************************************************//

#include "main.h"
#include <math.h>	// We include math.h so we can use the sqrt() function


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Because in our future tutorials we will be reusing all of this code quite often,
// we will put it in a separate .cpp file.  Also this is nice because then we can
// just copy 3DMath.cpp and 3DMath.h into a new project without having to cut and paste code.
// Eventually, once this gets big enough you will probably want to make a library out of it.

// *FINDING THE NORMAL OF A POLYGON*

// In order to find the normal of a polygon, we need to take the cross product of 2
// vectors from that polygon.  What that means, is that we need to get the directions
// of 2 of the sides of our triangle.  After all, that is all a vector is, just a direction
// and a length.  The length of our vectors in this case isn't really important.  We are
// just going to use the direction.  Then, with those 2 vectors we can find a vector coming
// straight out of the polygon, perpendicular (90 degrees) to the polygon.  To give you some
// examples of a cross product of 2 vectors, let's assume that we have a piece of paper
// lying flat on a table.  Where would the cross product of that paper be pointing?
// Straight up!  But, the paper isn't a polygon really?  Well, all you are really getting
// is the normal to a plane.  A plane is infinite, it goes on forever.  If you drew a triangle
// in the middle of the piece of paper, the normal of that triangle would be the same as the paper,
// so it doesn't matter does it?  That's why you are really getting a normal to the polygon's plane.
// If you stuck the piece of paper straight up vertically, where would the normal be?  It would 
// follow the paper perfectly as a 90 degree angle, so it would now we perfectly horizontal.  Now, 
// depending on the order of your vertices, it could either being pointing out on one side of the paper, 
// or on the other side.  You just have to decide which order you want to use, then ALWAYS stick
// with that.  Usually, you want to do something called back face culling which doesn't
// draw the back of a polygon.  That way you aren't drawing twice as much as you could be.
// Think about it, it you have a huge 3D model, do you really need to draw the inside of
// the model?  Of course not, that would be wasteful.  You always want to have your objects
// enclosed so they don't show back of polygons.  Like in a 3D maze, you would just have
// polygon thin walls, you would have thick walls which comprised of a cube like shape.
// That way, when you get into BSP trees, you can do something called Solid Node BSP tree's.


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
	// The equation for the cross product is simple, but difficult at first to memorize:
	
	// The X value for the vector is:  (V1.y * V2.z) - (V1.z * V2.y)													// Get the X value
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
														
	// The Y value for the vector is:  (V1.z * V2.x) - (V1.x * V2.z)
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
														
	// The Z value for the vector is:  (V1.x * V2.y) - (V1.y * V2.x)
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// *IMPORTANT* This is not communitive. You can not change the order or this or
	// else it won't work correctly.  It has to be exactly like that.  Just remember,
	// If you are trying to find the X, you don't use the X value of the 2 vectors, and
	// it's the same for the Y and Z.  You notice you use the other 2, but never that axis.
	// If you look at the camera rotation tutorial, you will notice it's the same for rotations.

	// Now we have a vector pointing straight out of the plane of the polygon.  This is the
	// direction the polygon is facing.  You use this information for numerous things like
	// lighting, camera viewing, back face culling, collision and tons of other stuff.

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

	// Now that we have our new vector between the 2 points, we will return it.

	return vVector;										// Return our new vector
}


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a normal (or any other vector)
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// This will give us the magnitude or "Norm" as some say of, our normal.
	// The magnitude has to do with the length of the vector.  We use this
	// information to normalize a vector, which gives it a length of 1.
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)   Where V is the vector

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

CVector3 Normal(CVector3 vTriangle[])					// You might want to error check to make sure it's valid
{														// Get 2 vectors from the polygon
	CVector3 vVector1 = Vector(vTriangle[2], vTriangle[0]);
	CVector3 vVector2 = Vector(vTriangle[1], vTriangle[0]);

	// We Chose to get the vectors surrounding the first point of the polygon.
	// We could have chosen to get 2 other sides of the triangle, but we chose these 2.
	// Now that we have the 2 side vectors, we will take their cross product.
	// (*NOTE*) It is important that pass in the vector of the bottom side of the triangle
	// first, and then pass in the vector of the left side second.  If we switch them,
	// it will turn the normal the opposite way.  Try it, switch them like this: Cross(vVector2, vVector1);
	// Like I said before, it's important to ALWAYS work in the same direction.  In our case,
	// we chose that we always work counter-clockwise.

	CVector3 vNormal = Cross(vVector1, vVector2);

	// Now that we have the direction of the normal, we want to do one last thing.
	// Right now, it's an unknown length, it is probably pretty long in length.
	// We want to do something which gives the normal a length of 1.  This is called
	// normalizing.  To do this we divide the normal by it's magnitude.  Well how do we
	// find it's magnitude? We use this equation: magnitude = sqrt(x^2 + y^2 + z^2)

	vNormal = Normalize(vNormal);						// Use our function we created to normalize the normal (Makes it a length of one)

	// Now return the normalized normal
	// (*NOTE*) If you want to understand what normalizing our normal does, comment out
	// the above line.  Then you can see how long the normal is before we normalize it.
	// I strongly recommend this.  And remember, it doesn't matter how long the normal is,
	// (of course besides (0, 0, 0)), if we normalize it, it will always be of length 1 afterwards.

	return vNormal;										// Return our normal at our desired length
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// That how you find the normal to a polygon.  Pretty complicated huh? :)
// well, the good thing about this is that once you code, you never have to do it
// again.  There is no need, once you already have the functions.  Eventually you
// will build up a big library of math.  As we start getting into collision you will
// see what I mean.  That is why we separated the math into a separate file, because
// you will use the same code over and over in your applications.  I know I do.
// I feel naked with out my gamemath.cpp :)
// 
// Basically, let me explain the steps again to finding the normal of a polygon.
// 
// 1)  First, if you have 3 points of a polygon, you can find a normal to that plane.
//     The first step you need to do is get 2 vectors from that polygon.  That means
//     that you need to pick to sides of the triangle (order is important) and subtract
//     the points from each other to get the vector of that side.  When it comes to triangles,
//     I usually get the first vector from Point3 - Point1. (V1 = P3 - P1)
//     Next, I choose Vector 2 from Point2 and Point1 (V2 = P2 - P1).  
//
// 2)  Now that we have 2 vectors:  V1 and V2, we can take the cross product of those 
//     2 vectors, with vector1 cross vector2.  Remember, the order is important.  
//     I always go counter-clockwise.  Pick and stick!  This is important because it decides the
//     direction of the normal.  It doesn't matter which direction, as long as you do the same
//     thing for all the rest of your polygons.  You don't want mixed up normal directions.
// 
// 3)  Now we should have the normal (Or direction) from the cross product of our polygon.
//     This normal has a strange length, so let's make the length (or distance from the origin) 1.
//     To do this, we take the magnitude (Deals with the length of the vector) of the normal.
//     With the magnitude, we divide the normal BY that magnitude.  That means the X, Y and Z values
//     get divided by the magnitude scalar.  A scalar is a single number like 1, 1.232, etc...
// 
// That's it!  Three basic steps to finding the normal.  It is nice not to have to worry about
// all that once you have a function that does it all for you.  We call ours "Normal()".  It
// won't matter it we have a polygon of 20 vertices, all we need is just 3 points (the first 3)
// from that polygon.  That is enough to define a plane.  Then we find the normal to that plane.
//
// 
// I hope this help!  Let us know at www.GameTutorials.com if this helped you at all.
//
// Good luck, and get ready for the collision tutorial next!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
