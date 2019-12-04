#include "DirectX11Engine.hpp"
#include "GraphicsHelpers.hpp"


//PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above
//ID3D11Buffer*     mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure

//PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
//ID3D11Buffer*     mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure

CDX11Engine::CDX11Engine(_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nCmdShow)
{

	mhInstance = hInstance;
	mhPrevInstance = hPrevInstance;
	mlpCmdLine = lpCmdLine;
	mnCmdShow = nCmdShow;
	engineRunning = false;

}

CDX11Engine::CDX11Engine()
{

}


void CDX11Engine::StartWindowed()
{
	// Create a window to display the scene

	if (!InitWindow(mhInstance, mnCmdShow, mHWnd))
	{		
		return;
	}

	// Initialise Direct3D
	if (!InitDirect3D())
	{
		MessageBoxA(mHWnd, mLastError.c_str(), NULL, MB_OK);
		return;
	}
	myScene = new CScene(this);
	mPerFrameConstants = myScene->GetFrameConstants();
}

void CDX11Engine::Messages()
{
	//Once per frame code**

	// Check for and deal with any window messages (input, window resizing, minimizing, etc.).
	// The actual message processing happens in the function WndProc below
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Deal with messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

void CDX11Engine::Delete()
{
	// Release everything before quitting

	for (unsigned int i = 0; i < allMeshes.size(); ++i)
	{
		delete allMeshes[i];
	}
	for (unsigned int i = 0; i < allModels.size(); ++i)
	{
		delete allModels[i];
	}
	for (unsigned int i = 0; i < mLight.size(); ++i)
	{
		delete mLight[i];
	}

	myScene->ReleaseResources();
	ShutdownDirect3D();
}

void CDX11Engine::Stop()
{
	//Once per frame code**
	DestroyWindow(mHWnd); // This will close the window and ultimately exit this loop
}

void CDX11Engine::EngineRun()
{
	UNREFERENCED_PARAMETER(mhPrevInstance); // Stop warnings when not using some parameters
	UNREFERENCED_PARAMETER(mlpCmdLine);


	// Prepare TL-Engine style input functions
	InitInput();


	// Initialise scene
	if (!myScene->InitGeometry() || !myScene->InitScene())
	{
		MessageBoxA(mHWnd, mLastError.c_str(), NULL, MB_OK);
		myScene->ReleaseResources();
		ShutdownDirect3D();
		return;
	}


	mPerFrameConstantBuffer = myScene->GetFrameConstantBuffer();
	mPerModelConstantBuffer = myScene->GetModelConstantBuffer();
	mPerFrameConstants = myScene->GetFrameConstants();
	mPerModelConstants = myScene->GetModelConstants();
	camera = myScene->GetCamera();


	// Main message loop - this is a Windows equivalent of the loop in a TL-Engine application
	msg = {};
}

bool CDX11Engine::IsRunning()
{
	if (msg.message != WM_QUIT)// As long as window is open
	{
		engineRunning = true;
	}
	else
	{
		engineRunning = false;
	}
	return engineRunning;
}

CDX11Engine::~CDX11Engine()
{

}

std::vector<ID3D11ShaderResourceView*> CDX11Engine::GetSRVMaps()
{
	return diffuseSpecularMapSRVs;
}

IMesh* CDX11Engine::LoadMesh(const std::string& modelFile)
{	
	IMesh* newMesh = nullptr;

	//Checks for whether the file is inside a folder
	bool directory = false;
	const char slash = '\\';
	for (unsigned int i = 0; i < mediaFolders.size(); ++i)
	{
		for (unsigned int j = 0; j < 2; ++j)
		{
			if (mediaFolders[i][mediaFolders[i].size() - j] == slash)
			{
				directory = true;
			}
			else
			{
				directory = false;
			}
		}
		if (directory)
		{
			newMesh = new Mesh(mediaFolders[i] + modelFile, this, true);
		}
		else
		{
			newMesh = new Mesh(mediaFolders[i] + slash + modelFile, this, true);
		}
	}

	if (newMesh == nullptr)
	{
		newMesh = new Mesh(modelFile, this,true);
	}

	allMeshes.push_back(newMesh);

	if (mediaFolders.size() > 0)
	{
		allMeshes[0]->AddFolders(mediaFolders);
	}

	return newMesh;
}

std::vector<std::string> CDX11Engine::GetMediaFolders()
{
	return mediaFolders;
}

void CDX11Engine::AddMediaFolder(const std::string& newFolder)
{
	mediaFolders.push_back(newFolder);
}

void CDX11Engine::RemoveMediaFolder(const std::string& rogueFolder)
{
	for (auto i = mediaFolders.begin(); i != mediaFolders.end(); ++i)
	{
		if (*i == rogueFolder)
		{
			mediaFolders.erase(i);
		}
	}
}

//--------------------------------------------------------------------------------------
// Initialise / uninitialise Direct3D
//--------------------------------------------------------------------------------------
// Returns false on failure
bool CDX11Engine::InitDirect3D()
{
	// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
	// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
	HRESULT hr = S_OK;


	//// Initialise DirectX ////

	// Create a Direct3D device (i.e. initialise D3D) and create a swap-chain (create a back buffer to render to)
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.OutputWindow = mHWnd;                           // Target window
	swapDesc.Windowed = TRUE;
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = gViewportWidth;             // Target window size
	swapDesc.BufferDesc.Height = gViewportHeight;            // --"--
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;        // Refresh rate of monitor (provided as fraction = 60/1 here)
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	UINT flags = D3D11_CREATE_DEVICE_DEBUG; // Set this to D3D11_CREATE_DEVICE_DEBUG to get more debugging information (in the "Output" window of Visual Studio)
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
		&swapDesc, &mSwapChain, &mD3DDevice, nullptr, &mD3DContext);
	if (FAILED(hr))
	{
		mLastError = "Error creating Direct3D device";
		return false;
	}


	// Get a "render target view" of back-buffer - standard behaviour
	ID3D11Texture2D* backBuffer;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		mLastError = "Error creating swap chain";
		return false;
	}
	hr = mD3DDevice->CreateRenderTargetView(backBuffer, NULL, &mBackBufferRenderTarget);
	backBuffer->Release();
	if (FAILED(hr))
	{
		mLastError = "Error creating render target view";
		return false;
	}


	//// Create depth buffer to go along with the back buffer ////

	// First create a texture to hold the depth buffer values
	D3D11_TEXTURE2D_DESC dbDesc = {};
	dbDesc.Width = gViewportWidth; // Same size as viewport / back-buffer
	dbDesc.Height = gViewportHeight;
	dbDesc.MipLevels = 1;
	dbDesc.ArraySize = 1;
	dbDesc.Format = DXGI_FORMAT_D32_FLOAT; // Each depth value is a single float
	dbDesc.SampleDesc.Count = 1;
	dbDesc.SampleDesc.Quality = 0;
	dbDesc.Usage = D3D11_USAGE_DEFAULT;
	dbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dbDesc.CPUAccessFlags = 0;
	dbDesc.MiscFlags = 0;
	hr = mD3DDevice->CreateTexture2D(&dbDesc, nullptr, &mDepthStencilTexture);
	if (FAILED(hr))
	{
		mLastError = "Error creating depth buffer texture";
		return false;
	}

	// Create the depth stencil view - an object to allow us to use the texture
	// just created as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = dbDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = mD3DDevice->CreateDepthStencilView(mDepthStencilTexture, &dsvDesc,
		&mDepthStencil);
	if (FAILED(hr))
	{
		mLastError = "Error creating depth buffer view";
		return false;
	}

	return true;
}


