#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Create a 3D Scene - HIGH LEVEL CODE
//--------------------------------------------------------------------------------------

#include "DirectX11Engine.hpp"
#include "IScene.hpp"
#include "IParser.hpp"
#include "JSONParser.hpp"


#include "imgui.h"

#include <string>
#include <vector>
#include <memory>
#include <Windows.h>
#include <conio.h>

//======================================================================================
namespace developer
{

class CSceneManager
{
public:
//---------------------------------------
// Constructors / Destructor
//---------------------------------------
	CSceneManager(umbra_engine::IEngine* engine); 
	~CSceneManager();

private:
//---------------------------------------
// Private Member Methods
//---------------------------------------
	void PulsateLight(umbra_engine::ILight* chosenLight, const float& lightStrength); //Make light brighter and dimmer
	void CycleLightColours(umbra_engine::ILight* chosenLight); //Change RGB values constantly
	void UpdateColourChannels(float& channelValue, bool& rgbBool);
	
//---------------------------------------
// Private Member Variables
//---------------------------------------
	//Raw Pointers
	umbra_engine::IEngine* myEngine;// Engine pointer
	umbra_engine::IScene* myScene;
	std::vector<umbra_engine::ILight*> lights;	//Create Lights

	umbra_engine::IGui* myGui;

	//Unique Pointers
	std::unique_ptr<umbra_engine::IParser> myParser;

	const std::string mMediaFolder = "media"; //Folder to store all files such as textures and meshes for scene

	int lightIndex = 0;

	//Extract frame time
	Timer mTimer;
	float mFrameTime;
	float mTotalTime = 0.0f;

	//Constants effecting speed
	const float PULSATE_SPEED = 10.0f;
	const float COLOUR_CHANGE_SPEED = 0.1f;
	const float CUBE_MOVEMENT_SPEED = 100.0f;

	//Turn parallax mapping on or off
	bool parallaxOn = true;

	//Keycode constants
	const KeyCode ParallaxOnOff = Key_1;
	const KeyCode Quit = Key_Escape;
	const KeyCode MoveCube = Key_Period;
	const KeyCode toZBuffer = Key_1;
	const KeyCode toPCF = Key_2;

	umbra_engine::EShadowEffect myShadow;
	float mDayNightCycle = 0.0f;


	


};//Class
}//Namespace
//======================================================================================
#endif//Header Guard