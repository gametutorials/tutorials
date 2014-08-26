//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Camera											 //
//																		 //
//		$Description:	Create and move a camera around in our world	 //

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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

GLvoid CCamera::PositionCamera(float positionX, float positionY, float positionZ,
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

///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will move the camera forward or backward depending on the speed
/////
///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::MoveCamera(float speed)
{
	CVector3 vVector={0};						// Init a vector for our view

	// Below we are getting the "View Vector".  To get a vector from
	// 2 points, we subtract the first point from the second point.  Basically,
	// what this does is give us a direction that we are looking.  That is
	// how we know which direction we are looking at in 3D. Later we will create
	// functions that will do this stuff for us instead of doing it like this. Read 
	// "Quick Notes" for some more detail on vectors and the camera's view vector.

	// Get our view vector (The direction we are facing)
	vVector.x = m_vView.x - m_vPosition.x;		// This gets the direction of the X	
	vVector.y = m_vView.y - m_vPosition.y;		// This gets the direction of the Y
	vVector.z = m_vView.z - m_vPosition.z;		// This gets the direction of the Z

	// Below is the code that will move our view and camera position forward or backward.
	// What we are doing is adding our view vector times the speed, plus
	// our current position and view.  You might be thinking that we should just
	// add/subtract the speed to our view and position, and forget about the vector.
	// Well, the reason why we do this is because we want to go in the direction
	// that we are looking at.  It won't work if we just add the constant speed
	// to the X and Z values of our position and view.  Try it and you will see.
	// Sure, if we are looking DIRECTLY down the X or Z axis it would work, but
	// when we start to rotate our view at a unique angle, it won't work.  Trust me.
	// So basically, if we are looking at a 45 degree angle, we move in that direction.
	// If we want to move forward we pass in a positive speed, and if we want to move
	// backwards, just pass in a negative speed.

	m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
	m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file is a new addition for this tutorial.  We will be adding more to it
// in the next tutorial.  You will notice that I just made functions for going
// forward and backward.  I wanted to just introduce you to the basics of getting
// a camera moving around in your world.  The rest of the camera code comes in 
// the next tutorial.  Next what we need is code for rotating left or right.
// 
// CCamera()  - This just initializes our camera to a default position and view.
// If for some CRAZY reason we didn't call the PositionCamera() function it would
// at least allow us to have a valid position, view vector and up vector.  If we
// didn't do this, we would not see anything because there would be no view vector.
// 
// PositionCamera() - This should be used to initialize our camera at the beginning
// of our program.  This is considered a necessary operation.  Usually it should be
// called in an Init() function of the program, like we have in main.cpp.
//
// MoveCamera() - Depending on our view vector, this moves us along that vector by
// the acceleration that is passed in.  Say, if we are looking at a 45 degree angle,
// we will walk perfectly in that direction.  So, it works just like a camera should
// work. If we are facing a certain direction and we go forward, we should go in that
// direction.  We can use this for going forward and backward by passing in a positive
// speed for forward and a negative speed for going backwards.
//
// The next function we will code in the next Tutorial is RotateView().  Suck up this
// code and then move along!
//
// * VIEW VECTOR *
//
// If you haven't picked up what the view vector is, let me try and explain.  
// To get a view vector, or ANY vector, you need 2 points.  For example, P and V.
//
// P = Position				V = View	
//					*V
// 
//					
//				
//					* P
//
// Looking at point P and point V, let's assume that our position is point P, and	
// our view is point V.  Well, can you see which direction we are looking???  We
// are looking right down the Z axis, in 3D.  In 2D, we would be looking straight up.
// Well, let's say our position is at (0, 0, 0) and our view is at (0, 0, -1).
// Without looking at the 2 points above, we know that we are facing the negative Z axis.
// How do we know that?? Well, we might not realize it, but we are doing some math in our heads.
// 
// Let me demonstrate:  We subconsciously got the VECTOR of those 2 positions.
//
//						(V.x - P.x,  V.y - P.y,  V.z - P.z)
// Like so:  Vector V = (0   - 0,    0   - 0,     -1 - 0  )
// 
// To get the vector of 2 points, you subtract the 2 points from each other.
// In our case, we want the VIEW vector so we subtract the Position from the View (V - P).
//
// It's even more clear when we connect the points to visually for a direction.
// 
//					*V
//					|
//					|  <-- This is Vector V
//					|
//					*P
//
// See?  That is the direction we are looking at.  Let's move the view around to get
// a better picture that it's a direction we are looking at.
//
//
//						*V
//					   /
//					  /    <--- Vector V
//					 /				
//					*P
//
// Now we are looking to the right.  Notice the position doesn't change, just our view point.
// That's all a view vector is, it's just where we are looking, in a certain direction.
// How ELSE would you do it if you didn't do it this way??!?!?
//
//
// ©2000-2005 GameTutorials

