#ifndef _JSONPARSER_H_
#define _JSONPARSER_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Implement a parser using JSON
//--------------------------------------------------------------------------------------

#include "IParser.hpp"

//Rapid JSON parser --> Can be found via this link: https://github.com/Tencent/rapidjson
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filereadstream.h"
#include "istreamwrapper.h"

#include <iostream>
#include <fstream>
#include <vector>

//======================================================================================
namespace umbra_engine
{
//---------------------------------------
// Class Forward Declarations
//---------------------------------------
class IEngine;
class IModel;
class IMesh;
class ILight;

class CJSONParser : public IParser
{
public:
//---------------------------------------
// Constructors / Destructor
//---------------------------------------
	CJSONParser(IEngine* engine);
	~CJSONParser();

//---------------------------------------
// Data Access
//---------------------------------------
	std::vector<ILight*> GetLights() { return allLights; };

//---------------------------------------
// Operational Methods
//---------------------------------------
	void LoadParser(const std::string& fileName);//Load json parser from file

private:
//---------------------------------------
// Private Member Methods
//---------------------------------------
	void LoadModels();//Loads all models from json file
	void LoadMeshes(IMesh** mesh, rapidjson::Value & value);
	void LoadLights();

//---------------------------------------
// Private Member Variables
//---------------------------------------
	rapidjson::Document d;//Instantiate parser

	//All models and meshes in the scene added by level editor
	std::vector<IMesh*> allMeshes;
	std::vector<std::unique_ptr<IModel>> allModels;
	std::vector<std::string> meshFileNames;
	std::vector<ILight*> allLights;

	IEngine* myEngine;//Engine passed over from scene manager	
};//Class
}//Namespace
//======================================================================================
#endif//Header Guard