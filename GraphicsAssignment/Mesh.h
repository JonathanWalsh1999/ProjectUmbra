//--------------------------------------------------------------------------------------
// Class encapsulating a mesh
//--------------------------------------------------------------------------------------
// The mesh class splits the mesh into sub-meshes that only use one texture each.
// ** THIS VERSION WILL ONLY KEEP THE FIRST SUB-MESH OTHER PARTS WILL BE MISSING **
// ** DO NOT USE THIS VERSION FOR PROJECTS **
// The class also doesn't load textures, filters or shaders as the outer code is
// expected to select these things. A later lab will introduce a more robust loader.

#include "common.h"
#include "externs.h"

#include <string>

#ifndef _MESH_H_INCLUDED_
#define _MESH_H_INCLUDED_

class Model;
class CDX11Engine;

class Mesh
{
public:
	// Pass the name of the mesh file to load. Uses assimp (http://www.assimp.org/) to support many file types
	// Optionally request tangents to be calculated (for normal and parallax mapping - see later lab)
	// Will throw a std::runtime_error exception on failure (since constructors can't return errors).
	Mesh(const std::string& fileName, CDX11Engine* engine = nullptr, bool requireTangents = false);

	Model* CreateModel(const std::string& textureFile = "brick1.jpg", const float x = 0, const float y = 0, const float z = 0,
		const std::string& psShaderFile = "PixelLighting_ps", const std::string vsShaderFile = "PixelLighting_vs");
	~Mesh();

	// The render function assumes shaders, matrices, textures, samplers etc. have been set up already.
	// It simply draws this mesh with whatever settings the GPU is currently using.
	void Render();



	void AddFolders(std::vector<std::string> mediaFolders) { mMediaFolders = mediaFolders; }
	static std::vector<std::string> GetMediaFolders() {return mMediaFolders;}

	std::string GetTextureFile() { return textureFile; }


	//std::vector<SModelCreation> GetAllModels();
	//static std::vector<SModelCreation> mModels; //This variable belongs to the class not just a single object



private:
	CDX11Engine * myEngine;

	static std::vector<std::string> mMediaFolders;

    unsigned int       mVertexSize;             // Size in bytes of a single vertex (depends on what it contains, uvs, tangents etc.)
    ID3D11InputLayout* mVertexLayout = nullptr; // DirectX specification of data held in a single vertex

    // GPU-side vertex and index buffers
    unsigned int       mNumVertices;
    ID3D11Buffer*      mVertexBuffer = nullptr;

    unsigned int       mNumIndices;
    ID3D11Buffer*      mIndexBuffer  = nullptr;

	std::string textureFile;
};


#endif //_MESH_H_INCLUDED_