// Release the memory held by all objects created
void CDX11Engine::ShutdownDirect3D()
{
	// Release each Direct3D object to return resources to the system. Missing these out will cause memory
	// leaks. Check documentation to see which objects need to be released when adding new features in your
	// own projects.
	if (mD3DContext)
	{
		mD3DContext->ClearState(); // This line is also needed to reset the GPU before shutting down DirectX
		mD3DContext->Release();
	}
	if (mDepthStencil)           mDepthStencil->Release();
	if (mDepthStencilTexture)    mDepthStencilTexture->Release();
	if (mBackBufferRenderTarget) mBackBufferRenderTarget->Release();
	if (mSwapChain)              mSwapChain->Release();
	if (mD3DDevice)              mD3DDevice->Release();
	if (gLightDiffuseMap) gLightDiffuseMap->Release();
	if (mShadowMapTexture) mShadowMapTexture->Release();
	if (mShadowMapDepthStencil) mShadowMapDepthStencil->Release();
	if (mShadowMapSRV) mShadowMapSRV->Release();
	if (mDepthOnly) mDepthOnly->Release();
	if (mBasicPixel) mBasicPixel->Release();

	
}

//--------------------------------------------------------------------------------------
// State creation / destruction
//--------------------------------------------------------------------------------------

// Create all the states used in this app, returns true on success
bool CDX11Engine::CreateStates()
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
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullBackState)))
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
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullFrontState)))
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
	if (FAILED(mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mCullNoneState)))
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
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mNoBlendingState)))
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
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mAdditiveBlendingState)))
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
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mMultiplicativeBlendingState)))
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
	if (FAILED(mD3DDevice->CreateBlendState(&blendDesc, &mAlphaBlendingState)))
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
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mUseDepthBufferState)))
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
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthReadOnlyState)))
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
	if (FAILED(mD3DDevice->CreateDepthStencilState(&depthStencilDesc, &mNoDepthBufferState)))
	{
		mLastError = "Error creating no-depth-buffer state";
		return false;
	}

	return true;
}


