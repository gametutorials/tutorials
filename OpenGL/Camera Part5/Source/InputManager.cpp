//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 5									 //
//																		 //
//		$Description:	Camera Strafing									 //
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

		// We removed the left and right input keys in Camera Part 3 since the mouse handled our
		// rotations, but we can add them back again so that strafing is available to the camera.

		// If the user hits the LEFT or A key, strafe the camera to the left
		case Left: case A: case a:
			Camera->Strafe(-1 * Camera->GetSpeed() * TimeManager::Instance().DeltaTime);
			break;
			// If the user hits the RIGHT or D key, strafe the camera to the right
		case Right: case D: case d:
			Camera->Strafe(Camera->GetSpeed() * TimeManager::Instance().DeltaTime);
			break;

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			
	}
}


// This sends in an updated mouse x and y position that should be a delta from (0, 0)
void InputManager::MouseMoved(float mouseX, float mouseY)
{
	// Return if we don't have a valid camera assigned
	if ( Camera == nullptr )
		return;

	// Have our manager send the mouse x and y deltas to our camera to process the rotations
	Camera->SetViewByMouse(mouseX, mouseY);
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// The InputManager allows us to abstract the input away from the WindowManager so
// it wouldn't matter if we used GLFW, Win32, SDL, Glut, etc., and just sends standardized
// input codes from the WindowManager to this this class and then to the camera.
// 
// In this tutorial we added back the A/D or LEFT/RIGHT keys to enable strafing.
//
// © 2000-2015 GameTutorials