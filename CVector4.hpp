#pragma once
//--------------------------------------------------------------------------------------
// Vector3 class (cut down version), to hold points and vectors
//--------------------------------------------------------------------------------------
// Code in .cpp file

#ifndef _CVECTOR4_H_DEFINED_
#define _CVECTOR4_H_DEFINED_


#include <cmath>
namespace umbra_engine
{
namespace maths
{

class CVector4
{
	// Concrete class - public access
public:
	// Vector components
	float x;
	float y;
	float z;
	float w;

	/*-----------------------------------------------------------------------------------------
		Constructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CVector4() {}

	// Construct with 3 values
	CVector4(const float xIn, const float yIn, const float zIn, const float wIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
		w = wIn;
	}

	// Construct using a pointer to four floats
	CVector4(const float* pfElts)
	{
		x = pfElts[0];
		y = pfElts[1];
		z = pfElts[2];
		w = pfElts[3];
	}


	/*-----------------------------------------------------------------------------------------
		Member functions
	-----------------------------------------------------------------------------------------*/

	// Addition of another vector to this one, e.g. Position += Velocity
	CVector4& operator+= (const CVector4& v);

	// Subtraction of another vector from this one, e.g. Velocity -= Gravity
	CVector4& operator-= (const CVector4& v);

	// Negate this vector (e.g. Velocity = -Velocity)
	CVector4& operator- ();

	// Plus sign in front of vector - called unary positive and usually does nothing. Included for completeness (e.g. Velocity = +Velocity)
	CVector4& operator+ ();

	// Multiply vector by scalar (scales vector);
	CVector4& operator*= (const float s);
};


/*-----------------------------------------------------------------------------------------
	Non-member operators
-----------------------------------------------------------------------------------------*/

// Vector-vector addition
CVector4 operator+ (const CVector4& v, const CVector4& w);

// Vector-vector subtraction
CVector4 operator- (const CVector4& v, const CVector4& w);

// Vector-scalar multiplication
CVector4 operator* (const CVector4& v, float s);
CVector4 operator* (float s, const CVector4& v);

/*-----------------------------------------------------------------------------------------
	Non-member functions
-----------------------------------------------------------------------------------------*/

// Dot product of two given vectors (order not important) - non-member version
float Dot(const CVector4& v1, const CVector4& v2);

// Cross product of two given vectors (order is important) - non-member version
//CVector4 Cross(const CVector4& v1, const CVector4& v2);

// Return unit length vector in the same direction as given one
CVector4 Normalise(const CVector4& v);

// Returns length of a vector
float Length(const CVector4& v);

} }//Namespaces
#endif // _CVECTOR3_H_DEFINED_
