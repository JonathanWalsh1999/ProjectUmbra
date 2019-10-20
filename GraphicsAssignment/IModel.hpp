#pragma once

#include "Common.hpp"
#include "CVector3.hpp"
#include "CMatrix4x4.hpp"
#include "Input.hpp"
#include "Camera.hpp"

class Mesh;
class IEngine;
class IScene;

class IModel
{
public:
	virtual ~IModel() {}

	// The render function sets the world matrix in the per-frame constant buffer and makes that buffer available
// to vertex & pixel shader. Then it calls Mesh:Render, which renders the geometry with current GPU settings.
// So all other per-frame constants must have been set already along with shaders, textures, samplers, states etc.
	virtual void Render() = 0;


	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	virtual void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward) = 0;

	//-------------------------------------
	// Data access
	//-------------------------------------

	// Getters / setters
	virtual CVector3 Position() = 0;
	virtual CVector3 Rotation() = 0;
	virtual CVector3 Scale() = 0;
	virtual CMatrix4x4 GetMatrix() = 0;

	virtual void SetMatrix(CMatrix4x4 model) = 0;
	virtual void SetPosition(CVector3 position) = 0;
	virtual void SetRotation(CVector3 rotation) = 0;

	// Two ways to set scale: x,y,z separately, or all to the same value
	virtual void SetScale(CVector3 scale) = 0;
	virtual void SetScale(float scale) = 0;

	// Read only access to model world matrix, updated on request
	virtual CMatrix4x4 WorldMatrix() = 0;

	virtual void LookAt(IModel* target) = 0;
	virtual void LookAtCamera(ICamera * target) = 0;

	virtual void MoveLocalX(float speed) = 0;
	virtual void MoveLocalY(float speed) = 0;
	virtual void MoveLocalZ(float speed) = 0;

	virtual void MoveX(float speed) = 0;
	virtual void MoveY(float speed) = 0;
	virtual void MoveZ(float speed) = 0;
	virtual void Move(float x, float y, float z) = 0;

	virtual void SetX(float pos) = 0;
	virtual void SetY(float pos) = 0;
	virtual void SetZ(float pos) = 0;

	virtual float GetX() = 0;
	virtual float GetY() = 0;
	virtual float GetZ() = 0;

	virtual void RotateX(float angle) = 0;
	virtual void RotateY(float angle) = 0;
	virtual void RotateZ(float angle) = 0;

	virtual void SetSkin(const std::string& colour) = 0;
	virtual void SetDiffuseSRVMap(ID3D11ShaderResourceView* newDiffuseSpecularMapSRV) = 0;
	virtual void SetDiffuseMap(ID3D11Resource* newDiffuseSpecularMap) = 0;

	virtual void SetTextureFile(const std::string& file) = 0;

	virtual void SetPSShader(const std::string& shaderFile) = 0;
	virtual void SetVSShader(const std::string& shaderFile) = 0;
	virtual void SetAddBlend(const EBlendingType& newBlend) = 0;

	virtual ID3D11PixelShader* GetPSShader() = 0;
	virtual ID3D11VertexShader* GetVSShader() = 0;
	virtual ID3D11ShaderResourceView* GetDiffuseSRVMap() = 0;
	virtual ID3D11Resource* GetDiffuseMap() = 0;
	virtual std::string GetTextureFile() = 0;
	virtual ID3D11ShaderResourceView* GetDiffuseSRVMap2() = 0;
	virtual ID3D11Resource* GetDiffuseMap2() = 0;
	virtual std::string GetTextureFile2() = 0;
	virtual EBlendingType GetAddBlend() = 0;

	virtual void AddSecondaryTexture(const std::string& texture2) = 0;
};