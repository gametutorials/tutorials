#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

/*
	We want to add "swivel" to our camera system.  Before we get into the nitty gritty implementation,
	it's important to define a few terms.  Typically, rotation of the camera (or lots of other
	objects for that matter) is referred to by pitch, bank, and heading (with heading also known as 
	yaw).  The short form is PBH.  Pitch, bank and heading come from airplane terms, so if 
	your a pilot you should already understand these terms.  If you have yet to fly that 747, then 
	here's an explanation:
	
	Imagine a machine gun sitting on a tripod.  You walk up to the machine gun and it's barrel
	is pointing perfectly straight forward, away from you.  Okay continue this exercise in imagination
	by pretending a axes coordinate system was protruding from the machine gun.  The +Z axis would
	run parallel to the machine gun barrel, the +Y axis would point up towards the sky, and the 
	+X axis would point to your right and be perpendicular to the +Y and +Z.  So the coordinate 
	system would be something like this:
	
	+Y   +Z
	 |   /
	 |  /
	 | /
	 + ------ +X
	 
	 Look familiar?  It should since we've been using it for everything :)
	 
	 Now if you were to rotate the gun so it's barrel was pointing towards the sky that
	 means you have PITCHED the gun up.  If you rotate the barrel so it was pointing towards
	 the ground, you have PITCHED the gun down.  Thus pitch is rotation around the X axis.
	 
	 Now assuming your put the gun back to it's original position and then turn it so the barrel
	 is facing more towards your left, you have changed the guns heading.  A positive change in heading
	 will turn the gun to the right, a negative change in heading will turn the gun left.  So heading
	 is rotation around the Y axis.
	 
	 Putting the gun back at the original position and then turning the gun on it's side would
	 be equivalent to changing the gun's bank.  When you change the bank of an object, you
	 are rotation it around the Z axis.
	 
	 So that's PBH in a nutshell.  Now it is VERY important to remember that depending 
	 on how you axes are set up and if you are using left vs a right hand coordinate system
	 PBH may take on a different meaning.  Coordinate systems are totally relative to the 
	 program and how the programmer decided to use them, so be cautious when looking at other
	 code and when writing your own.
	 
	 We are going to implement two functions that will enable us to do camera swivel. 
	 One of the functions will rotate the camera around the world Y axis, and the other
	 will rotate the camera around it's local X axis.
*/

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
