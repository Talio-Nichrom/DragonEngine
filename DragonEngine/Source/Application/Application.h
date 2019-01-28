#pragma once

#include "Core/Core.h"
#include <Windows.h>

class CApplication
{
public:

	static CApplication* Get();

	static CApplication* Init(HINSTANCE hInstance);
	static void Shutdown();

	int InitWindow(HINSTANCE InhInstance);
	void BeginLoop();

	void Render();

private:
	static CApplication* Instance;

	HWND hWnd;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);