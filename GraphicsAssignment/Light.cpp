#include "Light.hpp"
#include "DirectX11Engine.hpp"
#include "GraphicsHelpers.hpp"
#include "Scene.hpp"

#include "Model.hpp"

int Light::mLightCount = 0;


Light::Light(IEngine * engine, ELightType type)
{ 
	myEngine = engine;
	mLightType = type;
	mPSShader = LoadPixelShader("main_ps", myEngine);
	mVSShader = LoadVertexShader("main_vs", myEngine);
	myScene = myEngine->GetScene();
	mLightIndex = mLightCount;
	mLightCount += 1;
	if (mLightCount > PerFrameConstants::MAX_LIGHTS)
	{
		std::string str = "Max lights reached. Max= " + std::to_string(PerFrameConstants::MAX_LIGHTS);
		throw std::runtime_error(str);
	}


}
Light::~Light() 
{
	if (mPSShader) mPSShader->Release();
	if (mVSShader) mVSShader->Release();
}

IMesh* Light::GetMesh() { return lightMesh; }
IModel* Light::GetModel() { return lightModel; }
CVector4 Light::GetPosition() { return mLightPosition; }
CVector4 Light::GetColour() { return mLightColour; }
float Light::GetSpecularPower() { return mSpecularPower; }
CVector3 Light::GetAmbientColour() { return mAmbientColour; }
float Light::GetLightStrength() { return mLightStrength; }
int Light::GetLightNumber() { return mLightCount; }

void Light::SetPosition(const CVector4& newPos) { mLightPosition = newPos; }
void Light::SetLightColour(const CVector4& newColour)
{
	mLightColour = newColour;

}
void Light::SetSpecularPower(const float& newSpecularPower) 
{ 
	mSpecularPower = newSpecularPower; 
}
void Light::SetAmbientColour(const CVector3& newAmbientColour) 
{ 
	mAmbientColour = newAmbientColour; 
}
void Light::SetLightStrength(const float& newLightStrength) 
{ 
	mLightStrength = newLightStrength; 
}
void Light::SetLightNumber(const int& newLightNumber) 
{ 
	//mLightNumber = newLightNumber; 
}

void Light::SetMesh(IMesh* newMesh) 
{ 
	lightMesh = newMesh; 
}
void Light::SetModel(IModel* newModel) 
{ 
	lightModel = newModel; 
}


void Light::RenderLight(PerFrameConstants& perFrameConstants, PerModelConstants& perModelConstants)
{
	perFrameConstants.lightCount = mLightCount;
	perFrameConstants.lightColours[mLightIndex] = mLightColour * mLightStrength;
	perFrameConstants.lightColours[mLightIndex].w = static_cast<float>(mLightType);//Pass the light type to shaders, 
	perFrameConstants.lightPositions[mLightIndex] = mLightPosition;				//so they know what sort of lighting to do e.g. point, spot etc.

	//View matrix for spotlight
	CMatrix4x4 lightViewMatrix = InverseAffine(lightModel->WorldMatrix());
	//projection matrix
	CMatrix4x4 lightProjectionMatrix = myEngine->MakeProjectionMatrix(1.0f, ToRadians(coneAngle));

	CVector3 lightFacings3 = { perFrameConstants.lightFacings[mLightIndex].x, perFrameConstants.lightFacings[mLightIndex].y , perFrameConstants.lightFacings[mLightIndex].z };

	lightFacings3 = Normalise(lightModel->WorldMatrix().GetZAxis());
	perFrameConstants.lightFacings[mLightIndex] = { lightFacings3.x, lightFacings3.y, lightFacings3.z, 0 };
	perFrameConstants.lightFacings[mLightIndex].w = cos(ToRadians(coneAngle / 2));
	perFrameConstants.lightViewMatrix[mLightIndex] = lightViewMatrix;
	perFrameConstants.lightProjectionMatrix[mLightIndex] = lightProjectionMatrix;
	
	perModelConstants.objectColour = mLightColour;
	perFrameConstants.ambientColour = mAmbientColour;
	perFrameConstants.specularPower = mSpecularPower;
}


void Light::SetLightAngle(const float& angle)
{
	coneAngle = angle;
}

