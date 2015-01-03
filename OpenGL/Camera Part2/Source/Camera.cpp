//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 2									 //
//																		 //
//		$Description:	Rotate our existing camera around in the world	 //
//																		 //
//***********************************************************************//

#include "../Headers/Camera.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
In this tutorial we added a rotation matrix that is created from the Yaw and Pitch and is used
to generate the camera's view vector and view matrix.  This camera class can now enable the
user to rotate the view right and left by using the LEFT/RIGHT or A/D keys, and then move
forward in that new direction.  Review the GetRotationMatrix() function to first see how to
create a rotation matrix from the Yaw and Pitch angles (in radians).  Then review the GetView(),
GetViewMatrix() and GetUp() to see how to use that rotation matrix to calculate the camera
vectors and final view matrix that is used by our Models.
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This builds and returns a rotation matrix from the yaw and pitch rotations
mat4 Camera::GetRotationMatrix()
{
	// To create a rotation matrix from our Yaw and Pitch angles, we just need to
	// first create a blank identity matrix, the call the glm::rotate() function
	// to pass in that new matrix with the angle and axis to rotate on.  In the case
	// of Pitch, this is a rotation along the x-axis so we use vec3(1, 0, 0).
	// For Yaw, this is a rotation along the y-axis so we use vec3(0, 1, 0).
	// The rotation matrix can now be used to calculate the rest of our camera details.

	// Create an identity matrix
	mat4 rotationMatrix(1.0f);

	// Add the Pitch rotation along the x-axis
	rotationMatrix = rotate(rotationMatrix, Pitch, vec3(1, 0, 0));

	// Add the Yaw rotation along the y-axis
	rotationMatrix = rotate(rotationMatrix, Yaw, vec3(0, 1, 0));

	// Return the final rotation matrix that stores our camera rotations
	return rotationMatrix;
}


// This returns the camera's view matrix that is used to pass to our shaders
mat4 Camera::GetViewMatrix()
{
	// To create a view matrix you multiply the camera's rotation matrix by the inverse
	// of the translation matrix.  The rotation matrix contains all the rotations for
	// our Yaw and Pitch, and the translation matrix contains the Position of the camera.
	// This matrix will be used to pass into our shaders from our models to tell OpenGL
	// what we are looking at so it knows what and where to draw on the screen.
	// In the last tutorial we just returned the inverse of the translation matrix,
	// but now we combine the rotation and the translation matrix to give a final matrix.
	return GetRotationMatrix() * inverse(translate(mat4(), Position));
}


// This returns the current view vector (the direction the camera is looking) from the rotation matrix
vec3 Camera::GetView()
{
	// Since we don't store a view vector, we need to calculate the view vector by taking
	// the inverse of our rotation matrix multiplied by a vector looking down the negative
	// z-axis.  By taking the inverse of our rotation matrix and initial view vector, this
	// essentially gives us the vector that would have been used to create the current matrix.
	// So this essentially gives us a unit vector (length of 1) that represents the direction
	// the camera is looking at.
	vec4 viewVector = inverse(GetRotationMatrix()) * vec4(0, 0, -1, 1);

	// We are using a 4x4 matrix so our result is a vec4, but just cast it back to a vec3
	return vec3(viewVector);
}


// This returns the camera's up vector (the direction pointing up) from the rotation matrix
vec3 Camera::GetUp()
{
	// Just like we abstracted the view vector from our rotation matrix, we do the same
	// thing to get the up vector but instead change the vector to a vector pointing up.
	// By taking the inverse of the rotation matrix multiplied by a default up vector,
	// it will give us the current up vector of the camera.  The up vector will always
	// be vec3(0, 1, 0) until our next tutorial since we aren't looking up or down yet.
	vec4 upVector = inverse(GetRotationMatrix()) * vec4(0, 1, 0, 1);

	// We are using a 4x4 matrix so our result is a vec4, but just cast it back to a vec3
	return vec3(upVector);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This will move the camera forward or backward depending on the speed
void Camera::MoveCamera(float speed)
{
	// Get our normalized view vector (The direction we are facing)
	vec3 viewVector = GetView();

	// Move our camera position forward or backward along the view vector
	Position.x += viewVector.x * speed;		// Add our acceleration to our position's X
	Position.z += viewVector.z * speed;		// Add our acceleration to our position's Z
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This is our Camera class which controls the user's view in the 3D world.  In
// this tutorial we added the ability to rotate the camera left or right by
// changing the yaw (y-axis rotation) and building a rotation matrix.  This matrix
// is used to determine our current view vector in GetView() and up vector in 
// GetUp().  We also use the rotation matrix to build our final view matrix that
// is passed to the shaders so OpenGL knows how to render the scene according to
// the camera's position.
//
// ©2000-2015 GameTutorials