#ifndef _LIGHT_H_
#define _LIGHT_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class that manages Spot and directional lights
//--------------------------------------------------------------------------------------

#include "ILight.hpp"

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class IScene;
class IEngine;

class Light : public ILight
{
public:
//---------------------------------------
// Constructor / Destructor
//---------------------------------------
	Light(IEngine * engine, ELightType light);
	~Light();

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	IMesh* GetMesh();
	IModel* GetModel();
	maths::CVector4 GetPosition();
	maths::CVector4 GetColour();
	float GetSpecularPower();
	maths::CVector3 GetAmbientColour();
	float GetLightStrength();
	int GetLightNumber();

	//Setters
	void SetPosition(const maths::CVector4& Pos);
	void SetLightColour(const maths::CVector4& newColour);
	void SetSpecularPower(const float& newSpecularPower);
	void SetAmbientColour(const maths::CVector3& newAmbientColour);
	void SetLightStrength(const float& newLightStrength);
	void SetLightNumber(const int& newLightNumber);
	void SetLightAngle(const float& angle);
	void SetMesh(IMesh* newMesh);
	void SetModel(IModel* newModel);

//---------------------------------------
// Operational Methods
//---------------------------------------
	void RenderLight(PerFrameConstants& perFrameConstants, PerModelConstants& perModelConstants);
	bool ShadowDepthBuffer();
	void ClearDepthStencil(ID3D11DeviceContext* context);
	void SendShadowMap2Shader(int textureSlot, ID3D11DeviceContext* context);

private:
//---------------------------------------
// Private Member Variables
//---------------------------------------
	ID3D11Device* mDevice;
	IEngine * myEngine;
	IScene* myScene;

	IModel* lightModel;
	IMesh* lightMesh;

	maths::CVector4 mLightPosition{ 0,0,0,0 };
	maths::CVector4 mLightColour{ 0,0,0,0 };
	float mSpecularPower = 0;
	maths::CVector3 mAmbientColour{ 0,0,0 };
	float mLightStrength = 0;

	int mLightIndex;
	ELightType mLightType;
	float coneAngle;

	int mTextureSlot;


	ID3D11PixelShader* mPSShader = nullptr;
	ID3D11VertexShader* mVSShader = nullptr;

	//shadow
	ID3D11Texture2D* mShadow = nullptr;
	ID3D11DepthStencilView* mShadowDepthStencil = nullptr;
	ID3D11ShaderResourceView* mShadowSRV = nullptr;
	ID3D11SamplerState* mPointSampler = nullptr;
	ID3D11RenderTargetView* mShadowRenderTarget = nullptr;

};//Class
}//Namespace
//======================================================================================
#endif//Header Guard