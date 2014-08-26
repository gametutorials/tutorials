#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

const float kDefaultAmt = 1.0f; // Number of units to move by default

// Simple camera class, that moves forward, back, right and left
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
	
		CVector right; // The normalized axis that points to the "right"
		CVector up; // The normalized axis that points "up"
		CVector forward; // The normalized axis that points "forward"
		
		CPos eye; // The position of the camera (ie the camera's eye and the origin of the
				 // camera's coordinate system)
};

// Externed camera
extern CCamera *gCamera;

#endif
