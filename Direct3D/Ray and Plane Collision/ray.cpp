#include "ray.h"
#include "d3d_obj.h"

// Overloaded constructor
CRay::CRay(const CPos &origin, const CVector &dir, float len)
{
	setOrigin(origin);
	setDir(dir);
	setLength(len);
}

// Set a ray based on two points
void CRay::set(const CPos &ptA, const CPos &ptB)
{
	// Create the vector from point A to B 
	CVector vec = ptB - ptA; 
	
	setOrigin(ptA); // Set the origin
	setDir(vec); // Set the direction
	setLength(vec.magnitude()); // Set the length	
}

// Render the ray
void CRay::render(int argb)
{
	const int kVertCount = 2; // Number of vertices needed to draw the ray

	// Declare two verts to draw the ray with
	SVertex verts[kVertCount] = {0}; 
	
	// Set origin
	verts[0].color = argb;
	verts[0].x = mOrigin.x;
	verts[0].y = mOrigin.y;
	verts[0].z = mOrigin.z;
	
	// Set end point
	verts[1].color = argb;
	verts[1].x = mOrigin.x + mDir.x * mLength;
	verts[1].y = mOrigin.y + mDir.y * mLength;
	verts[1].z = mOrigin.z + mDir.z * mLength;
	
	// Render the ray
	g3D->renderLine(verts, kVertCount);
}