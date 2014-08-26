#ifndef _3DMATH_H
#define _3DMATH_H

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This file will be added upon heavily in the future tutorials, especially for collision.
// Math is a intricate part of game programming, especially in 3D game programming.
// If you are not comfortable with the concepts explain, make it a huge goal to practice
// and strive to understand them.  You can not be an effective 3D programmer without
// these basics.  Don't get too proud if you do, there is still much more after we cover
// all the basics :)  The Dot Product and Plain Equation are our next topics.

// This is used to hold 3D points and vectors. This class is also used in our camera
// tutorials.  Please refer back to those for more information.  Basically, we just
// want a class that holds 3 floats for our x, y, and z values.  This will be added upon later. 
struct CVector3
{
public:
	float x, y, z;						
};


//	This returns a perpendicular vector from 2 given vectors by taking the cross product.
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

//	This returns a vector between 2 points
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2);

//	This returns the magnitude of a normal (or any other vector)
float Magnitude(CVector3 vNormal);

//	This returns a normalize vector (A vector exactly of length 1)
CVector3 Normalize(CVector3 vNormal);

//	This returns the normal of a polygon (The direction the polygon is facing)
CVector3 Normal(CVector3 vTriangle[]);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We can now use our 3DMath.cpp and 3DMath.h in any project now by simple
// including it.  We will want at least ALL of these functions from now on
// when ever we do any 3D programming (except for basic OpenGL concepts).
// It is important to know how to use these, even if you don't know how to
// code them if you had to.  As long as you know WHAT they do and what they are
// for, that's a start.  To be a decent 3D programmer though, you have to have 
// this stuff down pretty well where you want have to think about instances when you
// would use them.  If you need a perpendicular vector for something, like to strafe
// the camera left and right, you should immediately think CROSS PRODUCT.
// If this is really confusing for you, don't worry about it.  I wasn't a math
// man until I started actually putting what I learned in school to use.  Even though
// I had taken high calculus classes, I still had to relearn all this stuff when I went
// to 3D programming.  It's one thing to do something on paper, and it's another to actually
// put it to a practical use.  I think math is taught horribly in most schools.  It doesn't
// leave the average person with skills as much as it should.  Of course it is always
// up to the student to learn, but I think it could be taught it a way where you actually
// understand what you are learning and for what reasons and applications it holds.
//
// DigiBen
// www.GameTutorials.com
//
//


