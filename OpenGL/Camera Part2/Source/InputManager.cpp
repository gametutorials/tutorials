//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 2									 //
//																		 //
//		$Description:	Rotate our existing camera around in the world	 //
//																		 //
//***********************************************************************//

#include "../Headers/InputManager.h"
#include "../Headers/TimeManager.h"


// This takes in a code and depending on the value moves the camera
void InputManager::KeyPressed(InputCodes code)
{
	// If there isn't a camera specified, just return
	if ( Camera == nullptr )
		return;

	// Depending on the code, move the camera back or forward fixed by a speed and time slice
	// to make sure the movement is the same on anyone's computer regardless of the speed.
	switch ( code )
	{
		// If the user moves up, move the camera up along the view vector
		case Up: case W: case w:
			Camera->MoveCamera(Camera->GetSpeed() * TimeManager::Instance().DeltaTime);
			break;
		// If the user moved down, move the camera down along the view vector
		case Down: case S: case s:
			Camera->MoveCamera(-1 * Camera->GetSpeed() * TimeManager::Instance().DeltaTime);
			break;		


		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// If the user hits the LEFT or A key, rotate the camera to the left
		case Left: case A: case a:
			Camera->SetYaw(Camera->GetYaw() - Camera->GetRotationSpeed() * TimeManager::Instance().DeltaTime);
			break;
			// If the user hits the RIGHT or D key, rotate the camera to the right
		case Right: case D: case d:
			Camera->SetYaw(Camera->GetYaw() + Camera->GetRotationSpeed() * TimeManager::Instance().DeltaTime);
			break;

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// The InputManager allows us to abstract the input away from the WindowManager so
// it wouldn't matter if we used GLFW, Win32, SDL, Glut, etc., and just sends standardized
// input codes from the WindowManager to this this class and then to the camera.
// In this tutorial we add the ability to rotate the camera along the y-axis when
// the user hits the LEFT/RIGHT or A/D keys.  This is done by setting the Yaw
// rotation (in radians) against the camera's rotation speed and a time slice to 
// deliver consistent speed.  The rotation matrix of the camera is then calculated
// each frame to give a newly rotated view.
//
// © 2000-2015 GameTutorials