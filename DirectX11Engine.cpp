#include "DirectX11Engine.hpp"

namespace umbra_engine
{

CDX11Engine::CDX11Engine(_In_     HINSTANCE hInstance,
						 _In_opt_ HINSTANCE hPrevInstance,
						 _In_     LPWSTR    lpCmdLine,
						 _In_     int       nCmdShow)
{

	mhInstance = hInstance;
	mhPrevInstance = hPrevInstance;
	mlpCmdLine = lpCmdLine;
	mnCmdShow = nCmdShow;
	mEngineRunning = false;
}

bool InitWindow(HINSTANCE& hInstance, int nCmdShow, HWND& hWnd)
{
	// Get a stock icon to show on the taskbar for this program.
	SHSTOCKICONINFO stockIcon;
	stockIcon.cbSize = sizeof(stockIcon);
	if (SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON, &stockIcon) != S_OK) // Returns false on failure
	{
		return false;
	}

	// Register window class. Defines various UI features of the window for our application.
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;    // Which function deals with windows messages
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0; SIID_APPLICATION;
	wcex.hInstance = hInstance;
	wcex.hIcon = stockIcon.hIcon; // Which icon to use for the window
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // What cursor appears over the window
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"CO2409WindowClass";
	wcex.hIconSm = stockIcon.hIcon;
	if (!RegisterClassEx(&wcex)) // Returns false on failure
	{
		return false;
	}


	// Select the type of window to show our application in
	DWORD windowStyle = WS_OVERLAPPEDWINDOW; // Standard window
											 //DWORD windowStyle = WS_POPUP;          // Alternative: borderless. If you also set the viewport size to the monitor resolution, you 
											 // get a "fullscreen borderless" window, which works better with alt-tab than DirectX fullscreen,
											 // which is an option in Direct3DSetup.cpp. DirectX fullscreen has slight better performance though.

											 // Calculate overall dimensions for the window. We will render to the *inside* of the window. But the
											 // overall winder will be larger because it includes the borders, title bar etc. This code calculates
											 // the overall size of the window given our choice of viewport size.
	RECT rc = { 0, 0, gViewportWidth, gViewportHeight };
	AdjustWindowRect(&rc, windowStyle, FALSE);

	// Create window, the second parameter is the text that appears in the title bar
	//gHInst = hInstance;
	hWnd = CreateWindow(L"CO2409WindowClass", L"Direct3D 11", windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return false;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT: // A necessary message to ensure the window content is displayed
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY: // Another necessary message to deal with the window being closed
		PostQuitMessage(0);
		break;


		// The WM_KEYXXXX messages report keyboard input to our window.
		// This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
		// so you don't need to change this code. Instead simply use KeyHit, KeyHeld etc.
	case WM_KEYDOWN:
		KeyDownEvent(static_cast<KeyCode>(wParam));
		break;

	case WM_KEYUP:
		KeyUpEvent(static_cast<KeyCode>(wParam));
		break;


		// The following WM_XXXX messages report mouse movement and button presses
		// Use KeyHit to get mouse buttons, GetMouseX, GetMouseY for its position
	case WM_MOUSEMOVE:
	{
		MouseMoveEvent(LOWORD(lParam), HIWORD(lParam));

		break;
	}
	case WM_LBUTTONDOWN:
	{
		KeyDownEvent(Mouse_LButton);
		break;
	}
	case WM_LBUTTONUP:
	{
		KeyUpEvent(Mouse_LButton);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		KeyDownEvent(Mouse_RButton);
		break;
	}
	case WM_RBUTTONUP:
	{
		KeyUpEvent(Mouse_RButton);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		KeyDownEvent(Mouse_MButton);
		break;
	}
	case WM_MBUTTONUP:
	{
		KeyUpEvent(Mouse_MButton);
		break;
	}


	// Any messages we don't handle are passed back to Windows default handling
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void CDX11Engine::StartWindowed()
{
	// Create a window to display the scene

	if (!InitWindow(mhInstance, mnCmdShow, mHWnd))
	{
		MessageBoxA(mHWnd, mLastError.c_str(), NULL, MB_OK);
		return;
	}
	SetCapture(mHWnd);
	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);

	// Initialise Direct3D
	if (!InitDirect3D())
	{
		MessageBoxA(mHWnd, mLastError.c_str(), NULL, MB_OK);
		return;
	}
	myScene = std::make_unique<CScene>(this);
	mPerFrameConstants = myScene->GetFrameConstants();
}

void CDX11Engine::Messages()
{
	//Once per frame code**
	// Check for and deal with any window messages (input, window resizing, minimizing, etc.).
	// The actual message processing happens in the function WndProc below
	if (PeekMessage(&mMsg, nullptr, 0, 0, PM_REMOVE))
	{
		// Deal with messages
		TranslateMessage(&mMsg);
		DispatchMessage(&mMsg);
	}
}

void CDX11Engine::Delete()
{
	// Release everything before quitting
	myScene->ReleaseResources();
	ShutdownDirect3D();
}

void CDX11Engine::Stop()
{
	//Once per frame code**
	ReleaseCapture();
	DestroyWindow(mHWnd); // This will close the window and ultimately exit this loop
}

void CDX11Engine::InitEngine()
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

