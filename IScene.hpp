#ifndef _ISCENE_H_
#define _ISCENE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for rendering a scene
//--------------------------------------------------------------------------------------

#include "Camera.hpp"
#include "ILight.hpp"
#include "Shader.hpp"
#include "DirectX11Engine.hpp"
#include "ColourRGBA.hpp"
#include <vector>

//======================================================================================
namespace umbra_engine
{
class IScene
{
public:
//---------------------------------------
// Destructor - no constructor
//---------------------------------------
	virtual ~IScene() {}

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	virtual ID3D11Buffer* GetFrameConstantBuffer() = 0;
	virtual ID3D11Buffer* GetModelConstantBuffer() = 0;
	virtual PerFrameConstants GetFrameConstants() = 0;
	virtual PerModelConstants GetModelConstants() = 0;
	virtual ICamera* GetCamera() = 0;
	virtual ID3D11BlendState* GetNoBlendState() = 0;
	virtual ID3D11BlendState* GetAddBlendState() = 0;
	virtual ID3D11BlendState* GetMultiBlendState() = 0;
	virtual ID3D11BlendState* GetAlphaBlendState() = 0;
	virtual ID3D11RasterizerState* GetCullBackState() = 0;
	virtual ID3D11RasterizerState* GetCullNoneState() = 0;
	virtual ID3D11DepthStencilState* GetDepthBufferState() = 0;
	virtual ID3D11DepthStencilState* GetDepthReadOnlyState() = 0;
	virtual ID3D11SamplerState* GetPointSampler() = 0;
	virtual ID3D11SamplerState* GetAnisotropic4xSampler() = 0;


	//Setters
	virtual void SetFrameConstants(PerFrameConstants& constants) = 0;
	virtual void SetDayNight(float& dayNight) = 0;

//---------------------------------------
// Opearational Methods
//---------------------------------------
	virtual bool InitGeometry() = 0;
	virtual bool InitScene() = 0;

	virtual void RenderSceneFromCamera() = 0;
	virtual void RenderScene(float& frameTime) = 0;
	virtual void RenderModels(float& frameTime) = 0;
	virtual void RenderLights(std::vector<ILight*> lights) = 0;
	virtual void RenderShadow(D3D11_VIEWPORT& vp) = 0;

	virtual void UpdateScene(float frameTime) = 0;

	virtual void RenderDepthBufferFromLight(int lightIndex) = 0;
	virtual void ReleaseResources() = 0;

	// Create all the states used in this app, returns true on success
	virtual bool CreateStates() = 0;

};//Class
}//Namespace
//======================================================================================
#endif//Header Guard