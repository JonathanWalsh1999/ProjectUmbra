#pragma once

//--------------------------------------------------------------------------------------
// Class encapsulating a camera
//--------------------------------------------------------------------------------------
// Holds position, rotation, near/far clip and field of view. These to a view and projection matrices as required
#include "Input.hpp"

class ICamera
{
public:
	virtual ~ICamera() {}

	// Control the camera's position and rotation using keys provided
	virtual void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight) = 0;

	//-------------------------------------
	// Data access
	//-------------------------------------

	// Getters / setters
	virtual CVector3 Position() = 0;
	virtual CVector3 Rotation() = 0;
	virtual void SetPosition(CVector3 position) = 0;
	virtual void SetRotation(CVector3 rotation) = 0;

	virtual float FOV() = 0;
	virtual float NearClip() = 0;
	virtual float FarClip() = 0;

	virtual void SetFOV(float fov) = 0;
	virtual void SetNearClip(float nearClip) = 0;
	virtual void SetFarClip(float farClip) = 0;

	// Read only access to camera matrices, updated on request from position, rotation and camera settings
	virtual CMatrix4x4 ViewMatrix() = 0;
	virtual CMatrix4x4 ProjectionMatrix() = 0;
	virtual CMatrix4x4 ViewProjectionMatrix() = 0;
};



