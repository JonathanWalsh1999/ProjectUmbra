#pragma once

#include "Common.hpp"
#include "Shader.hpp"

class CScene;
class IEngine;
class IModel;
class IMesh;

class ILight 
{
public:
	virtual ~ILight() {}

	virtual IMesh* GetMesh() = 0;
	virtual IModel* GetModel() = 0;
	virtual CVector4 GetPosition() = 0;
	virtual CVector4 GetColour() = 0;
	virtual float GetSpecularPower() = 0;
	virtual CVector3 GetAmbientColour() = 0;
	virtual float GetLightStrength() = 0;
	virtual int GetLightNumber() = 0;

	virtual void SetPosition(const CVector4& newPos) = 0;
	virtual void SetLightColour(const CVector4& newColour) = 0;
	virtual void SetSpecularPower(const float& newSpecularPower) = 0;
	virtual void SetAmbientColour(const CVector3& newAmbientColour) = 0;
	virtual void SetLightStrength(const float& newLightStrength) = 0;
	virtual void SetLightNumber(const int& newLightNumber) = 0;
	virtual void SetLightFacing(const CVector3& localZ) = 0;
	virtual void SetLightAngle(const float& angle) = 0;
	virtual void SetMesh(IMesh* newMesh) = 0;
	virtual void SetModel(IModel* newModel) = 0;

	virtual void RenderLight() = 0;
	virtual void RenderDepthBufferFromLight(std::vector<IModel*> allShadowModels) = 0;

};