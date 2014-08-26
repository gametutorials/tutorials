#include "aabb.h"

const int kNumCorners = 8;
const int kMaxIndices = 36;

// Overloaded constructor
CAxisAlgnBB::CAxisAlgnBB(const CPos &min, const CPos &max)
{
	set(min, max);
}

void CAxisAlgnBB::render(int argb)
{
	SVertex verts[kNumCorners];
	WORD indexList[kMaxIndices] = {	0, 2, 3,
									3, 1, 0,
									7, 6, 4,
									4, 5, 7,
									4, 6, 2, 
									2, 0, 4,
									1, 3, 7,
									7, 5, 1,
									4, 0, 1,
									1, 5, 4,
									2, 6, 7,
									7, 3, 2 };

	// Fill in the 8 vertices
	for(int i = 0; i < kNumCorners; ++i)
	{
		CPos pos = getCorner(i);
		
		verts[i].x = pos.x;
		verts[i].y = pos.y;
		verts[i].z = pos.z;
		verts[i].color = argb;
	}

	// Render the AABB
	g3D->render(verts, kNumCorners, indexList, kMaxIndices);

	// Render lines on the top and bottom of the the front and 
	// back sides of the box.  This allow us to see the full dimensions
	// of the AABB better.
	for(int i = 0; i < kNumCorners; ++i)
		verts[i].color = D3DCOLOR_ARGB(255, 255, 255, 255);

	g3D->renderLine(verts, kNumCorners);
}

/*	So lets quickly digest how this algorithm works.  First we assume that the
	origin of the ray is inside of the AABB.  If this is true,  then we know that 
	the ray MUST collide with AABB because it is contained inside of the AABB.

	If the origin is not inside of the AABB, then we check to see if the 
	direction the ray is heading is away from the AABB.  If it is, then it
	must NOT collide.

	Lastly, if the origin is outside of the AABB, and the ray is heading towards
	the AABB,  we use the length of the ray and test to see if there is an
	intersection.
*/
bool CAxisAlgnBB::intersect(const CRay &ray)
{
	// Compute the ray delta
	CVector rayDelta = ray.getDir() * ray.getLength();

	// First we check to see if the origin of the ray is 
	// inside the AABB.  If it is, the ray intersects the AABB so 
	// we'll return true.  We start by assuming the ray origin is 
	// inside the AABB
	bool inside = true;

	// This stores the distance from either the min or max (x,y,z) to the ray's
	// origin (x,y,z) respectively, divided by the length of the ray.  The largest
	// value has the delta time of a possible intersection.
	float xt, yt, zt;
	
	// Test the X component of the ray's origin to see if we are inside or not
	if(ray.getOrigin().x < mMin.x)
	{
		xt = mMin.x - ray.getOrigin().x;
		
		if(xt > rayDelta.x) // If the ray is moving away from the AABB, there is no intersection 
			return false;
		
		xt /= rayDelta.x; 
		inside = false;
	} 
	else if(ray.getOrigin().x > mMax.x)
	{
		xt = mMax.x - ray.getOrigin().x;
		
		if(xt < rayDelta.x) // If the ray is moving away from the AABB, there is no intersection 
			return false;
		
		xt /= rayDelta.x;
		inside = false;
	} 
	else
	{
		// Later on we use the "xt", "yt", and "zt" variables to determine which plane (either
		// xy, xz, or yz) we may collide with.  Since the x component of the ray
		// origin is in between, the AABB's left and right side (which reside in the yz plane), 
		// we know we don't have to test those sides so we set this to a negative value.
		xt = -1.0f; 
	}

	// Test the X component of the ray's origin to see if we are inside or not
	if(ray.getOrigin().y < mMin.y)
	{
		yt = mMin.y - ray.getOrigin().y;
		
		if(yt > rayDelta.y) // If the ray is moving away from the AABB, there is no intersection 
			return false;

		yt /= rayDelta.y;
		inside = false;
	} 
	else if(ray.getOrigin().y > mMax.y)
	{
		yt = mMax.y - ray.getOrigin().y;
		
		if(yt < rayDelta.y) // If the ray is moving away from the AABB, there is no intersection 
			return false;
		
		yt /= rayDelta.y;
		inside = false;
	} 
	else
	{
		// Later on we use the "xt", "yt", and "zt" variables to determine which plane (either
		// xy, xz, or yz) we may collide with.  Since the y component of the ray
		// origin is in between, the AABB's top and bottom side (which reside in the xz plane), 
		// we know we don't have to test those sides so we set this to a negative value.
		yt = -1.0f;
	}

	if(ray.getOrigin().z < mMin.z)
	{
		zt = mMin.z - ray.getOrigin().z;
		
		if(zt > rayDelta.z) // If the ray is moving away from the AABB, there is no intersection 
			return false;
		
		zt /= rayDelta.z;
		inside = false;
	} 
	else if(ray.getOrigin().z > mMax.z)
	{
		zt = mMax.z - ray.getOrigin().z;
		
		if(zt < rayDelta.z) // If the ray is moving away from the AABB, there is no intersection 
			return false;
		
		zt /= rayDelta.z;
		inside = false;
	} 
	else
	{
		// Later on we use the "xt", "yt", and "zt" variables to determine which plane (either
		// xy, xz, or yz) we may collide with.  Since the z component of the ray
		// origin is in between, the AABB's front and back side (which reside in the xy plane), 
		// we know we don't have to test those sides so we set this to a negative value.
		zt = -1.0f;
	}

	// If the origin inside the AABB
	if(inside)
		return true; // The ray intersects the AABB
	
	// Otherwise we have some checking to do...

	// We want to test the AABB planes with largest value out of xt, yt, and zt.  So
	// first we determine which value is the largest.

	float t = xt;
	
	if(yt > t)
		t = yt;
	
	if(zt > t)
		t = zt;

	// **NOTE** Normally comparing two floating point numbers won't necessarily work, however,
	//			since we set to explicitly to equal either xt, yt, or zt above, the equality test
	//			will pass

	if(t == xt) // If the ray intersects with the AABB's YZ plane
	{
		// Compute intersection values
		float y = ray.getOrigin().y + rayDelta.y * t;
		float z = ray.getOrigin().z + rayDelta.z * t;
	
		// Test to see if collision takes place within the bounds of the AABB
		if(y < mMin.y || y > mMax.y)
			return false;
		else if(z < mMin.z || z > mMax.z)
			return false;
	}
	else if(t == yt) // Intersects with the XZ plane
	{
		// Compute intersection values
		float x = ray.getOrigin().x + rayDelta.x * t;
		float z = ray.getOrigin().z + rayDelta.z * t;

		// Test to see if collision takes place within the bounds of the AABB
		if(x < mMin.x || x > mMax.x)
			return false;
		else if(z < mMin.z || z > mMax.z) 
			return false;
	}
	else // Intersects with the XY plane
	{
		assert(t == zt);

		// Compute intersection values
		float x = ray.getOrigin().x + rayDelta.x * t;
		float y = ray.getOrigin().y + rayDelta.y * t;

		// Test to see if collision takes place within the bounds of the AABB
		if(x < mMin.x || x > mMax.x)
			return false;
		else if(y < mMin.y || y > mMax.y)
			return false;
	}

	// The ray intersects the AABB
	return true;
}

void CAxisAlgnBB::set(const CPos &min, const CPos &max)
{
	assert(min.x < max.x);
	assert(min.y < max.y);
	assert(min.z < max.z);

	mMin = min;
	mMax = max;
}

CPos CAxisAlgnBB::getCorner(int which)
{
	// Make sure "which" is valid
	assert((which >= 0) && (which < kNumCorners));

	// Return the corner
	return CPos((which & 1) ? mMax.x : mMin.x,
				(which & 2) ? mMax.y : mMin.y,
				(which & 4) ? mMax.z : mMin.z);
}