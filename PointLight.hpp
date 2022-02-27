#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for Point lights
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
class ICamera;

class CPointLight : public ILight
{
public:

//---------------------------------------
// Constructors / Destructor
//---------------------------------------
	CPointLight(IEngine* engine, ELightType light);
	~CPointLight();

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
	void ConstructCubeFaceCameras(maths::CVector3 lightPosition);
	void RenderCubeMap();

private:
//---------------------------------------
// Private Member Methods
//---------------------------------------
	void GetCubeViewProjection();

//---------------------------------------
// Private Member Variables
//---------------------------------------
	static int mPointLightCount;
	IEngine* myEngine;
	IScene* myScene;
	IModel* lightModel;
	IMesh* lightMesh;
	ID3D11Device* mDevice;
	maths::CVector4 mLightPosition{ 0,0,0,0 };
	maths::CVector4 mLightColour{ 0,0,0,0 };
	float mSpecularPower = 0;
	maths::CVector3 mAmbientColour{ 0,0,0 };
	float mLightStrength = 0;
	int mLightIndex;
	int mPointLightIndex;
	ELightType mLightType;
	float coneAngle;
	ID3D11PixelShader* mPSShader = nullptr;
	ID3D11VertexShader* mVSShader = nullptr;
	ID3D11GeometryShader* mGSShader = nullptr;
	//shadow
	ID3D11Texture2D* mCubeShadow = nullptr;
	ID3D11DepthStencilView* mShadowDepthStencil = nullptr;
	ID3D11ShaderResourceView* mShadowSRV = nullptr;
	ID3D11SamplerState* mPointSampler = nullptr;
	std::vector<maths::CMatrix4x4> lightViewMatrix;
	std::vector<maths::CMatrix4x4> lightProjectionMatrix;
	std::vector<IModel*> mLightModels;
	std::vector<ICamera*> mCubeMapCameras;
	D3D11_VIEWPORT mCubeMapViewport;
	ID3D11RenderTargetView* mCubeMapRTV[6];//Create render target for each face
	maths::CMatrix4x4 mCubeViewProj[6];
};//Class
}//Namespace
#endif//Header Guard