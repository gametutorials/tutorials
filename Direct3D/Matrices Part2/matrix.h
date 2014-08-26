#ifndef MATRIX_H
#define MATRIX_H

#include <d3dx9math.h> // Included so we can use D3DXMATRIX
#include "vector.h"

/*
	Matrix Math 101
	
		Before we talk about our matrix class, lets do a quick high-level review of 
		matrix math.
		
		First off, what is a matrix?  Well it's a row by column of numbers.  A square
		matrix is one whose number of rows is equal to the number of columns.  Also
		there is a distinction between ROW matrices and COLUMN matrices.  A row matrix is 
		one whose elements are laid out from left to right in row order.  A column matrix
		is one whose elements are laid our from top to bottom in column order.  Here's some
		examples:
		
		[ 0 1 2 ]				[ 0 3 ]						[ 0  1  2  3  ]
		[ 3 4 5 ]				[ 1 4 ]						[ 4  5  6  7  ]
		[ 6 7 8 ]				[ 2 5 ]						[ 8  9  10 11 ]
															[ 12 13 14 15 ]
		A 3x3 row matrix		A 2x3 column matrix			
		Also a square matrix								A 4x4 row matrix
															Also known as a homogenous matrix
															
		When talking about matrix math, there are three general operations that come to 
		mind:  scaling, rotating, and translating.
		
		Scaling is the process of growing or shrinking something.  So lets say you 
		have a list of vertices that make up a triangle.  You could scale the size of 
		the triangle by multiplying each vertex by a scale matrix.
		
		Here's a quick ASCII example:
		
		 /\						 /\
		/  \ Object A			/  \	Object A scaled larger
	    ====				   /    \									
							  /		 \
							  ========
							  
		Rotating is the process of spinning something around an axis.  The mathematical 
		formulas for rotation depend on the coordinate system you are using and 
		whether you are using a row or column based matrix.  The coordinate system we will
		be using is a left handed coordinate system that looks like this:
		
	   +Y   +Z
		|   /
		|  /
		| /
		*------- +X
		
		So imagine you're on a merry-go-around and your using the above coordinate system.  You'd be
		rotating around the +Y axis.  And if you think about the tire on a car centered on the
		above coordinate system, it would be rotating around the +X axis.
		
		Translation is the process of moving an object in a linear path.  If you have a point
		in 3D space and add a vector to it, you have translated that point to a new point.  Any 
		time you move something in a straight line, a translation is taken place.
		
		That in a nutshell is a high level look at matrix math.
		
		**NOTE** Matrix math (linear algebra) is much more complex than what is presented here
				 If you truly want to do 3D graphics, you will need to learn some linear 
				 algebra.  You don't have to be an Einstein at math, just better than Bart Simpson 
				 to do 3D graphics.
*/

/*	
	For our matrix class we choose to use a 4x4.  Why 4x4?  Well to simplify the answer, it allows
	us to do scaling, rotation, and translation all in one matrix.  Also, it is what D3D and 
	OpenGL natively use, so taking our matrix and massaging it in to the appropriate form for 
	whatever 3D API we're using shouldn't be to difficult.
*/  
		
// A homogenous matrix
class CMatrix
{
	public:
	
		CMatrix() { identity(); } // Constructor -- Init to identity matrix
		CMatrix(float mat[16]); // Constructor #2
		CMatrix(float m11, float m12, float m13, float m14,
				float m21, float m22, float m23, float m24, 
				float m31, float m32, float m33, float m34,
				float m41, float m42, float m43, float m44); // Constructor #3
		CMatrix(const CMatrix &mat); // Copy Constructor
		
		CMatrix& operator =(const CMatrix &mat); // Assignment operator
		CMatrix operator *(const CMatrix &mat); // Multiply operator
		
		void identity(); // Makes the matrix the identity matrix
		void transpose(); // Transposes the matrix
		
		// Crate the rotation matrix from the passed in values
		void makeRotX(float angle); // Rotation matrix around the world X-axis
		void makeRotY(float angle); // Rotation matrix around the world Y-axis
		void makeRotZ(float angle); // Rotation matrix around the world Z-axis
	
		// Create the translation matrix from the passed in values
		void makeTranslate(const CVector &vec);
		
		D3DXMATRIX getD3DMatrix(); // Returns a D3D version of the matrix
		
		// Public data
		float m[16]; // We use a one dimensional array to store all 16 elements
					// in our matrix
		
		// Matrix elements looks like this:
		// [  0  1  2  3 ]
		// [  4  5  6  7 ]
		// [  8  9 10 11 ]
		// [ 12 13 14 15 ]
};
	

#endif
