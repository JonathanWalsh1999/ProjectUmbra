#include "Scene.hpp"
#include "DirectX11Engine.hpp"
#include "Light.hpp"
#include "Model.hpp"

namespace umbra_engine
{

CScene::CScene(IEngine* engine)
{
	mEngine = engine;

	mD3DContext = mEngine->GetContext();
	mD3DDevice = mEngine->GetDevice();
}

CScene::~CScene()
{
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

bool CScene::InitGeometry()
{
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
	//if (!ShadowDepthBuffer())
	//{		
	//	return false;
	//}

	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!CreateStates())
	{
		mLastError = "Error creating states";
		return false;
	}
	return true;
}

bool CScene::InitScene()
{
	//mParticleSystem = new CParticleSystem(100, {1, 20, 0}, "smoke3.png", mEngine);

	//if (!mParticleSystem->InitParticles()) return false;

	mDepthOnly = LoadPixelShader("main_ps", mEngine);
	mBasicPixel = LoadVertexShader("main_vs", mEngine);
	mlightModelps = LoadPixelShader("LightModel_ps", mEngine);
	mlightModelvs = LoadVertexShader("LightModel_vs", mEngine);

	//// Set up cameras ////
	camera = std::make_unique<CCamera>();
	camera->SetPosition({ 200, 10, 20 });
	camera->SetRotation( { maths::ToRadians(0.0f), maths::ToRadians(-90.0f), 0.0f } );
	camera->SetNearClip(5);
	camera->SetFarClip(100000.0f);

	mPerFrameConstants.viewportHeight = gViewportHeight;
	mPerFrameConstants.viewportWidth = gViewportWidth;
	mPerFrameConstants.cameraMatrix = maths::InverseAffine(camera->ViewMatrix());

	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	ImGui_ImplWin32_Init(mEngine->GetHWnd());
	   	  
	ImGui_ImplDX11_Init(mEngine->GetDevice(), mEngine->GetContext());
	//ImGui::StyleColorsDark();
	//ImGui_ImplDX11_CreateDeviceObjects();
	mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(mEngine->GetContext());
	mFont = std::make_unique<DirectX::SpriteFont>(mEngine->GetDevice(), L"myfile.spritefont");

	return true;
}

//--------------------------------------------------------------------------------------
// State creation / destruction
//--------------------------------------------------------------------------------------

// Create all the states used in this app, returns true on success
bool CScene::CreateStates()
{
	//--------------------------------------------------------------------------------------
	// Texture Samplers
	//--------------------------------------------------------------------------------------
	// Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See texturing lab for details
	D3D11_SAMPLER_DESC samplerDesc = {};

	////-------- Point Sampling (pixelated textures) --------////
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // Point filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Wrap addressing mode for texture coordinates outside 0->1
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
	samplerDesc.MaxAnisotropy = 1;                       // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
	samplerDesc.MinLOD = 0;                 // --"--

											// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(mD3DDevice->CreateSamplerState(&samplerDesc, &mPointSampler)))
	{
		mLastError = "Error creating point sampler";
		return false;
	}
	
	////-------- Trilinear Sampling --------////
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Point filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Wrap addressing mode for texture coordinates outside 0->1
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
	samplerDesc.MaxAnisotropy = 1;                       // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
	samplerDesc.MinLOD = 0;                 // --"--

											// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(mD3DDevice->CreateSamplerState(&samplerDesc, &mTrilinearSampler)))
	{
		mLastError = "Error creating point sampler";
		return false;
	}
	
	////-------- Anisotropic filtering --------////
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Trilinear filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap addressing mode for texture coordinates outside 0->1
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
	samplerDesc.MaxAnisotropy = 4;                        // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
	samplerDesc.MinLOD = 0;                 // --"--

											// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(mD3DDevice->CreateSamplerState(&samplerDesc, &mAnisotropic4xSampler)))
	{
		mLastError = "Error creating anisotropic 4x sampler";
		return false;
	}
	
	//--------------------------------------------------------------------------------------
	// Rasterizer States
	//--------------------------------------------------------------------------------------
	// Rasterizer states adjust how triangles are filled in and when they are shown
	// Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	////-------- Back face culling --------////
	// This is the usual mode - don't show inside faces of objects
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; // Can also set this to wireframe - experiment if you wish
	rasterizerDesc.CullMode = D3D11_CULL_BACK;  // Setting that decides whether the "front" and "back" side of each
												// triangle is drawn or not. Culling back faces is the norm
	rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

										   // Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullBackState.p)))
	{
		mLastError = "Error creating cull-back state";
		return false;
	}
	
	////-------- Front face culling --------////
	// This is an unusual mode - it shows inside faces only so the model looks inside-out
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Remove front faces
	rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

										   // Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullFrontState.p)))
	{
		mLastError = "Error creating cull-front state";
		return false;
	}
	
	////-------- No culling --------////
	// Used for transparent or flat objects - show both sides of faces
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;  // Don't cull any faces
	rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

										   // Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullNoneState.p)))
	{
		mLastError = "Error creating cull-none state";
		return false;
	}
	
	//--------------------------------------------------------------------------------------
	// Blending States
	//--------------------------------------------------------------------------------------
	// Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See blending lab for details
	D3D11_BLEND_DESC blendDesc = {};

	////-------- Blending Off State --------////
	blendDesc.RenderTarget[0].BlendEnable = FALSE;              // Disable blending
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;      // How to blend the source (texture colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;     // How to blend the destination (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD

															  //** Leave the following settings alone, they are used only in highly unusual cases
															  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Then create a DirectX object for the description that can be used by a shader
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mNoBlendingState.p)))
	{
		mLastError = "Error creating no-blend state";
		return false;
	}
	
	////-------- Additive Blending State --------////
	blendDesc.RenderTarget[0].BlendEnable = TRUE;             // Enable blending
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;    // How to blend the source (texture colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;    // How to blend the destination (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // How to combine the above two, almost always ADD

															//** Leave the following settings alone, they are used only in highly unusual cases
															//** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Then create a DirectX object for the description that can be used by a shader
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mAdditiveBlendingState.p)))
	{
		mLastError = "Error creating additive blending state";
		return false;
	}


	/////-------Multiplicative --------------------////

	blendDesc.RenderTarget[0].BlendEnable = TRUE;              // Enable blending
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;      // How to blend the source (texture colour) - See lab notes
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;     // How to blend the destination (colour already on screen) - See lab notes
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;     // How to blend the destination (colour already on screen) - See lab notes
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD, leave this alone.

															  //** Leave the following settings alone, they are used only in highly unusual cases
															  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mMultiplicativeBlendingState.p)))
	{
		mLastError = "Error creating multiplicative state";
		return false;
	}

	////-------- Alpha Blending State --------////
	blendDesc.RenderTarget[0].BlendEnable = TRUE;             // Enable blending
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;    // How to blend the source (texture colour)
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;    // How to blend the destination (colour already on screen)
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // How to combine the above two, almost always ADD

															//** Leave the following settings alone, they are used only in highly unusual cases
															//** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Then create a DirectX object for the description that can be used by a shader
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mAlphaBlendingState.p)))
	{
		mLastError = "Error creating alpha blending state";
		return false;
	}
	//--------------------------------------------------------------------------------------
	// Depth-Stencil States
	//--------------------------------------------------------------------------------------
	// Depth-stencil states adjust how the depth and stencil buffers are used. The stencil buffer is rarely used so 
	// these states are most often used to switch the depth buffer on and off. See depth buffers lab for details
	// Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	////-------- Enable depth buffer --------////
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mUseDepthBufferState.p)))
	{
		mLastError = "Error creating use-depth-buffer state";
		return false;
	}
	
	////-------- Enable depth buffer reads only --------////
	// Disables writing to depth buffer - used for transparent objects because they should not be entered in the buffer but do need to check if they are behind something
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable writing to depth buffer
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthReadOnlyState.p)))
	{
		mLastError = "Error creating depth-read-only state";
		return false;
	}
	
	////-------- Disable depth buffer --------////
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	// Create a DirectX object for the description above that can be used by a shader
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mNoDepthBufferState.p)))
	{
		mLastError = "Error creating no-depth-buffer state";
		return false;
	}
	return true;
}

