#ifndef _SCENE_H_
#define _SCENE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for managing a DirectX scene
//--------------------------------------------------------------------------------------

#include "IScene.hpp"
#include "CParticleSystem.hpp"
#include <cmath>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_sdl.h"

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declaration
//---------------------------------------
class Model;

class CScene : public IScene
{
public:
	//---------------------------------------
	//Constructors / Destructor
	//---------------------------------------
	CScene(IEngine * engine);
	~CScene();

	//---------------------------------------
	//Data Access
	//---------------------------------------
		//Getters
	ID3D11Buffer* GetFrameConstantBuffer()			 { return mPerFrameConstantBuffer.Get(); }
	ID3D11Buffer* GetModelConstantBuffer()			 { return mPerModelConstantBuffer.Get(); }
	PerFrameConstants GetFrameConstants()			 { return mPerFrameConstants; }
	PerModelConstants GetModelConstants()			 { return mPerModelConstants; }
	ICamera* GetCamera()							 { return camera.get(); }
	ID3D11BlendState* GetNoBlendState()				 { return mNoBlendingState; }
	ID3D11BlendState* GetAddBlendState()			 { return mAdditiveBlendingState; }
	ID3D11BlendState* GetMultiBlendState()			 { return mMultiplicativeBlendingState; }
	ID3D11BlendState* GetAlphaBlendState()			 { return mAlphaBlendingState; }
	ID3D11RasterizerState* GetCullBackState()		 { return mCullBackState; }
	ID3D11RasterizerState* GetCullNoneState()		 { return mCullNoneState; }
	ID3D11DepthStencilState* GetDepthBufferState()	 { return mUseDepthBufferState; }
	ID3D11DepthStencilState* GetDepthReadOnlyState() { return mDepthReadOnlyState; }
	ID3D11SamplerState* GetPointSampler()			 { return mPointSampler; }
	ID3D11SamplerState* GetAnisotropic4xSampler()	 { return mAnisotropic4xSampler; }


	//Setters
	void SetFrameConstants(PerFrameConstants& constants) { mPerFrameConstants = constants; } 
	void SetDayNight(float& dayNight) { mPerFrameConstants.dayNightCycle = dayNight; }
//---------------------------------------
//Operational Methods
//---------------------------------------
	bool InitGeometry();
	bool InitScene();
	bool CreateStates();
	void RenderSceneFromCamera();
	void RenderScene(float& frameTime);
	void RenderModels(float& frameTime);
	void RenderLights(std::vector<ILight*> lights);
	void RenderShadow(D3D11_VIEWPORT& vp);
	void UpdateScene(float frameTime);
	void RenderDepthBufferFromLight(int lightIndex);
	void ReleaseResources();

private:
	//Unique pointers
	std::unique_ptr<ICamera> camera;
	CParticleSystem* mParticleSystem;
	std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> mFont;

	float mTotalTime = 0.0f;

	//Raw pointers "observers"
	IEngine* mEngine;
	std::vector<IModel*> allModels;
	std::vector<ILight*> mLights;

	//Com pointers
	CComPtr<ID3D11Texture2D> mShadowMapTexture = nullptr; // This object represents the memory used by the texture on the GPU
	CComPtr<ID3D11DepthStencilView>   mShadowMapDepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
	CComPtr<ID3D11ShaderResourceView> mShadowMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
	CComPtr<ID3D11PixelShader> mDepthOnly = nullptr;
	CComPtr<ID3D11VertexShader> mBasicPixel = nullptr;
	CComPtr<ID3D11PixelShader> mlightModelps = nullptr;
	CComPtr<ID3D11VertexShader> mlightModelvs = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure
	Microsoft::WRL::ComPtr<ID3D11Buffer>   mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure
	ID3D11RenderTargetView* mBackBufferRenderTarget = nullptr;
	CComPtr<ID3D11DeviceContext> mD3DContext = nullptr;
	CComPtr<ID3D11Device> mD3DDevice = nullptr;

	CComPtr<ID3D11Texture2D> mCubicShadowTexture = nullptr;

	// GPU "States" //
	ID3D11SamplerState* mPointSampler = nullptr;
	ID3D11SamplerState* mTrilinearSampler = nullptr;
	ID3D11SamplerState* mAnisotropic4xSampler = nullptr;

	CComPtr<ID3D11BlendState> mNoBlendingState = nullptr;
	CComPtr<ID3D11BlendState> mAdditiveBlendingState = nullptr;
	CComPtr<ID3D11BlendState> mMultiplicativeBlendingState = nullptr;
	CComPtr<ID3D11BlendState> mAlphaBlendingState = nullptr;

	CComPtr<ID3D11RasterizerState>   mCullBackState = nullptr;
	CComPtr<ID3D11RasterizerState>   mCullFrontState = nullptr;
	CComPtr<ID3D11RasterizerState>   mCullNoneState = nullptr;

	CComPtr<ID3D11DepthStencilState> mUseDepthBufferState = nullptr;
	CComPtr<ID3D11DepthStencilState> mDepthReadOnlyState = nullptr;
	CComPtr<ID3D11DepthStencilState> mNoDepthBufferState = nullptr;

	std::string mLastError;
	const int mShadowMapSize = 1024;//resolution of shadows - Quality vs performance
	ColourRGBA mBackgroundColour;
	PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
	PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above



};//Class
}//Namespace
#endif//Header Guard