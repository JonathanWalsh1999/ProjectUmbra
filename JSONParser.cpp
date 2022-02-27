#include "JSONParser.hpp"

#include "DirectX11Engine.hpp"
#include "IEngine.hpp"
#include "IModel.hpp"
#include "IMesh.hpp"
#include "ILight.hpp"

namespace umbra_engine
{

CJSONParser::CJSONParser(IEngine* engine)
{
	myEngine = engine;
}
CJSONParser::~CJSONParser()
{

}

void CJSONParser::LoadParser(const std::string& fileName)
{
	//Create an input file stream
	std::ifstream inFile(fileName);

	//Checks if file is open
	if (inFile.is_open())
	{
		//Use rapidjson repo to parse through data in json file
		rapidjson::IStreamWrapper isw(inFile);//Puts data from file into a stream

		d.ParseStream(isw);
	}
	if (d.HasParseError())
	{
		//Check fro errors
		std::cout << "parse error" << std::endl;
	}
	else
	{
		//Load meshes and create models for scene from json file
		LoadModels();

		LoadLights();
	}

	//Close the file as we have finished with it
	inFile.close();
}

void CJSONParser::LoadMeshes(IMesh** mesh, rapidjson::Value & value)
{
	//Create mesh from filename if not done so all ready
	if (meshFileNames.size() == 0)
	{
		*mesh = myEngine->LoadMesh(value["meshFileName"].GetString());//Get the name of the mesh and load mesh
		allMeshes.push_back(*mesh);//Keeps track of all meshes

		meshFileNames.push_back(value["meshFileName"].GetString());//Keeps track of all file names for meshes
	}
	else
	{
		for (int iFile = 0; iFile < meshFileNames.size(); ++iFile)
		{
			//If the mesh is already created from the file name then use the mesh
			//again. Not a good idea to give every model a mesh
			if (meshFileNames[iFile] == value["meshFileName"].GetString())
			{
				*mesh = allMeshes[iFile];
				break;
			}
		}

		//If no mesh found for current file name, create one
		if (*mesh == nullptr)
		{
			*mesh = myEngine->LoadMesh(value["meshFileName"].GetString());//Get the name of the mesh and load mesh
			allMeshes.push_back(*mesh);
			meshFileNames.push_back(value["meshFileName"].GetString());
		}
	}
}

void CJSONParser::LoadModels()
{
	rapidjson::Value& models = d["models"];//Find the array labels "models"
	assert(models.IsArray());//Check if it is an array

	//Current mesh/model
	IMesh* mesh = nullptr;
	std::unique_ptr<IModel> model = nullptr;

	//Loop through all models in json file
	for (rapidjson::SizeType i = 0; i < models.Size(); ++i)
	{
		LoadMeshes(&mesh, models[i]);

		//Get the position of the model
		rapidjson::Value& position = models[i]["position"];
		assert(position.IsArray());

		//Create model
		model = mesh->CreateModel(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());

		//Get scale
		model->SetScale(models[i]["scale"].GetFloat());

		//Get rotation
		rapidjson::Value& rotation = models[i]["rotation"];
		assert(rotation.IsArray());
		model->SetRotation({ rotation[0].GetFloat(), rotation[1].GetFloat(), rotation[2].GetFloat() });

		//Keep track of all models by adding them to vector
		allModels.push_back(std::move(model));

		model = nullptr;
		mesh = nullptr;
	}

	//Release memory

}

void CJSONParser::LoadLights()
{
	rapidjson::Value& lights = d["lights"];//Find the array labels "models"
	assert(lights.IsArray());//Check if it is an array

	IMesh* mesh = nullptr;
	std::unique_ptr<IModel> model = nullptr;
	ILight* light = nullptr;

	for (rapidjson::SizeType i = 0; i < lights.Size(); ++i)
	{
		LoadMeshes(&mesh, lights[i]);

		//Get the position of the model
		rapidjson::Value& position = lights[i]["position"];
		assert(position.IsArray());

		model = mesh->CreateModel(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());

		model->SetAddBlend(Add);

		rapidjson::Value& scale = lights[i]["modelScale"];
		model->SetScale(scale.GetFloat());

		rapidjson::Value& lightType = lights[i]["lightType"];
		ELightType type;
		std::string lightTypeString = lightType.GetString();
		if (lightTypeString == "Directional")
		{
			type = Directional;
		}
		else if (lightTypeString == "Point")
		{
			type = Point;
		}
		else if (lightTypeString == "Spot")
		{
			type = Spot;
		}
		light = myEngine->CreateLight(type);
		light->SetPosition({ position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat(), position[3].GetFloat() });

		rapidjson::Value& ambientColour = lights[i]["ambientColour"];
		assert(ambientColour.IsArray());
		light->SetAmbientColour({ ambientColour[0].GetFloat(), ambientColour[1].GetFloat(), ambientColour[2].GetFloat() });

		rapidjson::Value& lightColour = lights[i]["lightColour"];
		assert(lightColour.IsArray());
		light->SetLightColour({ lightColour[0].GetFloat(), lightColour[1].GetFloat(), lightColour[2].GetFloat(), lightColour[3].GetFloat() });

		rapidjson::Value& lightStrength = lights[i]["lightStrength"];
		light->SetLightStrength(lightStrength.GetFloat());

		rapidjson::Value& specularPower = lights[i]["specularPower"];
		light->SetSpecularPower(specularPower.GetFloat());

		rapidjson::Value& lightAngle = lights[i]["coneAngle"];
		light->SetLightAngle(lightAngle.GetFloat());

		light->SetModel(model.get());
		light->SetMesh(mesh);

		allLights.push_back(light);
		allModels.push_back(std::move(model));



		mesh = nullptr;
		model = nullptr;
		light = nullptr;
	}

	//allLights[0]->GetModel()->RotateX(maths::ToRadians(170.0f));
	//allLights[0]->GetModel()->RotateX(maths::ToRadians(-10.0f));





}

}