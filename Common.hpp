#ifndef _COMMON_H_
#define _COMMON_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Commonly used definitions across entire project
//--------------------------------------------------------------------------------------

#include "CVector4.hpp"
#include "CVector3.hpp"
#include "CVector2.hpp"

#include "CMatrix4x4.hpp"
#include "MathHelpers.hpp"

#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <time.h>
#include <windows.h>

#include <atlbase.h> // C-std::string to unicode conversion function CA2CT
#include <vector>
#include <string>
#include <memory>
#include <stdlib.h>

//======================================================================================
namespace umbra_engine
{

//---------------------------------------
// Enum Variables
//---------------------------------------
enum EBlendingType { None, Add, Multi, Alpha };
enum EShadowEffect { ZBuffer, PCF };
enum class ETextureTypes { Diffuse, Normal, Height, Unknown };

enum ELightType
{
	Point,
	Spot,
	Directional,
	Area
};

//---------------------------------------
// Structures
//---------------------------------------
struct STextureMap
{
	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* textureSRV = nullptr;
};

//---------------------------------------
// Constant Variables
//---------------------------------------

// Viewport size
const int gViewportWidth = 1920;
const int gViewportHeight = 1080;

// Input constants
const float ROTATION_SPEED = 1.0f;
const float MOVEMENT_SPEED = 15.0f;

//---------------------------------------
// Constant Buffers
//---------------------------------------
// Variables sent over to the GPU each frame

// Data that remains constant for an entire frame, updated from C++ to the GPU shaders *once per frame*
// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
// we have finished updating the scene. There is a structure in the shader code that exactly matches this one
struct PerFrameConstants
{
	const static int MAX_LIGHTS = 20;//statics work differently therefore padding isn't neccessary here

	// These are the matrices used to position the camera
	maths::CMatrix4x4 viewMatrix;
	maths::CMatrix4x4 projectionMatrix;
	maths::CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects

	//**Individual light variables will probably become obsolete due to the addition of the arrays at the bottom. Delete CAREFULLY when ready - Ensure in blocks of 4
	maths::CVector4   light1Position; // 3 floats: x, y z
							  // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
							  // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
							  // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
	maths::CVector4   light1Colour;

	maths::CVector4   light2Position;

	float     padding3;
	float dayNightCycle;
	float gParallaxDepth;
	int lightCount;

	maths::CVector3   light2Colour;
	float blendAmount;


	maths::CVector3   ambientColour;
	float      specularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

	maths::CVector3   cameraPosition;
	int shadowEffect;//Each shadow effect will have a number assigned to them, so that it will be easy to change on demand in C++. e.g. z-buffer = 0 pcf = 1 etc.

	maths::CVector4 lightFacings[MAX_LIGHTS];

	maths::CMatrix4x4 lightViewMatrix[MAX_LIGHTS];

	maths::CMatrix4x4 lightProjectionMatrix[MAX_LIGHTS];


	//Had to implement a CVector4 based on CVector3 as shaders only like getting stuff in chunks of 4 (16 bytes - 4 per var e.g. float = 4 bytes * 4 = 16)
	maths::CVector4 lightPositions[MAX_LIGHTS];

	maths::CVector4 lightColours[MAX_LIGHTS];

	maths::CMatrix4x4 cubeViewProj[6];

	float viewportWidth;
	float viewportHeight;
	maths::CVector2 padding;

	maths::CMatrix4x4 cameraMatrix;
};//Structure

static const int MAX_BONES = 64;

// This is the matrix that positions the next thing to be rendered in the scene. Unlike the structure above this data can be
// updated and sent to the GPU several times every frame (once per model). However, apart from that it works in the same way.
struct PerModelConstants
{
	maths::CMatrix4x4 worldMatrix;
	maths::CVector4   objectColour; // Allows each light model to be tinted to match the light colour they cast


	maths::CMatrix4x4 boneMatrices[MAX_BONES];

};//Structure
}//Namespace
//======================================================================================
#endif //Header Guard
