#ifndef _MODEL_H_
#define _MODEL_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Class encapsulating a model
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.
//--------------------------------------------------------------------------------------

#include "IModel.hpp"

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class IMesh;
class IEngine;
class IScene;
class ITexture;

class Model : public IModel
{
public:

//---------------------------------------
// Constructors / Destructor
//---------------------------------------
	Model(IMesh* mesh, IEngine * engine, maths::CVector3 position = { 0,0,0 }, maths::CVector3 rotation = { 0,0,0 }, float scale = 1);
	~Model()
	{
		if (mapTexture) mapTexture->Release();
		if (depthStencil) depthStencil->Release();
		if (textureShader) textureShader->Release();
		if (associatedPSShader) associatedPSShader->Release();
		if (associatedVSShader) associatedVSShader->Release();
	}

//---------------------------------------
// Data Access
//---------------------------------------
	// Getters
	maths::CVector3 Position() { return mPosition; }
	maths::CVector3 Rotation() { return mRotation; }
	maths::CVector3 Scale() { return mScale; }
	maths::CMatrix4x4 GetMatrix();
	// Read only access to model world matrix, updated on request
	maths::CMatrix4x4 WorldMatrix() { UpdateWorldMatrix();  return mWorldMatrix; }
	float GetX();
	float GetY();
	float GetZ();
	ID3D11PixelShader* GetPSShader();
	ID3D11VertexShader* GetVSShader();
	ID3D11ShaderResourceView* GetDiffuseSRVMap();
	ID3D11Resource* GetDiffuseMap();
	std::string GetTextureFile();
	ID3D11ShaderResourceView* GetDiffuseSRVMap2();
	ID3D11Resource* GetDiffuseMap2();
	std::string GetTextureFile2();
	ID3D11Resource* GetDiffuseMap3();
	std::string GetTextureFile3();
	ID3D11ShaderResourceView* GetDiffuseSRVMap3();
	EBlendingType GetAddBlend() { return blend; }
	//HOLD ALL OBJECTS IN THIS CLASS
	static std::vector<IModel*> GetAllObjects();

	//Setters
	void SetMatrix(maths::CMatrix4x4 model);
	void SetPosition(maths::CVector3 position) { mPosition = position; }
	void SetRotation(maths::CVector3 rotation) { mRotation = rotation; }
	// Two ways to set scale: x,y,z separately, or all to the same value
	void SetScale(maths::CVector3 scale) { mScale = scale; }
	void SetScale(float scale) { mScale = { scale, scale, scale }; }
	void SetX(float pos);
	void SetY(float pos);
	void SetZ(float pos);
	void SetSkin(const std::string& colour);
	void SetDiffuseSRVMap(ID3D11ShaderResourceView* newDiffuseSpecularMapSRV) { diffuseSpecularMapSRV = newDiffuseSpecularMapSRV; }
	void SetDiffuseMap(ID3D11Resource* newDiffuseSpecularMap) { diffuseSpecularMap = newDiffuseSpecularMap; }
	void Set2ndDiffuseSRVMap(ID3D11ShaderResourceView* DiffuseSpecularMapSRV) { diffuseSpecularMap3SRV = DiffuseSpecularMapSRV; }
	void Set2ndDiffuseMap(ID3D11Resource* newDiffuseSpecularMap) { diffuseSpecular3Map = newDiffuseSpecularMap; }
	void Set3rdDiffuseSRVMap(ID3D11ShaderResourceView*	DiffuseSpecularMapSRV) { diffuseSpecularMap3SRV = DiffuseSpecularMapSRV; }
	void Set3rdDiffuseMap(ID3D11Resource* newDiffuseSpecularMap) { diffuseSpecular3Map = newDiffuseSpecularMap; }
	void SetNormalSRVMap(ID3D11ShaderResourceView* newDiffuseSpecularMapSRV) { diffuseSpecularMap2SRV = newDiffuseSpecularMapSRV; }
	void SetNormalMap(ID3D11Resource* newDiffuseSpecularMap) { diffuseSpecular2Map = newDiffuseSpecularMap; }
	void SetTextureFile(const std::string& file);
	void SetPSShader(const std::string& shaderFile);
	void SetVSShader(const std::string& shaderFile);
	void SetAddBlend(const EBlendingType& newBlend) { blend = newBlend; }
	void AddSecondaryTexture(const std::string& texture2);
	void AddThirdTexture(const std::string& texture3);

//---------------------------------------
// Operational Methods
//---------------------------------------
	// The render function sets the world matrix in the per-frame constant buffer and makes that buffer available
	// to vertex & pixel shader. Then it calls Mesh:Render, which renders the geometry with current GPU settings.
	// So all other per-frame constants must have been set already along with shaders, textures, samplers, states etc.
	void Render();
	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward);
	void LookAt(IModel* target);
	void LookAtCamera(ICamera * target);
	void MoveLocalX(float speed);
	void MoveLocalY(float speed);
	void MoveLocalZ(float speed);
	void MoveX(float speed);
	void MoveY(float speed);
	void MoveZ(float speed);
	void Move(float x, float y, float z);
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

private:
//-------------------------------------
// Private data / members
//-------------------------------------
	IEngine* myEngine;
	IScene* myScene;

	std::unique_ptr<ITexture> mTexture;
	static std::vector<IModel*> objectList;
	bool addBlending;
	EBlendingType blend;
	ID3D11ShaderResourceView* diffuseSpecularMapSRV = nullptr;
	ID3D11Resource* diffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* diffuseSpecularMap2SRV = nullptr;
	ID3D11Resource* diffuseSpecular2Map = nullptr;
	ID3D11ShaderResourceView* diffuseSpecularMap3SRV = nullptr;
	ID3D11Resource* diffuseSpecular3Map = nullptr;
	std::string textureFile = "";
	std::string texture2File = "";
	std::string texture3File = "";
	ID3D11PixelShader* associatedPSShader = nullptr;
	ID3D11VertexShader* associatedVSShader = nullptr;
	ID3D11Texture2D* mapTexture = nullptr;
	ID3D11DepthStencilView* depthStencil = nullptr;
	ID3D11ShaderResourceView* textureShader = nullptr;
	void UpdateWorldMatrix();
	void UpdateScale();
	bool lookingAt = false;
	IMesh* mMesh = nullptr;
	static std::vector<std::string> mMediaFolders;
	// Position, rotation and scaling for the model
	maths::CVector3 mPosition;
	maths::CVector3 mRotation;
	maths::CVector3 mScale;
	// World matrix for the model - built from the above
	maths::CMatrix4x4 mWorldMatrix;

	PerModelConstants mPerModelConstants;
	ID3D11Buffer* mPerModelConstantBuffer;

	// World matrices for the model
	// Now that meshes have multiple parts, we need multiple matrices. The root matrix (the first one) is the world matrix
	// for the entire model. The remaining matrices are relative to their parent part. The hierarchy is defined in the mesh (nodes)
	std::vector<maths::CMatrix4x4> mWorldMatrices;
};//Class
}//Namespace
//======================================================================================
#endif //Header Guard
