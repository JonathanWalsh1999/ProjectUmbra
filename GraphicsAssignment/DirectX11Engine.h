#pragma once

//C++ libraries
#include <vector>
#include <sstream>
#include <iterator>
#include <windows.h>
#include <string>

//Allows different texture formats to be used
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <atlbase.h> // C-std::string to unicode conversion function CA2CT

//Graphics Component headers
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Light.h"
#include "Scene.h"

//Graphics helpers
#include "Shader.h"

#include "ColourRGBA.h" 
#include "Input.h"
#include "Timer.h"

//Definitions and point of communication with graphics side
#include "Common.h"

#include <cmath>
#include <cctype>


class Light;
class CScene;

class CDX11Engine
{
public:
	CDX11Engine(_In_     HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_     LPWSTR    lpCmdLine,
		_In_     int       nCmdShow);
	CDX11Engine();

	~CDX11Engine();

	//-------------------------------------------------------------------------------------
	// Direct3D Setup
	//-------------------------------------------------------------------------------------
	// Returns false on failure
	bool InitDirect3D();
	// Release the memory held by all objects created
	void ShutdownDirect3D();	

	Mesh* LoadMesh(const std::string& file);//Load Meshes from file

	//Major engine methods

	void EngineRun();
	bool IsRunning();
	void StartWindowed();
	void Messages();
	void Delete();
	void Stop();

	//Media folder methods
	std::vector<std::string> GetMediaFolders();
	void AddMediaFolder(const std::string& newFolder);
	void RemoveMediaFolder(const std::string& rogueFolder);
	

	//--------------------------------------------------------------------------------------
	// State creation / destruction
	//--------------------------------------------------------------------------------------

	Light* CreateLight();
	// Create all the states used in this app, returns true on success
	bool CreateStates();

	// Release DirectX state objects
	void ReleaseStates();

	//--------------------------------------------------------------------------------------
	// Constant buffers
	//--------------------------------------------------------------------------------------

	// Template function to update a constant buffer. Pass the DirectX constant buffer object and the C++ data structure
	// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
	// be available to shaders. This is used to update model and camera positions, lighting data etc.
	template <class T>
	inline void UpdateConstantBuffer(ID3D11Buffer* buffer, const T& bufferData)
	{
		D3D11_MAPPED_SUBRESOURCE cb;

		mD3DContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &bufferData, sizeof(T));
		mD3DContext->Unmap(buffer, 0);
	}


	//--------------------------------------------------------------------------------------
	// Texture Loading
	//--------------------------------------------------------------------------------------

	// Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify file loading
	// This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
	// texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
	// The function will fill in these pointers with usable data. Returns false on failure
	bool LoadTexture(std::string filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureSRV, CDX11Engine* engine);


	// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
	// - Aspect ratio is screen width / height (like 4:3, 16:9)
	// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
	// - near and far clip are the range of z distances that can be rendered
	CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 16.0f / 9.0f, float FOVx = ToRadians(60),
		float nearClip = 0.1f, float farClip = 10000.0f);


	//Getters
	PerFrameConstants GetFrameConstants();
	PerModelConstants GetModelConstants();

	ID3D11Buffer* GetFrameConstantBuffer();
	ID3D11Buffer* GetModelConstantBuffer();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();

	ID3D11BlendState* GetNoBlendState();
	ID3D11BlendState* GetAddBlendState();
	ID3D11BlendState* GetMultiBlendState();
	ID3D11BlendState* GetAlphaBlendState();

	ID3D11RasterizerState* GetCullBackState();

	ID3D11DepthStencilState* GetDepthBufferState();

	ID3D11RenderTargetView* GetBackBufferRenderTarget();

	ID3D11DepthStencilView* GetDepthStencil();

	ID3D11SamplerState* GetPointSampler();


	ColourRGBA GetBackgroundColour();

	IDXGISwapChain* GetSwapChain();

	ID3D11SamplerState* GetAnisotropic4xSampler();

	ID3D11DepthStencilState* GetDepthReadOnlyState();

	HWND GetHWnd();

	std::vector<Model*> GetAllModels();
	std::vector<Light*> GetAllLights();

	std::vector<ID3D11Resource*> GetDiffuseSpecularMaps();
	std::vector<ID3D11ShaderResourceView*> GetDiffuseSpecularMapSRVs();

	CScene* GetScene();

	//Setters
	void SetFrameConstants(PerFrameConstants& constants);
	void SetModelConstants(PerModelConstants& constants);

	void SetAllModels(std::vector<Model*>& models);
	void SetAllLights(std::vector<Light*>& lights);

	void SetScene(CScene* scene);

