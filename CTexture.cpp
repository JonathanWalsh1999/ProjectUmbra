//--------------------------------------------------------------------------------------
// Encapsulation for creating and managing a texture
//--------------------------------------------------------------------------------------

#include "CTexture.h"

namespace umbra_engine
{

CTexture::CTexture(ETextureTypes type)
{
	mTextureType = type;
}

bool CTexture::LoadTexture(std::string filename, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// DDS files need a different function from other files
	std::string dds = ".dds"; // So check the filename extension (case insensitive)
	if (filename.size() >= 4 &&
		std::equal(dds.rbegin(), dds.rend(), filename.rbegin(), [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); }))
	{
		return SUCCEEDED(DirectX::CreateDDSTextureFromFile(device, CA2CT(filename.c_str()), &mTexture, &mTextureSRV));
	}
	else
	{
		return SUCCEEDED(DirectX::CreateWICTextureFromFile(device, context, CA2CT(filename.c_str()), &mTexture, &mTextureSRV));
	}
}
}