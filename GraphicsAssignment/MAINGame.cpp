//This is now the main game file
//This will hopefully look similar to the file when you initially create a TL-Engine project

#include "SceneManager.h"//The manager class --- > Entire Game Code

void main(CDX11Engine* myEngine)
{
	CSceneManager* scene = new CSceneManager(myEngine);
	delete scene;
}
