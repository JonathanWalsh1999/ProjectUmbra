#ifndef _CIMGUI_H_
#define _CIMGUI_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Create/setup and render an ImGui User interface
//--------------------------------------------------------------------------------------

#include "IGui.hpp"
#include "imgui.h"
#include "examples\imgui_impl_dx11.h"
#include "examples\imgui_impl_win32.h"
#include "Common.hpp"

//======================================================================================
namespace umbra_engine
{

class CImGui : public IGui
{
public:
//---------------------------------------
// Constructor / Destructor
//---------------------------------------
	CImGui() = delete;//Don't want an incomplete class being constructor
	CImGui(ID3D11DeviceContext* context, ID3D11RenderTargetView* target);
	~CImGui() = default;

//---------------------------------------
// Operational Methods
//---------------------------------------
	void RenderGUI();//Render the GUI once per frame

private:
//---------------------------------------
// Private Member variables
//---------------------------------------
	ID3D11DeviceContext* mContext;
	ID3D11RenderTargetView* mBackBufferRenderTarget;
	ImGuiContext* mGuiContext;
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard