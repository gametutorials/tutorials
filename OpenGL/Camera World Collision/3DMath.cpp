//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		CameraWorldCollision							 //
//																		 //
//		$Description:	Shows how to check if camera and world collide	 //
//																		 //
//***********************************************************************//

#include "main.h"
#include <math.h>	// We include math.h so we can use the sqrt() function
#include <float.h>	// This is so we can use _isnan() for acos()

/////////////////////////////////////// ABSOLUTE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the absolute value of the number passed in
/////
/////////////////////////////////////// ABSOLUTE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Absolute(float num)
{
	// If num is less than zero, we want to return the absolute value of num.
	// This is simple, either we times num by -1 or subtract it from 0.
	if(num < 0)
		return (0 - num);

	// Return the original number because it was already positive
	return num;
}


/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;									// The vector to hold the cross product

	// The X value for the vector is:  (V1.y * V2.z) - (V1.z * V2.y)													// Get the X value
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
														
	// The Y value for the vector is:  (V1.z * V2.x) - (V1.x * V2.z)
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
														
	// The Z value for the vector is:  (V1.x * V2.y) - (V1.y * V2.x)
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;										// Return the cross product (Direction the polygon is facing - Normal)
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


/////////////////////////////////////// NORMAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the normal of a polygon (The direction the polygon is facing)
/////
/////////////////////////////////////// NORMAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normal(CVector3 vPolygon[])					
{														// Get 2 vectors from the polygon (2 sides), Remember the order!
	CVector3 vVector1 = vPolygon[2] - vPolygon[0];
	CVector3 vVector2 = vPolygon[1] - vPolygon[0];

	CVector3 vNormal = Cross(vVector1, vVector2);		// Take the cross product of our 2 vectors to get a perpendicular vector

	// Now we have a normal, but it's at a strange length, so let's make it length 1.

	vNormal = Normalize(vNormal);						// Use our function we created to normalize the normal (Makes it a length of one)

	return vNormal;										// Return our normal at our desired length
}


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


////////////////////////////// CLOSEST POINT ON LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the point on the line vA_vB that is closest to the point vPoint
/////
////////////////////////////// CLOSEST POINT ON LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint)
{
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


/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the distance between a plane and the origin
/////
/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
									
float PlaneDistance(CVector3 Normal, CVector3 Point)
{	
	float distance = 0;									// This variable holds the distance from the plane tot he origin

	// Use the plane equation to find the distance (Ax + By + Cz + D = 0)  We want to find D.
	// So, we come up with D = -(Ax + By + Cz)
														// Basically, the negated dot product of the normal of the plane and the point. (More about the dot product in another tutorial)
	distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

	return distance;									// Return the distance
}


/////////////////////////////////// INTERSECTED PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks to see if a line intersects a plane
/////
/////////////////////////////////// INTERSECTED PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
											
bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance)
{
	float distance1=0, distance2=0;						// The distances from the 2 points of the line from the plane
			
	vNormal = Normal(vPoly);							// We need to get the normal of our plane to go any further

	// Let's find the distance our plane is from the origin.  We can find this value
	// from the normal to the plane (polygon) and any point that lies on that plane (Any vertex)
	originDistance = PlaneDistance(vNormal, vPoly[0]);

	// Get the distance from point1 from the plane using: Ax + By + Cz + D = (The distance from the plane)

	distance1 = ((vNormal.x * vLine[0].x)  +					// Ax +
		         (vNormal.y * vLine[0].y)  +					// Bx +
				 (vNormal.z * vLine[0].z)) + originDistance;	// Cz + D
	
	// Get the distance from point2 from the plane using Ax + By + Cz + D = (The distance from the plane)
	
	distance2 = ((vNormal.x * vLine[1].x)  +					// Ax +
		         (vNormal.y * vLine[1].y)  +					// Bx +
				 (vNormal.z * vLine[1].z)) + originDistance;	// Cz + D

	// Now that we have 2 distances from the plane, if we times them together we either
	// get a positive or negative number.  If it's a negative number, that means we collided!
	// This is because the 2 points must be on either side of the plane (IE. -1 * 1 = -1).

	if(distance1 * distance2 >= 0)			// Check to see if both point's distances are both negative or both positive
	   return false;						// Return false if each point has the same sign.  -1 and 1 would mean each point is on either side of the plane.  -1 -2 or 3 4 wouldn't...
					
	return true;							// The line intersected the plane, Return TRUE
}


