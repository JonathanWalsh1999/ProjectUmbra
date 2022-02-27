#ifndef _ICAMERA_H_
#define _ICAMERA_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class encapsulating a camera
//--------------------------------------------------------------------------------------
// Holds position, rotation, near/far clip and field of view. These to a view and projection matrices as required

#include "Input.hpp"
#include "Common.hpp"

//======================================================================================
namespace umbra_engine
{

class ICamera
{
public:
//---------------------------------------
// Destructor - Can't construct an Interface
//---------------------------------------
	virtual ~ICamera() {}

//---------------------------------------
// Data access
//---------------------------------------

	// Getters / setters
	virtual maths::CVector3 Position() = 0;
	virtual maths::CVector3 Rotation() = 0;
	virtual void SetPosition(maths::CVector3 position) = 0;
	virtual void SetRotation(maths::CVector3 rotation) = 0;

	virtual float FOV() = 0;
	virtual float NearClip() = 0;
	virtual float FarClip() = 0;

	virtual void SetFOV(float fov) = 0;
	virtual void SetNearClip(float nearClip) = 0;
	virtual void SetFarClip(float farClip) = 0;

	// Read only access to camera matrices, updated on request from position, rotation and camera settings
	virtual maths::CMatrix4x4 ViewMatrix() = 0;
	virtual maths::CMatrix4x4 ProjectionMatrix() = 0;
	virtual maths::CMatrix4x4 ViewProjectionMatrix() = 0;
	virtual maths::CMatrix4x4 WorldMatrix() = 0;

//---------------------------------------
// Operational Methods
//---------------------------------------
	// Control the camera's position and rotation using keys provided
	virtual void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight) = 0;

};//Class
}//Namespace
//======================================================================================
#endif//Header guard

