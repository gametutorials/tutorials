//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Camera2											 //
//																		 //
//		$Description:	Rotate our existing camera around in the world	 //

#include "main.h"


///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the class constructor
/////
///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CCamera::CCamera()
{
	CVector3 vZero = {0.0, 0.0, 0.0};		// Init a vector to 0 0 0 for our position
	CVector3 vView = {0.0, 1.0, 0.5};		// Init a starting view vector (looking up and out the screen) 
	CVector3 vUp   = {0.0, 0.0, 1.0};		// Init a standard up vector (Rarely ever changes)

	m_vPosition	= vZero;					// Init the position to zero
	m_vView		= vView;					// Init the view to a std starting view
	m_vUpVector	= vUp;						// Init the UpVector
}

///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the camera's position and view and up vector.
/////
///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
					  		   float viewX,     float viewY,     float viewZ,
							   float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= {positionX, positionY, positionZ};
	CVector3 vView		= {viewX, viewY, viewZ};
	CVector3 vUpVector	= {upVectorX, upVectorY, upVectorZ};

	// The code above just makes it cleaner to set the variables.
	// Otherwise we would have to set each variable x y and z.

	m_vPosition = vPosition;					// Assign the position
	m_vView = vView;							// Assign the view
	m_vUpVector = vUpVector;					// Assign the up vector
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This rotates the view around the position using an axis-angle rotation
/////
///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;
	CVector3 vView;	

	// Get our view vector (The direciton we are facing)
	vView.x = m_vView.x - m_vPosition.x;		// This gets the direction of the X	
	vView.y = m_vView.y - m_vPosition.y;		// This gets the direction of the Y
	vView.z = m_vView.z - m_vPosition.z;		// This gets the direction of the Z

	// Now that we have a view vector stored in the CVector3 "vView", we can rotate it.
	// This function will be called when we want to move the camera LEFT and RIGHT.
	// So, we need to rotate our view around our position.  Think about it like this:
	// Say we we are standing in one place looking forward.  We are looking in a
	// direction right?  Well, if we rotate our head LEFT or RIGHT, our view changes.
	// Our position does not change... just what we are looking at.  That is why
	// we are just changing m_vView because that is where we are looking.  We want
	// to rotate m_vView around m_vPosition in a circular fashion to simulate this.
	// To rotate something, we want to use sine and cosine.  We need to include
	// math.h in order to use the sin() and cos() functions.
	//
	// To do our camera rotations from now on, we will be using Axis-Angle
	// rotations.  I must warn you that the equations to calculate this
	// rotation isn't intuitive, but isn't a lot of code.  An axis-angle rotation
	// allows us to rotate a point in space around an arbitrary axis.  That means
	// we can take our view point (m_vView) and rotate around our position 
	// (m_vPosition), in any direction.  This is going to come in handy when we
	// get to moving the camera's view by the mouse.  Since the explanation behind
	// the math is larger than I want to write, I suggest checking out Paul Bourke's
	// page for a decent explanation of the math involved:  
	// http://astronomy.swin.edu.au/~pbourke/geometry/rotate/

	// Calculate the sine and cosine of the angle once
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// Find the new x position for the new rotated point
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Find the new y position for the new rotated point
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// Find the new z position for the new rotated point
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	m_vView.x = m_vPosition.x + vNewView.x;
	m_vView.y = m_vPosition.y + vNewView.y;
	m_vView.z = m_vPosition.z + vNewView.z;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will move the camera forward or backward depending on the speed
/////
///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::MoveCamera(float speed)
{
	CVector3 vVector={0};					// Init a vector for our view

	// Get our view vector (The direciton we are facing)
	vVector.x = m_vView.x - m_vPosition.x;	// This gets the direction of the X	
	vVector.y = m_vView.y - m_vPosition.y;	// This gets the direction of the Y
	vVector.z = m_vView.z - m_vPosition.z;	// This gets the direction of the Z

	m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
	m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Now we have a camera function that allows us to rotate around a certain axis.
// If we want to rotate the opposite direction we just pass in a negative speed.
// You can see this done in CheckForMovement() in main.cpp.
// 
// The next tutorial will focus a bit more on the camera, but more on adding to the
// CVector3 class.  Now that we are moving on, we need more math functions.
// 
// 
// ©2000-2005 GameTutorials
