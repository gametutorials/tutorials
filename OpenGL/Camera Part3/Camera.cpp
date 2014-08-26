//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Camera3											 //
//																		 //
//		$Description:	Allow the mouse to control our camera's view	 //

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;									// The vector to hold the cross product

	// If we are given 2 vectors (the view and up vector) then we have a plane define.  
	// The cross product finds a vector that is perpendicular to that plane, 
	// which means its point is straight out of the plane at a 90 degree angle.
	// The equation for the cross product is simple, but difficult at first to memorize:
	
	// The X value for the vector is:  (V1.y * V2.z) - (V1.z * V2.y)													// Get the X value
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
														
	// The Y value for the vector is:  (V1.z * V2.x) - (V1.x * V2.z)
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
														
	// The Z value for the vector is:  (V1.x * V2.y) - (V1.y * V2.x)
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// *IMPORTANT* This is not communitive. You can not change the order of this or
	// else it won't work correctly.  It has to be exactly like that.  Just remember,
	// If you are trying to find the X, you don't use the X value of the 2 vectors, and
	// it's the same for the Y and Z.  You notice you use the other 2, but never that axis.
	// If you look at the camera rotation tutorial, you will notice it's the same for rotations.

	// So why do I need the cross product to do a first person view?  Well, we need
	// to find the axis that our view has to rotate around.  Rotating the camera left
	// and right is simple, the axis is always (0, 1, 0).  Rotating around the camera
	// up and down is different because we are constantly going in and out of axises.
	// We need to find the axis that our camera is on, and that is why we use the cross
	// product.  By taking the cross product between our view vector and up vector,
	// we get a perpendicular vector to those 2 vectors, which is our desired axis.
	// Pick up a linear algebra book if you don't already have one, you'll need it.

	// Return the cross product
	return vNormal;										 
}


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// This will give us the magnitude or "norm" as some say, of our normal.
	// The magnitude has to do with the length of the vector.  We use this
	// information to normalize a vector, which gives it a length of 1.
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)   Where V is the vector

	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a normalize vector (A vector exactly of length 1)
/////
/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normalize(CVector3 vVector)
{
	// What's this function for you might ask?  Well, since we are using the cross
	// product formula, we need to make sure our view vector is normalized.  
	// For a vector to be normalized, it means that it has a length of 1.
	// For instance, a vector (2, 0, 0) would be (1, 0, 0) once normalized.
	// Most equations work well with normalized vectors.  If in doubt, normalize.

	// Get the magnitude of our normal
	float magnitude = Magnitude(vVector);				

	// Now that we have the magnitude, we can divide our vector by that magnitude.
	// That will make our vector a total length of 1.  
	// This makes it easier to work with too.
	vVector = vVector / magnitude;		
	
	// Finally, return our normalized vector
	return vVector;										
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the class constructor
/////
///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// Init a vVector to 0 0 0 for our position
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// Init a starting view vVector (looking up and out the screen) 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		// Init a standard up vVector (rarely ever changes)

	m_vPosition	= vZero;					// Init the position to zero
	m_vView		= vView;					// Init the view to a std starting view
	m_vUpVector	= vUp;						// Init the UpVector
}


