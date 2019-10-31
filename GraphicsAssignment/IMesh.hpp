#pragma once
#include "common.hpp"
#include "externs.hpp"

#include <string>

class IModel;
class IMesh
{
public:
	virtual ~IMesh() {}

	// The render function assumes shaders, matrices, textures, samplers etc. have been set up already.
	// It simply draws this mesh with whatever settings the GPU is currently using.
	virtual void Render() = 0;
	virtual IModel* CreateModel(const std::string& textureFile = "brick1.jpg", const float x = 0, const float y = 0, const float z = 0,
		const std::string& psShaderFile = "main_ps", const std::string vsShaderFile = "main_vs") = 0;
	virtual void AddFolders(std::vector<std::string> mediaFolders) = 0;
	virtual std::string GetTextureFile() = 0;
};