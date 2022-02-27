//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.

#include "Model.hpp"
#include "Scene.hpp"


#include "Mesh.hpp"

#include "DirectX11Engine.hpp"

namespace umbra_engine
{

std::vector<IModel*> Model::objectList;
std::vector<std::string> Model::mMediaFolders;

Model::Model(IMesh* mesh, IEngine * engine = nullptr, maths::CVector3 position /*= { 0,0,0 }*/, maths::CVector3 rotation /*= { 0,0,0 }*/, float scale /*= 1*/)
	: mMesh(mesh), mPosition(position), mRotation(rotation), mScale({ scale, scale, scale })
{
	addBlending = false;
	blend = None;
	objectList.push_back(this);
	myEngine = engine;
	mTexture = std::make_unique<CTexture>();

	// Set default matrices from mesh
	mWorldMatrices.resize(mesh->NumberNodes());
	for (int i = 0; i < mWorldMatrices.size(); ++i)
	{
		mWorldMatrices[i] = mesh->GetNodeDefaultMatrix(i);
	}

}

std::vector<IModel*> Model::GetAllObjects()
{
	return objectList;
}

void Model::LookAt(IModel* target)
{
	maths::CVector3 yAxis = { 0.0f, 1.0f, 0.0f };
	maths::CVector3 vecZ = Normalise(target->Position() - Position());
	maths::CVector3 vecX = Normalise(Cross(yAxis, vecZ));
	maths::CVector3 vecY = Normalise(Cross(vecZ, vecX));

	maths::CMatrix4x4 newMatrix;
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
	maths::CVector3 yAxis = { 0.0f, 1.0f, 0.0f };
	maths::CVector3 vecZ = Normalise(target->Position() - Position());
	maths::CVector3 vecX = Normalise(Cross(yAxis, vecZ));
	maths::CVector3 vecY = Normalise(Cross(vecZ, vecX));

	maths::CMatrix4x4 newMatrix;
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
	//Set the correct vs and ps for each model
	myEngine->GetContext()->PSSetShader(associatedPSShader, nullptr, 0);
	myEngine->GetContext()->VSSetShader(associatedVSShader, nullptr, 0);

	// Select the approriate textures and sampler to use in the pixel shader
	//myEngine->GetContext()->PSSetShaderResources(0, 1, &diffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
	myEngine->GetContext()->PSSetShaderResources(1, 1, &diffuseSpecularMap2SRV);
	myEngine->GetContext()->PSSetShaderResources(3, 1, &diffuseSpecularMap3SRV);

	myScene = myEngine->GetScene();
	ID3D11SamplerState* mAnisotropic4xSampler = myScene->GetAnisotropic4xSampler();

	myEngine->GetContext()->PSSetSamplers(0, 1, &mAnisotropic4xSampler);


	mPerModelConstants = myEngine->GetModelConstants();


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
	UpdateConstantBuffer(myEngine->GetModelConstantBuffer(), myEngine->GetModelConstants(), myEngine->GetContext()); // Send to GPU

	mPerModelConstantBuffer = myEngine->GetModelConstantBuffer();
	
	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	myEngine->GetContext()->VSSetConstantBuffers(1, 1, &mPerModelConstantBuffer); // First parameter must match constant buffer number in the shader
	myEngine->GetContext()->PSSetConstantBuffers(1, 1, &mPerModelConstantBuffer);



	mMesh->Render(mWorldMatrices);

}

void Model::SetSkin(const std::string& colour)
{
	//std::vector<SModelCreation> allModels;


	try
	{
		diffuseSpecularMap = nullptr;
		diffuseSpecularMapSRV = nullptr;



		std::vector<std::string> mediaFolders = myEngine->GetMediaFolders();

		IMesh* newMesh = nullptr;
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
			if (this != nullptr)
			{
				if (directory)
				{
					if (!mTexture->LoadTexture(mediaFolders[i] + slash + colour, myEngine->GetDevice(), myEngine->GetContext()))
					{
						throw std::runtime_error("File name does not exist in this location");
					}
				}
				else
				{
					if (!mTexture->LoadTexture(mediaFolders[i] + slash + colour, myEngine->GetDevice(), myEngine->GetContext()))
					{
						throw std::runtime_error("File name does not exist in this location");
					}
				}
				diffuseSpecularMap = mTexture->GetTexture();
				diffuseSpecularMapSRV = mTexture->GetTextureSRV();
			}
			else
			{
				throw std::runtime_error("File name does not exist in this location");
			}

		}
	}
	catch (std::exception & e)
	{

	}
}

// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
void Model::Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
	KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward)
{
	UpdateWorldMatrix();

	if (KeyHeld(turnDown))
	{
		mRotation.x += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnUp))
	{
		mRotation.x -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnRight))
	{
		mRotation.y += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnLeft))
	{
		mRotation.y -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnCW))
	{
		mRotation.z += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnCCW))
	{
		mRotation.z -= ROTATION_SPEED * frameTime;
	}

	// Local Z movement - move in the direction of the Z axis, get axis from world matrix
	if (KeyHeld(moveForward))
	{
		mPosition.x += mWorldMatrix.e20 * MOVEMENT_SPEED * frameTime;
		mPosition.y += mWorldMatrix.e21 * MOVEMENT_SPEED * frameTime;
		mPosition.z += mWorldMatrix.e22 * MOVEMENT_SPEED * frameTime;
	}
	if (KeyHeld(moveBackward))
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
	mPosition.z += speed;
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

maths::CMatrix4x4 Model::GetMatrix()
{
	UpdateWorldMatrix();
	return mWorldMatrix;
}

void Model::SetMatrix(maths::CMatrix4x4 model)
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
	mWorldMatrix = maths::MatrixScaling(mScale) * maths::MatrixRotationZ(mRotation.z) * maths::MatrixRotationX(mRotation.x) * maths::MatrixRotationY(mRotation.y) * maths::MatrixTranslation(mPosition);
}

void Model::UpdateScale()
{
	mWorldMatrix = maths::MatrixScaling(mScale) * maths::MatrixTranslation(mPosition);
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
		//if (directory)
		//{
		//	if (!myEngine->LoadTexture(mMediaFolders[i] + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
		//	{
		//		//gLastError = "Error loading textures";
		//	}

		//}
		//else
		//{
		//	if (!myEngine->LoadTexture(mMediaFolders[i] + slash + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
		//	{
		//		//gLastError = "Error loading textures";
		//	}
		//}
		//}
		//else
		//{
		//	break;
		//}
	}
	//if (diffuseSpecular2Map == nullptr)
	//{
	//	if (!myEngine->LoadTexture(texture2File, &diffuseSpecular2Map, &diffuseSpecularMap2SRV))
	//	{
	//		//gLastError = "Error loading textures";
	//	}
	//}

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

ID3D11Resource* Model::GetDiffuseMap3()
{
	return diffuseSpecular3Map;
}
std::string Model::GetTextureFile3()
{
	return texture3File;
}
void Model::AddThirdTexture(const std::string& texture3)
{
	mMediaFolders = Mesh::GetMediaFolders();
	texture3File = texture3;

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
		////{
		//if (directory)
		//{
		//	if (!myEngine->LoadTexture(mMediaFolders[i] + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap3SRV))
		//	{
		//		//gLastError = "Error loading textures";
		//	}

		//}
		//else
		//{
		//	if (!myEngine->LoadTexture(mMediaFolders[i] + slash + texture2File, &diffuseSpecular2Map, &diffuseSpecularMap3SRV))
		//	{
		//		//gLastError = "Error loading textures";
		//	}
		//}
		//}
		//else
		//{
		//	break;
		//}
	}
	if (diffuseSpecular2Map == nullptr)
	{
		//if (!myEngine->LoadTexture(texture2File, &diffuseSpecular2Map, &diffuseSpecularMap3SRV))
		//{
		//	//gLastError = "Error loading textures";
		//}
	}
}
ID3D11ShaderResourceView* Model::GetDiffuseSRVMap3()
{
	return diffuseSpecularMap3SRV;
}

}