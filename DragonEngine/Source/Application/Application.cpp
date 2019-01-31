#include "Application.h"
#include "GraphicsCore/GraphicsCore.h"
#include "Engine/Engine.h"
#include "InputCore/InputCore.h"
#include "WidgetCore/WidgetCore.h"

CApplication* CApplication::Instance = nullptr;

CApplication* CApplication::Get()
{
	return Instance;
}

CApplication* CApplication::Init(HINSTANCE hInstance)
{
	if (Instance == nullptr)
	{
		Instance = new CApplication();

		if (Instance->InitWindow(hInstance))
		{
			Shutdown();
		}

		if (CEngine::Init() == nullptr)
		{
			Shutdown();
		}

		if (CGraphicsCore::Init(Instance->hWnd) == nullptr)
		{
			Shutdown();
		}

		if (CInputCore::Init() == nullptr)
		{
			Shutdown();
		}

		if (CWidgetCore::Init() == nullptr)
		{
			Shutdown();
		}
	}

	return Instance;
}

void CApplication::Shutdown()
{
	CWidgetCore::Shutdown();
	CInputCore::Shutdown();
	CGraphicsCore::Shutdown();
	CEngine::Shutdown();

	if (Instance != nullptr)
	{
		delete Instance;
	}
}

int CApplication::InitWindow(HINSTANCE InhInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = InhInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindowClass";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(L"MainWindowClass", L"Dragon Engine", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, InhInstance,
		NULL);

	if (!hWnd)
		return E_FAIL;

	ShowWindow(hWnd, 1);

	return S_OK;
}

void CApplication::BeginLoop()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}
}

void CApplication::Render()
{
	CGraphicsCore* GraphicsCore = CGraphicsCore::Get();
	if (GraphicsCore != nullptr)
	{
		GraphicsCore->Render();
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}