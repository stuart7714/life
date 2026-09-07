#include <stdexcept>

#include "D3D11Framework.h"

D3D11Framework::D3D11Framework()
{
    // Create Windows window
    m_wc = {
        sizeof(m_wc),
        CS_CLASSDC,
        &D3D11Framework::WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"ImGuiTest",
        nullptr
    };

    ::RegisterClassExW(&m_wc);

    m_hwnd = ::CreateWindowW(
        m_wc.lpszClassName,
        L"D3D11 Window",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        800,
        600,
        nullptr,
        nullptr,
        m_wc.hInstance,
        this);

    // ------------------------------------------------------------
    // Create DirectX 11 device
    // ------------------------------------------------------------

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &m_pSwapChain,
        &m_pd3dDevice,
        &featureLevel,
        &m_pd3dDeviceContext);

    if (FAILED(hr))
    {
        throw std::runtime_error("Unable to create device and swap chain");
    }

    CreateRenderTarget();
}

D3D11Framework::~D3D11Framework()
{
    m_extension.reset();

    CleanupRenderTarget();

    if (m_pSwapChain)
        m_pSwapChain->Release();

    if (m_pd3dDeviceContext)
        m_pd3dDeviceContext->Release();

    if (m_pd3dDevice)
        m_pd3dDevice->Release();

    ::DestroyWindow(m_hwnd);
    ::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
}

D3D11Internals D3D11Framework::GetD3D11Internals() const
{
    return { m_hwnd, m_pd3dDevice, m_pd3dDeviceContext };
}

void D3D11Framework::SetExtension(std::unique_ptr<ID3D11FrameworkExtension> extension)
{
    m_extension = std::move(extension);
}

void D3D11Framework::EventLoop() const
{
    // Show window
    ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_hwnd);

    // ------------------------------------------------------------
    // Main loop
    // ------------------------------------------------------------

    bool running = true;

    while (running)
    {
        MSG msg;

        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                running = false;
        }

        if (!running)
            break;

        m_extension->Render();

        const float clearColor[4] = {
            0.1f,
            0.1f,
            0.1f,
            1.0f
        };

        m_pd3dDeviceContext->OMSetRenderTargets(
            1,
            &m_mainRenderTargetView,
            nullptr);

        m_pd3dDeviceContext->ClearRenderTargetView(
            m_mainRenderTargetView,
            clearColor);

        m_extension->Present();

        m_pSwapChain->Present(1, 0);
    }
}

// Create render target
void D3D11Framework::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;

    m_pSwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&pBackBuffer));

    m_pd3dDevice->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &m_mainRenderTargetView);

    pBackBuffer->Release();
}

// Cleanup render target
void D3D11Framework::CleanupRenderTarget()
{
    if (m_mainRenderTargetView)
    {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

// Windows message handler
LRESULT WINAPI D3D11Framework::WndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    D3D11Framework* framework = reinterpret_cast<D3D11Framework*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (framework && framework->m_extension && framework->m_extension->ProcessEvent(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_NCCREATE:

        {
            auto* createStruct =
                reinterpret_cast<CREATESTRUCTW*>(lParam);

            framework =
                static_cast<D3D11Framework*>(createStruct->lpCreateParams);

            SetWindowLongPtrW(
                hWnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(framework));
        }

        break;

    case WM_SIZE:

        if (framework && framework->m_pd3dDevice != nullptr &&
            wParam != SIZE_MINIMIZED)
        {
            framework->CleanupRenderTarget();

            framework->m_pSwapChain->ResizeBuffers(
                0,
                LOWORD(lParam),
                HIWORD(lParam),
                DXGI_FORMAT_UNKNOWN,
                0);

            framework->CreateRenderTarget();
        }

        return 0;

    case WM_SYSCOMMAND:

        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;

        break;

    case WM_DESTROY:

        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam);
}
