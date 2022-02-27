//--------------------------------------------------------------------------------------
// Create/setup and render an ImGui User interface
//--------------------------------------------------------------------------------------

#include "CImGui.hpp"

namespace umbra_engine
{

CImGui::CImGui(ID3D11DeviceContext* context, ID3D11RenderTargetView* target)
{
	mContext = context;
	mBackBufferRenderTarget = target;
}

void CImGui::RenderGUI()
{
	//Prepare
	//ImGui_ImplDX11_Frame();
	//ImGui_ImplWin32_NewFrame();

	//ImGui::NewFrame();
	////

	//ImGui::Begin("Settings");//Make new window
	//ImGui::SetWindowSize({ 230.0f, 250.0f });//Set the size of window

	////IMGUI
	////*******************************
	//// Finalise ImGUI for this frame
	////*******************************
	//ImGui::Render();
	//mContext->OMSetRenderTargets(1, &mBackBufferRenderTarget, nullptr);
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


}