///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the camera's position and view and up vVector.
/////
///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	// The code above just makes it cleaner to set the variables.
	// Otherwise we would have to set each variable x y and z.

	m_vPosition = vPosition;					// Assign the position
	m_vView     = vView;						// Assign the view
	m_vUpVector = vUpVector;					// Assign the up vector
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This allows us to look around using the mouse, like in most first person games.
/////
///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::SetViewByMouse()
{
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = SCREEN_WIDTH  >> 1;				// This is a binary shift to get half the width
	int middleY = SCREEN_HEIGHT >> 1;				// This is a binary shift to get half the height
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
	static float currentRotX = 0.0f;
	
	// Get the mouse's current X,Y position
	GetCursorPos(&mousePos);						
	
	// Now that we got the mouse position, we want to put the mouse position
	// back at the middle of the screen.  We pass in half of our screen width and height.
	// The >> operator is a binary shift.  So, we are shifting our width and height 
	// to the right by 1.  If you do the binary math, it is the same thing as dividing by 2, 
	// but extremely faster.  The reason why we put the cursor in the middle of the 
	// screen each time is we can get a delta (difference) of how far we move 
	// each frame, so we know how much to rotate the camera.

	// If our cursor is still in the middle, we never moved... so don't update the screen.
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// Set the mouse position to the middle of our window
	SetCursorPos(middleX, middleY);							

	// Now we need to get the direction (or VECTOR) that the mouse moved.
	// To do this, it's a simple subtraction.  Just take the middle point,
	// and subtract the new point from it:  VECTOR = P1 - P2;   with P1
	// being the middle point (400, 300) and P2 being the new mouse location.
	// Look below at the *Quick Notes* for more information and examples.
	// After we get the X and Y delta (or direction), I divide by 1000 to bring the number
	// down a bit, otherwise the camera would move lightning fast!

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)( (middleX - mousePos.x) ) / 1000.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 1000.0f;
	
	static float lastRotX = 0.0f; 
 	lastRotX = currentRotX; // We store off the currentRotX and will use it in when the angle is capped
	
	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	currentRotX += angleZ;
 
	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if(currentRotX > 1.0f)     
	{
		currentRotX = 1.0f;
		
		// Rotate by remaining angle if there is any
		if(lastRotX != 1.0f) 
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
				
			// Rotate the camera by the remaining angle (1.0f - lastRotX)
			RotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue.
	else if(currentRotX < -1.0f)
	{
		currentRotX = -1.0f;
		
		// Rotate by the remaining angle if there is any
		if(lastRotX != -1.0f)
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
			
			// Rotate the camera by ( -1.0f - lastRotX)
			RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Otherwise, we can rotate the view around our position
	else 
	{	
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		// Before using the axis, it's a good idea to normalize it first.
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);
	
		// Rotate around our perpendicular axis
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Always rotate the camera around the y-axis
	RotateView(angleY, 0, 1, 0);
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

	// Get the view vector (The direction we are facing)
	CVector3 vView = m_vView - m_vPosition;		

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
	m_vView = m_vPosition + vNewView;
}


///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will move the camera forward or backward depending on the speed
/////
///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::MoveCamera(float speed)
{
	// Get our view vector (The direciton we are facing)
	CVector3 vVector = m_vView - m_vPosition;
	
	m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
	m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Now we have the ability to look around our world with the mouse!  It's a great
// addition to our camera class isn't it?  Not much code either.  
// 
// Let me further explain what makes this work.  What we are doing is continuously
// putting the cursor in the middle of the screen every time we move the mouse, but
// before we do this, we get the new position that it moved.  Now that we have the
// position it moved from the center, we can do some simple math to find out which
// way we need to move/rotate the camera.  Let's do an example:
// 
// Let's say we changed our SCREEN_WIDTH and SCREEN_HEIGHT to 640 480.  
// That would make our middle point at (320, 240).  The '*' is the cursor position.
// We will call it P1 (Point 1):
//
//	 -----------------------------------
//  |									|
//  |									|
//  |									|
//  |									|
//  |									|
//  |				 * P1 (320, 240)	|
//  |									|
//  |									|
//  |									|
//  |									|
//  |									|
//  |									|
//	 -----------------------------------
// 
//	Now, when we move the mouse, we store that position of where we moved.
//  Let's say we moved the mouse diagonally up and left a little bit.  Our new
//  point would be, let's say  P2 (319, 239):
//
//	 -----------------------------------
//  |									|
//  |									|
//  |									|
//  |									|
//  |			   * P2 (319, 239)		|	(Figure is not pixel perfect :) )
//  |				\					|
//  |				 * (320, 240)		|
//  |									|
//  |									|
//  |									|
//  |									|
//  |									|
//  |									|
//	 -----------------------------------
//
//  Well, logic tells us that we need to rotate the camera to the left a bit,
//  and move the camera up a bit as well right???  Of course right.
// 
//  Well, the math is simple to find that out.  We just subtract P1 - P2.
//  That will give us a new x and y.  That is called the vector.
// 
//  What will the vector of those 2 points be then?  
//
//	(320 - 319, 240 - 239)			=         (1, 1)
// 
//  Now we have a vector of (1, 1).  In our case, those numbers are too high
//  when we are dealing with radians, not degrees.  To remedy this, we need to
//  make the number a bit smaller, I just divide by a number like 1000.  Now
//  we have a vector of (0.001, 0.001).  We want to pass the X value in as our
//  Y-axis rotation, BUT we need to negate it.  Because if we rotate by a positive number
//  it will move the camera to the RIGHT, and we want to go to the left.  This will take care
//  of when we move the mouse right too.  If the vector is negative, it will make it positive.
//
//  You'll notice we added some math functions at the top of this file.  These help us
//  determine the axis that we need to rotate around for up and down.  By grabbing
//  a 90 degree perpendicular vector to the side of the camera, we can then use
//  that as our axis to rotate around.  This took a bit to figure out at first.
//  It wasn't as simple as just rotating around the x-axis, because we are walking
//  around the x and z axis all the time, so the rotations need to change axises.
//
//  Notice that with the new CVector3 additions, we can't say:
//
//  CVector3 vVector = {0, 0, 0};
//  
//  It must be:
//
//  CVector3 vVector = CVector(0, 0, 0);
//
//  This is because we have a default constructor, so there is no more {}'s.
//  We can now create a CVector3 on the fly, without having to directory assing
//  a variable name to it.  The default copy constructor does all the work.
//
//
//  ©2000-2005 GameTutorials  
