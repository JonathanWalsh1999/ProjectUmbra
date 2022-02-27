#ifndef _CTEXTURE_H_
#define _CTEXTURE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for creating and managing a texture
//--------------------------------------------------------------------------------------

#include "ITexture.h"

//======================================================================================
namespace umbra_engine
{

class CTexture : public ITexture
{
public:
//---------------------------------------
// Constructor / Destructor
//---------------------------------------
	CTexture(ETextureTypes type = ETextureTypes::Unknown);
	~CTexture() = default;

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	ID3D11ShaderResourceView* GetTextureSRV() { return mTextureSRV; }
	ETextureTypes GetTextureType() { return mTextureType; }
	ID3D11Resource* GetTexture() { return mTexture; }

//---------------------------------------
// Operational Methods
//---------------------------------------
	//Create a shader resource view texture based on filename of texture - .DDS also supported
	bool LoadTexture(std::string filename, ID3D11Device* device, ID3D11DeviceContext* context);

private:
//---------------------------------------
// Private member variables
//---------------------------------------
	CComPtr<ID3D11ShaderResourceView> mTextureSRV = nullptr;
	CComPtr<ID3D11Resource> mTexture = nullptr;

	ETextureTypes mTextureType;
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard