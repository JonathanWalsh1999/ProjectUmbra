#pragma once

#include "IScene.hpp"

class Model;

class CScene : public IScene
{
public:
	CScene(IEngine * engine);
	~CScene();


	bool InitGeometry();
	bool InitScene();

	void RenderSceneFromCamera(ICamera * cam);
	void RenderScene(float& frameTime);
	void RenderModels();
	void RenderLights(std::vector<ILight*> lights);
	void RenderShadow(D3D11_VIEWPORT& vp);

	void UpdateScene(float frameTime);

	bool ShadowDepthBuffer();
	void RenderDepthBufferFromLight(int lightIndex);

	void ReleaseResources();

	//Getters
	ID3D11Buffer* GetFrameConstantBuffer();
	ID3D11Buffer* GetModelConstantBuffer();
	PerFrameConstants GetFrameConstants();
	PerModelConstants GetModelConstants();

	void SetFrameConstants(PerFrameConstants& constants);

	ICamera* GetCamera();

private:
	IEngine* mEngine;

	std::string mLastError;

	const int mShadowMapSize = 1024;//resolution of shadows - Quality vs performance
	ID3D11Texture2D* mShadowMapTexture = nullptr; // This object represents the memory used by the texture on the GPU
	ID3D11DepthStencilView*   mShadowMapDepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
	ID3D11ShaderResourceView* mShadowMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

	ID3D11PixelShader* mDepthOnly = nullptr;
	ID3D11VertexShader* mBasicPixel = nullptr;

	PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
	ID3D11Buffer*     mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure
	PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above
	ID3D11Buffer*     mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure
	ICamera* camera;





	std::vector<IModel*> allModels;
	std::vector<ILight*> mLights;

	ID3D11RenderTargetView* mBackBufferRenderTarget;
	ColourRGBA mBackgroundColour;

	ID3D11SamplerState* mPointSampler;
	ID3D11SamplerState* mAnisotropic4xSampler;
	
	ID3D11DeviceContext* mD3DContext;
	ID3D11Device* mD3DDevice;

	ID3D11DepthStencilState* mDepthBufferState;
};

