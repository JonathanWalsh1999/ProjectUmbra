#include "Scene.hpp"
#include "DirectX11Engine.hpp"
#include "Light.hpp"
#include "Model.hpp"

CScene::CScene(IEngine * engine)
{
	mEngine = engine;
}

CScene::~CScene()
{

}

bool CScene::InitGeometry()
{
	// Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
	// IMPORTANT NOTE: Will only keep the first object from the mesh - multipart objects will have parts missing - see later lab for more robust loader
	try
	{
		//Load all meshes here
		//lightMesh = new Mesh("Light.x");
	}
	catch (std::runtime_error e)  // Constructors cannot return error messages so use exceptions to catch mesh errors (fairly standard approach this)
	{
		mLastError = e.what(); // This picks up the error message put in the exception (see Mesh.cpp)
		return false;
	}


	// Load the shaders required for the geometry we will use (see Shader.cpp / .h)
	if (!LoadShaders())
	{
		mLastError = "Error loading shaders";
		return false;
	}


	mPerModelConstants = mEngine->GetModelConstants();

	// Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
	// These allow us to pass data from CPU to shaders such as lighting information or matrices
	// See the comments above where these variable are declared and also the UpdateScene function
	mPerFrameConstantBuffer = CreateConstantBuffer(sizeof(mPerFrameConstants), mEngine);
	mPerModelConstantBuffer = CreateConstantBuffer(sizeof(mPerModelConstants), mEngine);
	if (mPerFrameConstantBuffer == nullptr || mPerModelConstantBuffer == nullptr)
	{
		mLastError = "Error creating constant buffers";
		return false;
	}


	mEngine->SetModelConstants(mPerModelConstants);

	//// Load / prepare textures on the GPU ////

	// Load textures and create DirectX objects for them
	// The LoadTexture function requires you to pass a ID3D11Resource* (e.g. &gCubeDiffuseMap), which manages the GPU memory for the
	// texture and also a ID3D11ShaderResourceView* (e.g. &gCubeDiffuseMapSRV), which allows us to use the texture in shaders
	// The function will fill in these pointers with usable data. The variables used here are globals found near the top of the file.
	//if (!LoadTexture(".\\Flare.jpg", &gLightDiffuseMap, &gLightDiffuseMapSRV))
	//{
	//	gLastError = "Error loading textures";
	//	return false;
	//}

	if (!ShadowDepthBuffer())
	{		
		return false;
	}

	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!mEngine->CreateStates())
	{
		mLastError = "Error creating states";
		return false;
	}
	return true;

}
bool CScene::InitScene()
{
	mDepthOnly = LoadPixelShader("main_ps", mEngine);

	mBasicPixel = LoadVertexShader("main_vs", mEngine);
	//// Set up scene ////
	//light = new Model(lightMesh);

	//light->SetPosition({ 30, 300, 0 });
	//light->SetScale(pow(gLight1Strength, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.

	//// Set up cameras ////

	camera = new CCamera();
	camera->SetPosition({ 0, 50, -100 });
	camera->SetRotation({ ToRadians(30.0f), 0.0f, 0.0f });
	camera->SetNearClip(5);
	camera->SetFarClip(100000.0f);

	return true;
}

void CScene::RenderSceneFromCamera(ICamera * cam)
{

	mPerModelConstants = mEngine->GetModelConstants();

	// Set camera matrices in the constant buffer and send over to GPU

	mPerFrameConstants.viewMatrix = cam->ViewMatrix(); 
	mPerFrameConstants.projectionMatrix = cam->ProjectionMatrix();
	mPerFrameConstants.viewProjectionMatrix = cam->ViewProjectionMatrix();

	mEngine->UpdateConstantBuffer(mEngine->GetFrameConstantBuffer(), mPerFrameConstants);

	mPerFrameConstantBuffer = mEngine->GetFrameConstantBuffer();

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	mEngine->GetContext()->VSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	mEngine->GetContext()->PSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer);


	//// Render lit models - ground first ////

	// Select which shaders to use next ***
