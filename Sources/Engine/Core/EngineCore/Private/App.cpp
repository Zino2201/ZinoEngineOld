#include "App.h"
#include <cstdlib>

namespace ZE
{


CApp::CApp(const int& InArgc, const char** InArgv) : bRun(true) 
{ 
	must(!CurrentApp); CurrentApp = this; 
}

void CApp::Run()
{
	while(bRun)
	{
		ProcessEvents();
		Loop();
	}
}

namespace App
{

void Exit(const int& InErrCode)
{
	CApp::GetCurrentApp()->~CApp();
	exit(InErrCode);
}

}

}