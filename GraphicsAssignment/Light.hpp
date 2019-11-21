#pragma once
#include "ILight.hpp"

class IScene;
class IEngine;
class Model;
class Mesh;

class Light : public ILight
{
public:
	Light(IEngine * engine);
	~Light();

	IMesh* GetMesh();
	IModel* GetModel();
	CVector4 GetPosition();
	CVector4 GetColour();
	float GetSpecularPower();
	CVector3 GetAmbientColour();
	float GetLightStrength();
	int GetLightNumber();

	void SetPosition(const CVector4& newPos);
	void SetLightColour(const CVector4& newColour);
	void SetSpecularPower(const float& newSpecularPower);
	void SetAmbientColour(const CVector3& newAmbientColour);
	void SetLightStrength(const float& newLightStrength);
	void SetLightNumber(const int& newLightNumber);
	void SetLightFacing(const CVector3& localZ);
	void SetLightAngle(const float& angle);
	void SetMesh(IMesh* newMesh);
	void SetModel(IModel* newModel);

	void RenderLight();
	void RenderDepthBufferFromLight(std::vector<IModel*> allShadowModels);

private:
	IEngine * myEngine;
	IScene* myScene;
	IModel* lightModel;
	IMesh* lightMesh;

	CVector4 mLightPosition{ 0,0,0,0 };
	CVector4 mLightColour{ 0,0,0,0 };
	float mSpecularPower = 0;
	CVector3 mAmbientColour{ 0,0,0 };
	float mLightStrength = 0;
	int mLightNumber;

	float coneAngle;


	ID3D11PixelShader* mPSShader = nullptr;
	ID3D11VertexShader* mVSShader = nullptr;

	PerFrameConstants mPerFrameConstants;
	PerModelConstants mPerModelConstants;

	ID3D11Buffer* mPerFrameConstantBuffer;

};

