//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.

#include "Model.hpp"



#include "Mesh.hpp"

#include "DirectX11Engine.hpp"
#include "GraphicsHelpers.hpp"


std::vector<Model*> Model::objectList;
std::vector<std::string> Model::mMediaFolders;

Model::Model(Mesh* mesh, IEngine * engine = nullptr, CVector3 position /*= { 0,0,0 }*/, CVector3 rotation /*= { 0,0,0 }*/, float scale /*= 1*/)
	: mMesh(mesh), mPosition(position), mRotation(rotation), mScale({ scale, scale, scale })
{
	addBlending = false;
	blend = None;
	objectList.push_back(this);
	myEngine = engine;
	myScene = myEngine->GetScene();
	mPerFrameConstants = myScene->GetFrameConstants();
}

std::vector<Model*> Model::GetAllObjects()
{
	return objectList;
}

void Model::LookAt(Model* target)
{	
	CVector3 yAxis = { 0.0f, 1.0f, 0.0f };
	CVector3 vecZ = Normalise(target->Position() - Position());
	CVector3 vecX = Normalise(Cross(yAxis, vecZ));
	CVector3 vecY = Normalise(Cross(vecZ, vecX));

	CMatrix4x4 newMatrix;
	newMatrix.MakeIdentity();

	newMatrix.SetRow(0, vecX);
	newMatrix.SetRow(1, vecY);
	newMatrix.SetRow(2, vecZ);
	newMatrix.SetRow(3, Position());

	mWorldMatrix = newMatrix;
	lookingAt = true;
}

void Model::LookAtCamera(ICamera * target)
{
	CVector3 yAxis = { 0.0f, 1.0f, 0.0f };
	CVector3 vecZ = Normalise(target->Position() - Position());
	CVector3 vecX = Normalise(Cross(yAxis, vecZ));
	CVector3 vecY = Normalise(Cross(vecZ, vecX));

	CMatrix4x4 newMatrix;
	newMatrix.MakeIdentity();

	newMatrix.SetRow(0, vecX);
	newMatrix.SetRow(1, vecY);
	newMatrix.SetRow(2, vecZ);
	newMatrix.SetRow(3, Position());

	mWorldMatrix = newMatrix;
	lookingAt = true;
}

void Model::SetTextureFile(const std::string& file)
{
	textureFile = file;
}

ID3D11Resource* Model::GetDiffuseMap()
{
	return diffuseSpecularMap;
}

ID3D11ShaderResourceView* Model::GetDiffuseSRVMap()
{
	return diffuseSpecularMapSRV;
}

std::string Model::GetTextureFile()
{
	return textureFile;
}

void Model::Render()
{
	mPerModelConstants = myEngine->GetModelConstants();
	mPerFrameConstants = myScene->GetFrameConstants();

	if (!lookingAt)
	{
		UpdateWorldMatrix();
	}
	else 
	{
		UpdateScale();
	}


    
	mPerModelConstants.worldMatrix = mWorldMatrix; // Update C++ side constant buffer
	myEngine->SetModelConstants(mPerModelConstants);
    myEngine->UpdateConstantBuffer(myEngine->GetModelConstantBuffer(), myEngine->GetModelConstants()); // Send to GPU

	mPerModelConstantBuffer = myEngine->GetModelConstantBuffer();

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    myEngine->GetContext()->VSSetConstantBuffers(1, 1, &mPerModelConstantBuffer); // First parameter must match constant buffer number in the shader
	myEngine->GetContext()->PSSetConstantBuffers(1, 1, &mPerModelConstantBuffer);

	

    mMesh->Render();

}

void Model::SetSkin(const std::string& colour)
{
	//std::vector<SModelCreation> allModels;



	//diffuseSpecularMap = nullptr;
	//diffuseSpecularMapSRV = nullptr;



	std::vector<std::string> mediaFolders = myEngine->GetMediaFolders();
	//ID3D11ShaderResourceView* diffuseSpecularMapSRV = nullptr;
	//ID3D11Resource* diffuseSpecularMap = nullptr;
	Mesh* newMesh = nullptr;
	//allModels = newMesh->GetAllModels();
	bool directory = false;
	const char slash = '\\';
	for (unsigned int i = 0; i < mediaFolders.size(); ++i)
	{
		for (unsigned int j = 0; j < 2; ++j)
		{
			if (mediaFolders[i][mediaFolders[i].size() - j] == slash)
			{
				directory = true;
			}
			else
			{
				directory = false;
			}
		}
		if (newMesh != nullptr)
		{
			if (directory)
			{
				if (!myEngine->LoadTexture(mediaFolders[i] + slash + colour, &diffuseSpecularMap, &diffuseSpecularMapSRV))
				{
					//gLastError = "Error loading textures";
				}
			}
			else
			{
				if (!myEngine->LoadTexture(mediaFolders[i] + slash + colour, &diffuseSpecularMap, &diffuseSpecularMapSRV))
				{
					//gLastError = "Error loading textures";
				}
			}
		}
		else
		{
			break;
		}
	}

	if (newMesh == nullptr)
	{
		if (!myEngine->LoadTexture(colour, &diffuseSpecularMap, &diffuseSpecularMapSRV))
		{
			//gLastError = "Error loading textures";
		}
	}

	SetDiffuseSRVMap(diffuseSpecularMapSRV);
	SetDiffuseMap(diffuseSpecularMap);

	//// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	//mD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	//mD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


	//// Select which shaders to use next
	//mD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	//mD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	//mD3DContext->OMSetBlendState(mNoBlendingState, nullptr, 0xffffff);
	//mD3DContext->OMSetDepthStencilState(mUseDepthBufferState, 0);
	//mD3DContext->RSSetState(mCullBackState);


	//mD3DContext->PSSetShaderResources(0, 1, &diffuseSpecularMapSRV);
	//mD3DContext->PSSetSamplers(0, 1, &mAnisotropic4xSampler);
	//this->Render();

}

// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
void Model::Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
                                     KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward)
{
    UpdateWorldMatrix();

	if (KeyHeld( turnDown ))
	{
		mRotation.x += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld( turnUp ))
	{
		mRotation.x -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld( turnRight ))
	{
		mRotation.y += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld( turnLeft ))
	{
		mRotation.y -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld( turnCW ))
	{
		mRotation.z += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld( turnCCW ))
	{
		mRotation.z -= ROTATION_SPEED * frameTime;
	}

	// Local Z movement - move in the direction of the Z axis, get axis from world matrix
	if (KeyHeld( moveForward ))
	{
		mPosition.x += mWorldMatrix.e20 * MOVEMENT_SPEED * frameTime;
		mPosition.y += mWorldMatrix.e21 * MOVEMENT_SPEED * frameTime;
		mPosition.z += mWorldMatrix.e22 * MOVEMENT_SPEED * frameTime;
	}
	if (KeyHeld( moveBackward ))
	{
		mPosition.x -= mWorldMatrix.e20 * MOVEMENT_SPEED * frameTime;
		mPosition.y -= mWorldMatrix.e21 * MOVEMENT_SPEED * frameTime;
		mPosition.z -= mWorldMatrix.e22 * MOVEMENT_SPEED * frameTime;
	}
}

void Model::MoveLocalX(float speed)
{
	if (!lookingAt)
	{
		UpdateWorldMatrix();
	}
	mPosition.x += mWorldMatrix.e00 * speed;
	mPosition.y += mWorldMatrix.e01 * speed;
	mPosition.z += mWorldMatrix.e02 * speed;
}
void Model::MoveLocalY(float speed)
{
	if (!lookingAt)
	{
		UpdateWorldMatrix();
	}
	mPosition.x += mWorldMatrix.e10 * speed;
	mPosition.y += mWorldMatrix.e11 * speed;
	mPosition.z += mWorldMatrix.e12 * speed;
}
void Model::MoveLocalZ(float speed)
{
	if (!lookingAt)
	{
		UpdateWorldMatrix();
	}
	mPosition.x += mWorldMatrix.e20 * speed;
	mPosition.y += mWorldMatrix.e21 * speed;
	mPosition.z += mWorldMatrix.e22 * speed;
}

void Model::MoveX(float speed)
{
	mPosition.x += speed;
}
void Model::MoveY(float speed)
{
	mPosition.y += speed;
}
void Model::MoveZ(float speed)
{
	mPosition.z +=  speed;
}
void Model::Move(float x, float y, float z)
{
	mPosition.x += x;
	mPosition.y += y;
	mPosition.z += z;
}


void Model::RotateX(float angle)
{
	mRotation.x += ROTATION_SPEED * angle;
}
void Model::RotateY(float angle)
{
	mRotation.y += ROTATION_SPEED * angle;
}
void Model::RotateZ(float angle)
{
	mRotation.z -= ROTATION_SPEED * angle;
}

CMatrix4x4 Model::GetMatrix()
{
	UpdateWorldMatrix();
	return mWorldMatrix;
}

