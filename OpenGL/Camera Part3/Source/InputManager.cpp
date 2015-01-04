//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 3									 //
//																		 //
//		$Description:	Rotate the camera by the mouse					 //
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
	
		// We removed the rotation of our Yaw from the last tutorial since that is done with the mouse
	
		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This sends in an updated mouse x and y position that should be a delta from (0, 0)
void InputManager::MouseMoved(float mouseX, float mouseY)
{
	// Return if we don't have a valid camera assigned
	if ( Camera == nullptr )
		return;

	// Have our manager send the mouse x and y deltas to our camera to process the rotations
	Camera->SetViewByMouse(mouseX, mouseY);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// The InputManager allows us to abstract the input away from the WindowManager so
// it wouldn't matter if we used GLFW, Win32, SDL, Glut, etc., and just sends standardized
// input codes from the WindowManager to this this class and then to the camera.
// 
// In this tutorial we added the MouseMoved() function which takes in the mouse x
// and y deltas to send to the camera to have the rotations calculated.  The mouse
// positions should be deltas from (0, 0) since we assume the cursor position keeps
// getting set to (0, 0) and then giving the change every frame.  We also removed
// the LEFT/RIGHT key checks for the yaw rotation because this is now being done
// with the mouse movements.
//
// © 2000-2015 GameTutorials