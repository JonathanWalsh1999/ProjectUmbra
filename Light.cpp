#include "Light.hpp"
#include "DirectX11Engine.hpp"

#include "Scene.hpp"

#include "Model.hpp"

namespace umbra_engine
{

int Light::mLightCount = 0;


Light::Light(IEngine* engine, ELightType type)
{

	myEngine = engine;
	mDevice = myEngine->GetDevice();
	mLightType = type;
	if (!ShadowDepthBuffer())
	{
		throw std::runtime_error("depth buffer not loaded for shadow");
	}

	mPSShader = LoadPixelShader("main_ps", myEngine);
	mVSShader = LoadVertexShader("main_vs", myEngine);

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
	if (mShadow) mShadow->Release();
	if (mShadowDepthStencil) mShadowDepthStencil->Release();
	if (mShadowSRV) mShadowSRV->Release();
}

IMesh* Light::GetMesh() { return lightMesh; }
IModel* Light::GetModel() { return lightModel; }
maths::CVector4 Light::GetPosition() { return mLightPosition; }
maths::CVector4 Light::GetColour() { return mLightColour; }
float Light::GetSpecularPower() { return mSpecularPower; }
maths::CVector3 Light::GetAmbientColour() { return mAmbientColour; }
float Light::GetLightStrength() { return mLightStrength; }
int Light::GetLightNumber() { return mLightCount; }

void Light::SetPosition(const maths::CVector4& newPos) { mLightPosition = newPos; }
void Light::SetLightColour(const maths::CVector4& newColour)
{
	mLightColour = newColour;

}
void Light::SetSpecularPower(const float& newSpecularPower)
{
	mSpecularPower = newSpecularPower;
}
void Light::SetAmbientColour(const maths::CVector3& newAmbientColour)
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

	maths::CMatrix4x4 lightViewMatrix = InverseAffine(lightModel->WorldMatrix());

	//projection matrix
	maths::CMatrix4x4 lightProjectionMatrix = MakeProjectionMatrix(1.0f, maths::ToRadians(coneAngle));

	maths::CVector3 lightFacings3 = { perFrameConstants.lightFacings[mLightIndex].x, perFrameConstants.lightFacings[mLightIndex].y , perFrameConstants.lightFacings[mLightIndex].z };

	lightFacings3 = Normalise(lightModel->WorldMatrix().GetZAxis());
	perFrameConstants.lightFacings[mLightIndex] = { lightFacings3.x, lightFacings3.y, lightFacings3.z, 0 };
	perFrameConstants.lightFacings[mLightIndex].w = cos(maths::ToRadians(coneAngle / 2));
	perFrameConstants.lightViewMatrix[mLightIndex] = lightViewMatrix;
	perFrameConstants.lightProjectionMatrix[mLightIndex] = lightProjectionMatrix;
	//perFrameConstants.lightWorldMatrix[mLightIndex] = lightModel->WorldMatrix();

	perModelConstants.objectColour = mLightColour;
	perFrameConstants.ambientColour = mAmbientColour;
	perFrameConstants.specularPower = mSpecularPower;

}


void Light::SetLightAngle(const float& angle)
{
	coneAngle = angle;
}

bool Light::ShadowDepthBuffer()
{
	//**** Create Shadow Map texture ****//

// We also need a depth buffer to go with our portal
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 1024; // Size of the shadow map determines quality / resolution of shadows
	textureDesc.Height = 1024;
	textureDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	if (FAILED(mDevice->CreateTexture2D(&textureDesc, NULL, &mShadow)))
	{
		return false;
	}

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	if (FAILED(mDevice->CreateDepthStencilView(mShadow, &dsvDesc, &mShadowDepthStencil)))
	{

		return false;
	}


	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
											// but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if (FAILED(mDevice->CreateShaderResourceView(mShadow, &srvDesc, &mShadowSRV)))
	{

		return false;
	}
	return true;
}
void Light::ClearDepthStencil(ID3D11DeviceContext* context)
{
	context->OMSetRenderTargets(0, nullptr, mShadowDepthStencil);
	context->ClearDepthStencilView(mShadowDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Light::SendShadowMap2Shader(int textureSlot, ID3D11DeviceContext* context)
{
	myScene = myEngine->GetScene();
	mPointSampler = myScene->GetPointSampler();
	context->PSSetShaderResources(textureSlot, 1, &mShadowSRV);
	//myEngine->GetContext()->PSSetSamplers(1, 1, &mPointSampler);
}

}