void Model::SetMatrix(CMatrix4x4 model)
{
	//Need to set pos, rot and scale in order to acutally change the models position relative to another
	UpdateWorldMatrix();

	// Calculate matrix scaling
	float scaleX = sqrt(model.e00*model.e00 + model.e01*model.e01 + model.e02*model.e02);
	float scaleY = sqrt(model.e10*model.e10 + model.e11*model.e11 + model.e12*model.e12);
	float scaleZ = sqrt(model.e20*model.e20 + model.e21*model.e21 + model.e22*model.e22);

	// Calculate inverse scaling to extract rotational values only
	float invScaleX = 1.0f / scaleX;
	float invScaleY = 1.0f / scaleY;
	float invScaleZ = 1.0f / scaleZ;

	float sX, cX, sY, cY, sZ, cZ;

	sX = -model.e21 * invScaleZ;
	cX = sqrt(1.0f - sX * sX);

	// If no gimbal lock...
	if (abs(cX) > 0.001f)
	{
		float invCX = 1.0f / cX;
		sZ = model.e01 * invCX * invScaleX;
		cZ = model.e11 * invCX * invScaleY;
		sY = model.e20 * invCX * invScaleZ;
		cY = model.e22 * invCX * invScaleZ;
	}
	else
	{
		// Gimbal lock - force Z angle to 0
		sZ = 0.0f;
		cZ = 1.0f;
		sY = -model.e02 * invScaleX;
		cY = model.e00 * invScaleX;
	}

	mRotation = { atan2(sX, cX), atan2(sY, cY), atan2(sZ, cZ) };
	mPosition = { model.e30, model.e31, model.e32 };
	mScale = { scaleX, scaleY, scaleZ };
}

void Model::UpdateWorldMatrix()
{
    mWorldMatrix = MatrixScaling(mScale) * MatrixRotationZ(mRotation.z) * MatrixRotationX(mRotation.x) * MatrixRotationY(mRotation.y) * MatrixTranslation(mPosition);
}

void Model::UpdateScale()
{
	mWorldMatrix = MatrixScaling(mScale) * MatrixTranslation(mPosition);
}

void Model::SetX(float pos)
{
	mPosition.x = pos;
}
void Model::SetY(float pos)
{
	mPosition.y = pos;
}
void Model::SetZ(float pos)
{
	mPosition.z = pos;
}

float Model::GetX()
{
	return mWorldMatrix.e30;
}
float Model::GetY()
{
	return mWorldMatrix.e31;
}
float Model::GetZ()
{
	return mWorldMatrix.e32;
}

void Model::SetPSShader(const std::string& shaderFile)
{
	associatedPSShader = LoadPixelShader(shaderFile, myEngine);
}
void Model::SetVSShader(const std::string& shaderFile)
{
	associatedVSShader = LoadVertexShader(shaderFile, myEngine);
}

ID3D11PixelShader* Model::GetPSShader()
{
	return associatedPSShader;
}
ID3D11VertexShader* Model::GetVSShader()
{
	return associatedVSShader;
}

void Model::AddSecondaryTexture(const std::string& texture2)
{
	mMediaFolders = Mesh::GetMediaFolders();
	texture2File = texture2;

	bool directory = false;
	const char slash = '\\';
	for (unsigned int i = 0; i < mMediaFolders.size(); ++i)
	{
		for (unsigned int j = 0; j < 2; ++j)
		{
			if (mMediaFolders[i][mMediaFolders[i].size() - j] == slash)
			{
				directory = true;
			}
			else
			{
				directory = false;
			}
		}
		//if (newMesh != nullptr)
		//{
		if (directory)
		{
			if (!myEngine->LoadTexture(mMediaFolders[i] + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
			{
				//gLastError = "Error loading textures";
			}

		}
		else
		{
			if (!myEngine->LoadTexture(mMediaFolders[i] + slash + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
			{
				//gLastError = "Error loading textures";
			}
		}
		//}
		//else
		//{
		//	break;
		//}
	}
	if (diffuseSpecular2Map == nullptr)
	{
		if (!myEngine->LoadTexture(texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
		{
			//gLastError = "Error loading textures";
		}
	}

}

ID3D11ShaderResourceView* Model::GetDiffuseSRVMap2()
{
	return diffuseSpecularMap2SRV;
}
ID3D11Resource* Model::GetDiffuseMap2()
{
	return diffuseSpecular2Map;
}
std::string Model::GetTextureFile2()
{
	return texture2File;
}
