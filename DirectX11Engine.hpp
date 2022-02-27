#ifndef _CDX11ENGINE_H_
#define _CDX11ENGINE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for the main operations of the DirectX engine
//--------------------------------------------------------------------------------------

#include "IEngine.hpp"
#include <memory>

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class Light;
class CScene;

class CDX11Engine : public IEngine
{
public:
//---------------------------------------
// Constructors / Destructor
//---------------------------------------
	CDX11Engine(_In_     HINSTANCE hInstance,
		        _In_opt_ HINSTANCE hPrevInstance,
				_In_     LPWSTR    lpCmdLine,
				_In_     int       nCmdShow);
	CDX11Engine() = delete;
	~CDX11Engine() = default;

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	PerModelConstants GetModelConstants()				{ return mPerModelConstants; }
	ID3D11Buffer* GetModelConstantBuffer()				{ return mPerModelConstantBuffer; }
	ID3D11Device* GetDevice()							{ return mD3DDevice; }
	ID3D11DeviceContext* GetContext()					{ return mD3DContext; }
	ID3D11RenderTargetView* GetBackBufferRenderTarget() { return mBackBufferRenderTarget; }
	ID3D11DepthStencilView* GetDepthStencil()			{ return mDepthStencil; }
	ColourRGBA GetBackgroundColour()					{ return mBackgroundColor; }
	IDXGISwapChain* GetSwapChain()						{ return mSwapChain; }
	HWND GetHWnd()										{ return mHWnd; }
	std::vector<IModel*> GetAllModels()					{ return mAllModels; }
	std::vector<ILight*> GetAllLights();
	IScene* GetScene()									{ return myScene.get(); }
	std::vector<std::string> GetMediaFolders()			{ return mMediaFolders; }
	ID3D11ShaderResourceView* GetDepthShaderView()		{ return mDepthShaderView; }

	//Setters
	void SetModelConstants(PerModelConstants& constants) { mPerModelConstants = constants; }
	void SetAllModels(std::vector<IModel*>& models)		 { mAllModels = models; }
	void SetShadowEffect(EShadowEffect& setEffect);


//---------------------------------------
// Operational Methods
//---------------------------------------
	IMesh* LoadMesh(const std::string& file);//Load Meshes from file
	ILight* CreateLight(ELightType type);
	IGui* CreateGUI();

	void InitEngine();
	bool IsRunning();
	void StartWindowed();
	void Messages();
	void Delete();
	void Stop();
	void AddMediaFolder(const std::string& newFolder);
	void RemoveMediaFolder(const std::string& rogueFolder);

private:
//---------------------------------------
// Private Member Methods
//---------------------------------------
	bool InitDirect3D();
	void ShutdownDirect3D();

//---------------------------------------
// Private Member Variables
//---------------------------------------
	//Unique pointers
	std::unique_ptr<IScene> myScene;// Rendering of scene
	std::vector<std::unique_ptr<ILight>> mAllLights; //Cumulative lights
	std::vector<std::unique_ptr<IMesh>> mAllMeshes;//Cumulative meshes

	//Raw Pointers - "observers"
	std::vector<IModel*> mAllModels;//Cumulative models

	//Com Pointers - Unique pointers for DirectX
	CComPtr<ID3D11DeviceContext> mD3DContext = nullptr;
	CComPtr<ID3D11Texture2D> mDepthStencilTexture = nullptr; // The texture holding the depth values
	CComPtr<ID3D11DepthStencilView> mDepthStencil = nullptr; // The depth buffer referencing above texture
	CComPtr<ID3D11Device> mD3DDevice = nullptr; // D3D device for overall features
	CComPtr<IDXGISwapChain> mSwapChain = nullptr;// Swap chain and back buffer
	CComPtr<ID3D11RenderTargetView> mBackBufferRenderTarget = nullptr;
	CComPtr<ID3D11Buffer>     mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure
	CComPtr<ID3D11Buffer>     mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure
	CComPtr<ID3D11Texture2D>          mShadowMapTexture = nullptr; // This object represents the memory used by the texture on the GPU
	CComPtr<ID3D11DepthStencilView>   mShadowMapDepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
	CComPtr<ID3D11ShaderResourceView> mShadowMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
	CComPtr<ID3D11PixelShader> mDepthOnly = nullptr;
	CComPtr<ID3D11VertexShader> mBasicPixel = nullptr;
	CComPtr<ID3D11ShaderResourceView> mDepthShaderView = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

	ColourRGBA mBackgroundColor = { 0.2f, 0.2f, 0.3f , 1.0f };
	float mFrameTime;//The time it takes to render one frame.

	//Main Entry variables
	HINSTANCE mhInstance;
	HINSTANCE mhPrevInstance;
	LPWSTR    mlpCmdLine;
	int      mnCmdShow;
	HWND mHWnd;

	bool mEngineRunning;
	PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer 
	PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer
	std::vector<std::string> mMediaFolders;
	MSG mMsg;
	int mLightAmount = 0;
	int mMeshAmount = 0;
	const std::string mMediaFolder = "C:\\ProgramData\\TL-Engine\\Media";

	//Shadows
	const int mShadowMapSize = 256;
	std::string mLastError;

};//Class

//--------------------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------------------

// Template function to update a constant buffer. Pass the DirectX constant buffer object and the C++ data structure
// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
// be available to shaders. This is used to update model and camera positions, lighting data etc.
template <typename T>
void UpdateConstantBuffer(ID3D11Buffer* buffer, const T& bufferData, ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE cb;

	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
	memcpy(cb.pData, &bufferData, sizeof(T));
	context->Unmap(buffer, 0);
}

bool InitWindow(HINSTANCE& hInstance, int nCmdShow, HWND& hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

}//Namespace
//======================================================================================
#endif//Header Guard

