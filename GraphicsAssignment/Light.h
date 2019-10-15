#pragma once
#include "Common.h"
#include "Shader.h"

class CDX11Engine;
class Model;
class Mesh;

class Light
{
public:
	Light(CDX11Engine* engine);
	~Light();
	Model* lightModel;
	Mesh* lightMesh;

	void AddToVector();


	CVector3 GetPosition();
	CVector3 GetColour();
	float GetSpecularPower();
	CVector3 GetAmbientColour();
	float GetLightStrength();
	float GetLightNumber();

	void SetPosition(const CVector3& newPos);
	void SetLightColour(const CVector3& newColour);
	void SetSpecularPower(const float& newSpecularPower);
	void SetAmbientColour(const CVector3& newAmbientColour);
	void SetLightStrength(const float& newLightStrength);
	void SetLightNumber(const int& newLightNumber);
	void SetLightFacing(const CVector3& localZ);
	void SetLightAngle(const float& angle);
	void RenderLight();
	void RenderDepthBufferFromLight(std::vector<Model*> allShadowModels);

	static std::vector<CVector3> lightPositions;
	static std::vector<CVector3> lightColours;
	static std::vector<float> lightStrengths;

private:
	CDX11Engine * myEngine;

	CVector3 mLightPosition{ 0,0,0 };
	CVector3 mLightColour{ 0,0,0 };
	float mSpecularPower = 0;
	CVector3 mAmbientColour{ 0,0,0 };
	float mLightStrength = 0;
	int mLightNumber;

	float coneAngle;


	ID3D11PixelShader* mPSShader = nullptr;
	ID3D11VertexShader* mVSShader = nullptr;

	//PerFrameConstants mPerFrameConstants;
	PerModelConstants mPerModelConstants;

	ID3D11Buffer* mPerFrameConstantBuffer;

};
