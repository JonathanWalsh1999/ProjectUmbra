//--------------------------------------------------------------------------------------
// Math convenience functions
//--------------------------------------------------------------------------------------

#ifndef _MATH_HELPERS_H_
#define _MATH_HELPERS_H_

#include <cmath>
#include "CMatrix4x4.hpp"

namespace umbra_engine
{
namespace maths
{
// Surprisingly, pi is not *officially* defined anywhere in C++
const float PI = 3.14159265359f;



// Test if a float value is approximately 0
// Epsilon value is the range around zero that is considered equal to zero
const float EPSILON = 0.5e-6f; // For 32-bit floats, requires zero to 6 decimal places
inline bool IsZero(const float x)
{
	return std::abs(x) < EPSILON;
}


// 1 / Sqrt. Used often (e.g. normalising) and can be optimised, so it gets its own function
inline float InvSqrt(const float x)
{
	return 1.0f / std::sqrt(x);
}


// Pass an angle in degrees, returns the angle in radians
inline float ToRadians(float d)
{
	return  d * PI / 180.0f;
}

// Pass an angle in radians, returns the angle in degrees
inline float ToDegrees(float r)
{
	return  r * 180.0f / PI;
}

inline float Distance(const CVector3& pos1, const CVector3& pos2)
{
	float distance = std::sqrt(pow(pos1.x - pos2.x, 2) +
							   pow(pos1.y - pos2.y, 2) +
							   pow(pos1.z - pos2.z, 2));

	return distance;
}

// Can only return up to RAND_MAX different values, spread evenly across the given range
// RAND_MAX is defined in stdlib.h and is compiler-specific (32767 on VS-2005, higher elsewhere)
inline float Random(const float a, const float b)
{
	return a + (b - a) * (static_cast<float>(rand()) / RAND_MAX);
}
} }//Namespaces
#endif // _MATH_HELPERS_H_
