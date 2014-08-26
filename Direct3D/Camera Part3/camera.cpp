#include "camera.h"
#include <assert.h>

// Default Constructor
CCamera::CCamera()
{
	right.set(1.0f, 0.0f, 0.0f); // +X axis
	up.set(0.0f, 1.0f, 0.0f); // +Y axis
	forward.set(0.0f, 0.0f, 1.0f); // +Z axis

	// Set eye back kCamDist units on the -Z axis
	eye.set(0.0f, 0.0f, -kCamDist);
}

// Rotates camera's eye around the world Y-axis
// Using your standard Y-axis rotation matrix, we'll rotate around the Y-axis
// ca == cosf(angle)	sa == sinf(angle)
// [ ca 0 -sa ]
// [ 0  1   0 ]
// [ sa 0  ca ]
void CCamera::rotateY(float angle, const CPos &target)
{
	float xxx, zzz;
	float sinAng, cosAng;
	
	// Get sin()/cos() of angle
	sinAng = sinf(angle);
	cosAng = cosf(angle);
	
	// First translate to the world's origin
	eye -= target;
	
	// Save off forward components for computation
	xxx = eye.x;
	zzz = eye.z;
	
	// Rotate forward vector
	eye.x = xxx * cosAng + zzz * sinAng;
	eye.z = xxx * -sinAng + zzz * cosAng;
	
	// Translate back
	eye += target;
	
	setTarget(target); // Build the camera's axes 
}

// Using the matrix for rotation around an arbitrary axis, we'll rotate the eye around
// the camera's local right axis
/*
	Matrix for rotating around an arbitrary axis:
	
	x = X component of arbitrary normalized axis
	y = Y component of arbitrary normalized axis
	z = Z component of arbitrary normalized axis
	t = angle theta to rotate by
	
	--> Scroll over it's long --> 
	[(x * x * (1 - cos(t)) + cos(t))			(x * y * (1 - cos(t)) + z * sin(t))		(x * z * (1 - cos(t)) - y * sin(t))]
	[(x * y * (1 - cos(t)) - z * sin(t))		(y * y * (1 - cos(t)) + cos(t))			(y * z * (1 - cos(t)) + x * sin(t))]
	[(x * z * (1 - cos(t)) + y * sin(t))		(y * z * (1 - cos(t)) - x * sin(t))		(z * z * (1 - cos(t)) + cos(t))	   ]
*/
void CCamera::pitch(float angle, const CPos &target)
{
	// Compute the sin and cos of the angle 
	float sinAng = sinf(angle);
	float cosAng = cosf(angle);
	
	eye -= target; // Translate to world's origin

	// Calculate new eye X component
	float xxx = (right.x * right.x * (1.0f - cosAng) + cosAng) * eye.x +
				(right.x * right.y * (1.0f - cosAng) + right.z * sinAng) * eye.y +
				(right.x * right.z * (1.0f - cosAng) - right.y * sinAng) * eye.z;

	// Calculate new Y component	   
	float yyy = (right.x * right.y * (1.0f - cosAng) - right.z * sinAng) * eye.x +
				(right.y * right.y * (1.0f - cosAng) + cosAng) * eye.y +
				(right.y * right.z * (1.0f - cosAng) + right.x * sinAng) * eye.z;

	// Calculate new Z component
	float zzz = (right.x * right.z * (1.0f - cosAng) + right.y * sinAng) * eye.x +
				(right.y * right.z * (1.0f - cosAng) - right.x * sinAng) * eye.y +
				(right.z * right.z * (1.0f - cosAng) + cosAng) * eye.z;

	eye.set(xxx, yyy, zzz); // Set the eye
	eye += target; // Translate back
	
	// Get vector from target to eye
	CVector vec = eye - target;
	vec.normalize();
	
	// Position eye so that's always "kCamDist" from target
	eye = target + (vec * kCamDist);
	
	setTarget(target); // Build the camera's axes
}
	
// Builds the camera's axes by using the eye, target, and world's up vector
void CCamera::setTarget(const CPos &target)
{
	// Compute new forward vector (line of sight vector)
	forward = target - eye;
	forward.normalize();
	
	// Compute new right vector
	right = kWorldUp.cross(forward);
	right.normalize();
	
	// Compute new camera up vector
	up = forward.cross(right);
}

// Externed camera
CCamera theCamera;
CCamera *gCamera = &theCamera;
