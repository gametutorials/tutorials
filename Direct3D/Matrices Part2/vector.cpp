#include "vector.h"

// Checks for two floats to be equal within a certain tolerance
inline bool ToleranceEqual(float a, float b, float t)
{
	return ((a > b - t) && (a < b + t));
}

// Assignment operator
CVector& CVector::operator =(const CVector &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
		return *this;
}

// Dot product
float CVector::operator *(const CVector &vec) const
{
	return (x * vec.x + y * vec.y + z * vec.z);
}

// Set the CVector to the (x,y,z) passed in
void CVector::set(float xxx, float yyy, float zzz)
{
	x = xxx;
	y = yyy;
	z = zzz;
}

// Set the CVector to the CVector passed in
void CVector::set(const CVector &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

// Normalizes the CVector (ie makes it a unit vector)
void CVector::normalize()
{
	if(ToleranceEqual(magnitude(), 0.0f, .001f))
		return; // Can't normalize a vector of zero magnitude
	
	float oneOverLen = 1.0f / magnitude();
	
	x *= oneOverLen;
	y *= oneOverLen;
	z *= oneOverLen;
}

// Returns the magnitude (ie length or norm) of the CVector
float CVector::magnitude() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

// Crosses "this" vector with "vec" -- Stores the cross product in "result"
void CVector::cross(const CVector &vec, CVector &result) const
{
	result.x = (y * vec.z) - (vec.y * z);
	result.y = (z * vec.x) - (vec.z * x);
	result.z = (x * vec.y) - (vec.x * y);
}

// Crosses "this" vector with "vec" and returns the resulting CVector
CVector CVector::cross(const CVector &vec) const
{
	return CVector((y * vec.z) - (vec.y * z), 
				   (z * vec.x) - (vec.z * x),
				   (x * vec.y) - (vec.x * y));
}