/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This computers the dot product of 2 vectors
/////
/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Dot(CVector3 vVector1, CVector3 vVector2) 
{
	// The dot product is this equation: V1.V2 = (V1.x * V2.x  +  V1.y * V2.y  +  V1.z * V2.z)
	// In math terms, it looks like this:  V1.V2 = ||V1|| ||V2|| cos(theta)
	
			 //    (V1.x * V2.x        +        V1.y * V2.y        +        V1.z * V2.z)
	return ( (vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z) );
}


/////////////////////////////////// ANGLE BETWEEN VECTORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks to see if a point is inside the ranges of a polygon
/////
/////////////////////////////////// ANGLE BETWEEN VECTORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

double AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2)
{							
	// Get the dot product of the vectors
	float dotProduct = Dot(Vector1, Vector2);				

	// Get the product of both of the vectors magnitudes
	float vectorsMagnitude = Magnitude(Vector1) * Magnitude(Vector2) ;

	// Get the angle in radians between the 2 vectors
	double angle = acos( dotProduct / vectorsMagnitude );

	// Here we make sure that the angle is not a -1.#IND0000000 number, which means indefinate
	if(_isnan(angle))
		return 0;
	
	// Return the angle in radians
	return( angle );
}


/////////////////////////////////// INTERSECTION POINT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the intersection point of the line that intersects the plane
/////
/////////////////////////////////// INTERSECTION POINT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
											
CVector3 IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance)
{
	CVector3 vPoint, vLineDir;					// Variables to hold the point and the line's direction
	double Numerator = 0.0, Denominator = 0.0, dist = 0.0;

	// 1)  First we need to get the vector of our line, Then normalize it so it's a length of 1
	vLineDir = vLine[1] - vLine[0];		// Get the Vector of the line
	vLineDir = Normalize(vLineDir);				// Normalize the lines vector


	// 2) Use the plane equation (distance = Ax + By + Cz + D) to find the 
	// distance from one of our points to the plane.
	Numerator = - (vNormal.x * vLine[0].x +		// Use the plane equation with the normal and the line
				   vNormal.y * vLine[0].y +
				   vNormal.z * vLine[0].z + distance);

	// 3) If we take the dot product between our line vector and the normal of the polygon,
	Denominator = Dot(vNormal, vLineDir);		// Get the dot product of the line's vector and the normal of the plane
				  
	// Since we are using division, we need to make sure we don't get a divide by zero error
	// If we do get a 0, that means that there are INFINATE points because the the line is
	// on the plane (the normal is perpendicular to the line - (Normal.Vector = 0)).  
	// In this case, we should just return any point on the line.

	if( Denominator == 0.0)						// Check so we don't divide by zero
		return vLine[0];						// Return an arbitrary point on the line

	dist = Numerator / Denominator;				// Divide to get the multiplying (percentage) factor
	
	// Now, like we said above, we times the dist by the vector, then add our arbitrary point.
	vPoint.x = (float)(vLine[0].x + (vLineDir.x * dist));
	vPoint.y = (float)(vLine[0].y + (vLineDir.y * dist));
	vPoint.z = (float)(vLine[0].z + (vLineDir.z * dist));

	return vPoint;								// Return the intersection point
}


/////////////////////////////////// INSIDE POLYGON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks to see if a point is inside the ranges of a polygon
/////
/////////////////////////////////// INSIDE POLYGON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount)
{
	const double MATCH_FACTOR = 0.99;		// Used to cover up the error in floating point
	double Angle = 0.0;						// Initialize the angle
	CVector3 vA, vB;						// Create temp vectors
	
	for (int i = 0; i < verticeCount; i++)		// Go in a circle to each vertex and get the angle between
	{	
		vA = Poly[i] - vIntersection;			// Subtract the intersection point from the current vertex
												// Subtract the point from the next vertex
		vB = Poly[(i + 1) % verticeCount] - vIntersection;
												
		Angle += AngleBetweenVectors(vA, vB);	// Find the angle between the 2 vectors and add them all up as we go along
	}
											
	if(Angle >= (MATCH_FACTOR * (2.0 * PI)) )	// If the angle is greater than 2 PI, (360 degrees)
		return true;							// The point is inside of the polygon
		
	return false;								// If you get here, it obviously wasn't inside the polygon, so Return FALSE
}


