//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.

#include "Common.hpp"
#include "CVector3.hpp"
#include "CMatrix4x4.hpp"
#include "Input.hpp"
#include "Camera.hpp"

#ifndef _MODEL_H_INCLUDED_
#define _MODEL_H_INCLUDED_

class Mesh;
class IEngine;
class CScene;

class Model
{
public:

						
	Model(Mesh* mesh, IEngine * engine, CVector3 position = { 0,0,0 }, CVector3 rotation = { 0,0,0 }, float scale = 1);

    // The render function sets the world matrix in the per-frame constant buffer and makes that buffer available
    // to vertex & pixel shader. Then it calls Mesh:Render, which renders the geometry with current GPU settings.
    // So all other per-frame constants must have been set already along with shaders, textures, samplers, states etc.
    void Render();


	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control( float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,  
				  KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward );





	//-------------------------------------
	// Data access
	//-------------------------------------

	// Getters / setters
	CVector3 Position()  { return mPosition; }
	CVector3 Rotation()  { return mRotation; }
	CVector3 Scale()     { return mScale;    }
	CMatrix4x4 GetMatrix();

	void SetMatrix(CMatrix4x4 model);
	void SetPosition( CVector3 position )  { mPosition = position; }
	void SetRotation( CVector3 rotation )  { mRotation = rotation; }

	// Two ways to set scale: x,y,z separately, or all to the same value
	void SetScale   ( CVector3 scale    )  { mScale = scale;       } 
	void SetScale   ( float scale       )  { mScale = { scale, scale, scale }; }

	// Read only access to model world matrix, updated on request
	CMatrix4x4 WorldMatrix()  { UpdateWorldMatrix();  return mWorldMatrix; }

	void LookAt(Model* target);
	void LookAtCamera(ICamera * target);

	void MoveLocalX(float speed);
	void MoveLocalY(float speed);
	void MoveLocalZ(float speed);

	void MoveX(float speed);
	void MoveY(float speed);
	void MoveZ(float speed);
	void Move(float x, float y, float z);

	void SetX(float pos);
	void SetY(float pos);
	void SetZ(float pos);

	float GetX();
	float GetY();
	float GetZ();

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	void SetSkin(const std::string& colour);
	void SetDiffuseSRVMap(ID3D11ShaderResourceView* newDiffuseSpecularMapSRV) { diffuseSpecularMapSRV = newDiffuseSpecularMapSRV; }
	void SetDiffuseMap(ID3D11Resource* newDiffuseSpecularMap) { diffuseSpecularMap = newDiffuseSpecularMap; }

	void SetTextureFile(const std::string& file);

	void SetPSShader(const std::string& shaderFile);
	void SetVSShader(const std::string& shaderFile);
	void SetAddBlend(const EBlendingType& newBlend) { blend = newBlend; }

	ID3D11PixelShader* GetPSShader();
	ID3D11VertexShader* GetVSShader();
	ID3D11ShaderResourceView* GetDiffuseSRVMap();
	ID3D11Resource* GetDiffuseMap();
	std::string GetTextureFile();
	ID3D11ShaderResourceView* GetDiffuseSRVMap2();
	ID3D11Resource* GetDiffuseMap2();
	std::string GetTextureFile2();
	EBlendingType GetAddBlend() { return blend; }

	void AddSecondaryTexture(const std::string& texture2);


	//HOLD ALL OBJECTS IN THIS CLASS
	static std::vector<Model*> GetAllObjects();


	
	//-------------------------------------
	// Private data / members
	//-------------------------------------
private:
	IEngine * myEngine;
	CScene* myScene;

	static std::vector<Model*> objectList;

	bool addBlending;

	EBlendingType blend;

	ID3D11ShaderResourceView* diffuseSpecularMapSRV = nullptr;
	ID3D11Resource* diffuseSpecularMap = nullptr;

	ID3D11ShaderResourceView* diffuseSpecularMap2SRV = nullptr;
	ID3D11Resource* diffuseSpecular2Map = nullptr;

	std::string textureFile = "";
	std::string texture2File = "";

	ID3D11PixelShader* associatedPSShader = nullptr;
	ID3D11VertexShader* associatedVSShader = nullptr;


	ID3D11Texture2D* mapTexture = nullptr;
	ID3D11DepthStencilView* depthStencil = nullptr;
	ID3D11ShaderResourceView* textureShader = nullptr;


	void UpdateWorldMatrix();
	void UpdateScale();

	bool lookingAt = false;

    Mesh* mMesh = nullptr;
	static std::vector<std::string> mMediaFolders;

	// Position, rotation and scaling for the model
	CVector3 mPosition;
	CVector3 mRotation;
	CVector3 mScale;

	// World matrix for the model - built from the above
	CMatrix4x4 mWorldMatrix;
	//PerModelConstants modelConstants;


	PerFrameConstants mPerFrameConstants;
	PerModelConstants mPerModelConstants;

	ID3D11Buffer* mPerModelConstantBuffer;

};


#endif //_MODEL_H_INCLUDED_
