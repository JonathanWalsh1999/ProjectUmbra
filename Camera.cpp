//--------------------------------------------------------------------------------------
// Class encapsulating a camera
//--------------------------------------------------------------------------------------
// Holds position, rotation, near/far clip and field of view. These to a view and projection matrices as required

#include "Camera.hpp"

namespace umbra_engine
{

// Control the camera's position and rotation using keys provided
void CCamera::Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
	KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight)
{
	static bool activateMouse = false;

	//Toggle on/off FPS camera movement with the mouse
	if (KeyHit(Key_M))
	{
		if (activateMouse)
		{
			ShowCursor(true);
			activateMouse = false;
		}
		else
		{
			ShowCursor(false);
			activateMouse = true;

		}
	}

	//Work out boundaries, so the camera knows where to move depending on the direction the mouse moves
	if (activateMouse)
	{
		SetCursorPos(gViewportWidth / 2, gViewportHeight / 2);
		if (GetMouseX() > gViewportWidth / 2 - 5)
		{
			mRotation.y += ROTATION_SPEED * frameTime * 2.0f; // Use of frameTime to ensure same speed on different machines
		}
		if (GetMouseX() < gViewportWidth / 2 - 10)
		{
			mRotation.y -= ROTATION_SPEED * frameTime * 2.0f; // Use of frameTime to ensure same speed on different machines
		}
		if (GetMouseY() > gViewportHeight / 2 - 30)
		{
			mRotation.x += ROTATION_SPEED * frameTime * 2.0f; // Use of frameTime to ensure same speed on different machines
		}
		if (GetMouseY() < gViewportHeight / 2 - 32)
		{
			mRotation.x -= ROTATION_SPEED * frameTime * 2.0f; // Use of frameTime to ensure same speed on different machines
		}
	}


	//**** ROTATION ****
	if (KeyHeld(turnDown))
	{
		mRotation.x += ROTATION_SPEED * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(turnUp))
	{
		mRotation.x -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnRight))
	{
		mRotation.y += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnLeft))
	{
		mRotation.y -= ROTATION_SPEED * frameTime;
	}



	//**** LOCAL MOVEMENT ****
	if (KeyHeld(moveRight))
	{
		mPosition.x += MOVEMENT_SPEED * frameTime * mWorldMatrix.e00 * mRun;//TO DO: Implement run mode
		mPosition.y += MOVEMENT_SPEED * frameTime * mWorldMatrix.e01 * mRun;
		mPosition.z += MOVEMENT_SPEED * frameTime * mWorldMatrix.e02 * mRun;
	}
	if (KeyHeld(moveLeft))
	{
		mPosition.x -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e00 * mRun;
		mPosition.y -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e01 * mRun;
		mPosition.z -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e02 * mRun;
	}
	if (KeyHeld(moveForward))
	{
		mPosition.x += MOVEMENT_SPEED * frameTime * mWorldMatrix.e20 * mRun;
		mPosition.y += MOVEMENT_SPEED * frameTime * mWorldMatrix.e21 * mRun;
		mPosition.z += MOVEMENT_SPEED * frameTime * mWorldMatrix.e22 * mRun;
	}
	if (KeyHeld(moveBackward))
	{
		mPosition.x -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e20 * mRun;
		mPosition.y -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e21 * mRun;
		mPosition.z -= MOVEMENT_SPEED * frameTime * mWorldMatrix.e22 * mRun;
	}
}

// Update the matrices used for the camera in the rendering pipeline
void CCamera::UpdateMatrices()
{
	// "World" matrix for the camera - treat it like a model at first
	mWorldMatrix = maths::MatrixRotationZ(mRotation.z) * maths::MatrixRotationX(mRotation.x) * maths::MatrixRotationY(mRotation.y) * MatrixTranslation(mPosition);

	// View matrix is the usual matrix used for the camera in shaders, it is the inverse of the world matrix (see lectures)
	mViewMatrix = InverseAffine(mWorldMatrix);

	// Projection matrix, how to flatten the 3D world onto the screen (needs field of view, near and far clip, aspect ratio)
	float tanFOVx = std::tan(mFOVx * 0.5f);
	float scaleX = 1.0f / tanFOVx;
	float scaleY = mAspectRatio / tanFOVx;
	float scaleZa = mFarClip / (mFarClip - mNearClip);
	float scaleZb = -mNearClip * scaleZa;

	mProjectionMatrix = maths::CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
									  0.0f, scaleY,    0.0f,   0.0f,
									  0.0f,   0.0f, scaleZa,   1.0f,
									  0.0f,   0.0f, scaleZb,   0.0f };

	// The view-projection matrix combines the two matrices usually used for the camera into one, which can save a multiply in the shaders (optional)
	mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;
}


maths::CMatrix4x4 MakeProjectionMatrix(float aspectRatio /*= 4.0f / 3.0f*/, float FOVx /*= maths::ToRadians(60)*/,
	float nearClip /*= 0.1f*/, float farClip /*= 10000.0f*/)
{
	float tanFOVx = std::tan(FOVx * 0.5f);
	float scaleX = 1.0f / tanFOVx;
	float scaleY = aspectRatio / tanFOVx;
	float scaleZa = farClip / (farClip - nearClip);
	float scaleZb = -nearClip * scaleZa;

	return maths::CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
						 0.0f, scaleY,    0.0f,   0.0f,
						 0.0f,   0.0f, scaleZa,   1.0f,
						 0.0f,   0.0f, scaleZb,   0.0f };
}

}//Namespace

