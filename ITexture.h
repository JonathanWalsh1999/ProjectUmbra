#ifndef _ITEXTURE_H_
#define _ITEXTURE_H_
//======================================================================================

//--------------------------------------------------------------------------------------
//Interface to setup a texture
//--------------------------------------------------------------------------------------

#include "Common.hpp"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <cctype>
#include <cmath>

//======================================================================================
namespace umbra_engine
{
class ITexture
{
public:
//---------------------------------------
// Destructor - no constructor
//---------------------------------------
	virtual ~ITexture() {}

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	virtual ID3D11ShaderResourceView* GetTextureSRV() = 0;
	virtual ID3D11Resource* GetTexture() = 0;
	virtual ETextureTypes GetTextureType() = 0;

//---------------------------------------
// Operational Methods
//---------------------------------------
	virtual bool LoadTexture(std::string filename, ID3D11Device* device, ID3D11DeviceContext* context) = 0;
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard