#include "App.h"
#include <cstdlib>

namespace ZE
{


CApp::CApp(const int& InArgc, const char** InArgv) : bRun(true), ErrCode(0)
{ 
	must(!CurrentApp); CurrentApp = this; 
}

int CApp::Run()
{
	while(bRun)
	{
		ProcessEvents();
		Loop();
	}

	return ErrCode;
}

void CApp::Exit(const int& InErrCode)
{
	ErrCode = InErrCode;
	bRun = false;
}

namespace App
{

void Exit(const int& InErrCode)
{
	CApp::GetCurrentApp()->Exit(InErrCode);
}

}

}