#include "Light.hpp"
#include "DirectX11Engine.hpp"
#include "GraphicsHelpers.hpp"
#include "Scene.hpp"

#include "Model.hpp"




Light::Light(IEngine * engine)
{ 
	myEngine = engine;

	mPSShader = LoadPixelShader("main_ps", myEngine);
	mVSShader = LoadVertexShader("main_vs", myEngine);
	myScene = myEngine->GetScene();
	mPerFrameConstants = myScene->GetFrameConstants();
}
Light::~Light() {}

IMesh* Light::GetMesh() { return lightMesh; }
IModel* Light::GetModel() { return lightModel; }
CVector3 Light::GetPosition() { return mLightPosition; }
CVector3 Light::GetColour() { return mLightColour; }
float Light::GetSpecularPower() { return mSpecularPower; }
CVector3 Light::GetAmbientColour() { return mAmbientColour; }
float Light::GetLightStrength() { return mLightStrength; }
float Light::GetLightNumber() { return mLightNumber; }

void Light::SetPosition(const CVector3& newPos) { mLightPosition = newPos; }
void Light::SetLightColour(const CVector3& newColour)
{
	mLightColour = newColour;

}
void Light::SetSpecularPower(const float& newSpecularPower) { mSpecularPower = newSpecularPower; }
void Light::SetAmbientColour(const CVector3& newAmbientColour) { mAmbientColour = newAmbientColour; }
void Light::SetLightStrength(const float& newLightStrength) { mLightStrength = newLightStrength; }
void Light::SetLightNumber(const int& newLightNumber) { mLightNumber = newLightNumber; }
void Light::SetMesh(IMesh* newMesh) { lightMesh = newMesh; }
void Light::SetModel(IModel* newModel) { lightModel = newModel; }


void Light::RenderLight()
{
	mPerFrameConstants = myScene->GetFrameConstants();
	mPerModelConstants = myEngine->GetModelConstants();
	if (mLightNumber == 1)
	{
		mPerFrameConstants.light1Colour = mLightColour * mLightStrength;
		mPerFrameConstants.light1Position = mLightPosition;

		myEngine->SetFrameConstants(mPerFrameConstants);
	}

	if (mLightNumber == 0)
	{
		mPerFrameConstants.lightColours[0] = mLightColour * mLightStrength;
		mPerFrameConstants.lightPositions[0] = mLightPosition;

		myScene->SetFrameConstants(mPerFrameConstants);

		//View matrix for spotlight
		CMatrix4x4 lightViewMatrix = InverseAffine(this->lightModel->WorldMatrix());
		//projection matrix
		CMatrix4x4 lightProjectionMatrix = myEngine->MakeProjectionMatrix(1.0f, ToRadians(coneAngle));

		mPerFrameConstants.lightFacings = Normalise(this->lightModel->WorldMatrix().GetZAxis());
		mPerFrameConstants.lightCosHalfAngles = cos(ToRadians(coneAngle / 2));
		mPerFrameConstants.lightViewMatrix = lightViewMatrix;
		mPerFrameConstants.lightProjectionMatrix = lightProjectionMatrix;

		myScene->SetFrameConstants(mPerFrameConstants);
	}

	mPerModelConstants.objectColour = mLightColour;
	myEngine->SetModelConstants(mPerModelConstants);
	mPerFrameConstants.ambientColour = mAmbientColour;
	mPerFrameConstants.specularPower = mSpecularPower;
	myScene->SetFrameConstants(mPerFrameConstants);
}

// Render the scene from the given light's point of view. Only renders depth buffer
void Light::RenderDepthBufferFromLight(std::vector<IModel*> allShadowModels)
{
	//UPDATE ONCE PER FRAME
	mPerFrameConstants = myEngine->GetFrameConstants();
	mPerFrameConstantBuffer = myEngine->GetFrameConstantBuffer();


	// Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
	mPerFrameConstants.viewMatrix = InverseAffine(this->lightModel->WorldMatrix());
	mPerFrameConstants.projectionMatrix = myEngine->MakeProjectionMatrix(1.0f, ToRadians(coneAngle));
	mPerFrameConstants.viewProjectionMatrix = myEngine->GetFrameConstants().viewMatrix * myEngine->GetFrameConstants().projectionMatrix;
	myScene->SetFrameConstants(mPerFrameConstants);
	myEngine->UpdateConstantBuffer(myEngine->GetFrameConstantBuffer(), mPerFrameConstants);


	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	myEngine->GetContext()->VSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	myEngine->GetContext()->PSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer);

	// Use special depth-only rendering shaders
	myEngine->GetContext()->VSSetShader(mVSShader, nullptr, 0);
	myEngine->GetContext()->PSSetShader(mPSShader, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	myEngine->GetContext()->OMSetBlendState(myEngine->GetNoBlendState(), nullptr, 0xffffff);
	myEngine->GetContext()->OMSetDepthStencilState(myEngine->GetDepthBufferState(), 0);
	myEngine->GetContext()->RSSetState(myEngine->GetCullBackState());

	for (unsigned int i = 0; i < allShadowModels.size(); ++i)
	{
		allShadowModels[i]->Render();
	}
	
}



void Light::SetLightFacing(const CVector3& localZ)
{

}
void Light::SetLightAngle(const float& angle)
{
	coneAngle = angle;
}

