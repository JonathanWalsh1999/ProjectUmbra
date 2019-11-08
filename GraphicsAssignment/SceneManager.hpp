#pragma once
//Where everything is actully implemented

#include <string>
#include <vector>
#include "DirectX11Engine.hpp"



class CSceneManager
{
public:
	CSceneManager(IEngine * engine); //Constructor, pass over engine
	~CSceneManager(); //Destructor

private:
	IEngine * myEngine;// Engine pointer

	void LoadMeshes(); //Load all the mesh files
	void CreateModels(); //Create and initialise models
	void SetUpScene(); 
	void SetUpLighting();

	void PulsateLight(ILight* chosenLight, const float& lightStrength); //Make light brighter and dimmer

	void CycleLightColours(ILight* chosenLight); //Change RGB values constantly
	void UpdateColourChannels(float& channelValue, bool& rgbBool);

	void WiggleUpdate();//Change the wiggle variable values up and down depending on frame time.

	const std::string mMediaFolder = "media"; //Folder to store all files such as textures and meshes for scene

	//Extract frame time
	Timer mTimer; 
	float mFrameTime;
	
	//Meshes created in the scene
	IMesh* mHillMesh;
	IMesh* mTeaPotMesh;
	IMesh* mSphereMesh;
	IMesh* mCubeMesh;
	IMesh* mLightMesh;

	//Models created in the scene
	IModel* mHill;
	IModel* mTeaPot;
	IModel* mSphere;
	IModel* mCube;
	IModel* mCubeNormal;
	IModel* mCubeParallax;
	IModel* mCubeMultiplicative;
	IModel* mSmoke;
	IModel* mLightModel1;
	IModel* mLightModel2;

	//Create Lights
	ILight* mLight1; //Spotlight
	ILight* mLight2; //Point light
	std::vector<ILight*> lights;

	//Light variables
	float mLight1Strength;
	CVector3 mLight1Colour;
	CVector3 mLight1Position;
	float mLight1ConeAngle;

	float mLight2Strength;
	CVector3 mLight2Position;

	CVector3 mAmbientColour;
	float mSpecularPower;
	float mLightModelScale;
	const std::string mLightMeshFile = "Light.x";
	const std::string mLightTextureFile = "Flare.jpg";
	const std::string mLightPixelShaderFile = "LightModel_ps";
	const std::string mLightVertexShaderFile = "LightModel_vs";



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

	//Mesh file constants
	const std::string mHillMeshFile = "Ground.x";
	const std::string mTeaMeshFile = "Teapot.x";
	const std::string mSphereMeshFile = "Sphere.x";
	const std::string mCubeMeshFile = "Cube.x";

	//Texture File Constants
	const std::string mCobbleTextureFile = "CobbleDiffuseSpecular.dds";
	const std::string mCobbleHeightTextureFile = "CobbleNormalHeight.dds";
	const std::string mTechTextureFile = "TechDiffuseSpecular.dds";
	const std::string mTechHeightTextureFile = "TechNormalHeight.dds";
	const std::string mStoneTextureFile = "StoneDiffuseSpecular.dds";
	const std::string mWoodTextureFile = "WoodDiffuseSpecular.dds";
	const std::string mBrickTextureFile = "brick1.jpg";
	const std::string mPatternTextureFile = "PatternDiffuseSpecular.dds";
	const std::string mPatternNormalTextureFile = "PatternNormal.dds";
	const std::string mGlassTextureFile = "Glass.jpg";
	const std::string mSmokeTextureFile = "Smoke.png";

	//Shader file constants
	struct ShaderFile { const std::string ps, vs; };

	ShaderFile mParallaxShaderFile{ "ParallaxMapping_ps", "ParallaxMapping_vs" };
	ShaderFile mSphereModelShaderFile{ "SphereModel_ps", "SphereModel_vs" };
	ShaderFile mCubeModelShaderFile{ "CubeModel_ps", "CubeModel_vs" };
	ShaderFile mNormalShaderFile{ "NormalMapping_ps", "NormalMapping_vs"};
	ShaderFile mPixelShaderFile{ "PixelLighting_ps", "PixelLighting_vs" };
	ShaderFile mMainShaderFile{ "main_ps", "main_vs" };

	//Model Initial Positions
	CVector3 mHillPos{ 0.0f, 0.0f, 0.0f };
	CVector3 mTeaPos{ 0.0f, 4.0f, 0.0f };
	CVector3 mSpherePos{ 25.0f, 8.0f, 0.0f };
	CVector3 mCubePos{ 17.0f, 8.0f, -20.0f };
	CVector3 mCubeNormalPos{ 34.0f, 8.0f, -30.0f };
	CVector3 mCubeParallaxPos{ -10.0f, 28.0f, 25.0f };
	CVector3 mCubeMultiplicativePos{50.0f, 8.0f, 0.0f};
	CVector3 mSmokePos{40.0f, 28.0f, -20.0f};

	IScene* myScene;
};