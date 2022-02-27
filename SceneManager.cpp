#include "SceneManager.hpp"


namespace developer
{

CSceneManager::CSceneManager(umbra_engine::IEngine* engine)
{
	myEngine = engine;

	myParser = std::make_unique<umbra_engine::CJSONParser>(myEngine);

	myEngine->StartWindowed();
	myEngine->InitEngine();

	mTimer.Start();

	/*Set up the scene here*/
	myShadow = umbra_engine::ZBuffer;

	myEngine->AddMediaFolder(mMediaFolder);
	myParser->LoadParser("LevelEditor.json");
	   
	lights = myParser->GetLights();
	myScene = myEngine->GetScene();
	myGui = myEngine->CreateGUI();



	while (myEngine->IsRunning())
	{
		//Draw the scene
		mFrameTime = mTimer.GetLapTime();
		mTotalTime += mFrameTime;

		myScene->RenderLights(lights);
		myGui->RenderGUI();

		myScene->RenderScene(mFrameTime);

		static bool toDay = true;
		if (toDay)
		{
			mDayNightCycle += mFrameTime * 0.1f;
		}
		else
		{
			mDayNightCycle -= mFrameTime * 0.1f;
		}
		if (mDayNightCycle < 1.0f && mDayNightCycle < 5.0f && !toDay)
		{
			toDay = true;
		}
		else if(mDayNightCycle >= 1.0f && mDayNightCycle >= 5.0f && toDay)
		{
			toDay = false;
		}
		myScene->SetDayNight(mDayNightCycle);

		if (mTotalTime > 15.0f)
		{
			/*Code to be updated once per frame*/
			PulsateLight(lights[2], 20.0f);
		}

		//Toggle between the different shadowing techniques
		if (KeyHit(toZBuffer))
		{
			myShadow = umbra_engine::ZBuffer;
			myEngine->SetShadowEffect(myShadow);
		}
		if (KeyHit(toPCF))
		{
			myShadow = umbra_engine::PCF;
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


void CSceneManager::PulsateLight(umbra_engine::ILight* chosenLight, const float& lightStrength)
{
	static float pulsator = lightStrength;
	static bool fadingIn = true;
	if (!fadingIn)
	{
		if (pulsator <= 0.0f)
		{
			fadingIn = true;
		}
		else
		{
			//Light gets dimmer
			pulsator -= PULSATE_SPEED;
		}
	}
	else
	{
		if (pulsator >= lightStrength)
		{
			fadingIn = false;
		}
		else
		{
			//Light gets brighter
			pulsator += PULSATE_SPEED;
		}
	}
	chosenLight->SetLightStrength(pulsator);
}

void CSceneManager::CycleLightColours(umbra_engine::ILight* chosenLight)
{
	struct SBoolCoords { bool x, y, z; };
	static umbra_engine::maths::CVector4 colourCycle = { 0.0f,0.5f,1.0f,0.0f };
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

}