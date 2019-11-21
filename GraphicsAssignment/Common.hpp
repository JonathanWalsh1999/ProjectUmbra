//--------------------------------------------------------------------------------------
// Commonly used definitions across entire project
//--------------------------------------------------------------------------------------
#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#include <windows.h>
#include <d3d11.h>
#include <vector>


#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

#include "CVector4.hpp"
#include "CVector3.hpp"
#include "CVector2.hpp"

#include "CMatrix4x4.hpp"



	enum EBlendingType { None, Add, Multi, Alpha };
	enum EShadowEffect { ZBuffer, PCF};

	// Viewport size
	const int gViewportWidth = 1920;
	const int gViewportHeight = 1080;

	// Input constants
	const float ROTATION_SPEED = 1.0f;
	const float MOVEMENT_SPEED = 50.0f;

	//--------------------------------------------------------------------------------------
	// Constant Buffers
	//--------------------------------------------------------------------------------------
	// Variables sent over to the GPU each frame

	// Data that remains constant for an entire frame, updated from C++ to the GPU shaders *once per frame*
	// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
	// we have finished updating the scene. There is a structure in the shader code that exactly matches this one
	struct PerFrameConstants
	{	
		const static int lightAmount = 2;//statics work differently therefore padding isn't neccessary here

		// These are the matrices used to position the camera
		CMatrix4x4 viewMatrix;
		CMatrix4x4 projectionMatrix;
		CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects

		//**Individual light variables will probably become obsolete due to the addition of the arrays at the bottom. Delete CAREFULLY when ready - Ensure in blocks of 4
		CVector4   light1Position; // 3 floats: x, y z
								  // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
								  // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
								  // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
		CVector4   light1Colour;

		CVector4   light2Position;

		CVector3      padding3;
		float gParallaxDepth;

		CVector3   light2Colour;
		float blendAmount;
		//**

		CVector3   ambientColour;
		float      specularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

		CVector3   cameraPosition;	
		int shadowEffect;//Each shadow effect will have a number assigned to them, so that it will be easy to change on demand in C++. e.g. z-buffer = 0 pcf = 1 etc.

		CVector3 lightFacings;
		float lightCosHalfAngles;

		CMatrix4x4 lightViewMatrix;
		CMatrix4x4 lightProjectionMatrix;



		//Had to implement a CVector4 based on CVector3 as shaders only like getting stuff in chunks of 4 (16 bytes - 4 per var e.g. float = 4 bytes * 4 = 16)
		CVector4 lightPositions[lightAmount];

		CVector4 lightColours[lightAmount];


	};


	// This is the matrix that positions the next thing to be rendered in the scene. Unlike the structure above this data can be
	// updated and sent to the GPU several times every frame (once per model). However, apart from that it works in the same way.
	struct PerModelConstants
	{
		CMatrix4x4 worldMatrix;
		CVector4   objectColour; // Allows each light model to be tinted to match the light colour they cast
		//float      padding6;
	};

	//temp globals
	//extern PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above
	//extern ID3D11Buffer*     mPerFrameConstantBuffer; // This variable controls the GPU-side constant buffer matching to the above structure

	//extern PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
	//extern ID3D11Buffer*     mPerModelConstantBuffer;  // This variable controls the GPU-side constant buffer related to the above structure

#endif //_COMMON_H_INCLUDED_
