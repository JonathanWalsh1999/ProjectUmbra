//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.




#ifndef _MODEL_H_INCLUDED_
#define _MODEL_H_INCLUDED_

#include "IModel.hpp"

class IMesh;
class IEngine;
class IScene;

class Model : public IModel
{
public:

						
	Model(IMesh* mesh, IEngine * engine, CVector3 position = { 0,0,0 }, CVector3 rotation = { 0,0,0 }, float scale = 1);

    // The render function sets the world matrix in the per-frame constant buffer and makes that buffer available
    // to vertex & pixel shader. Then it calls Mesh:Render, which renders the geometry with current GPU settings.
    // So all other per-frame constants must have been set already along with shaders, textures, samplers, states etc.
    void Render();


	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control( float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,  
				  KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward );


	~Model()
	{
		if (mapTexture) mapTexture->Release();
		if (depthStencil) depthStencil->Release();
		if (textureShader) textureShader->Release();
		if (diffuseSpecularMapSRV) diffuseSpecularMapSRV->Release();
		if (diffuseSpecularMap) diffuseSpecularMap->Release();
		if (diffuseSpecularMap2SRV) diffuseSpecularMap2SRV->Release();
		if (diffuseSpecular2Map) diffuseSpecular2Map->Release();
		if (diffuseSpecularMap3SRV) diffuseSpecularMap3SRV->Release();
		if (diffuseSpecular3Map) diffuseSpecular3Map->Release();
		if (associatedPSShader) associatedPSShader->Release();
		if (associatedVSShader) associatedVSShader->Release();
	}


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

	void LookAt(IModel* target);
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
	ID3D11Resource* GetDiffuseMap3();
	std::string GetTextureFile3();
	ID3D11ShaderResourceView* GetDiffuseSRVMap3();
	EBlendingType GetAddBlend() { return blend; }

	void AddSecondaryTexture(const std::string& texture2);
	void AddThirdTexture(const std::string& texture3);

	//HOLD ALL OBJECTS IN THIS CLASS
	static std::vector<IModel*> GetAllObjects();


	
	//-------------------------------------
	// Private data / members
	//-------------------------------------
private:
	IEngine * myEngine;
	IScene* myScene;

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
