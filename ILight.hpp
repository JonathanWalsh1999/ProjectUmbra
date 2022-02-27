#ifndef _ILIGHT_H
#define _ILIGHT_H
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation of Lighting in DirectX
//--------------------------------------------------------------------------------------

#include "Common.hpp"
#include "Shader.hpp"

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declaration
//---------------------------------------
class CScene;
class IEngine;
class IModel;
class IMesh;

class ILight
{
public:
//---------------------------------------
// Destructor - no constructor
//---------------------------------------
	virtual ~ILight() {}

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	virtual IMesh* GetMesh() = 0;
	virtual IModel* GetModel() = 0;
	virtual maths::CVector4 GetPosition() = 0;
	virtual maths::CVector4 GetColour() = 0;
	virtual float GetSpecularPower() = 0;
	virtual maths::CVector3 GetAmbientColour() = 0;
	virtual float GetLightStrength() = 0;
	virtual int GetLightNumber() = 0;

	//Setters
	virtual void SetPosition(const maths::CVector4& Pos) = 0;
	virtual void SetLightColour(const maths::CVector4& newColour) = 0;
	virtual void SetSpecularPower(const float& newSpecularPower) = 0;
	virtual void SetAmbientColour(const maths::CVector3& newAmbientColour) = 0;
	virtual void SetLightStrength(const float& newLightStrength) = 0;
	virtual void SetLightNumber(const int& newLightNumber) = 0;
	virtual void SetLightAngle(const float& angle) = 0;
	virtual void SetMesh(IMesh* newMesh) = 0;
	virtual void SetModel(IModel* newModel) = 0;

//---------------------------------------
// Operational Methods
//---------------------------------------
	virtual void RenderLight(PerFrameConstants& perFrameConstants, PerModelConstants& perModelConstants) = 0;
	virtual bool ShadowDepthBuffer() = 0;
	virtual void ClearDepthStencil(ID3D11DeviceContext* context) = 0;
	virtual void SendShadowMap2Shader(int textureSlot, ID3D11DeviceContext* context) = 0;
	virtual void ConstructCubeFaceCameras(maths::CVector3 lightPosition) {}
	virtual void RenderCubeMap() {}

	//Get light count - owned by class not object
	static int mLightCount;
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard