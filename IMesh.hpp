#ifndef _IMESH_H_
#define _IMESH_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation of Creating and managing a Mesh
//--------------------------------------------------------------------------------------

#include "CTexture.h"

#include "common.hpp"


#include <string>

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class IModel;
class IMesh
{
public:
//---------------------------------------
// Destructor - No constructor
//---------------------------------------
	virtual ~IMesh() {}

//---------------------------------------
// Data Access
//---------------------------------------
	virtual std::string GetTextureFile() = 0;

//---------------------------------------
// Operational Methods
//---------------------------------------
	// The render function assumes shaders, matrices, textures, samplers etc. have been set up already.
	// It simply draws this mesh with whatever settings the GPU is currently using.
	virtual void Render(std::vector<maths::CMatrix4x4>& modelMatrices) = 0;
	virtual std::unique_ptr<IModel> CreateModel(const float x = 0, const float y = 0, const float z = 0,
		const std::string& psShaderFile = "main_ps", const std::string vsShaderFile = "main_vs") = 0;
	virtual void AddFolders(std::vector<std::string> mediaFolders) = 0;
	// How many nodes are in the hierarchy for this mesh. Nodes can control individual parts (rigid body animation),
	// or bones (skinned animation), or they can be dummy nodes to create child parts in a more convenient way
	virtual unsigned int NumberNodes() = 0;

	// The default matrix for a given node - used to set the initial position for a new model
	virtual maths::CMatrix4x4 GetNodeDefaultMatrix(unsigned int node) = 0;
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard