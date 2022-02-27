//--------------------------------------------------------------------------------------
// Vector3 class (cut down version), to hold points and vectors
//--------------------------------------------------------------------------------------

#include "CVector4.hpp"
#include "MathHelpers.hpp"

namespace umbra_engine
{
namespace maths
{
/*-----------------------------------------------------------------------------------------
	Operators
-----------------------------------------------------------------------------------------*/

// Addition of another vector to this one, e.g. Position += Velocity
CVector4& CVector4::operator+= (const CVector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

// Subtraction of another vector from this one, e.g. Velocity -= Gravity
CVector4& CVector4::operator-= (const CVector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

// Negate this vector (e.g. Velocity = -Velocity)
CVector4& CVector4::operator- ()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

// Plus sign in front of vector - called unary positive and usually does nothing. Included for completeness (e.g. Velocity = +Velocity)
CVector4& CVector4::operator+ ()
{
	return *this;
}


// Multiply vector by scalar (scales vector);
CVector4& CVector4::operator*= (const float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}


// Vector-vector addition
CVector4 operator+ (const CVector4& v, const CVector4& w)
{
	return CVector4{ v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w };
}

// Vector-vector subtraction
CVector4 operator- (const CVector4& v, const CVector4& w)
{
	return CVector4{ v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w };
}

// Vector-scalar multiplication
CVector4 operator* (const CVector4& v, float s)
{
	return CVector4{ v.x * s, v.y * s, v.z * s, v.w * s };
}
CVector4 operator* (float s, const CVector4& v)
{
	return CVector4{ v.x * s, v.y * s, v.z * s, v.w * s };
}

/*-----------------------------------------------------------------------------------------
	Non-member functions
-----------------------------------------------------------------------------------------*/

// Dot product of two given vectors (order not important) - non-member version
float Dot(const CVector4& v1, const CVector4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Cross product of two given vectors (order is important) - non-member version
//CVector3 Cross(const CVector4& v1, const CVector4& v2)
//{
//	return CVector4{ v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
//}

// Return unit length vector in the same direction as given one
CVector4 Normalise(const CVector4& v)
{
	float lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if (IsZero(lengthSq))
	{
		return CVector4{ 0.0f, 0.0f, 0.0f, 0.0f };
	}
	else
	{
		float invLength = InvSqrt(lengthSq);
		return CVector4{ v.x * invLength, v.y * invLength, v.z * invLength, v.w * invLength };
	}
}


// Returns length of a vector
float Length(const CVector4& v)
{
	return sqrt(Dot(v, v));
}

} } //Namespaces