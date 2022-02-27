#include "PointLight.hpp"
#include "DirectX11Engine.hpp"

#include "Scene.hpp"
#include "ICamera.hpp"
#include "Model.hpp"

namespace umbra_engine
{
int CPointLight::mPointLightCount = 0;


CPointLight::CPointLight(IEngine* engine, ELightType type)
{
	mLightModels.resize(5);
	mCubeMapCameras.resize(6);
	myEngine = engine;
	mDevice = myEngine->GetDevice();
	mLightType = type;
	if (!ShadowDepthBuffer())
	{
		throw std::runtime_error("depth buffer not loaded for shadow");
	}
	mPSShader = LoadPixelShader("main_ps", myEngine);
	mVSShader = LoadVertexShader("main_vs", myEngine);
	mGSShader = LoadGeometryShader("PointShadow_gs", myEngine);

	mLightIndex = mLightCount;
	mPointLightIndex = mPointLightCount;
	mLightCount += 1;
	mPointLightCount += 1;
	if (mLightCount > PerFrameConstants::MAX_LIGHTS)
	{
		std::string str = "Max lights reached. Max= " + std::to_string(PerFrameConstants::MAX_LIGHTS);
		throw std::runtime_error(str);
	}


}
CPointLight::~CPointLight()
{
	if (mPSShader) mPSShader->Release();
	if (mVSShader) mVSShader->Release();
	if (mGSShader) mGSShader->Release();
	if (mCubeShadow) mCubeShadow->Release();
	if (mShadowDepthStencil) mShadowDepthStencil->Release();
	if (mShadowSRV) mShadowSRV->Release();
}

IMesh* CPointLight::GetMesh() { return lightMesh; }
IModel* CPointLight::GetModel() { return lightModel; }
maths::CVector4 CPointLight::GetPosition() { return mLightPosition; }
maths::CVector4 CPointLight::GetColour() { return mLightColour; }
float CPointLight::GetSpecularPower() { return mSpecularPower; }
maths::CVector3 CPointLight::GetAmbientColour() { return mAmbientColour; }
float CPointLight::GetLightStrength() { return mLightStrength; }
int CPointLight::GetLightNumber() { return mLightCount; }

void CPointLight::SetPosition(const maths::CVector4& newPos) { mLightPosition = newPos; }
void CPointLight::SetLightColour(const maths::CVector4& newColour)
{
	mLightColour = newColour;

}
void CPointLight::SetSpecularPower(const float& newSpecularPower)
{
	mSpecularPower = newSpecularPower;
}
void CPointLight::SetAmbientColour(const maths::CVector3& newAmbientColour)
{
	mAmbientColour = newAmbientColour;
}
void CPointLight::SetLightStrength(const float& newLightStrength)
{
	mLightStrength = newLightStrength;
}
void CPointLight::SetLightNumber(const int& newLightNumber)
{
	//mLightNumber = newLightNumber; 
}

void CPointLight::SetMesh(IMesh* newMesh)
{
	lightMesh = newMesh;
}
void CPointLight::SetModel(IModel* newModel)
{
	lightModel = newModel;

	for (int i = 0; i < mLightModels.size(); ++i)
	{
		mLightModels[i] = newModel;
	}
	//0 - Front
	mLightModels[1]->RotateX(maths::PI * 0.5f);//centre
	mLightModels[2]->RotateY(maths::PI * 0.5f); //Right
	mLightModels[3]->RotateY(maths::PI);//Rear
	mLightModels[4]->RotateY(-maths::PI * 0.5f);//Left


}


void CPointLight::RenderLight(PerFrameConstants& perFrameConstants, PerModelConstants& perModelConstants)
{
	myEngine->GetContext()->RSSetViewports(1, &mCubeMapViewport);
	perFrameConstants.lightCount = mLightCount;
	perFrameConstants.lightColours[mLightIndex] = mLightColour * mLightStrength;
	perFrameConstants.lightColours[mLightIndex].w = static_cast<float>(mLightType);//Pass the light type to shaders, 
	perFrameConstants.lightPositions[mLightIndex] = mLightPosition;				//so they know what sort of lighting to do e.g. point, spot etc.

	//View matrix for spotlight
	maths::CMatrix4x4 lightViewMatrix = InverseAffine(mLightModels[0]->WorldMatrix());
	//projection matrix
	maths::CMatrix4x4 lightProjectionMatrix = MakeProjectionMatrix(1.0f, maths::ToRadians(coneAngle));

	maths::CVector3 lightFacings3 = { perFrameConstants.lightFacings[mLightIndex].x, perFrameConstants.lightFacings[mLightIndex].y , perFrameConstants.lightFacings[mLightIndex].z };

	GetCubeViewProjection();

	lightFacings3 = Normalise(mLightModels[0]->WorldMatrix().GetZAxis());
	perFrameConstants.lightFacings[mLightIndex] = { lightFacings3.x, lightFacings3.y, lightFacings3.z, 0 };
	perFrameConstants.lightFacings[mLightIndex].w = cos(maths::ToRadians(coneAngle / 2));
	perFrameConstants.lightViewMatrix[mLightIndex] = lightViewMatrix;
	perFrameConstants.lightProjectionMatrix[mLightIndex] = lightProjectionMatrix;

	perModelConstants.objectColour = mLightColour;
	perFrameConstants.ambientColour = mAmbientColour;
	perFrameConstants.specularPower = mSpecularPower;

	for (int i = 0; i < 6; ++i)
	{
		perFrameConstants.cubeViewProj[i] = mCubeViewProj[i];
	}

	//int viewMatrixIndex = mPointLightIndex * (mLightModels.size() - 1);//0,4,8,12
	//for (int modelIndex = 1; modelIndex < 5; ++modelIndex)
	//{
	//	perFrameConstants.lightPointViewMatrix[viewMatrixIndex] = InverseAffine(mLightModels[modelIndex]->WorldMatrix());
	//	perFrameConstants.lightPointFacings[viewMatrixIndex] = { Normalise(mLightModels[modelIndex]->WorldMatrix().GetZAxis()).x, Normalise(mLightModels[modelIndex]->WorldMatrix().GetZAxis()).y,	
	//															 Normalise(mLightModels[modelIndex]->WorldMatrix().GetZAxis()).z, 0};
	//	++viewMatrixIndex;
	//}

}


void CPointLight::SetLightAngle(const float& angle)
{
	coneAngle = angle;
}

//bool CPointLight::ShadowDepthBuffer()
//{
//	//**** Create Shadow Map texture ****//
//
//// We also need a depth buffer to go with our portal
//	D3D11_TEXTURE2D_DESC textureDesc = {};
//	textureDesc.Width = 1024; // Size of the shadow map determines quality / resolution of shadows
//	textureDesc.Height = 1024;
//	textureDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
//	textureDesc.ArraySize = 1;
//	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
//	textureDesc.SampleDesc.Count = 1;
//	textureDesc.SampleDesc.Quality = 0;
//	textureDesc.Usage = D3D11_USAGE_DEFAULT;
//	textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
//	textureDesc.CPUAccessFlags = 0;
//	textureDesc.MiscFlags = 0;
//	if (FAILED(mDevice->CreateTexture2D(&textureDesc, NULL, &mCubeShadow)))
//	{
//		return false;
//	}
//
//	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	dsvDesc.Texture2D.MipSlice = 0;
//	dsvDesc.Flags = 0;
//	if (FAILED(mDevice->CreateDepthStencilView(mCubeShadow, &dsvDesc, &mShadowDepthStencil)))
//	{
//
//		return false;
//	}
//
//
//	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
//											// but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.MipLevels = 1;
//	if (FAILED(mDevice->CreateShaderResourceView(mCubeShadow, &srvDesc, &mShadowSRV)))
//	{
//
//		return false;
//	}
//	return true;
//}

bool CPointLight::ShadowDepthBuffer()
{
	//Initialisation of shadow map
	//**** Create Shadow Map texture ****//

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 1024; // Size of the shadow map determines quality / resolution of shadows
	textureDesc.Height = 1024;
	textureDesc.MipLevels = 0; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


	if (FAILED(mDevice->CreateTexture2D(&textureDesc, NULL, &mCubeShadow)))
	{
		return false;
	}


	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	if (FAILED(mDevice->CreateDepthStencilView(mCubeShadow, &dsvDesc, &mShadowDepthStencil)))
	{
		return false;
	}


	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
											// but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if (FAILED(mDevice->CreateShaderResourceView(mCubeShadow, &srvDesc, &mShadowSRV)))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;//Create view to 1 array element

	//for (int i = 0; i < 6; ++i)
	//{

	//	//Create rtv
	//	//rtvDesc.Texture2DArray.FirstArraySlice = i;
	//	//if (FAILED(mDevice->CreateRenderTargetView(mCubeShadow, &rtvDesc, &mCubeMapRTV[i])))//Always fails
	//	//{
	//	//	return false;
	//	//}
	//}

	mCubeMapViewport.TopLeftX = 0.0f;
	mCubeMapViewport.TopLeftY = 0.0f;
	mCubeMapViewport.Width = 1024.0f;
	mCubeMapViewport.Height = 1024.0f;
	mCubeMapViewport.MinDepth = 0.0f;
	mCubeMapViewport.MaxDepth = 1.0f;


	////Add data to each face of cube
	//D3D11_SUBRESOURCE_DATA data[6];
	//for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
	//{
	//	//data[faceIndex].pSysMem = 
	//}
	//
	return true;
}

void CPointLight::ClearDepthStencil(ID3D11DeviceContext* context)
{
	myEngine->GetContext()->OMSetRenderTargets(0, nullptr, mShadowDepthStencil);
	myEngine->GetContext()->ClearDepthStencilView(mShadowDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void CPointLight::SendShadowMap2Shader(int textureSlot, ID3D11DeviceContext* context)
{
	myScene = myEngine->GetScene();
	mPointSampler = myScene->GetPointSampler();
	myEngine->GetContext()->PSSetShaderResources(2, 1, &mShadowSRV);
	myEngine->GetContext()->PSSetSamplers(1, 1, &mPointSampler);
}

void CPointLight::ConstructCubeFaceCameras(maths::CVector3 lightPosition)
{
	//Light position used to generate the cameras for each face of cube map
	maths::CVector3 center = lightPosition;
	maths::CVector3 worldUp{ 0.0f, 1.0f, 0.0f };

	//Face each axes
	maths::CVector3 axes[6] =
	{
		maths::CVector3{lightPosition.x + 1.0f, lightPosition.y, lightPosition.z},//Face to the RIGHT +X
		maths::CVector3{lightPosition.x - 1.0f, lightPosition.y, lightPosition.z},//Face to the LEFT -X
		maths::CVector3{lightPosition.x, lightPosition.y + 1.0f, lightPosition.z},//Face UP +Y 
		maths::CVector3{lightPosition.x, lightPosition.y - 1.0f, lightPosition.z},//Face DOWN -Y
		maths::CVector3{lightPosition.x, lightPosition.y, lightPosition.z + 1.0f},//Face FORWARD +Z
		maths::CVector3{lightPosition.x, lightPosition.y, lightPosition.z - 1.0f},//Face BACKWARD -Z
	};

	//Up vectors
	maths::CVector3 ups[6] =
	{
		maths::CVector3{0.0f, 1.0f, 0.0f}, //+X
		maths::CVector3{0.0f, 1.0f, 0.0f}, //-X
		maths::CVector3{0.0f, 0.0f, -1.0f}, //+Y need different up vector when facing in y axes
		maths::CVector3{0.0f, 0.0f, 1.0f}, //-Y
		maths::CVector3{0.0f, 1.0f, 0.0f}, //+Z
		maths::CVector3{0.0f, 1.0f, 0.0f}, //+Z
	};

	for (int i = 0; i < 6; ++i)
	{
		mCubeMapCameras[i] = new CCamera(center, axes[i], maths::PI * 0.5f, 1.0f, 0.1f, 1000.0f);
	}
}

void CPointLight::RenderCubeMap()
{
	ID3D11RenderTargetView* renderTargets[1];

	//Render to each cube map face
	myEngine->GetContext()->RSSetViewports(1, &mCubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		//Clear face and depth buffer
		myEngine->GetContext()->ClearRenderTargetView(mCubeMapRTV[i], 0);
		myEngine->GetContext()->ClearDepthStencilView(mShadowDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//bind face as render target
		renderTargets[0] = mCubeMapRTV[i];
		myEngine->GetContext()->OMSetRenderTargets(1, renderTargets, mShadowDepthStencil);
	}


	myEngine->GetContext()->GenerateMips(mShadowSRV);


}

//Create the cube view projection matrices for each face
void CPointLight::GetCubeViewProjection()
{

	maths::CMatrix4x4 lightProjection, position, spotView, toShadow;

	//Position of light in world space
	maths::CVector3 worldPos = lightModel->Position();

	position = MatrixTranslation(-worldPos);
	lightProjection = MakeProjectionMatrix(1.0f, maths::ToRadians(90));

	//Cube +X
	spotView = maths::MatrixRotationY(maths::PI + maths::PI * 0.5f);
	toShadow = position * spotView * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[0] = toShadow;

	//Cube -X
	spotView = maths::MatrixRotationY(maths::PI * 0.5f);
	toShadow = position * spotView * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[1] = toShadow;

	//Cube +Y
	spotView = maths::MatrixRotationY(maths::PI * 0.5f);
	toShadow = position * spotView * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[2] = toShadow;

	//Cube -Y
	spotView = maths::MatrixRotationY(maths::PI + maths::PI * 0.5f);
	toShadow = position * spotView * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[3] = toShadow;

	//Cube +Z
	toShadow = position * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[4] = toShadow;

	//Cube -Z
	spotView = maths::MatrixRotationY(maths::PI);
	toShadow = position * spotView * lightProjection;
	toShadow.Transpose();
	mCubeViewProj[5] = toShadow;
}
}