// Release DirectX state objects
void CDX11Engine::ReleaseStates()
{
	if (mUseDepthBufferState)    mUseDepthBufferState->Release();
	if (mDepthReadOnlyState)     mDepthReadOnlyState->Release();
	if (mNoDepthBufferState)     mNoDepthBufferState->Release();
	if (mCullBackState)          mCullBackState->Release();
	if (mCullFrontState)         mCullFrontState->Release();
	if (mCullNoneState)          mCullNoneState->Release();
	if (mNoBlendingState)        mNoBlendingState->Release();
	if (mAdditiveBlendingState)  mAdditiveBlendingState->Release();
	if (mAnisotropic4xSampler)   mAnisotropic4xSampler->Release();
	if (mTrilinearSampler)       mTrilinearSampler->Release();
	if (mPointSampler)           mPointSampler->Release();
	if (mMultiplicativeBlendingState) mMultiplicativeBlendingState->Release();
	if (mAlphaBlendingState) mAlphaBlendingState->Release();
	
}

ILight* CDX11Engine::CreateLight(ELightType type)
{	
	try
	{
		ILight* newLight = new Light(this, type);
		return newLight;
	}
	catch (std::exception& e)
	{
		std::string s = e.what();
		std::wstring str = std::wstring(s.begin(), s.end());

		MessageBox(GetHWnd(), str.c_str(), NULL, MB_OK);
	}
	return nullptr;
}


//--------------------------------------------------------------------------------------
// Texture Loading
//--------------------------------------------------------------------------------------

// Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify texture loading
// This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
// texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
// The function will fill in these pointers with usable data. Returns false on failure
bool CDX11Engine::LoadTexture(std::string filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureSRV)
{
	// DDS files need a different function from other files
	std::string dds = ".dds"; // So check the filename extension (case insensitive)
	if (filename.size() >= 4 &&
		std::equal(dds.rbegin(), dds.rend(), filename.rbegin(), [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); }))
	{
		return SUCCEEDED(DirectX::CreateDDSTextureFromFile(mD3DDevice, CA2CT(filename.c_str()), texture, textureSRV));
	}
	else
	{
		return SUCCEEDED(DirectX::CreateWICTextureFromFile(mD3DDevice, mD3DContext, CA2CT(filename.c_str()), texture, textureSRV));
	}
}


//--------------------------------------------------------------------------------------
// Camera Helpers
//--------------------------------------------------------------------------------------

// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 CDX11Engine::MakeProjectionMatrix(float aspectRatio /*= 4.0f / 3.0f*/, float FOVx /*= ToRadians(60)*/,
	float nearClip /*= 0.1f*/, float farClip /*= 10000.0f*/)
{
	float tanFOVx = std::tan(FOVx * 0.5f);
	float scaleX = 1.0f / tanFOVx;
	float scaleY = aspectRatio / tanFOVx;
	float scaleZa = farClip / (farClip - nearClip);
	float scaleZb = -nearClip * scaleZa;

	return CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
						 0.0f, scaleY,    0.0f,   0.0f,
						 0.0f,   0.0f, scaleZa,   1.0f,
						 0.0f,   0.0f, scaleZb,   0.0f };
}

//Getters
PerFrameConstants CDX11Engine::GetFrameConstants()
{
	return mPerFrameConstants;
}
PerModelConstants CDX11Engine::GetModelConstants()
{
	return mPerModelConstants;
}

ID3D11Buffer* CDX11Engine::GetFrameConstantBuffer()
{
	return mPerFrameConstantBuffer;
}
ID3D11Buffer* CDX11Engine::GetModelConstantBuffer()
{
	return mPerModelConstantBuffer;
}

ID3D11Device* CDX11Engine::GetDevice()
{
	return mD3DDevice;
}
ID3D11DeviceContext* CDX11Engine::GetContext()
{
	return mD3DContext;
}

ID3D11BlendState* CDX11Engine::GetNoBlendState()
{
	return mNoBlendingState;
}
ID3D11BlendState* CDX11Engine::GetAddBlendState()
{
	return mAdditiveBlendingState;
}

ID3D11BlendState* CDX11Engine::GetMultiBlendState()
{
	return mMultiplicativeBlendingState;
}

ID3D11BlendState* CDX11Engine::GetAlphaBlendState()
{
	return mAlphaBlendingState;
}

ID3D11RasterizerState* CDX11Engine::GetCullBackState()
{
	return mCullBackState;
}

ID3D11DepthStencilState* CDX11Engine::GetDepthBufferState()
{
	return mUseDepthBufferState;
}

ID3D11RenderTargetView* CDX11Engine::GetBackBufferRenderTarget()
{
	return mBackBufferRenderTarget;
}

ID3D11DepthStencilView* CDX11Engine::GetDepthStencil()
{
	return mDepthStencil;
}

ID3D11SamplerState* CDX11Engine::GetPointSampler()
{
	return mPointSampler;
}


ColourRGBA CDX11Engine::GetBackgroundColour()
{
	return gBackgroundColor;
}

IDXGISwapChain* CDX11Engine::GetSwapChain()
{
	return mSwapChain;
}

ID3D11SamplerState* CDX11Engine::GetAnisotropic4xSampler()
{
	return mAnisotropic4xSampler;
}

ID3D11DepthStencilState* CDX11Engine::GetDepthReadOnlyState()
{
	return mDepthReadOnlyState;
}

HWND CDX11Engine::GetHWnd()
{
	return mHWnd;
}

std::vector<IModel*> CDX11Engine::GetAllModels()
{
	return allModels;
}
std::vector<ILight*> CDX11Engine::GetAllLights()
{
	return mLight;
}

std::vector<ID3D11Resource*> CDX11Engine::GetDiffuseSpecularMaps()
{
	return diffuseSpecularMaps;
}
std::vector<ID3D11ShaderResourceView*> CDX11Engine::GetDiffuseSpecularMapSRVs()
{
	return diffuseSpecularMapSRVs;
}
IScene* CDX11Engine::GetScene()
{
	return myScene;
}

//Setters
void CDX11Engine::SetFrameConstants(PerFrameConstants& constants)
{
	mPerFrameConstants = constants;
}
void CDX11Engine::SetModelConstants(PerModelConstants& constants)
{
	mPerModelConstants = constants;
}

void CDX11Engine::SetAllModels(std::vector<IModel*>& models)
{
	allModels = models;
}
void CDX11Engine::SetAllLights(std::vector<ILight*>& lights)
{
	mLight = lights;
}
void CDX11Engine::SetScene(IScene* scene)
{
	myScene = scene;
}

void CDX11Engine::SetShadowEffect(EShadowEffect& setEffect)
{
	mPerFrameConstants.shadowEffect = static_cast<int>(setEffect);
	myScene->SetFrameConstants(mPerFrameConstants);
}
