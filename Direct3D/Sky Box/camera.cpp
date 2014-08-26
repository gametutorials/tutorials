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
	// Calculate new forward
	forward = rotateAroundAxis(forward, right, angle);
	
	// Calculate new camera up vector
	up = forward.cross(right);
}

// Rotates camera around the world Y-axis
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
