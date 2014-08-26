#ifndef PLANE_H
#define PLANE_H

#include "ray.h"

/* The plane, the plane!

	A plane is a two-dimensional surface spanned by two linearly independent vectors.
	You can think of it as a piece of notebook paper with each edge extending forever.
	Algerbraically, a plane can be defined as:  
	
		aX + bY + cZ + d = 0
			
	With:
		
		(a,b,c) == The unit length normal vector to the plane
		(X,Y,Z) == Any point on the plane
		d == The shortest signed distance from the origin of the coordinate system to the plane
		
	So with a little algebra we can come up with this definition as well:
	
		p * n = -d
		
	To get this definition, first we move d to the other side of the equal sign
	
		aX + bY + cZ = -d
		
	Then we rewrite (a,b,c) as 'n' and we rewrite (X,Y,Z) as 'p'.  The dot product between
	two vectors is defined as (A * B) == (A.x * B.x) + (A.y + B.y) + (A.z + B.z), so we can
	rewrite the equation as:
	
		p * n = -d  
		
	With:
	
		p == A point on the plane (ie (X,Y,Z))
		n == The unit length normal vector to the plane (ie (a,b,c))
		d == The shortest signed distance from the origin of the coordinate system to the plane
		* == The dot product operation
		
	An ASCII illustration looks something like this:
	
								|	
								|
		(backside of plane)		|--> Normal to plane	(front side of plane)	
								|
								|
							   Plane
*/

// This is the length of each side of one triangle that is used to represent the plane
// Two triangles are used to render the plane, with the size of the resulting quad
// being (kPlaneSize by kPlaneSize).   
const float kPlaneSize = 2.0f;   

class CPlane
{
	public:
	
		// Constructors
		CPlane() { /* do nothing */ }
		CPlane(const CVector &normal, const CPos &ptOnPlane);
		
		// Render's the plane with the passed in color
		void render(int argb);
		
		// Returns true if the ray and plane intersect, false 
		// otherwise
		bool intersect(const CRay &ray);
		
		// Set the plane	
		void set(const CVector &normal, const CPos &ptOnPlane);
		
		// Data Access ***
		
			const CVector& getNormal() { return mNormal; }
			float getDistance() { return mDist; }
		
		// *** End Data Access	
		
	private:
	
		CVector mNormal; // Unit normal to the plane
		CPos mPoint; // A point on the plane
		float mDist; // The signed distance from the origin to the plane 	
};

#endif