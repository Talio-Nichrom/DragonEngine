#include "GraphicsCore.h"

CGraphicsCore* CGraphicsCore::Instance = nullptr;

CGraphicsCore* CGraphicsCore::Get()
{
	return Instance;
}

CGraphicsCore* CGraphicsCore::Init(HWND InhWnd)
{
	if (Instance == nullptr)
	{
		Instance = new CGraphicsCore();

		if (Instance->InitDevice(InhWnd))
		{
			Shutdown();
		}
	}

	return Instance;
}

void CGraphicsCore::Shutdown()
{
	if (Instance != nullptr)
	{
		Instance->ReleaseDevice();

		delete Instance;
	}
}

int CGraphicsCore::InitDevice(HWND InhWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(InhWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = InhWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < NumDriverTypes; driverTypeIndex++)
	{
		DriverType = DriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, DriverType, NULL, createDeviceFlags, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &sd, &SwapChain, &D3DDevice, &FeatureLevel, &ImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer = NULL;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	ImmediateContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	ImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

void CGraphicsCore::ReleaseDevice()
{
	if (ImmediateContext)
	{
		ImmediateContext->ClearState();
	}

	if (RenderTargetView)
	{
		RenderTargetView->Release();
	}

	if (SwapChain)
	{
		SwapChain->Release();
	}

	if (ImmediateContext)
	{
		ImmediateContext->Release();
	}

	if (D3DDevice)
	{
		D3DDevice->Release();
	}
}

void CGraphicsCore::Render()
{
	float ClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImmediateContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	SwapChain->Present(0, 0);
}