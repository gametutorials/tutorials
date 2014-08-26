#include "camera.h"

// Default Constructor
CCamera::CCamera()
{
	reset();
}

// Sets right, up and forward to world X, Y, Z axis
// Sets eye to five units back on -Z
void CCamera::reset()
{
	right.set(1.0f, 0.0f, 0.0f); // +X axis
	up.set(0.0f, 1.0f, 0.0f); // +Y axis
	forward.set(0.0f, 0.0f, 1.0f); // +Z axis
	
	// Set eye back 5 units on -Z
	eye.set(0.0f, 0.0f, -5.0f);
}

// Pitches the camera up/down
void CCamera::pitch(float angle)
{
	/*
		Now when we pitch the camera up/down we need to make sure we keep all
		the camera axes perpendicular to each other, otherwise we loose the
		fact that they represent the camera's coordinate system.
		
		Thus when we rotate the forward vector, we also have to recalculate
		the up vector because it will rotate too.  We could just rotate it
		around the same axis, but it's faster to recompute it using 
		the cross product, so that's what we do.
		
		**NOTE** The right vector doesn't change when you pitch up/down.  Don't believe me?
				 Well draw it out on paper and see for yourself :)	
	*/  

	// Calculate new forward
	forward = rotateAroundAxis(forward, right, angle);
	
	// Calculate new camera up vector
	up = forward.cross(right);
}

// Using your standard Y-axis rotation matrix, we'll rotate around the Y-axis
// Rotates camera around the world Y-axis
// ca == cosf(angle)	sa == sinf(angle)
// [ ca 0 -sa ]
// [ 0  1   0 ]
// [ sa 0  ca ]
void CCamera::rotateY(float angle)
{
	float xxx, zzz;
	float sinAng, cosAng;
	
	// Get sin()/cos() of angle
	sinAng = sinf(angle);
	cosAng = cosf(angle);
	
	// Save off forward components for computation
	xxx = forward.x;
	zzz = forward.z;
	
	// Rotate forward vector
	forward.x = xxx * cosAng + zzz * sinAng;
	forward.z = xxx * -sinAng + zzz * cosAng;
	
	// Save off up components for computation
	xxx = up.x;
	zzz = up.z;
	
	// Rotate up vector
	up.x = xxx * cosAng + zzz * sinAng;
	up.z = xxx * -sinAng + zzz * cosAng;
	
	// Save off right components for computation
	xxx = right.x;
	zzz = right.z;
	
	// Rotate right vector
	right.x = xxx * cosAng + zzz * sinAng;
	right.z = xxx * -sinAng + zzz * cosAng;
}

// Moves the camera in the specified direction
void CCamera::move(ECamMoveDir dir, float amt)
{
	switch(dir)
	{
		case eForward: // Move along the camera's forward vector
			eye += (forward * amt);
				break;
		
		case eBack: // Move along the camera's negative forward vector
			eye -= (forward * amt);
				break;
				
		case eLeft: // Move along the camera's negative right vector
			eye -= (right * amt);
				break;
				
		case eRight: // Move along the camera's right vector
			eye += (right * amt);
				break;
	}
}

// Private Methods ////////////////////////////////////
//////////////////////////////////////////////////////
/////////////////////////////////////////////////////

// Here is where most of the magic happens.  This function takes an
// arbitrary  axis and rotates the view vector (the forward axis) of the 
// camera around this axis by "angle" radians.  So how do we rotate around
// an arbitrary axis?  Well lets first look at a couple of matrices that will
// rotate us around the world X, Y, and Z axis respectively.  We use 't' as 
// an angle theta to rotate around the axis.

//////////////////////////////////////////////////////////////////////////
//		X-Axis						Y-Axis					Z-Axis			
// [1    0	    0   ]		 [cos(t) 0 -sin(t)]		 [ cos(t) sin(t)  0]
// [0  cos(t) sin(t)]		 [  0	 1    0	  ]		 [-sin(t) cos(t)  0]
// [0 -sin(t) cos(t)]		 [sin(t) 0  cos(t)]		 [   0	    0	  1]
//////////////////////////////////////////////////////////////////////////

// Using the above matrices for rotation around a "known" axis, and a little
// bit (well maybe a lot) of math, we can come up with the matrix that will
// allow us to rotate around ANY axis.  Sparing the gritty math details, which
// feel free to look up if you're in to S&M :), you get the following:

// **NOTE**
// x = X component of arbitrary normalized axis
// y = Y component of arbitrary normalized axis
// z = Z component of arbitrary normalized axis
// t = angle theta to rotate by
// Oh and prepare to scroll over, this puppy is long :)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [(x * x * (1 - cos(t)) + cos(t))			(x * y * (1 - cos(t)) + z * sin(t))		(x * z * (1 - cos(t)) - y * sin(t))]
// [(x * y * (1 - cos(t)) - z * sin(t))		(y * y * (1 - cos(t)) + cos(t))			(y * z * (1 - cos(t)) + x * sin(t))]
// [(x * z * (1 - cos(t)) + y * sin(t))		(y * z * (1 - cos(t)) - x * sin(t))		(z * z * (1 - cos(t)) + cos(t))	   ]
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// With the above matrix and a tad more linear algebra we can now implement
// this function which takes "axis" as the axis to rotate the camera around
// by "angle" amount in radians

// Rotates around an arbritrary axis
CVector CCamera::rotateAroundAxis(const CVector &vec, const CVector &axis, float angle)
{
	float aX = axis.x; // Axis X
	float aY = axis.y; // Axis Y
	float aZ = axis.z; // Axis Z
	
	// Compute the sin and cos of the angle 
	float sinAng = sinf(angle);
	float cosAng = cosf(angle);

	// Calculate X component
	float xxx = (aX * aX * (1.0f - cosAng) + cosAng) * vec.x +
			    (aX * aY * (1.0f - cosAng) + aZ * sinAng) * vec.y +
			    (aX * aZ * (1.0f - cosAng) - aY * sinAng) * vec.z;
	
	// Calculate Y component	   
	float yyy = (aX * aY * (1.0f - cosAng) - aZ * sinAng) * vec.x +
			    (aY * aY * (1.0f - cosAng) + cosAng) * vec.y +
			    (aY * aZ * (1.0f - cosAng) + aX * sinAng) * vec.z;
			   
	// Calculate Z component
	float zzz = (aX * aZ * (1.0f - cosAng) + aY * sinAng) * vec.x +
			    (aY * aZ * (1.0f - cosAng) - aX * sinAng) * vec.y +
			    (aZ * aZ * (1.0f - cosAng) + cosAng) * vec.z;
			    
	return CVector(xxx, yyy, zzz); // Return result
}

// Externed camera
CCamera theCamera;
CCamera *gCamera = &theCamera;
