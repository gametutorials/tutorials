#include <assert.h>
#include <utility> // Include so we can use the C++ templated swap() function
#include "matrix.h"

using namespace std;

// Constructor #2
CMatrix::CMatrix(float mat[16])
{
	memcpy(m, mat, sizeof(float) * 16);
}

// Constructor #3
CMatrix::CMatrix(float m11, float m12, float m13, float m14,
				 float m21, float m22, float m23, float m24, 
				 float m31, float m32, float m33, float m34,
				 float m41, float m42, float m43, float m44)
{
	m[0] = m11;	m[1] = m12;	m[2] = m13;	m[3] = m14;
	m[4] = m21;	m[5] = m22;	m[6] = m23;	m[7] = m24;
	m[8] = m31;	m[9] = m32; m[10] = m33; m[11] = m34;
	m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
}

// Copy constructor
CMatrix::CMatrix(const CMatrix &mat)
{
	memcpy(m, mat.m, sizeof(float) * 16);
}

// Assignment operator
CMatrix& CMatrix::operator =(const CMatrix &mat)
{
	memcpy(m, mat.m, sizeof(float) * 16);
		return *this;
}

// Multiplies the 4x4 matrix "this" by the 4x4 matrix "mat"
// For matrix multiplication to even by valid the following must be true:
// Give a matrix A and matrix B with A being M rows by N columns and 
// B being N rows by Q columns, A*B = an M x Q matrix.  
// The following illustrates the pattern that matrix multiplication follows:
// **NOTE** This pattern can be extended two any matrix multiplication so 
//			long as the matrices are in this form (M rows x N cols) * (N rows x Q cols)
/*
	A = [ a1 a2 ]		B = [ b1 b2 ]		
		[ a3 a4 ]			[ b3 b4 ]
	
	A * B = C
	
	C = [ c1 c2 ]
		[ c3 c4 ]
		
	c1 = a1 * b1 + a2 * b3
	c2 = a1 * b2 + a2 * b4
	c3 = a3 * b1 + a4 * b3
	c4 = a3 * b2 + a4 * b4
*/
CMatrix CMatrix::operator *(const CMatrix &mat)	
{
	return CMatrix(
			m[0] * mat.m[0] + m[1] * mat.m[4] + m[2] * mat.m[8] + m[3] * mat.m[12],
			m[0] * mat.m[1] + m[1] * mat.m[5] + m[2] * mat.m[9] + m[3] * mat.m[13],
			m[0] * mat.m[2] + m[1] * mat.m[6] + m[2] * mat.m[10] + m[3] * mat.m[14],
			m[0] * mat.m[3] + m[1] * mat.m[7] + m[2] * mat.m[11] + m[3] * mat.m[15],
			m[4] * mat.m[0] + m[5] * mat.m[4] + m[6] * mat.m[8] + m[7] * mat.m[12],
			m[4] * mat.m[1] + m[5] * mat.m[5] + m[6] * mat.m[9] + m[7] * mat.m[13],
			m[4] * mat.m[2] + m[5] * mat.m[6] + m[6] * mat.m[10] + m[7] * mat.m[14],
			m[4] * mat.m[3] + m[5] * mat.m[7] + m[6] * mat.m[11] + m[7] * mat.m[15],
			m[8] * mat.m[0] + m[9] * mat.m[4] + m[10] * mat.m[8] + m[11] * mat.m[12],
			m[8] * mat.m[1] + m[9] * mat.m[5] + m[10] * mat.m[9] + m[11] * mat.m[13],
			m[8] * mat.m[2] + m[9] * mat.m[6] + m[10] * mat.m[10] + m[11] * mat.m[14],
			m[8] * mat.m[3] + m[9] * mat.m[7] + m[10] * mat.m[11] + m[11] * mat.m[15],
			m[12] * mat.m[0] + m[13] * mat.m[4] + m[14] * mat.m[8] + m[15] * mat.m[12],
			m[12] * mat.m[1] + m[13] * mat.m[5] + m[14] * mat.m[9] + m[15] * mat.m[13],
			m[12] * mat.m[2] + m[13] * mat.m[6] + m[14] * mat.m[10] + m[15] * mat.m[14],
			m[12] * mat.m[3] + m[13] * mat.m[7] + m[14] * mat.m[11] + m[15] * mat.m[15]
			);
}

