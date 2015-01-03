//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera											 //
//																		 //
//		$Description:	Moves a camera in 3D space from user input		 //
//																		 //
//***********************************************************************//

#include "../Headers/Camera.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
This is our new Camera class which stores a camera's Position, Yaw and Pitch rotations.
Unlike our legacy OpenGL tutorials, this camera class doesn't store the view and up vector,
but will use the Yaw (y-axis rotation) and Pitch (x-axis rotation) to calculate them on the fly.
We also added the creation of the projection matrix into our Camera class with SetPerspective().
In this first tutorial in our camera tutorial series, we just use the MoveCamera() function to
move the camera along it's default view vector, which is looking down the negative z-axis.
*/

// Initialize the camera to have a position at the origin
Camera::Camera()
{
	// Set the default position for the camera to the origin
	Position = vec3(0.0, 0.0, 0.0);
}


// This sets and returns the camera's perspective matrix from a field of view (in radians), aspect ratio 
// and the near and far planes which cull the geometry outside our desired viewing distance.
mat4 Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	// Create the projection matrix with GLM's perspective function, replacing gluPerspective.
	ProjectionMatrix = perspective(fov, aspectRatio, nearPlane, farPlane);

	return ProjectionMatrix;
}


// This function sets the camera's position and rotation values
void Camera::PositionCamera(float positionX, float positionY, float positionZ, float yaw, float pitch)
{
	// Assign our camera variables
	Position = vec3( positionX, positionY, positionZ );
	Yaw = yaw;
	Pitch = pitch;
}


// This returns the camera's view matrix that is used to pass to our shaders
mat4 Camera::GetViewMatrix()
{
	// In order for the graphics card to draw anything on the screen it needs to know
	// where the camera is and what's it looking at so it knows what to draw on screen.
	// We tell OpenGL about our camera with a View Matrix, which is passed to our shaders.
	// To create a view matrix you multiply the camera's rotation matrix by the inverse
	// of the translation matrix.  The rotation matrix contains all the rotations for
	// our Yaw and Pitch, and the translation matrix contains the Position of the camera.
	
	// In this tutorial we aren't using rotations so our view matrix is just the inverse
	// of a translation matrix.  This is the same as translate(mat4(), -Position).  The
	// negative position (or inverse) creates a matrix for the view that the camera is 
	// looking at, since in our case we are looking down the negative z-axis which is 
	// the default with no rotations.  In the next tutorial we will multiply this matrix 
	// by a created rotation matrix from the yaw and pitch values.  Just know for now 
	// that you would never leave it like this but to just break up the tutorials I left 
	// out the rotation matrix calculations.
	return inverse(translate(mat4(), Position));
}


// This returns the current view vector (the direction the camera is looking) from the rotation matrix
vec3 Camera::GetView()
{
	// Since we aren't using rotations yet, our view vector is just a unit vector (length of 1) 
	// looking down the negative z-axis.  In the next tutorial we will add our rotation matrix 
	// to get a changing and rotated view vector.  So when the camera moves it will be going
	// in this direction only.
	return vec3(0, 0, -1);
}


// This will move the camera forward or backward depending on the speed
void Camera::MoveCamera(float speed)
{
	// In order to move the camera, we first need to get the direction that the camera is
	// facing.  We have this from our GetView() function.  Until we get rotations in the 
	// next tutorial, we just hard code our view vector to look down the negative z-axis.  
	
	// *SIDE NOTE* If you were storing a position and view location like in our old OpenGL 
	// tutorials, you would need to take the view (the location we are looking) and 
	// subtract the camera's Position from it.  That will give us a view vector (direction) 
	// that the camera is looking.  You would need to then normalize() that vector to make 
	// sure it has a length of 1 to make our calculations for our speed consistent no 
	// matter how far away the view location is from the camera.  

	// Get our normalized view vector (The direction we are facing)
	vec3 viewVector = GetView();

	// Now that we have the direction the camera is facing, we will move the position
	// along that vector with a consistent speed.  The speed is consistent because we
	// multiply the camera's speed by a time slice from our InputManager.  That way it
	// doesn't matter how fast or slow your computer is, it will move at the same speed.
	// If the speed is positive, then the camera will move forward, and if it's negative
	// the camera will move backwards. Notice we don't move the y value of the camera's
	// position so the camera doesn't come off the ground when they look up.  This would
	// make sense if we were flying something or in space perhaps though.

	// Move our camera position forward or backward along the view vector
	Position.x += viewVector.x * speed;		// Add our acceleration to our position's X
	Position.z += viewVector.z * speed;		// Add our acceleration to our position's Z
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This is our Camera class which controls the user's view in the 3D world.  In
// this tutorial we only show how to move the camera back and forth along the
// z-axis, but in the next tutorial we go over rotations and start making it be
// a useful camera class.  We will now handle all projection and view matrices 
// inside this class going forward.  This way our Models can just take a camera
// instance and get the matrices from there instead of passing them in each frame.
// As a reminder, this class is super bare bones and we will be building on top
// of it throughout the next tutorials in the camera series.
//
// We are using the glm library for all of our 3D math functions, so the inverse()
// function comes in handy.  Taking the inverse of a matrix is the same idea as
// reciprocal of a number, like the reciprocal of 100 is 1/100.  Since you don't
// divide with matrices, you create an inverse of a matrix.  If you were to
// multiply a matrix by it's inverse it returns the identity matrix.  As you will
// see in the next tutorial we use the inverse to derive the view and up vector
// from the inverse of the rotation matrix.
//
// ©2000-2015 GameTutorials