#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

const float kDefaultAmt = 1.0f; // Default amount to move camera by
	
// The camera
class CCamera
{
	public:
		
		CCamera();
		
		void reset(); // Resets to a "starting" position
		
		void pitch(float angle); // Pitches camera by "angle"
		void rotateY(float angle); // Rotates around world Y-axis
		
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
		
		CPos getTarget() const { return eye + forward; } // Returns what the current camera's target is
		
		// Data Access ***
		
			CPos getEye() const { return eye; }
			CVector getCamForward() const { return forward; } // Returns camera's forward vector
			CVector getCamUp() const { return up; } // Returns camera's up vector
			CVector getCamRight() const { return right; } // Returns camera's right vector
		
		// *** End Data Access
		
	private:
	
		/*
			Initial camera axes			
			
			   (up)
				+Y   +Z (forward)
				|   /
				|  /				
				| /
				+ ------ +X  (right)
		*/
	
		// This allows us to rotate "vec" around an arbitrary "axis" by a
		// the "angle" in radians
		CVector rotateAroundAxis(const CVector &vec, const CVector &axis, float angle);
	
		CVector right; // The normalized axis that points to the "right"
		CVector up; // The normalized axis that points "up"
		CVector forward; // The normalized axis that points "forward"
		
		CPos eye; // The position of the camera (ie the camera's eye and the origin of the
				 // camera's coordinate system)
};

// Externed camera
extern CCamera *gCamera;

#endif