	// Main message loop - this is a Windows equivalent of the loop in a TL-Engine application
	mMsg = {};
}

bool CDX11Engine::IsRunning()
{
	if (mMsg.message != WM_QUIT)// As long as window is open
	{
		mEngineRunning = true;
	}
	else
	{
		mEngineRunning = false;
	}
	return mEngineRunning;
}

IMesh* CDX11Engine::LoadMesh(const std::string& modelFile)
{
	++mMeshAmount;
	std::unique_ptr<IMesh> newMesh = nullptr;

	//Checks for whether the file is inside a folder
	bool directory = false;
	const char slash = '\\';
	for (unsigned int i = 0; i < mMediaFolders.size(); ++i)
	{
		for (unsigned int j = 0; j < 2; ++j)
		{
			if (mMediaFolders[i][mMediaFolders[i].size() - j] == slash)
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
			newMesh = std::make_unique<Mesh>(mMediaFolders[i] + modelFile, this, true);
		}
		else
		{
			newMesh = std::make_unique<Mesh>(mMediaFolders[i] + slash + modelFile, this, true);
		}
	}

	if (newMesh == nullptr)
	{
		newMesh = std::make_unique<Mesh>(modelFile, this, true);
	}

	mAllMeshes.push_back(std::move(newMesh));

	if (mMediaFolders.size() > 0)
	{
		mAllMeshes[0]->AddFolders(mMediaFolders);
	}

	return mAllMeshes[mMeshAmount - 1].get();
}

void CDX11Engine::AddMediaFolder(const std::string& newFolder)
{
	mMediaFolders.push_back(newFolder);
}

void CDX11Engine::RemoveMediaFolder(const std::string& rogueFolder)
{
	for (auto i = mMediaFolders.begin(); i != mMediaFolders.end(); ++i)
	{
		if (*i == rogueFolder)
		{
			mMediaFolders.erase(i);
		}
	}
}

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
	dbDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Each depth value is a single float 
											  // Important point for when using depth buffer as texture, must use the TYPELESS constant shown here
	dbDesc.SampleDesc.Count = 1;
	dbDesc.SampleDesc.Quality = 0;
	dbDesc.Usage = D3D11_USAGE_DEFAULT;
	dbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
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
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = mD3DDevice->CreateDepthStencilView(mDepthStencilTexture, &dsvDesc,
		&mDepthStencil);
	if (FAILED(hr))
	{
		mLastError = "Error creating depth buffer view";
		return false;
	}
	// Also create a shader resource view for the depth buffer - required when we want to access the depth buffer as a texture (also note the two important comments in above code)
	// Note the veryt 
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	descSRV.Format = DXGI_FORMAT_R32_FLOAT;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = 1;
	descSRV.Texture2D.MostDetailedMip = 0;
	hr = mD3DDevice->CreateShaderResourceView(mDepthStencilTexture, &descSRV, &mDepthShaderView);
	if (FAILED(hr))
	{
		mLastError = "Error creating depth buffer shader resource view";
		return false;
	}


	return true;
}

void CDX11Engine::ShutdownDirect3D()
{
	// Release each Direct3D object to return resources to the system. Missing these out will cause memory
	// leaks. Check documentation to see which objects need to be released when adding new features in your
	// own projects.
	if (mD3DContext)
	{
		mD3DContext->ClearState(); // This line is also needed to reset the GPU before shutting down DirectX
	}
}

ILight* CDX11Engine::CreateLight(ELightType type)
{
	try
	{
		++mLightAmount;
		std::unique_ptr<ILight> newLight = nullptr;
		if (type == Point)
		{
			newLight = std::make_unique<CPointLight>(this, type);

		}
		else if (type == Directional || type == Spot)
		{
			newLight = std::make_unique<Light>(this, type);

		}
		mAllLights.push_back(std::move(newLight));
		return mAllLights[mLightAmount - 1].get();
	}
	catch (std::exception& e)
	{
		std::string s = e.what();
		std::wstring str = std::wstring(s.begin(), s.end());

		MessageBox(GetHWnd(), str.c_str(), NULL, MB_OK);
	}
	return nullptr;
}

IGui* CDX11Engine::CreateGUI()
{
	return new CImGui(mD3DContext, mBackBufferRenderTarget);
}

std::vector<ILight*> CDX11Engine::GetAllLights()
{
	std::vector<ILight*> lights;
	for (int i = 0; i < mAllLights.size(); ++i)
	{
		lights.push_back(mAllLights[i].get());
	}
	return lights;
}

void CDX11Engine::SetShadowEffect(EShadowEffect& setEffect)
{
	mPerFrameConstants.shadowEffect = static_cast<int>(setEffect);
	myScene->SetFrameConstants(mPerFrameConstants);
}

}