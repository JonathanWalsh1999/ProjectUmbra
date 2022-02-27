#ifndef _MESH_H_
#define _MESH_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class encapsulating a mesh
//--------------------------------------------------------------------------------------
#define NOMINMAX // Use this to stop Windows headers defining "min" and "max", which breaks some libraries (e.g. assimp)
#include "IMesh.hpp"
#include "CMatrix4x4.hpp"
#include <assimp/scene.h>

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class IModel;
class IEngine;

class Mesh : public IMesh
{
public:
//---------------------------------------
// Constructors / Destructors
//---------------------------------------
	// Pass the name of the mesh file to load. Uses assimp (http://www.assimp.org/) to support many file types
	// Optionally request tangents to be calculated (for normal and parallax mapping - see later lab)
	// Will throw a std::runtime_error exception on failure (since constructors can't return errors).
	Mesh(const std::string& fileName, IEngine * engine, bool requireTangents = false);
	~Mesh();

//---------------------------------------
// Data Access
//---------------------------------------
	//Getters
	static std::vector<std::string> GetMediaFolders() { return mMediaFolders; }
	std::string GetTextureFile() { return textureFile; }

	//Setters
	void AddFolders(std::vector<std::string> mediaFolders) { mMediaFolders = mediaFolders; }

//---------------------------------------
// Operational Methods
//---------------------------------------
	std::unique_ptr<IModel> CreateModel(const float x = 0, const float y = 0, const float z = 0,
		const std::string& psShaderFile = "main_ps", const std::string vsShaderFile = "main_vs");

	// The render function assumes shaders, matrices, textures, samplers etc. have been set up already.
	// It simply draws this mesh with whatever settings the GPU is currently using.
	void Render(std::vector<maths::CMatrix4x4>& modelMatrices);

	// How many nodes are in the hierarchy for this mesh. Nodes can control individual parts (rigid body animation),
	// or bones (skinned animation), or they can be dummy nodes to create child parts in a more convenient way
	unsigned int NumberNodes() { return static_cast<unsigned int>(mNodes.size()); }

	// The default matrix for a given node - used to set the initial position for a new model
	maths::CMatrix4x4 GetNodeDefaultMatrix(unsigned int node) { return mNodes[node].defaultMatrix; }

private:
//---------------------------------------
// Private Types
//---------------------------------------
	// A mesh is made of multiple sub-meshes. Each one uses a single material (texture).
	// Each sub-mesh has a vertex / index buffer on the GPU. Could share buffers for performance but that would be complex.
	struct SubMesh
	{
		unsigned int       vertexSize = 0;         // Size in bytes of a single vertex (depends on what it contains, uvs, tangents etc.)
		ID3D11InputLayout* vertexLayout = nullptr; // DirectX specification of data held in a single vertex

		// GPU-side vertex and index buffers
		unsigned int       numVertices = 0;
		ID3D11Buffer*      vertexBuffer = nullptr;

		unsigned int       numIndices = 0;
		ID3D11Buffer*      indexBuffer = nullptr;

		std::unique_ptr<ITexture> diffuseTexture = nullptr;
			   
	};


	// A mesh contains a hierarchy of nodes. A node represents a seperate animatable part of the mesh
	// A node can contain several sub-meshes (because a single node might use multiple textures)
	// A node can also have child nodes. The children will follow the motion of the parent node
	// Each node has a default matrix which is it's initial/ default position. Models using this mesh are
	// given these default matrices as a starting position.
	struct Node
	{
		std::string  name;

		maths::CMatrix4x4   defaultMatrix; // Starting position/rotation/scale for this node. Relative to parent. Used when first creating a model from this mesh
		maths::CMatrix4x4   offsetMatrix;

		unsigned int parentIndex;   // Index of the parent node (from the mNodes vector below). Root node refers to itself (0)

		std::vector<unsigned int> childNodes; // Child nodes that are controlled by this node (indexes into the mNodes vector below)
		std::vector<unsigned int> subMeshes;  // The geometry representing this node (indexes into the mSubMeshes vector below)
	};

//---------------------------------------
// Private Member Methods
//---------------------------------------
	// Count the number of nodes with given assimp node as root
	unsigned int CountNodes(aiNode* assimpNode);

	// Help build the arrays of submeshes and nodes from the assimp data - recursive
	unsigned int ReadNodes(aiNode* assimpNode, unsigned int nodeIndex, unsigned int parentIndex);

	// Helper function for Render function - renders a given sub-mesh. World matrices / textures / states etc. must already be set
	void RenderSubMesh(const SubMesh& subMesh);

//---------------------------------------
// Private Member Variables
//---------------------------------------
	IEngine * myEngine;
	

	static std::vector<std::string> mMediaFolders;


	ID3D11ShaderResourceView* mSrvTexture = nullptr;
	std::string textureFile;
	STextureMap diffuseMap;
	STextureMap normalMap;

	std::vector<STextureMap> diffuseMaps;

	std::vector<std::unique_ptr<ITexture>> mTextures;


	std::vector<SubMesh> mSubMeshes; // The mesh geometry. Nodes refer to sub-meshes in this vector
	std::vector<Node>    mNodes;     // The mesh hierarchy. First entry is root. remainder aree stored in depth-first order

	bool mHasBones; // If any submesh has bones, then all submeshes are given bones - makes rendering easier (one shader for the whole mesh)

};//Class
}//Namespace
//======================================================================================
#endif //Header Guard

