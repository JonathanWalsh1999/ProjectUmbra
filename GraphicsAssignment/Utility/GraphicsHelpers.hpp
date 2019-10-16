////--------------------------------------------------------------------------------------
//// Helper functions to unclutter and simplify main code (Scene.cpp/.h)
////--------------------------------------------------------------------------------------
//// Code in .cpp file
//#pragma once
//
//
//#include <WICTextureLoader.h>
//#include <DDSTextureLoader.h>
//
//#include "CMatrix4x4.h"
//
//
//class CDX11Engine;
//
////--------------------------------------------------------------------------------------
//// Constant buffers
////--------------------------------------------------------------------------------------
//
//// Template function to update a constant buffer. Pass the DirectX constant buffer object and the C++ data structure
//// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
//// be available to shaders. This is used to update model and camera positions, lighting data etc.
//template <class T>
//void UpdateConstantBuffer(ID3D11Buffer* buffer, const T& bufferData, CDX11Engine* engine);
////{
////	
////    D3D11_MAPPED_SUBRESOURCE cb;
////	engine->Stop();
////    engine->mD3DContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
////    memcpy(cb.pData, &bufferData, sizeof(T));
////    engine->mD3DContext->Unmap(buffer, 0);
////}
//
//
////--------------------------------------------------------------------------------------
//// Texture Loading
////--------------------------------------------------------------------------------------
//
//// Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify file loading
//// This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
//// texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
//// The function will fill in these pointers with usable data. Returns false on failure
//bool LoadTexture(std::string filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureSRV, CDX11Engine* engine);
//
//
////--------------------------------------------------------------------------------------
//// Camera helpers
////--------------------------------------------------------------------------------------
//
//// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
//// - Aspect ratio is screen width / height (like 4:3, 16:9)
//// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
//// - near and far clip are the range of z distances that can be rendered
//CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 16.0f / 9.0f, float FOVx = ToRadians(60),
//                                float nearClip = 0.1f, float farClip = 10000.0f);
//