//	mD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
//	mD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	mEngine->GetContext()->OMSetBlendState(mEngine->GetNoBlendState(), nullptr, 0xffffff);
	mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthBufferState(), 0); //If objects are behind other objects they will not get rendered
	mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());




	//// Select the texture and sampler to use in the pixel shader
	//mD3DContext->PSSetShaderResources(0, 1, &gLightDiffuseMapSRV); // First parameter must match texture slot number in the shaer
	//mD3DContext->PSSetSamplers(0, 1, &mAnisotropic4xSampler);

	////// Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
	//gPerModelConstants.objectColour = gLight1Colour; // Set any per-model constants apart from the world matrix just before calling render (light colour here)
}
void CScene::RenderScene(float& frameTime)
{

	D3D11_VIEWPORT vp;
	allModels = mEngine->GetAllModels();
	allModels = Model::GetAllObjects();
	mEngine->SetAllModels(allModels);

	mEngine->Messages();
	//// Render lights ////
	// Rendered with different shaders, textures, states from other models
	for (unsigned int i = 0; i < mEngine->GetAllLights().size(); ++i)
	{
		mEngine->GetAllLights()[i]->RenderLight();

	}

	vp.Width = static_cast<FLOAT>(mShadowMapSize);
	vp.Height = static_cast<FLOAT>(mShadowMapSize);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mEngine->GetContext()->RSSetViewports(1, &vp);

	// Select the shadow map texture as the current depth buffer. We will not be rendering any pixel colours
	// Also clear the the shadow map depth buffer to the far distance
	mEngine->GetContext()->OMSetRenderTargets(0, nullptr, mShadowMapDepthStencil);
	mEngine->GetContext()->ClearDepthStencilView(mShadowMapDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//mLight[0]->RenderDepthBufferFromLight(allModels);

	// Render the scene from the point of view of light 1 (only depth values written)
	//RenderDepthBufferFromLight(0);

	//// Common settings for both main scene and portal scene ////

	// Set up the light information in the constant buffer - this is the same for portal and main render
	// Don't send to the GPU yet, the function RenderSceneFromCamera will do that
	//gPerFrameConstants.light1Colour = gLight1Colour * gLight1Strength;
	//gPerFrameConstants.light1Position = light->Position();
	//gPerFrameConstants.ambientColour = gAmbientColour;
	//gPerFrameConstants.specularPower = gSpecularPower;
	mPerFrameConstants.cameraPosition = camera->Position();

	//// Main scene rendering ////

	// Now set the back buffer as the target for rendering and select the main depth buffer.
	// When finished the back buffer is sent to the "front buffer" - which is the monitor.
	mBackBufferRenderTarget = mEngine->GetBackBufferRenderTarget();
	mEngine->GetContext()->OMSetRenderTargets(1, &mBackBufferRenderTarget, mEngine->GetDepthStencil());

	mBackgroundColour = mEngine->GetBackgroundColour();

	// Clear the back buffer to a fixed colour and the depth buffer to the far distance
	mEngine->GetContext()->ClearRenderTargetView(mEngine->GetBackBufferRenderTarget(), &mBackgroundColour.r);
	mEngine->GetContext()->ClearDepthStencilView(mEngine->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);


	// Setup the viewport to the size of the main window
	vp.Width = static_cast<FLOAT>(gViewportWidth);
	vp.Height = static_cast<FLOAT>(gViewportHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mEngine->GetContext()->RSSetViewports(1, &vp);


	// Render the scene for the main window	
	RenderModels();
	//RenderShadow(vp);
	//RenderDepthBufferFromLight(0);


	// Set shadow maps in shaders
	// First parameter is the "slot", must match the Texture2D declaration in the HLSL code
	// In this app the diffuse map uses slot 0, the shadow maps use slots 1 onwards. If we were using other maps (e.g. normal map) then
	// we might arrange things differently
	mPointSampler = mEngine->GetPointSampler();

	mEngine->GetContext()->PSSetShaderResources(2, 1, &mShadowMapSRV);
	mEngine->GetContext()->PSSetSamplers(2, 1, &mPointSampler);

	RenderSceneFromCamera(camera);



	UpdateScene(frameTime);

	// Unbind shadow maps from shaders - prevents warnings from DirectX when we try to render to the shadow maps again next frame
	ID3D11ShaderResourceView* nullView = nullptr;
	mEngine->GetContext()->PSSetShaderResources(1, 1, &nullView);

	mEngine->GetSwapChain()->Present(0, 0);

}
void CScene::RenderModels()
{
	for (unsigned int j = 0; j < mEngine->GetAllModels().size(); ++j)
	{
		//Set the correct vs and ps for each model
		mEngine->GetContext()->PSSetShader(mEngine->GetAllModels()[j]->GetPSShader(), nullptr, 0);
		//mEngine->GetContext()->PSSetShader(mEngine->GetAllModels()[j]->GetPSShader(), nullptr, 0);
		mEngine->GetContext()->VSSetShader(mEngine->GetAllModels()[j]->GetVSShader(), nullptr, 0);

		ID3D11ShaderResourceView* currentDiffuseSpecularMapSRV = nullptr;
		ID3D11ShaderResourceView* secondaryDiffuseSpecularMapSRV = nullptr;

		currentDiffuseSpecularMapSRV = mEngine->GetAllModels()[j]->GetDiffuseSRVMap();
		secondaryDiffuseSpecularMapSRV = mEngine->GetAllModels()[j]->GetDiffuseSRVMap2();

		// Select the approriate textures and sampler to use in the pixel shader
		mEngine->GetContext()->PSSetShaderResources(0, 1, &currentDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
		if (mEngine->GetAllModels()[j]->GetDiffuseSRVMap2() != nullptr)
		{
			mEngine->GetContext()->PSSetShaderResources(1, 1, &secondaryDiffuseSpecularMapSRV);
		}
		mAnisotropic4xSampler = mEngine->GetAnisotropic4xSampler();
		mEngine->GetContext()->PSSetSamplers(0, 1, &mAnisotropic4xSampler);

		if (mEngine->GetAllModels()[j]->GetAddBlend() == Add)
		{
			// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
			mEngine->GetContext()->OMSetBlendState(mEngine->GetAddBlendState(), nullptr, 0xffffff);
			mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthReadOnlyState(), 0);
			mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());
			mEngine->GetAllModels()[j]->Render();


			//Change blending for the flare models.  (Additive by default)
			if (KeyHeld(Key_2))
			{
				//NO BLENDING
				// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
				mEngine->GetContext()->OMSetBlendState(mEngine->GetNoBlendState(), nullptr, 0xffffff);
				mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthReadOnlyState(), 0);
				mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());
				mEngine->GetAllModels()[j]->Render();
			}
		}
		else if (mEngine->GetAllModels()[j]->GetAddBlend() == Multi)
		{
			//MULTIPLICATIVE BLENDING
			// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
			mEngine->GetContext()->OMSetBlendState(mEngine->GetMultiBlendState(), nullptr, 0xffffff);
			mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthReadOnlyState(), 0);
			mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());
			mEngine->GetAllModels()[j]->Render();
		}
		else if (mEngine->GetAllModels()[j]->GetAddBlend() == Alpha)
		{
			//Alpha BLENDING
			// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
			mEngine->GetContext()->OMSetBlendState(mEngine->GetAlphaBlendState(), nullptr, 0xffffff);
			mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthReadOnlyState(), 0);
			mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());
			mEngine->GetAllModels()[j]->Render();
		}
		else
		{			
			mEngine->GetAllModels()[j]->Render();
			mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());

		}

	}
}
void CScene::RenderLights(std::vector<ILight*> lights)
{
	mLights = lights;
	mEngine->SetAllLights(mLights);
}
void CScene::RenderShadow(D3D11_VIEWPORT& vp)
{
	vp.Width = static_cast<FLOAT>(mShadowMapSize);
	vp.Height = static_cast<FLOAT>(mShadowMapSize);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mEngine->GetContext()->RSSetViewports(1, &vp);

	// Select the shadow map texture as the current depth buffer. We will not be rendering any pixel colours
	// Also clear the the shadow map depth buffer to the far distance
	mEngine->GetContext()->OMSetRenderTargets(0, nullptr, mShadowMapDepthStencil);
	mEngine->GetContext()->ClearDepthStencilView(mShadowMapDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//mLight[0]->RenderDepthBufferFromLight(allModels);

	// Render the scene from the point of view of light 1 (only depth values written)
	RenderDepthBufferFromLight(0);
}

void CScene::UpdateScene(float frameTime)
{

	// Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float rotate = 0.0f;
	//light->SetPosition(CVector3{ cos(rotate) * gLightOrbit, 210.0f, sin(rotate) * gLightOrbit });
	//rotate -= gLightOrbitSpeed * frameTime;


	// Control camera (will update its view matrix)
	camera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);

	// Show frame time / FPS in the window title //
	const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
	static float totalFrameTime = 0;
	static int frameCount = 0;
	totalFrameTime += frameTime;
	++frameCount;
	if (totalFrameTime > fpsUpdateTime)
	{
		// Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
		float avgFrameTime = totalFrameTime / frameCount;
		std::ostringstream frameTimeMs;
		frameTimeMs.precision(2);
		frameTimeMs << std::fixed << avgFrameTime * 1000;
		std::string windowTitle = "Graphics Assignment - Frame Time: " + frameTimeMs.str() +
			"ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
		SetWindowTextA(mEngine->GetHWnd(), windowTitle.c_str());
		totalFrameTime = 0;
		frameCount = 0;
	}
}

