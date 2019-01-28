#include "Core/Core.h"
#include "Application/Application.h"
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	CApplication* ApplicationInstance = CApplication::Init(hInstance);

	if (ApplicationInstance != nullptr)
	{
		ApplicationInstance->BeginLoop();
	}

	CApplication::Shutdown();

	return 0;
}