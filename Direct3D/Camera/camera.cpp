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

// Moves the camera in the specified direction
void CCamera::move(ECamMoveDir dir,  float amt)
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

// Externed camera
CCamera theCamera;
CCamera *gCamera = &theCamera;
