#pragma once

#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "ColourRGBA.h"
#include "DirectX11Engine.h"

class Light;
class Model;

class CScene
{
public:
	CScene(CDX11Engine* engine);
	~CScene();


	bool InitGeometry();
	bool InitScene();

	void RenderSceneFromCamera(Camera* cam);
	void RenderScene(float& frameTime);
	void RenderModels();
	void RenderLights(std::vector<Light*> lights);
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

	Camera* GetCamera();

private:
	CDX11Engine* mEngine;

	std::string mLastError;

	const int mShadowMapSize = 256;
	ID3D11Texture2D* mShadowMapTexture = nullptr; // This object represents the memory used by the texture on the GPU
	ID3D11DepthStencilView*   mShadowMapDepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
	ID3D11ShaderResourceView* mShadowMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

	ID3D11PixelShader* mDepthOnly = nullptr;
	ID3D11VertexShader* mBasicPixel = nullptr;

	PerModelConstants mPerModelConstants;      // This variable holds the CPU-side constant buffer described above
	ID3D11Buffer*     mPerModelConstantBuffer = nullptr;  // This variable controls the GPU-side constant buffer related to the above structure
	//PerFrameConstants mPerFrameConstants;      // This variable holds the CPU-side constant buffer described above
	ID3D11Buffer*     mPerFrameConstantBuffer = nullptr; // This variable controls the GPU-side constant buffer matching to the above structure
	Camera* camera;





	std::vector<Model*> allModels;
	std::vector<Light*> mLights;

	ID3D11RenderTargetView* mBackBufferRenderTarget;
	ColourRGBA mBackgroundColour;

	ID3D11SamplerState* mPointSampler;
	ID3D11SamplerState* mAnisotropic4xSampler;
};

