#pragma once

#include "Core/Core.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>

class CGraphicsCore
{
public:

	static CGraphicsCore* Get();
	static CGraphicsCore* Init(HWND InhWnd);
	static void Shutdown();

	int InitDevice(HWND InhWnd);
	void ReleaseDevice();

	void Render();

private:
	static CGraphicsCore* Instance;

	D3D_DRIVER_TYPE         DriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           D3DDevice = NULL;
	ID3D11DeviceContext*    ImmediateContext = NULL;
	IDXGISwapChain*         SwapChain = NULL;
	ID3D11RenderTargetView* RenderTargetView = NULL;

};