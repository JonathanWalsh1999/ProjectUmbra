//This is now the main game file
//This will hopefully look similar to the file when you initially create a TL-Engine project

#include "SceneManager.hpp"//The manager class --- > Entire Game Code

void main(IEngine* myEngine)
{
	CSceneManager* scene = new CSceneManager(myEngine);
	delete scene;
}
