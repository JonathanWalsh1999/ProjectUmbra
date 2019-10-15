//--------------------------------------------------------------------------------------
// Loading GPU shaders
// Creation of constant buffers to help send C++ values to shaders each frame
//--------------------------------------------------------------------------------------
#ifndef _SHADER_H_INCLUDED_
#define _SHADER_H_INCLUDED_

#include "Common.h"

class CDX11Engine;


//--------------------------------------------------------------------------------------
// Shader creation / destruction
//--------------------------------------------------------------------------------------

// Load shaders required for this app, returns true on success
bool LoadShaders();

// Release shaders used by the app
void ReleaseShaders();


//--------------------------------------------------------------------------------------
// Constant buffer creation / destruction
//--------------------------------------------------------------------------------------

// Create and return a constant buffer of the given size
// The returned pointer needs to be released before quitting. Returns nullptr on failure
ID3D11Buffer* CreateConstantBuffer(int size, CDX11Engine* engine);


//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------

// Load a shader, include the file in the project and pass the name (without the .hlsl extension)
// to this function. The returned pointer needs to be released before quitting. Returns nullptr on failure
ID3D11VertexShader* LoadVertexShader(std::string shaderName, CDX11Engine* engine);
ID3D11PixelShader*  LoadPixelShader (std::string shaderName, CDX11Engine* engine);

// Helper function. Returns nullptr on failure.
ID3DBlob* CreateSignatureForVertexLayout(const D3D11_INPUT_ELEMENT_DESC vertexLayout[], int numElements);


#endif //_SHADER_H_INCLUDED_
