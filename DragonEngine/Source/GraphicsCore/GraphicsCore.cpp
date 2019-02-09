#include "GraphicsCore.h"
#include <DirectXColors.h>

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
		Instance = nullptr;
	}
}

int CGraphicsCore::InitDevice(HWND InhWnd)
{
	HRESULT hr = S_OK;

#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ID3D12Debug* DebugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
		DebugController->EnableDebugLayer();
	}
#endif

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));

	if (FAILED(hr))
	{
		return hr;
	}

	hr = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));

	if (FAILED(hr))
	{
		return hr;
	}

	RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DSVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));

	if (FAILED(hr))
	{
		return hr;
	}

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue));

	if (FAILED(hr))
	{
		return hr;
	}

	hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator));

	if (FAILED(hr))
	{
		return hr;
	}

	hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList));

	if (FAILED(hr))
	{
		return hr;
	}

	CommandList->Close();

	RECT Rect;
	GetClientRect(InhWnd, &Rect);
	ClientWidth = Rect.right - Rect.left;
	ClientHeight = Rect.bottom - Rect.top;
	
	DXGI_SWAP_CHAIN_DESC SCDesc{};
	SCDesc.BufferDesc.Height = ClientHeight;
	SCDesc.BufferDesc.Width = ClientWidth;
	SCDesc.BufferDesc.RefreshRate.Numerator = 60;
	SCDesc.BufferDesc.RefreshRate.Denominator = 1;
	SCDesc.BufferDesc.Format = BackBufferFormat;
	SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SCDesc.SampleDesc.Count = 1;
	SCDesc.SampleDesc.Quality = 0;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.BufferCount = SwapChainBufferCount;
	SCDesc.OutputWindow = InhWnd;
	SCDesc.Windowed = true;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = DXGIFactory->CreateSwapChain(CommandQueue, &SCDesc, &SwapChain);
	
	if (FAILED(hr))
	{
		return hr;
	}

	D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc;
	RTVHeapDesc.NumDescriptors = SwapChainBufferCount;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVHeapDesc.NodeMask = 0;

	hr = Device->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&RTVHeap));

	if (FAILED(hr))
	{
		return hr;
	}

	D3D12_DESCRIPTOR_HEAP_DESC DSVHeapDesc;
	DSVHeapDesc.NumDescriptors = 1;
	DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSVHeapDesc.NodeMask = 0;

	hr = Device->CreateDescriptorHeap(&DSVHeapDesc, IID_PPV_ARGS(&DSVHeap));

	if (FAILED(hr))
	{
		return hr;
	}

	OnResize();

	return S_OK;
}

void CGraphicsCore::ReleaseDevice()
{
	if (DXGIFactory != nullptr)
	{
		DXGIFactory->Release();
		DXGIFactory = nullptr;
	}

	if (SwapChain != nullptr)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	if (Device != nullptr)
	{
		Device->Release();
		Device = nullptr;
	}

	if (Fence != nullptr)
	{
		Fence->Release();
		Fence = nullptr;
	}

	if (CommandQueue != nullptr)
	{
		CommandQueue->Release();
		CommandQueue = nullptr;
	}

	if (CommandAllocator != nullptr)
	{
		CommandAllocator->Release();
		CommandAllocator = nullptr;
	}

	if (CommandList != nullptr)
	{
		CommandList->Release();
		CommandList = nullptr;
	}

	if (RTVHeap != nullptr)
	{
		RTVHeap->Release();
		RTVHeap = nullptr;
	}

	if (DSVHeap != nullptr)
	{
		DSVHeap->Release();
		DSVHeap = nullptr;
	}
}

void CGraphicsCore::Update(float InTime)
{

}

void CGraphicsCore::Render(float InTime)
{
	
	CommandAllocator->Reset();

	CommandList->Reset(CommandAllocator, nullptr);
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CommandList->RSSetViewports(1, &ScreenViewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
	CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), DirectX::Colors::Red, 0, nullptr);
	CommandList->ClearDepthStencilView(DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &DSVHeap->GetCPUDescriptorHandleForHeapStart());
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	CommandList->Close();

	ID3D12CommandList* CMDSLists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(_countof(CMDSLists), CMDSLists);

	SwapChain->Present(0, 0);
	CurrentBackBuffer = (CurrentBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
	
}

void CGraphicsCore::FlushCommandQueue()
{
	++CurrentFence;

	CommandQueue->Signal(Fence, CurrentFence);

	if (Fence->GetCompletedValue() < CurrentFence)
	{
		HANDLE EventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		Fence->SetEventOnCompletion(CurrentFence, EventHandle);

		WaitForSingleObject(EventHandle, INFINITE);

		CloseHandle(EventHandle);
	}
}

void CGraphicsCore::OnResize()
{
	FlushCommandQueue();

	CommandList->Reset(CommandAllocator, nullptr);

	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		if (SwapChainBuffers[i] != nullptr)
		{
			SwapChainBuffers[i]->Release();
		}
		SwapChainBuffers[i] = nullptr;
	}

	if (DepthStencilBuffer != nullptr)
	{
		DepthStencilBuffer->Release();
	}

	DepthStencilBuffer = nullptr;

	SwapChain->ResizeBuffers(SwapChainBufferCount, ClientWidth, ClientHeight, BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	CurrentBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());
	
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffers[i]));
		Device->CreateRenderTargetView(SwapChainBuffers[i], nullptr, RTVHeapHandle);
		RTVHeapHandle.Offset(1, RTVDescriptorSize);
	}

	D3D12_RESOURCE_DESC DepthStencilDesc;
	DepthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthStencilDesc.Alignment = 0;
	DepthStencilDesc.Width = ClientWidth;
	DepthStencilDesc.Height = ClientHeight;
	DepthStencilDesc.DepthOrArraySize = 1;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE OptClear;
	OptClear.Format = DepthStencilFormat;
	OptClear.DepthStencil.Depth = 1.0f;
	OptClear.DepthStencil.Stencil = 0;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&DepthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&OptClear,
		IID_PPV_ARGS(&DepthStencilBuffer));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Format = DepthStencilFormat;
	DSVDesc.Texture2D.MipSlice = 0;
	Device->CreateDepthStencilView(DepthStencilBuffer, &DSVDesc, DSVHeap->GetCPUDescriptorHandleForHeapStart());

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	CommandList->Close();
	ID3D12CommandList* CMDSLists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(_countof(CMDSLists), CMDSLists);

	FlushCommandQueue();

	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = static_cast<float>(ClientWidth);
	ScreenViewport.Height = static_cast<float>(ClientHeight);
	ScreenViewport.MinDepth = 0.0f;
	ScreenViewport.MaxDepth = 1.0f;

	ScissorRect = { 0, 0, ClientWidth, ClientHeight };
}

ID3D12Resource* CGraphicsCore::GetCurrentBackBuffer()
{
	return SwapChainBuffers[CurrentBackBuffer];
}

D3D12_CPU_DESCRIPTOR_HANDLE CGraphicsCore::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RTVHeap->GetCPUDescriptorHandleForHeapStart(),
		CurrentBackBuffer,
		RTVDescriptorSize);
}