/////////////////////////////////// INTERSECTED POLYGON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks if a line is intersecting a polygon
/////
/////////////////////////////////// INTERSECTED POLYGON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount)
{
	CVector3 vNormal;
	float originDistance = 0;

	// First, make sure our line intersects the plane
									 // Reference   // Reference
	if(!IntersectedPlane(vPoly, vLine,   vNormal,   originDistance))
		return false;

	// Now that we have our normal and distance passed back from IntersectedPlane(), 
	// we can use it to calculate the intersection point.  
	CVector3 vIntersection = IntersectionPoint(vNormal, vLine, originDistance);

	// Now that we have the intersection point, we need to test if it's inside the polygon.
	if(InsidePolygon(vIntersection, vPoly, verticeCount))
		return true;							// We collided!	  Return success

	return false;								// There was no collision, so return false
}


///////////////////////////////// CLASSIFY SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tells if a sphere is BEHIND, in FRONT, or INTERSECTS a plane, also it's distance
/////
///////////////////////////////// CLASSIFY SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int ClassifySphere(CVector3 &vCenter, 
				   CVector3 &vNormal, CVector3 &vPoint, float radius, float &distance)
{
	// First we need to find the distance our polygon plane is from the origin.
	float d = (float)PlaneDistance(vNormal, vPoint);

	// Here we use the famous distance formula to find the distance the center point
	// of the sphere is from the polygon's plane.  
	distance = (vNormal.x * vCenter.x + vNormal.y * vCenter.y + vNormal.z * vCenter.z + d);

	// If the absolute value of the distance we just found is less than the radius, 
	// the sphere intersected the plane.
	if(Absolute(distance) < radius)
		return INTERSECTS;
	// Else, if the distance is greater than or equal to the radius, the sphere is
	// completely in FRONT of the plane.
	else if(distance >= radius)
		return FRONT;
	
	// If the sphere isn't intersecting or in FRONT of the plane, it must be BEHIND
	return BEHIND;
}


///////////////////////////////// EDGE SPHERE COLLSIION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns true if the sphere is intersecting any of the edges of the polygon
/////
///////////////////////////////// EDGE SPHERE COLLSIION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool EdgeSphereCollision(CVector3 &vCenter, 
						 CVector3 vPolygon[], int vertexCount, float radius)
{
	CVector3 vPoint;

	// This function takes in the sphere's center, the polygon's vertices, the vertex count
	// and the radius of the sphere.  We will return true from this function if the sphere
	// is intersecting any of the edges of the polygon.  

	// Go through all of the vertices in the polygon
	for(int i = 0; i < vertexCount; i++)
	{
		// This returns the closest point on the current edge to the center of the sphere.
		vPoint = ClosestPointOnLine(vPolygon[i], vPolygon[(i + 1) % vertexCount], vCenter);
		
		// Now, we want to calculate the distance between the closest point and the center
		float distance = Distance(vPoint, vCenter);

		// If the distance is less than the radius, there must be a collision so return true
		if(distance < radius)
			return true;
	}

	// The was no intersection of the sphere and the edges of the polygon
	return false;
}


