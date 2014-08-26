#include "plane.h"
#include "d3d_obj.h"

// Overloaded constructor
CPlane::CPlane(const CVector &normal, const CPos &ptOnPlane)
{
	// Set the plane
	set(normal, ptOnPlane);	
}

// Render the plane
void CPlane::render(int argb)
{
	/*
		Okay here's how this algorithm works.  First we use the point on the plane as
		the center point of the quad we going to draw to represent the plane.  Then we'll use
		the normal of the plane to derive two vectors both perpendicular to the normal of the plane
		and perpendicular to each other.  Then we generate four points "kPlaneSize" distance away
		from the point of the plane.  Here's a quick ASCII representation:
		
		_________________________
		|						|
		|		   vec 1		|
		|			|			|
		|			|			|		The four points we use are the tips of the four vectors.
		| -vec 2 ---*P--- vec 2 |
		|			|			|
		|			|			|
		|		  -vec 1		|
		|						|
		-------------------------
	*/

	const int kVertCount = 4; // Number of vertices to use
	const int kIndexCount = 6; // Number of indices to use
	const WORD kIndices[kIndexCount] =  { 0, 1, 2, 0, 2, 3, }; // The indices
	
	CVector v, vec1, vec2;
	
	// Calculate our first perpendicular vector
	// We make sure the normal is not pointing the same way as the world +Y axis so we
	// don't cross the normal with itself 
	if(mNormal == kWorldYAxis || mNormal == -kWorldYAxis)
		vec1 = kWorldZAxis.cross(mNormal);
	else
		vec1 = kWorldYAxis.cross(mNormal);
		
	vec1.normalize();
	
	// Calculate the second perpendicular vector
	vec2 = mNormal.cross(vec1);
	vec2.normalize();
	
	SVertex triVerts[kVertCount] = {0};
			
	// Loop through and generate the four vertices
	// based on kPlaneSize and the 2 perpendicular vectors
	for(int i = 0; i < kVertCount; ++i)
	{
		if(i % 2 == 0)
		{
			v = mPoint + (vec1 * kPlaneSize);
			vec2 = -vec2;
		}
		else
		{
			v = mPoint + (vec2 * kPlaneSize);
			vec1 = -vec1;
		}
	
		// Set the vert
		triVerts[i].color = argb;
		triVerts[i].x = v.x;
		triVerts[i].y = v.y;
		triVerts[i].z = v.z;
	}
	
	// Render the quad representing the plane
	g3D->render(triVerts, kVertCount, kIndices, kIndexCount);
}

/*
	It all boils down to this.  Here is the portion of code that determines collision between
	a plane and a ray.  Lets rexamin the equations of plane and ray:
	
	Plane:
		
		p * n = -d		Where p == point on plane, n == unit normal to plane, 
						d == signed distance to plane
		
	Ray:
	
		O + tD			Where O == origin of ray, t == "time" traveled along ray, 
						D == Direction vector of the ray
						
	If we replace 'p' with the equation of ray we get:
	
		(O + tD) * n = -d
		
	And with a little algerbra for a final answer we get:
	
		t = -((n * O) + d) / (n * D)
		  
	If we treat our ray like a parametric line, we know that when 't' is between 0.0f and 1.0f, then
	there is a collision with the ray, otherwise, the ray does not collide
*/  		 
bool CPlane::intersect(const CRay &ray)
{
	// Calculate the denominator
	// **NOTE** Our ray's direction vector is always normalized, so we must
	//			multiply by the ray's length so we correctly check collision against
	//			the desired portion of the ray			
	float denominator = mNormal * ray.getDir() * ray.getLength();
	
	// If the denominator is greater than or equal to 0.0f, the ray intersects on the backside of 
	// the plane or is collinear to the plane
	// **NOTE** The "backside" of a plane is defined to be the side of the plane the 
	//			plane's normal points away from
	if(denominator >= 0.0f)
		return false; 
	
	// Calculate the numerator	
	float numerator = -((mNormal * ray.getOrigin()) + mDist);
	
	// Calculate 't'
	float t = numerator / denominator;
	
	// This return statement reads like:
	// 
	// if(t < 0.0f || t > 1.0f)
	//	return false;
	// else
	//	return true;
	//
	// We use this shorter notation because it gets rid of the if statement.  Branching
	// is usually a speed hit on x86 architecture which Windows 2000 and XP run on. 	
	return (t >= 0.0f && t <= 1.0f);
}

// Create the plane by passing a normal to it and a point on it
void CPlane::set(const CVector &normal, const CPos &ptOnPlane)
{
	// Set the normal
	mNormal = normal;
	mNormal.normalize(); // Make sure it's unit length
	
	mPoint = ptOnPlane; // Set the point on the plane
	mDist = -(mPoint * mNormal); // Calculate the plane's distance from the origin
}