private:

	//
	HWND mHWnd;
	CScene* myScene;
	std::vector<Light*> mLight;
	std::vector<Model*> allModels;

	std::vector<ID3D11Resource*> diffuseSpecularMaps;
	std::vector<ID3D11ShaderResourceView*> diffuseSpecularMapSRVs;

	//Graphics Variables
	ColourRGBA gBackgroundColor = { 0.2f, 0.2f, 0.3f , 1.0f };

	float mFrameTime;//The time it takes to render one frame.

	//Other graphics variables
	_In_     HINSTANCE mhInstance;
	_In_opt_ HINSTANCE mhPrevInstance;
	_In_     LPWSTR    mlpCmdLine;
	_In_     int      mnCmdShow;
	bool engineRunning;

	ID3D11Resource*           gLightDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;

	//Important DX vars
	// Depth buffer (can also contain "stencil" values, which we will see later)
	ID3D11Texture2D*        mDepthStencilTexture = nullptr; // The texture holding the depth values
	ID3D11DepthStencilView* mDepthStencil = nullptr; // The depth buffer referencing above texture
													 // The main Direct3D (D3D) variables
	ID3D11Device*        mD3DDevice = nullptr; // D3D device for overall features
	ID3D11DeviceContext* mD3DContext = nullptr; // D3D context for specific rendering tasks

												// Swap chain and back buffer
	IDXGISwapChain*         mSwapChain = nullptr;
	ID3D11RenderTargetView* mBackBufferRenderTarget = nullptr;


	// GPU "States" //
	ID3D11SamplerState* mPointSampler = nullptr;
	ID3D11SamplerState* mTrilinearSampler = nullptr;
	ID3D11SamplerState* mAnisotropic4xSampler = nullptr;

	ID3D11BlendState* mNoBlendingState = nullptr;
	ID3D11BlendState* mAdditiveBlendingState = nullptr;
	ID3D11BlendState* mMultiplicativeBlendingState = nullptr;
	ID3D11BlendState* mAlphaBlendingState = nullptr;

	ID3D11RasterizerState*   mCullBackState = nullptr;
	ID3D11RasterizerState*   mCullFrontState = nullptr;
	ID3D11RasterizerState*   mCullNoneState = nullptr;

	ID3D11DepthStencilState* mUseDepthBufferState = nullptr;
	ID3D11DepthStencilState* mDepthReadOnlyState = nullptr;
	ID3D11DepthStencilState* mNoDepthBufferState = nullptr;



	ID3D11Buffer*     mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure

	PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
	ID3D11Buffer*     mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure
	//

	std::vector<ID3D11ShaderResourceView*> GetSRVMaps();
	std::vector<std::string> mediaFolders;

	MSG msg;


	const std::string mediaFolder = "C:\\ProgramData\\TL-Engine\\Media";
	const std::string localMediaFolder = "\\media";

	Camera* camera;
	Model* light;
	Mesh* lightMesh;

	//SHADOWS
	int mShadowMapSize = 256;

	ID3D11Texture2D*          mShadowMapTexture = nullptr; // This object represents the memory used by the texture on the GPU
	ID3D11DepthStencilView*   mShadowMapDepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
	ID3D11ShaderResourceView* mShadowMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)


	ID3D11PixelShader* mDepthOnly = nullptr;
	ID3D11VertexShader* mBasicPixel = nullptr;

	std::string mLastError;


	std::vector<Mesh*> allMeshes;


	//PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above


};

void main(CDX11Engine* myEngine);
