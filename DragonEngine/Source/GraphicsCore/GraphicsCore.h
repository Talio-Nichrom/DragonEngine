#pragma once

#include "Core/Core.h"
#include <Windows.h>
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "D3D12.lib")
#pragma comment (lib, "dxgi.lib")

class CGraphicsCore
{
public:

	static CGraphicsCore* Get();
	static CGraphicsCore* Init(HWND InhWnd);
	static void Shutdown();

	int InitDevice(HWND InhWnd);
	void ReleaseDevice();

	void Update(float InTime);
	void Render(float inTime);

	void FlushCommandQueue();

	void OnResize();

	ID3D12Resource* GetCurrentBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;

private:
	static CGraphicsCore*		Instance;
	static const int			SwapChainBufferCount = 2;

	IDXGIFactory1*				DXGIFactory = nullptr;
	IDXGISwapChain*				SwapChain = nullptr;
	ID3D12Device*				Device = nullptr;
	ID3D12Fence*				Fence = nullptr;
	ID3D12CommandQueue*			CommandQueue = nullptr;
	ID3D12CommandAllocator*		CommandAllocator = nullptr;
	ID3D12GraphicsCommandList*	CommandList = nullptr;
	ID3D12DescriptorHeap*		RTVHeap = nullptr;
	ID3D12DescriptorHeap*		DSVHeap = nullptr;
	
	ID3D12Resource*				SwapChainBuffers[SwapChainBufferCount] = {};
	ID3D12Resource*				DepthStencilBuffer = nullptr;

	DXGI_FORMAT					BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT					DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	UINT64						CurrentFence;

	int							ClientWidth;
	int							ClientHeight;
	int							CurrentBackBuffer;

	UINT						RTVDescriptorSize = 0;
	UINT						DSVDescriptorSize = 0;

	D3D12_VIEWPORT				ScreenViewport;
	D3D12_RECT					ScissorRect;
};