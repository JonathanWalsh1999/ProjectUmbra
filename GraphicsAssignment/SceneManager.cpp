#include "SceneManager.hpp"

CSceneManager::CSceneManager(IEngine * engine)
{
	myEngine = engine;

	myEngine->StartWindowed();
	myEngine->EngineRun();
	myShadow = ZBuffer;


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

		//PulsateLight(mLight1, mLight1Strength);
		CycleLightColours(mLight2);	
	


		//myEngine->mPerFrameConstants.gParallaxDepth = (parallaxOn ? 0.08f : 0);

		//mCubeNormal->Control(mFrameTime, Key_U, Key_J, Key_H, Key_K, Key_0, Key_0, Key_Period, Key_Comma);

		//Toggle between the different shadowing techniques
		if (KeyHit(toZBuffer))
		{
			myShadow = ZBuffer;
			myEngine->SetShadowEffect(myShadow);
		}
		if (KeyHit(toPCF))
		{
			myShadow = PCF;
			myEngine->SetShadowEffect(myShadow);
		}


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

	mHill = mHillMesh->CreateModel(mCobbleTextureFile, mHillPos.x, mHillPos.y, mHillPos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mHill->AddSecondaryTexture(mCobbleHeightTextureFile);
	mTeaPot = mTeaPotMesh->CreateModel(mTechTextureFile, mTeaPos.x, mTeaPos.y, mTeaPos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mTeaPot->AddSecondaryTexture(mTechHeightTextureFile);
	mSphere = mSphereMesh->CreateModel(mStoneTextureFile, mSpherePos.x, mSpherePos.y, mSpherePos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mCube = mCubeMesh->CreateModel(mWoodTextureFile, mCubePos.x, mCubePos.y, mCubePos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mCube->AddSecondaryTexture(mBrickTextureFile);
	mCubeNormal = mCubeMesh->CreateModel(mPatternTextureFile, mCubeNormalPos.x, mCubeNormalPos.y, mCubeNormalPos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mCubeNormal->AddSecondaryTexture(mPatternNormalTextureFile);
	mCubeParallax = mCubeMesh->CreateModel(mTechTextureFile, mCubeParallaxPos.x, mCubeParallaxPos.y, mCubeParallaxPos.z, mMainShaderFile.ps, mMainShaderFile.vs);
	mCubeParallax->AddSecondaryTexture(mTechHeightTextureFile);
	mCubeMultiplicative = mCubeMesh->CreateModel(mGlassTextureFile, mCubeMultiplicativePos.x, mCubeMultiplicativePos.y, mCubeMultiplicativePos.z,
		mMainShaderFile.ps, mMainShaderFile.vs);
	mCubeMultiplicative->SetAddBlend(Multi);
	mSmoke = mCubeMesh->CreateModel(mSmokeTextureFile, mSmokePos.x, mSmokePos.y, mSmokePos.z, mMainShaderFile.ps, mMainShaderFile.vs);
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

	mLight1Strength = 100.0f;
	mLight1Position = { 0.0f, 80.0f, -100.0f, 0.0f };
	mLight1ConeAngle = 90.0f;
	mLight1Colour = { 1.0f, 1.0f, 1.0f,0.0f };

	mAmbientColour = { 0.1f, 0.1f, 0.1f };
	mSpecularPower = 256.0f;
	mLightModelScale = 0.7f;

	mLight2Strength = 10.0f;
	mLight2Position = { 300.0f, 50.0f, 50.0f,0.0f };

	mLight3Position = { -300.0f, 50.0f, 50.0f, 0.0f };


	mLight1 = myEngine->CreateLight(Spot);
	mLight1->SetAmbientColour(mAmbientColour);
	mLight1->SetLightColour(mLight1Colour);

	mLight1->SetLightStrength(mLight1Strength);
	mLight1->SetSpecularPower(mSpecularPower);
	mLight1->SetPosition(mLight1Position);
	mLight1->SetLightAngle(mLight1ConeAngle);
	mLightMesh = myEngine->LoadMesh(mLightMeshFile);
	mLight1->SetMesh(mLightMesh);
	mLightModel1 = mLightMesh->CreateModel(mLightTextureFile, mLight1->GetPosition().x, mLight1->GetPosition().y,
														mLight1->GetPosition().z, mMainShaderFile.ps, mMainShaderFile.vs);

	mLightModel1->SetScale(pow(mLight1->GetLightStrength(), mLightModelScale));
	mLightModel1->SetAddBlend(newBlend);
	mLight1->SetModel(mLightModel1);
	lights.push_back(mLight1);

	mLight2 = myEngine->CreateLight(Point);
	mLight2->SetAmbientColour(mAmbientColour);

	mLight2->SetLightStrength(mLight1Strength);
	mLight2->SetSpecularPower(mSpecularPower);
	mLight2->SetPosition(mLight2Position);


	mLightModel2 = mLightMesh->CreateModel(mLightTextureFile, mLight2->GetPosition().x, mLight2->GetPosition().y,
														mLight2->GetPosition().z, mMainShaderFile.ps, mMainShaderFile.vs);
	mLightModel2->SetAddBlend(newBlend);
	mLightModel2->SetScale(pow(mLight2->GetLightStrength(), mLightModelScale));
	mLight2->SetModel(mLightModel2);
	//mLight2->AddToVector();
	lights.push_back(mLight2);

	mLight3 = myEngine->CreateLight(Point);
	mLight3->SetLightColour({1.0f, 1.0f,1.0f,0.0f});
	mLightModel3 = mLightMesh->CreateModel(mLightTextureFile, mLight3->GetPosition().x, mLight3->GetPosition().y,
		mLight3->GetPosition().z, mMainShaderFile.ps, mMainShaderFile.vs);
	mLightModel3->SetAddBlend(newBlend);
	mLightModel3->SetScale(pow(mLight3->GetLightStrength(), mLightModelScale));
	mLight3->SetModel(mLightModel3);


	mLight3->SetAmbientColour(mAmbientColour);

	mLight3->SetLightStrength(mLight1Strength);
	mLight3->SetSpecularPower(mSpecularPower);
	mLight3->SetPosition(mLight3Position);



	lights.push_back(mLight3);

}

void CSceneManager::PulsateLight(ILight* chosenLight, const float& lightStrength)
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

void CSceneManager::CycleLightColours(ILight* chosenLight)
{	
	struct SBoolCoords{ bool x, y, z; };
	static CVector4 colourCycle = { 0.0f,0.5f,1.0f,0.0f };
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



CSceneManager::~CSceneManager()
{	
	//Release memory
	myEngine->Delete();
}