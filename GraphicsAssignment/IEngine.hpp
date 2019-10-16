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
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include "Scene.hpp"

//Graphics helpers
#include "Shader.hpp"

#include "ColourRGBA.hpp" 
#include "Input.hpp"
#include "Timer.hpp"

//Definitions and point of communication with graphics side
#include "Common.hpp"

#include <cmath>
#include <cctype>
class Light;
class CScene;

class IEngine
{
public:
	virtual ~IEngine() {}

	//-------------------------------------------------------------------------------------
	// Direct3D Setup
	//-------------------------------------------------------------------------------------
	// Returns false on failure
	virtual bool InitDirect3D() = 0;
	// Release the memory held by all objects created
	virtual void ShutdownDirect3D() = 0;

	virtual Mesh* LoadMesh(const std::string& file) = 0;//Load Meshes from file

	//Major engine methods

	virtual void EngineRun() = 0;
	virtual bool IsRunning() = 0;
	virtual void StartWindowed() = 0;
	virtual void Messages() = 0;
	virtual void Delete() = 0;
	virtual void Stop() = 0;

	//Media folder methods
	virtual std::vector<std::string> GetMediaFolders() = 0;
	virtual void AddMediaFolder(const std::string& newFolder) = 0;
	virtual void RemoveMediaFolder(const std::string& rogueFolder) = 0;


	//--------------------------------------------------------------------------------------
	// State creation / destruction
	//--------------------------------------------------------------------------------------

	virtual Light* CreateLight() = 0;
	// Create all the states used in this app, returns true on success
	virtual bool CreateStates() = 0;

	// Release DirectX state objects
	virtual void ReleaseStates() = 0;

	//--------------------------------------------------------------------------------------
	// Constant buffers
	//--------------------------------------------------------------------------------------

	// Template function to update a constant buffer. Pass the DirectX constant buffer object and the C++ data structure
	// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
	// be available to shaders. This is used to update model and camera positions, lighting data etc.

	template <class T>
	void UpdateConstantBuffer(ID3D11Buffer* buffer, const T& bufferData)
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
	virtual bool LoadTexture(std::string filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureSRV) = 0;


	// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
	// - Aspect ratio is screen width / height (like 4:3, 16:9)
	// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
	// - near and far clip are the range of z distances that can be rendered
	virtual CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 16.0f / 9.0f, float FOVx = ToRadians(60),
		float nearClip = 0.1f, float farClip = 10000.0f) = 0;


	//Getters
	virtual PerFrameConstants GetFrameConstants() = 0;
	virtual PerModelConstants GetModelConstants() = 0;

	virtual ID3D11Buffer* GetFrameConstantBuffer() = 0;
	virtual ID3D11Buffer* GetModelConstantBuffer() = 0;

	virtual ID3D11Device* GetDevice() = 0;
	virtual ID3D11DeviceContext* GetContext() = 0;

	virtual ID3D11BlendState* GetNoBlendState() = 0;
	virtual ID3D11BlendState* GetAddBlendState() = 0;
	virtual ID3D11BlendState* GetMultiBlendState() = 0;
	virtual ID3D11BlendState* GetAlphaBlendState() = 0;

	virtual ID3D11RasterizerState* GetCullBackState() = 0;
	virtual ID3D11DepthStencilState* GetDepthBufferState() = 0;
	virtual ID3D11RenderTargetView* GetBackBufferRenderTarget() = 0;
	virtual ID3D11DepthStencilView* GetDepthStencil() = 0;
	virtual ID3D11SamplerState* GetPointSampler() = 0;
	virtual ColourRGBA GetBackgroundColour() = 0;
	virtual IDXGISwapChain* GetSwapChain() = 0;
	virtual ID3D11SamplerState* GetAnisotropic4xSampler() = 0;
	virtual ID3D11DepthStencilState* GetDepthReadOnlyState() = 0;
	virtual HWND GetHWnd() = 0;

	virtual std::vector<Model*> GetAllModels() = 0;
	virtual std::vector<Light*> GetAllLights() = 0;

	virtual std::vector<ID3D11Resource*> GetDiffuseSpecularMaps() = 0;
	virtual std::vector<ID3D11ShaderResourceView*> GetDiffuseSpecularMapSRVs() = 0;

	virtual CScene* GetScene() = 0;

	//Setters
	virtual void SetFrameConstants(PerFrameConstants& constants) = 0;
	virtual void SetModelConstants(PerModelConstants& constants) = 0;

	virtual void SetAllModels(std::vector<Model*>& models) = 0;
	virtual void SetAllLights(std::vector<Light*>& lights) = 0;

	virtual void SetScene(CScene* scene) = 0;


	ID3D11DeviceContext* mD3DContext;

};

void main(IEngine* myEngine);
