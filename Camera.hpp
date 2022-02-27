#ifndef _CAMERA_H_
#define _CAMERA_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class encapsulating a camera
// Holds position, rotation, near/far clip and field of view. These to a view and projection matrices as required
//--------------------------------------------------------------------------------------

#include "ICamera.hpp"
#include "CVector3.hpp"
#include "CMatrix4x4.hpp"

//======================================================================================
namespace umbra_engine
{

class CCamera : public ICamera
{
public:
//-------------------------------------
// Constructor / Destructor
//-------------------------------------
	// Constructor - initialise all settings, sensible defaults provided for everything.
	CCamera(maths::CVector3 position = { 0,0,0 }, maths::CVector3 rotation = { 0,0,0 },
		float fov = maths::PI / 3, float aspectRatio = 4.0f / 3.0f, float nearClip = 0.1f, float farClip = 10000.0f)
		: mPosition(position), mRotation(rotation), mFOVx(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip)
	{
	}
	~CCamera() = default;

//---------------------------------------
// Data access
//---------------------------------------
	// Getters
	maths::CVector3 Position() { return mPosition; }
	maths::CVector3 Rotation() { return mRotation; }
	float FOV() { return mFOVx; }
	float NearClip() { return mNearClip; }
	float FarClip() { return mFarClip; }
	// Read only access to camera matrices, updated on request from position, rotation and camera settings
	maths::CMatrix4x4 ViewMatrix() { UpdateMatrices(); return mViewMatrix; }
	maths::CMatrix4x4 ProjectionMatrix() { UpdateMatrices(); return mProjectionMatrix; }
	maths::CMatrix4x4 ViewProjectionMatrix() { UpdateMatrices(); return mViewProjectionMatrix; }
	maths::CMatrix4x4 WorldMatrix() { UpdateMatrices(); return mWorldMatrix; }

	//Setters
	void SetPosition(maths::CVector3 position) { mPosition = position; }
	void SetRotation(maths::CVector3 rotation) { mRotation = rotation; }
	void SetFOV(float fov) { mFOVx = fov; }
	void SetNearClip(float nearClip) { mNearClip = nearClip; }
	void SetFarClip(float farClip) { mFarClip = farClip; }

//---------------------------------------
// Operational Methods
//---------------------------------------
	// Control the camera's position and rotation using keys provided
	void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight);
private:
//---------------------------------------
// Private Member Functions
//---------------------------------------
	// Update the matrices used for the camera in the rendering pipeline
	void UpdateMatrices();

//---------------------------------------
// Private members
//---------------------------------------

	// Postition and rotations for the camera (rarely scale cameras)
	maths::CVector3 mPosition;
	maths::CVector3 mRotation;

	// Camera settings: field of view, aspect ratio, near and far clip plane distances.
	// Note that the FOVx angle is measured in radians (radians = degrees * PI/180) from left to right of screen
	float mFOVx;
	float mAspectRatio;
	float mNearClip;
	float mFarClip;

	// Current view, projection and combined view-projection matrices (DirectX matrix type)
	maths::CMatrix4x4 mWorldMatrix; // Easiest to treat the camera like a model and give it a "world" matrix...
	maths::CMatrix4x4 mViewMatrix;  // ...then the view matrix used in the shaders is the inverse of its world matrix

	maths::CMatrix4x4 mProjectionMatrix;     // Projection matrix holds the field of view and near/far clip distances
	maths::CMatrix4x4 mViewProjectionMatrix; // Combine (multiply) the view and projection matrices together, which
									  // can sometimes save a matrix multiply in the shader (optional)

	float mRun = 10.0f;
};//Class


//---------------------------------------
// Camera Helpers
//---------------------------------------

// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
maths::CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 4.0f / 3.0f, float FOVx = maths::ToRadians(60),
	float nearClip = 0.1f, float farClip = 10000.0f);
}//Namespace
//======================================================================================
#endif //Header guard