////////////////////////////// SPHERE POLYGON COLLISION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns true if our sphere collides with the polygon passed in
/////
////////////////////////////// SPHERE POLYGON COLLISION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool SpherePolygonCollision(CVector3 vPolygon[], 
							CVector3 &vCenter, int vertexCount, float radius)
{
	// 1) STEP ONE - Finding the sphere's classification
	
	// Let's use our Normal() function to return us the normal to this polygon
	CVector3 vNormal = Normal(vPolygon);

	// This will store the distance our sphere is from the plane
	float distance = 0.0f;

	// This is where we determine if the sphere is in FRONT, BEHIND, or INTERSECTS the plane
	int classification = ClassifySphere(vCenter, vNormal, vPolygon[0], radius, distance);

	// If the sphere intersects the polygon's plane, then we need to check further
	if(classification == INTERSECTS) 
	{
		// 2) STEP TWO - Finding the psuedo intersection point on the plane

		// Now we want to project the sphere's center onto the polygon's plane
		CVector3 vOffset = vNormal * distance;

		// Once we have the offset to the plane, we just subtract it from the center
		// of the sphere.  "vPosition" now a point that lies on the plane of the polygon.
		CVector3 vPosition = vCenter - vOffset;

		// 3) STEP THREE - Check if the intersection point is inside the polygons perimeter

		// If the intersection point is inside the perimeter of the polygon, it returns true.
		// We pass in the intersection point, the list of vertices and vertex count of the poly.
		if(InsidePolygon(vPosition, vPolygon, 3))
			return true;	// We collided!
		else
		{
			// 4) STEP FOUR - Check the sphere intersects any of the polygon's edges

			// If we get here, we didn't find an intersection point in the perimeter.
			// We now need to check collision against the edges of the polygon.
			if(EdgeSphereCollision(vCenter, vPolygon, vertexCount, radius))
			{
				return true;	// We collided!
			}
		}
	}

	// If we get here, there is obviously no collision
	return false;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// GET COLLISION OFFSET \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the offset to move the center of the sphere off the collided polygon
/////
///////////////////////////////// GET COLLISION OFFSET \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 GetCollisionOffset(CVector3 &vNormal, float radius, float distance)
{
	CVector3 vOffset = CVector3(0, 0, 0);

	// Once we find if a collision has taken place, we need make sure the sphere
	// doesn't move into the wall.  In our app, the position will actually move into
	// the wall, but we check our collision detection before we render the scene, which
	// eliminates the bounce back effect it would cause.  The question is, how do we
	// know which direction to move the sphere back?  In our collision detection, we
	// account for collisions on both sides of the polygon.  Usually, you just need
	// to worry about the side with the normal vector and positive distance.  If 
	// you don't want to back face cull and have 2 sided planes, I check for both sides.
	//
	// Let me explain the math that is going on here.  First, we have the normal to
	// the plane, the radius of the sphere, as well as the distance the center of the
	// sphere is from the plane.  In the case of the sphere colliding in the front of
	// the polygon, we can just subtract the distance from the radius, then multiply
	// that new distance by the normal of the plane.  This projects that leftover
	// distance along the normal vector.  For instance, say we have these values:
	//
	//	vNormal = (1, 0, 0)		radius = 5		distance = 3
	//
	// If we subtract the distance from the radius we get: (5 - 3 = 2)
	// The number 2 tells us that our sphere is over the plane by a distance of 2.
	// So basically, we need to move the sphere back 2 units.  How do we know which
	// direction though?  This part is easy, we have a normal vector that tells us the
	// direction of the plane.  
	// If we multiply the normal by the left over distance we get:  (2, 0, 0)
	// This new offset vectors tells us which direction and how much to move back.
	// We then subtract this offset from the sphere's position, giving is the new
	// position that is lying right on top of the plane.  Ba da bing!
	// If we are colliding from behind the polygon (not usual), we do the opposite
	// signs as seen below:
	
	// If our distance is greater than zero, we are in front of the polygon
	if(distance > 0)
	{
		// Find the distance that our sphere is overlapping the plane, then
		// find the direction vector to move our sphere.
		float distanceOver = radius - distance;
		vOffset = vNormal * distanceOver;
	}
	else // Else colliding from behind the polygon
	{
		// Find the distance that our sphere is overlapping the plane, then
		// find the direction vector to move our sphere.
		float distanceOver = radius + distance;
		vOffset = vNormal * -distanceOver;
	}

	// There is one problem with check for collisions behind the polygon, and that
	// is if you are moving really fast and your center goes past the front of the
	// polygon, it will then assume you were colliding from behind and not let
	// you back in.  Most likely you will take out the if / else check, but I
	// figured I would show both ways in case someone didn't want to back face cull.

	// Return the offset we need to move back to not be intersecting the polygon.
	return vOffset;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing really new added to this file since the last collision tutorial.  We did
// however tweak the EdgePlaneCollision() function to handle the camera collision
// better around edges. 
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
