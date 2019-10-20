#pragma once

#include <vector>

#include "Shader.hpp"
#include "Camera.hpp"
#include "ColourRGBA.hpp"
#include "DirectX11Engine.hpp"
#include "ILight.hpp"

class IScene
{
public:
	virtual ~IScene() {}

	virtual bool InitGeometry() = 0;
	virtual bool InitScene() = 0;

	virtual void RenderSceneFromCamera(ICamera * cam) = 0;
	virtual void RenderScene(float& frameTime) = 0;
	virtual void RenderModels() = 0;
	virtual void RenderLights(std::vector<ILight*> lights) = 0;
	virtual void RenderShadow(D3D11_VIEWPORT& vp) = 0;

	virtual void UpdateScene(float frameTime) = 0;

	virtual bool ShadowDepthBuffer() = 0;
	virtual void RenderDepthBufferFromLight(int lightIndex) = 0;

	virtual void ReleaseResources() = 0;

	//Getters
	virtual ID3D11Buffer* GetFrameConstantBuffer() = 0;
	virtual ID3D11Buffer* GetModelConstantBuffer() = 0;
	virtual PerFrameConstants GetFrameConstants() = 0;
	virtual PerModelConstants GetModelConstants() = 0;

	virtual void SetFrameConstants(PerFrameConstants& constants) = 0;

	virtual ICamera* GetCamera() = 0;
};