bool CScene::ShadowDepthBuffer()
{
	//**** Create Shadow Map texture ****//

	// We also need a depth buffer to go with our portal
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = mShadowMapSize; // Size of the shadow map determines quality / resolution of shadows
	textureDesc.Height = mShadowMapSize;
	textureDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	if (FAILED(mEngine->GetDevice()->CreateTexture2D(&textureDesc, NULL, &mShadowMapTexture)))
	{
		mLastError = "Error creating shadow map texture";
		return false;
	}

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	if (FAILED(mEngine->GetDevice()->CreateDepthStencilView(mShadowMapTexture, &dsvDesc, &mShadowMapDepthStencil)))
	{
		mLastError = "Error creating shadow map depth stencil view";
		return false;
	}


	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
											// but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if (FAILED(mEngine->GetDevice()->CreateShaderResourceView(mShadowMapTexture, &srvDesc, &mShadowMapSRV)))
	{
		mLastError = "Error creating shadow map shader resource view";
		return false;
	}
	return true;
}
void CScene::RenderDepthBufferFromLight(int lightIndex)
{


	// Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
	mPerFrameConstants.viewMatrix = InverseAffine(mEngine->GetAllLights()[lightIndex]->GetModel()->WorldMatrix());
	mPerFrameConstants.projectionMatrix = mEngine->MakeProjectionMatrix(1.0f, ToRadians(90.0f)); // Helper function in Utility\GraphicsHelpers.cpp
	mPerFrameConstants.viewProjectionMatrix = mPerFrameConstants.viewMatrix * mPerFrameConstants.projectionMatrix;


	mEngine->UpdateConstantBuffer(mPerFrameConstantBuffer, mPerFrameConstants);

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	mEngine->GetContext()->VSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	mEngine->GetContext()->PSSetConstantBuffers(0, 1, &mPerFrameConstantBuffer);


	//// Only render models that cast shadows ////

	// Use special depth-only rendering shaders
	mEngine->GetContext()->VSSetShader(mBasicPixel, nullptr, 0);
	mEngine->GetContext()->PSSetShader(mDepthOnly, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	mEngine->GetContext()->OMSetBlendState(mEngine->GetNoBlendState(), nullptr, 0xffffff);
	mEngine->GetContext()->OMSetDepthStencilState(mEngine->GetDepthBufferState(), 0);
	mEngine->GetContext()->RSSetState(mEngine->GetCullBackState());

	// Render models - no state changes required between each object in this situation (no textures used in this step)
	for (unsigned int i = 0; i < mEngine->GetAllModels().size() - 2; ++i)
	{
		mEngine->GetAllModels()[i]->Render();
	}
}

void CScene::ReleaseResources()
{
	mEngine->ReleaseStates();

	if (mShadowMapDepthStencil)  mShadowMapDepthStencil->Release();
	if (mShadowMapSRV)           mShadowMapSRV->Release();
	if (mShadowMapTexture)       mShadowMapTexture->Release();

	for (unsigned int i = 0; i < mEngine->GetDiffuseSpecularMaps().size(); ++i)
	{
		if (mEngine->GetDiffuseSpecularMaps()[i]) mEngine->GetDiffuseSpecularMaps()[i]->Release();
		if (mEngine->GetDiffuseSpecularMapSRVs()[i]) mEngine->GetDiffuseSpecularMapSRVs()[i]->Release();
	}

	if (mEngine->GetModelConstantBuffer())  mEngine->GetModelConstantBuffer()->Release();
	if (mEngine->GetFrameConstantBuffer())  mEngine->GetFrameConstantBuffer()->Release();

	ReleaseShaders();

	// See note in InitGeometry about why we're not using unique_ptr and having to manually delete
	delete camera;        camera = nullptr;
}

//Getters
ID3D11Buffer* CScene::GetFrameConstantBuffer()
{
	return mPerFrameConstantBuffer;
}

ID3D11Buffer* CScene::GetModelConstantBuffer()
{
	return mPerModelConstantBuffer;
}

PerFrameConstants CScene::GetFrameConstants()
{
	return mPerFrameConstants;
}

PerModelConstants CScene::GetModelConstants()
{
	return mPerModelConstants;
}

ICamera* CScene::GetCamera()
{
	return camera;
}

void CScene::SetFrameConstants(PerFrameConstants& constants)
{
	mPerFrameConstants = constants;
}