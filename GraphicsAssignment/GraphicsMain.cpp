//--------------------------------------------------------------------------------------
// Entry point for the application
// Window creation code
//--------------------------------------------------------------------------------------

#include "DirectX11Engine.h"



//--------------------------------------------------------------------------------------
// The entry function for a Windows application is called wWinMain
//--------------------------------------------------------------------------------------
int APIENTRY wWinMain(
_In_     HINSTANCE hInstance,
_In_opt_ HINSTANCE hPrevInstance,
_In_     LPWSTR    lpCmdLine,
_In_     int       nCmdShow)
{	
	//Create engine object
	CDX11Engine* myEngine;
	myEngine = new CDX11Engine(hInstance, hPrevInstance, lpCmdLine, nCmdShow);   
	main(myEngine);

    MSG msg = {};
    return (int)msg.wParam;
}