void CScene::RenderSceneFromCamera()
{
	mPerModelConstants = mEngine->GetModelConstants();

	// Set camera matrices in the constant buffer and send over to GPU

	mPerFrameConstants.viewMatrix = camera->ViewMatrix();
	mPerFrameConstants.projectionMatrix = camera->ProjectionMatrix();
	mPerFrameConstants.viewProjectionMatrix = camera->ViewProjectionMatrix();

	UpdateConstantBuffer(mPerFrameConstantBuffer.Get(), mPerFrameConstants, mD3DContext);

	//mPerFrameConstantBuffer = mEngine->GetFrameConstantBuffer();

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	mD3DContext->VSSetConstantBuffers(0, 1, mPerFrameConstantBuffer.GetAddressOf()); // First parameter must match constant buffer number in the shader 
	mD3DContext->PSSetConstantBuffers(0, 1, mPerFrameConstantBuffer.GetAddressOf());
	mD3DContext->GSSetConstantBuffers(0, 1, mPerFrameConstantBuffer.GetAddressOf());

	//// Render lit models - ground first ////

	// States - no blending, normal depth buffer and culling
	mD3DContext->OMSetBlendState(mNoBlendingState, nullptr, 0xffffff);
	mD3DContext->OMSetDepthStencilState(mUseDepthBufferState, 0); //If objects are behind other objects they will not get rendered
	mD3DContext->RSSetState(mCullBackState);


}
void CScene::RenderScene(float& frameTime)
{
	//Prepare	
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();//
	//ImGui::NewFrame();

	D3D11_VIEWPORT vp;
	allModels = mEngine->GetAllModels();
	allModels = Model::GetAllObjects();
	mEngine->SetAllModels(allModels);

	mEngine->Messages();
	mPerFrameConstants.cameraPosition = camera->Position();

	RenderShadow(vp);

	// Select the shadow map texture as the current depth buffer. We will not be rendering any pixel colours
	// Also clear the the shadow map depth buffer to the far distance
	//Only first light is casting shadows at this moment in time
	//mLights[3]->ClearDepthStencil(mD3DContext);
	//RenderDepthBufferFromLight(3);


	mLights[0]->ClearDepthStencil(mD3DContext);
	RenderDepthBufferFromLight(0);

	std::vector<IModel*> allCurrentModels = mEngine->GetAllModels();
	// Render models - no state changes required between each object in this situation (no textures used in this step)
	for (unsigned int i = 0; i < allCurrentModels.size(); ++i)
	{
		if (i == 0)
		{
			//This line effectively means, don't use any pixel shaders
			mD3DContext->PSSetShader(NULL, NULL, 0);//Get's rid of warning about pixel shader expecting render target view bound to 0...
		}
		allCurrentModels[i]->Render();
	}

	//// Render lights ////
	// Rendered with different shaders, textures, states from other models
	for (unsigned int i = 0; i < mLights.size(); ++i)
	{
		mLights[i]->RenderLight(mPerFrameConstants, mPerModelConstants);
	}

	//// Main scene rendering ////

	// Now set the back buffer as the target for rendering and select the main depth buffer.
	// When finished the back buffer is sent to the "front buffer" - which is the monitor.
	mBackBufferRenderTarget = mEngine->GetBackBufferRenderTarget();

	mD3DContext->OMSetRenderTargets(1, &mBackBufferRenderTarget, mEngine->GetDepthStencil());
	mD3DContext->ClearDepthStencilView(mEngine->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	mBackgroundColour = mEngine->GetBackgroundColour();

	// Clear the back buffer to a fixed colour and the depth buffer to the far distance
	mD3DContext->ClearRenderTargetView(mBackBufferRenderTarget, &mBackgroundColour.r);

	// Setup the viewport to the size of the main window
	vp.Width = static_cast<FLOAT>(gViewportWidth);
	vp.Height = static_cast<FLOAT>(gViewportHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mD3DContext->RSSetViewports(1, &vp);


	// Set shadow maps in shaders
	// First parameter is the "slot", must match the Texture2D declaration in the HLSL code
	// In this app the diffuse map uses slot 0, the shadow maps use slots 1 onwards. If we were using other maps (e.g. normal map) then
	// we might arrange things differently
	// Render the scene for the main window

	//mLights[3]->SendShadowMap2Shader(5, mD3DContext);
	mLights[0]->SendShadowMap2Shader(2, mD3DContext);

	RenderSceneFromCamera();
	RenderModels(frameTime);
	//mParticleSystem->Render();

	// Unbind shadow maps from shaders - prevents warnings from DirectX when we try to render to the shadow maps again next frame
	CComPtr<ID3D11ShaderResourceView> nullView = nullptr;
	CComPtr<ID3D11SamplerState> nullSampler = nullptr;
	mD3DContext->PSSetShaderResources(2, 1, &nullView);
	mD3DContext->PSSetSamplers(1, 1, &nullSampler);
	   
	UpdateScene(frameTime);


	mLights[0]->GetModel()->RotateY(maths::ToRadians(20.0f * frameTime));

	//ImGui::Begin("Settings");//Make new window
	//ImGui::SetWindowSize({ 230.0f, 250.0f });//Set the size of window

	//Create start button
	//if (ImGui::Button("Start Game", { 100.0f, 20.0f }))
	//{
	//	////Send start message when pressed
	//	//for (int i = 0; i < TankAs.size(); ++i)
	//	//{
	//	//	Messenger.SendMessageA(TankAs[i], msgStart);
	//	//	Messenger.SendMessageA(TankBs[i], msgStart);
	//	//}
	//	//gameStarted = true;
	//	mEngine->Stop();
	//}
	//ImGui::End();
	//ImGui::Render();
	//mD3DContext->OMSetRenderTargets(1, &mBackBufferRenderTarget, nullptr);

	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	mTotalTime += frameTime;

	if (mTotalTime < 15.0f)
	{
		mSpriteBatch->Begin();
		mFont->DrawString(mSpriteBatch.get(), L"CAUTION! EXTREME FLASHING LIGHTS", DirectX::XMFLOAT2(gViewportWidth / 3, gViewportHeight / 3));
		mFont->DrawString(mSpriteBatch.get(), L"This may cause seizures. Continue at your own risk!", DirectX::XMFLOAT2((gViewportWidth / 3) - 200, (gViewportHeight / 3) + 50));
		mSpriteBatch->End();
	}

	mEngine->GetSwapChain()->Present(0, 0);
}

void CScene::RenderModels(float& frameTime)
{
	//Add blending to models if required - Blending needs to be done last
	//Render each model
	for (unsigned int j = 0; j < allModels.size(); ++j)
	{
		float distFromCam = maths::Distance(allModels[j]->Position(), camera->Position());
		float maxRenderDist = 400.0f;
		//To ensure frame time doesn't drop too low models will only render within
		//a certain range and models should be loaded in order of importantance hence
		//the j < 3 part as they will be rendered no matter what
		if ( j < 15 || distFromCam < maxRenderDist)
		{
			if (allModels[j]->GetAddBlend() == Add)
			{
				// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
				mD3DContext->OMSetBlendState(mAdditiveBlendingState, nullptr, 0xffffff);
				mD3DContext->OMSetDepthStencilState(mDepthReadOnlyState, 0);
				mD3DContext->RSSetState(mCullBackState);
				allModels[j]->Render();


				//Change blending for the flare models.  (Additive by default)
				if (KeyHeld(Key_F1))
				{
					//NO BLENDING
					// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
					mD3DContext->OMSetBlendState(mNoBlendingState, nullptr, 0xffffff);
					mD3DContext->OMSetDepthStencilState(mDepthReadOnlyState, 0);
					mD3DContext->RSSetState(mCullBackState);
					allModels[j]->Render();
				}
			}
			else if (allModels[j]->GetAddBlend() == Multi)
			{
				//MULTIPLICATIVE BLENDING
				// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
				mD3DContext->OMSetBlendState(mMultiplicativeBlendingState, nullptr, 0xffffff);
				mD3DContext->OMSetDepthStencilState(mDepthReadOnlyState, 0);
				mD3DContext->RSSetState(mCullBackState);
				allModels[j]->Render();
			}
			else if (allModels[j]->GetAddBlend() == Alpha)
			{
				//Alpha BLENDING
				// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
				mD3DContext->OMSetBlendState(mAlphaBlendingState, nullptr, 0xffffff);
				mD3DContext->OMSetDepthStencilState(mDepthReadOnlyState, 0);
				mD3DContext->RSSetState(mCullBackState);
				allModels[j]->Render();
			}
			else
			{
				UpdateConstantBuffer(mEngine->GetModelConstantBuffer(), mEngine->GetModelConstants(), mD3DContext); // Send to GPU
				allModels[j]->Render();
				mD3DContext->RSSetState(mCullBackState);

				//Change blending for the flare models.  (Additive by default)
				if (KeyHeld(Key_F1))
				{
					//NO BLENDING
					// States - additive blending, read-only depth buffer and no culling (standard set-up for blending
					mD3DContext->OMSetBlendState(mAlphaBlendingState, nullptr, 0xffffff);
					mD3DContext->OMSetDepthStencilState(mDepthReadOnlyState, 0);
					mD3DContext->RSSetState(mCullBackState);
					allModels[j]->Render();
				}
			}
		}
	}
}

void CScene::RenderLights(std::vector<ILight*> lights)
{
	mLights = mEngine->GetAllLights();
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
}

void CScene::UpdateScene(float frameTime)
{
	//mParticleSystem->Update(frameTime);

	// Control camera (will update its view matrix)
	camera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);
	camera->SetPosition({ camera->Position().x, 10, camera->Position().z });

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

void CScene::RenderDepthBufferFromLight(int lightIndex)
{
	// Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
	mPerFrameConstants.viewMatrix = InverseAffine(mLights[lightIndex]->GetModel()->WorldMatrix());
	mPerFrameConstants.projectionMatrix = MakeProjectionMatrix(1.0f, acos(mPerFrameConstants.lightFacings[lightIndex].w) * 2.0f); // Helper function in Utility\GraphicsHelpers.cpp
	mPerFrameConstants.viewProjectionMatrix = mPerFrameConstants.viewMatrix * mPerFrameConstants.projectionMatrix;

	UpdateConstantBuffer(mPerFrameConstantBuffer.Get(), mPerFrameConstants, mD3DContext);

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	mD3DContext->VSSetConstantBuffers(0, 1, mPerFrameConstantBuffer.GetAddressOf()); // First parameter must match constant buffer number in the shader 
	mD3DContext->PSSetConstantBuffers(0, 1, mPerFrameConstantBuffer.GetAddressOf());
	
	//// Only render models that cast shadows ////

	// Use special depth-only rendering shaders
	mD3DContext->VSSetShader(mBasicPixel, nullptr, 0);
	mD3DContext->PSSetShader(mDepthOnly, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	mD3DContext->OMSetBlendState(mNoBlendingState, nullptr, 0xffffff);
	mD3DContext->OMSetDepthStencilState(mUseDepthBufferState, 0);
	mD3DContext->RSSetState(mCullBackState);

	mD3DContext->PSSetSamplers(0, 1, &mAnisotropic4xSampler);
	mD3DContext->PSSetSamplers(1, 1, &mPointSampler);
}

void CScene::ReleaseResources()
{
	mPerFrameConstantBuffer.Get()->Release();
	mPerModelConstantBuffer.Get()->Release();
}

}