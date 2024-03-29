#pragma once
#include "Renderer.h"
#include "Imgui.h"

// DirectX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif
#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

/**
 * @brief This class implements the DirectX12 backend for ImGui. It contains all the high level functions
 * to initialize the directx engine and call the official imgui renderer.
 *
 */
class Dx12Renderer : public Renderer
{
    /**
     * @brief
     *
     */
    struct FrameContext
    {
        ID3D12CommandAllocator *CommandAllocator;
        UINT64 FenceValue;
    };

private:
    // Data
    static int const NUM_FRAMES_IN_FLIGHT = 3;
    static int const NUM_BACK_BUFFERS = 3;
    FrameContext m_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
    UINT m_frameIndex = 0;

    ID3D12Device *m_pd3dDevice = NULL;
    ID3D12DescriptorHeap *m_pd3dRtvDescHeap = NULL;
    ID3D12DescriptorHeap *m_pd3dSrvDescHeap = NULL;
    ID3D12CommandQueue *m_pd3dCommandQueue = NULL;
    ID3D12GraphicsCommandList *m_pd3dCommandList = NULL;
    ID3D12Fence *m_fence = NULL;
    HANDLE m_fenceEvent = NULL;
    UINT64 m_fenceLastSignaledValue = 0;
    IDXGISwapChain3 *m_pSwapChain = NULL;
    HANDLE m_hSwapChainWaitableObject = NULL;
    ID3D12Resource *m_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

    bool CreateDeviceD3D(HWND hWnd);
    FrameContext *WaitForNextFrameResources();
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();

public:
    /**
     * @brief Construct a new Dx12 Renderer object
     *
     * @param hwnd Handle on the WINAPI Windows object
     */
    explicit Dx12Renderer(HWND hwnd);
    ~Dx12Renderer();
    bool Init();
    void NewFrame();
    void Shutdown();
    void Render();
    void Resize(void *lParam);
    void Destroy();
};