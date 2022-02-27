#ifndef _IENGINE_H_
#define _IENGINE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class Encapsulated The low-level DirectX 11 - Main engine class
//--------------------------------------------------------------------------------------

//Graphics Component headers
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include "PointLight.hpp"
#include "Scene.hpp"
#include "IGui.hpp"
#include "CImGui.hpp"
#include "Common.hpp"

//Graphics helpers
#include "Shader.hpp"
#include "ColourRGBA.hpp" 
#include "Input.hpp"
#include "Timer.hpp"

//Allows different texture formats to be used
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

//C++ libraries
#include <sstream>
#include <iterator>
#include <atlbase.h> // C-std::string to unicode conversion function CA2CT
#include <cmath>
#include <cctype>

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Forward Class Declarations
//---------------------------------------
class Light;
class CScene;

class IEngine
{
public:
//---------------------------------------
// Destructor - No constructor because you cannot construct a interface
//---------------------------------------
	virtual ~IEngine() {}

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	virtual std::vector<std::string> GetMediaFolders() = 0;
	virtual PerModelConstants GetModelConstants() = 0;
	virtual ID3D11Buffer* GetModelConstantBuffer() = 0;
	virtual ID3D11Device* GetDevice() = 0;
	virtual ID3D11DeviceContext* GetContext() = 0;
	virtual ID3D11RenderTargetView* GetBackBufferRenderTarget() = 0;
	virtual ID3D11DepthStencilView* GetDepthStencil() = 0;
	virtual ColourRGBA GetBackgroundColour() = 0;
	virtual IDXGISwapChain* GetSwapChain() = 0;
	virtual ID3D11ShaderResourceView* GetDepthShaderView() = 0;

	virtual HWND GetHWnd() = 0;
	virtual std::vector<IModel*> GetAllModels() = 0;
	virtual std::vector<ILight*> GetAllLights() = 0;
	virtual IScene* GetScene() = 0;

	//Setters
	virtual void SetModelConstants(PerModelConstants& constants) = 0;
	virtual void SetAllModels(std::vector<IModel*>& models) = 0;
	virtual void SetShadowEffect(EShadowEffect& setEffect) = 0;
	virtual void AddMediaFolder(const std::string& newFolder) = 0;
	virtual void RemoveMediaFolder(const std::string& rogueFolder) = 0;

//---------------------------------------
// Operational Methods
//---------------------------------------
	virtual IMesh* LoadMesh(const std::string& file) = 0;//Load Meshes from file
	virtual IGui* CreateGUI() = 0;
	virtual ILight* CreateLight(ELightType type) = 0;

	//Major engine methods
	virtual void InitEngine() = 0;
	virtual bool IsRunning() = 0;
	virtual void StartWindowed() = 0;
	virtual void Messages() = 0;
	virtual void Delete() = 0;
	virtual void Stop() = 0;


};//Class
}//Namespace
//======================================================================================
#endif//Header guard

