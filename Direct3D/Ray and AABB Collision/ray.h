#ifndef RAY_H
#define RAY_H

#include <assert.h>
#include "vector.h"

/*
	A ray is defined to be an origin and a direction vector (which extends forever) from
	the origin.   Another way to think of a ray is as a vector from a point A to a point B.
	Typically, a ray's length is thought of to be of half-infinity.  In computer graphics a ray
	has a finite length	making it really a glorified vector.  When an "infinite" ray is desired
	usually a really large number is used for the length which for practical purposes makes it
	seem like the ray is infinitely long.
	
	Algebraically we deifine a ray like this:
	
		O + tD
		
	With:
	
		O == The origin of the ray
		t == A scalar which represents the "time" traveled along the ray.  Plugging in different
			 't' values will get you different points on the ray.
		D == The direction vector of the ray
		
	You can easily calculate a ray with two points.  Given point A and point B,
	algerbracially it looks like:
	
		O = A
		D = Norm(B - A)		**NOTE** Norm() means take the normal of
		
	Which gives us back the orgianl equation:
	
		O + tD 
	
	Where 't' will determine how far we have traveled along the ray.  A value of 0.0f for 't' 
	puts us at the origin.  A value of 1.0f for 't' puts us as 'D' units away from the origin.
			
	The ASCII illustration of a ray would look something like this:
	
	O == Origin
	--> == Direction Vector
	
	O ------------------------------------------> (and the ray keeps on going...)
*/

// This is the default length to use as
const float kDefaultRayLen = 2.0f;

class CRay
{
	public:	
	
		// Constructors
		CRay() { /* do nothing */ }	
		CRay(const CPos &origin, const CVector &dir, float len = kDefaultRayLen);
		
		// Render the ray
		void render(int argb);
		
		// Creates a ray going from point A to point B
		void set(const CPos &ptA, const CPos &ptB);
		
		// Set functions
		void setOrigin(const CPos &origin) { mOrigin = origin; }
		void setDir(const CVector &dir) { mDir = dir; mDir.normalize(); }
		void setLength(float len) { assert(len > 0.0f); mLength = len; }
		
		// Data Access ***
		
			const CPos& getOrigin() const { return mOrigin; }
			const CVector& getDir() const { return mDir; }
			float getLength() const { return mLength; }
			
		// *** End Data Access
			
	private:
	
		CPos mOrigin; // Holds the ray's origin
		CVector mDir; // Holds the ray's direction, it will always be normalized
		
		float mLength; // Holds the length of the ray
};

#endif