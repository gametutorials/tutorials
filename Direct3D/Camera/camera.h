#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

/*
	This is our camera class in it's all it's glory!  There is certainly more than one way
	to do a camera system, but we are going to choose a common camera implementation.  We're 
	going to store the camera's eye (where we are looking from) and the camera's axes 
	with the eye acting as the origin of the camera's axes.
	
	If we recall our world coordinate system is define by: 
	
	+Y   +Z
	 |   /
	 |  /
	 | /
	 + ------ +X
	 
	 We will use the same left handed coordinate system for our camera.  The only real difference
	 for this particular tutorial, is that for the world (0,0,0) will always be the origin of the 
	 world coordinate system, but the eye position will always be the origin of the camera system.
	 
	 So here's a quick (and badly drawn :) ASCII picture of what a scene might look like:
	 (X, Y, Z) == World coordinate system
	 (cx, cy, cz) == Camera coordinate system
	 E == Camera's eye
	 
	 +Y			+Z
	 |          /
	 |         /
	 |        /
	 |       /
	 |      /	   +cy
	 |     /	    |   / +cz
	 |    /			|  /
	 |   /			| / 
	 |  /			E------ +cx
	 | /
	 + ------------------------------- +X
	 
*/

const float kDefaultAmt = 1.0f; // Number of units to move by default

class CCamera
{
	public:
		
		CCamera();
		
		void reset(); // Resets to a "starting" position
		
		// Valid directions to move the camera in
		enum ECamMoveDir
		{
			eForward, // Move forward relative to the camera's space
			eBack, // Move backward relative to the camera's space
			eLeft, // Move left relative to the camera's space
			eRight // Move right relative to the camera's space
		};
		
		void move(ECamMoveDir dir, float amt = kDefaultAmt);
						
		void setEye(const CPos &eyePos) { eye = eyePos; } // Sets the eye position of the camera
		
		// This function returns what the camera is looking at.  Our eye is ALWAYS the origin
		// of camera's coordinate system and we are ALWAYS looking straight down the "forward" axis
		// so to calculate the target it's just a matter of adding the eye plus the forward
		CPos getTarget() const { return eye + forward; } // Returns what the current camera's target is
		
		// Data Access ***
			
			CPos getEye() const { return eye; }
			CVector getCamForward() const { return forward; } // Returns camera's forward vector
			CVector getCamUp() const { return up; } // Returns camera's up vector
			CVector getCamRight() const { return right; } // Returns camera's right vector
		
		// *** End Data Access
		
	private:
	
		/*
			Here we store our camera's axes.  Since the direction they point is relative
			to the direction our camera is facing, we use more general naming conventions.  But
			for reference, since our camera axes are initialized to be identical to the world
			axes, you can see that indeed the labeling makes sense :)
			
				+Y   +Z
				|   /
				|  /				Initial camera axes
				| /
				+ ------ +X  
		*/	
	
		CVector right; // The normalized axis that points to the "right"
		CVector up; // The normalized axis that points "up"
		CVector forward; // The normalized axis that points "forward"
		
		CPos eye; // The position of the camera (ie the camera's eye and the origin of the
				 // camera's coordinate system)
};

// Externed camera
extern CCamera *gCamera;

#endif
