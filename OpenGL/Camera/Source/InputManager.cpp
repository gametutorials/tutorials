//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera											 //
//																		 //
//		$Description:	Moves a camera in 3D space from user input		 //
//																		 //
//***********************************************************************//

#include "../Headers/InputManager.h"
#include "../Headers/TimeManager.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// The InputManager allows us to abstract the input away from the WindowManager so
// it wouldn't matter if we used GLFW, Win32, SDL, Glut, etc., and just sends standardized
// input codes from the WindowManager to this this class and then to the camera.
// In this tutorial we just process the UP and DOWN arrow keys (or W,S) to move the
// camera back and forth, but later it will handle the left and right rotations and
// even movement by the mouse and strafing.
//
// © 2000-2015 GameTutorials