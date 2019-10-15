#include "SceneManager.h"

CSceneManager::CSceneManager(CDX11Engine* engine)
{
	myEngine = engine;

	myEngine->StartWindowed();
	myEngine->EngineRun();

	mTimer.Start();

	myEngine->AddMediaFolder(mMediaFolder);

	/*Set up the scene here*/
	SetUpScene();

	while (myEngine->IsRunning())
	{
		//Draw the scene
		mFrameTime = mTimer.GetLapTime();
		myScene = myEngine->GetScene();
		myScene->RenderLights(lights);
		myScene->RenderScene(mFrameTime);
		myEngine->SetScene(myScene);

		/*Code to be updated once per frame*/

		PulsateLight(mLight1, mLight1Strength);
		CycleLightColours(mLight2);	
	
		WiggleUpdate();

		//myEngine->mPerFrameConstants.gParallaxDepth = (parallaxOn ? 0.08f : 0);

		mCubeNormal->Control(mFrameTime, Key_U, Key_J, Key_H, Key_K, Key_0, Key_0, Key_Period, Key_Comma);


		if (KeyHit(ParallaxOnOff))
		{
			//Turn parallax mapping on or off
			parallaxOn = !parallaxOn;
		}
				
		if (KeyHit(Quit))
		{
			//Close the program
			myEngine->Stop();
		}
	}

}

void CSceneManager::LoadMeshes()
{
	mHillMesh = myEngine->LoadMesh(mHillMeshFile);
	mTeaPotMesh = myEngine->LoadMesh(mTeaMeshFile);
	mSphereMesh = myEngine->LoadMesh(mSphereMeshFile);
	mCubeMesh = myEngine->LoadMesh(mCubeMeshFile);
}

void CSceneManager::CreateModels()
{
	mHill = mHillMesh->CreateModel(mCobbleTextureFile, mHillPos.x, mHillPos.y, mHillPos.z, mParallaxShaderFile.ps, mParallaxShaderFile.vs);
	mHill->AddSecondaryTexture(mCobbleHeightTextureFile);
	mTeaPot = mTeaPotMesh->CreateModel(mTechTextureFile, mTeaPos.x, mTeaPos.y, mTeaPos.z, mParallaxShaderFile.ps, mParallaxShaderFile.vs);
	mTeaPot->AddSecondaryTexture(mTechHeightTextureFile);
	mSphere = mSphereMesh->CreateModel(mStoneTextureFile, mSpherePos.x, mSpherePos.y, mSpherePos.z, mSphereModelShaderFile.ps, mSphereModelShaderFile.vs);
	mCube = mCubeMesh->CreateModel(mWoodTextureFile, mCubePos.x, mCubePos.y, mCubePos.z, mCubeModelShaderFile.ps, mCubeModelShaderFile.vs);
	mCube->AddSecondaryTexture(mBrickTextureFile);
	mCubeNormal = mCubeMesh->CreateModel(mPatternTextureFile, mCubeNormalPos.x, mCubeNormalPos.y, mCubeNormalPos.z, mParallaxShaderFile.ps, mParallaxShaderFile.vs);
	mCubeNormal->AddSecondaryTexture(mPatternNormalTextureFile);
	mCubeParallax = mCubeMesh->CreateModel(mTechTextureFile, mCubeParallaxPos.x, mCubeParallaxPos.y, mCubeParallaxPos.z, mParallaxShaderFile.ps, mParallaxShaderFile.vs);
	mCubeParallax->AddSecondaryTexture(mTechHeightTextureFile);
	mCubeMultiplicative = mCubeMesh->CreateModel(mGlassTextureFile, mCubeMultiplicativePos.x, mCubeMultiplicativePos.y, mCubeMultiplicativePos.z,
																	mPixelShaderFile.ps, mPixelShaderFile.vs);
	mCubeMultiplicative->SetAddBlend(Multi);
	mSmoke = mCubeMesh->CreateModel(mSmokeTextureFile, mSmokePos.x, mSmokePos.y, mSmokePos.z, mPixelShaderFile.ps, mPixelShaderFile.vs);
	mSmoke->SetAddBlend(Alpha);
}

void CSceneManager::SetUpScene()
{
	LoadMeshes();
	CreateModels();

	//myEngine->mPerFrameConstants.wiggle = 0;

	SetUpLighting();
}

void CSceneManager::SetUpLighting()
{
	EBlendingType newBlend = Add;

	mLight1Strength = 120.0f;
	mLight1Position = { 0.0f, 30.0f, 50.0f };
	mLight1ConeAngle = 90.0f;
	mLight1Colour = { 1.0f, 1.0f, 1.0f };

	mAmbientColour = { 0.1f, 0.1f, 0.1f };
	mSpecularPower = 256.0f;
	mLightModelScale = 0.7f;

	mLight2Strength = 40.0f;
	mLight2Position = { 150.0f, 50.0f, 0.0f };


	mLight1 = myEngine->CreateLight();
	mLight1->SetAmbientColour(mAmbientColour);
	mLight1->SetLightColour(mLight1Colour);
	mLight1->SetLightNumber(0);
	mLight1->SetLightStrength(mLight1Strength);
	mLight1->SetSpecularPower(mSpecularPower);
	mLight1->SetPosition(mLight1Position);
	mLight1->SetLightAngle(mLight1ConeAngle);
	mLight1->AddToVector();
	mLight1->lightMesh = myEngine->LoadMesh(mLightMeshFile);
	mLight1->lightModel = mLight1->lightMesh->CreateModel(mLightTextureFile, mLight1->GetPosition().x, mLight1->GetPosition().y,
														mLight1->GetPosition().z, mLightPixelShaderFile, mLightVertexShaderFile);
	mLight1->lightModel->SetScale(pow(mLight1->GetLightStrength(), mLightModelScale));
	mLight1->lightModel->SetAddBlend(newBlend);
	lights.push_back(mLight1);

	mLight2 = myEngine->CreateLight();
	mLight2->SetAmbientColour(mAmbientColour);
	mLight2->SetLightNumber(1);
	mLight2->SetLightStrength(mLight1Strength);
	mLight2->SetSpecularPower(mSpecularPower);
	mLight2->SetPosition(mLight2Position);

	mLight2->lightMesh = myEngine->LoadMesh(mLightMeshFile);
	mLight2->lightModel = mLight2->lightMesh->CreateModel(mLightTextureFile, mLight2->GetPosition().x, mLight2->GetPosition().y,
														mLight2->GetPosition().z, mLightPixelShaderFile, mLightVertexShaderFile);
	mLight2->lightModel->SetAddBlend(newBlend);
	mLight2->lightModel->SetScale(pow(mLight2->GetLightStrength(), mLightModelScale));
	mLight2->AddToVector();
	lights.push_back(mLight2);
}

void CSceneManager::PulsateLight(Light* chosenLight, const float& lightStrength)
{
	static float pulsator = lightStrength;
	static bool fadingIn = false;
	if (!fadingIn)
	{
		//Light gets dimmer
		pulsator -= mFrameTime * PULSATE_SPEED;
		if (pulsator < 0.0f)
		{
			fadingIn = true;
		}
	}
	else
	{
		//Light gets brighter
		pulsator += mFrameTime * PULSATE_SPEED;
		if (pulsator > lightStrength)
		{
			fadingIn = false;
		}
	}
	chosenLight->SetLightStrength(pulsator);
}

void CSceneManager::CycleLightColours(Light* chosenLight)
{	
	struct SBoolCoords{ bool x, y, z; };
	static CVector3 colourCycle = { 0.0f,0.5f,1.0f };
	static SBoolCoords colourCycleBools = { true, true, true };

	//Cycle through each channel in RGB
	UpdateColourChannels(colourCycle.x, colourCycleBools.x);
	UpdateColourChannels(colourCycle.y, colourCycleBools.y);
	UpdateColourChannels(colourCycle.z, colourCycleBools.z);

	chosenLight->SetLightColour(colourCycle);
}

void CSceneManager::UpdateColourChannels(float& channelValue, bool& rgbBool)
{

	if (channelValue < 0.0f && !rgbBool)
	{
		rgbBool = true;
	}
	else if (channelValue > 1.0f && rgbBool)
	{
		rgbBool = false;
	}

	if (rgbBool)
	{
		//RGB value increases
		channelValue += mFrameTime * COLOUR_CHANGE_SPEED;
	}
	else
	{
		//RGB value decreases
		channelValue -= mFrameTime * COLOUR_CHANGE_SPEED;
	}
}

void CSceneManager::WiggleUpdate()
{
	static bool wigglePositive = true;

	if (wigglePositive)
	{
		//Increase wiggle value
		//myEngine->mPerFrameConstants.wiggle += 10.0f * mFrameTime;
	}
	else
	{
		//Decrease wiggle value
		//myEngine->mPerFrameConstants.wiggle -= 10.0f * mFrameTime;
	}


	//if (myEngine->mPerFrameConstants.wiggle > 10.0f)
	//{
	//	wigglePositive = false;
	//}
	//else if(myEngine->mPerFrameConstants.wiggle < 0.0f)
	//{
	//	wigglePositive = true;
	//}
}

CSceneManager::~CSceneManager()
{	
	//Release memory
	myEngine->Delete();
}