// Set the matrix to the identity matrix
// You can think of the identity matrix just like you think of the number 1 when
// thinking about multiplication.  So just as 2 * 1 = 2, given a square matrix A and 
// an identity matrix I, A * I = A
// [ 1 0 0 0 ]
// [ 0 1 0 0 ]
// [ 0 0 1 0 ]
// [ 0 0 0 1 ]
void CMatrix::identity()
{
	memset(m, 0, sizeof(float) * 16);
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// Transposes the matrix
// When you transpose a matrix, you swap all of it's rows and columns.  So 
// here's what you get: 
//  Matrix before	  ~~~	   Matrix after	
// [  0  1  2  3 ]			 [  0  4  8 12 ]
// [  4  5  6  7 ]			 [  1  5  9 13 ]
// [  8  9 10 11 ]			 [  2  6 10 14 ]
// [ 12 13 14 15 ]			 [  3  7 11 15 ]
void CMatrix::transpose()
{
	// m[0] doesn't change
	
	swap(m[1], m[4]);
	swap(m[2], m[8]);
	swap(m[3], m[12]);
	// m[5] doesn't change
	
	swap(m[6], m[9]);
	swap(m[7], m[13]);
	// m[10] doesn't change
	
	swap(m[11], m[14]);
	// m[15] doesn't change	
}

/*
	The following three rotation formulas are something you find in your favorite 3D 
	math book.  However, you may see them in a different form depending on the 
	matrix type (row or column) and coordinate system used for the formulas.
*/


// Creates a rotation matrix around the X-axis using "angle" specified 
// in radians
// [  1  0  0  0 ]
// [  0  c  s  0 ] with 'c' equaling cosine of the angle
// [  0 -s  c  0 ]      's' equaling sine of the angle 
// [  0  0  0  1 ]
void CMatrix::makeRotX(float angle)
{
	identity(); // Set to identity
	
	// Calculate sin/cos of angle
	float cosAng = cosf(angle);
	float sinAng = sinf(angle);
	
	// Set matrix 
	m[5] = m[10] = cosAng;
	m[6] = sinAng;
	m[9] = -sinAng;
}

// Creates a rotation matrix around the Y-axis using "angle" specified 
// in radians
// [  c  0 -s  0 ]
// [  0  1  0  0 ] with 'c' equaling cosine of the angle
// [  s  0  c  0 ]      's' equaling sine of the angle 
// [  0  0  0  1 ]
void CMatrix::makeRotY(float angle)
{
	identity(); // Set to identity
	
	// Calculate sin/cos of angle
	float cosAng = cosf(angle);
	float sinAng = sinf(angle);
	
	// Set matrix 
	m[0] = m[10] = cosAng;
	m[2] = -sinAng;
	m[8] = sinAng;
}

// Creates a rotation matrix around the Z-axis using "angle" specified 
// in radians
// [  c  s  0  0 ]
// [ -s  c  0  0 ] with 'c' equaling cosine of the angle
// [  0  0  1  0 ]      's' equaling sine of the angle 
// [  0  0  0  1 ]
void CMatrix::makeRotZ(float angle)
{
	identity(); // Set to identity
	
	// Calculate sin/cos of angle
	float cosAng = cosf(angle);
	float sinAng = sinf(angle);
	
	// Set matrix 
	m[0] = m[5] = cosAng;
	m[1] = sinAng;
	m[4] = -sinAng;
}

// Create the translation matrix from the passed in values
// Here is the translation portion of our matrix:
// [  0  1  2  3 ]
// [  4  5  6  7 ]
// [  8  9 10 11 ]
// [ t1 t2 t3 15 ]	Translation portion makes the vector (t1, t2, t3)
void CMatrix::makeTranslate(const CVector &vec)
{
	identity();
	
	m[12] = vec.x;
	m[13] = vec.y;
	m[14] = vec.z;
}

// Returns D3D version of the CMatrix
D3DXMATRIX CMatrix::getD3DMatrix()
{
	return D3DXMATRIX(m[0], m[1], m[2], m[3],
					  m[4], m[5], m[6], m[7],
					  m[8], m[9], m[10], m[11],
					  m[12], m[13], m[14], m[15]);
}
