#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <tchar.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static void CreateRenderTarget();
static void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPSTR,
    int)
{
    // Create Windows window
    WNDCLASSEXW wc = {
        sizeof(wc),
        CS_CLASSDC,
        WndProc,
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

    ::RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(
        wc.lpszClassName,
        L"Dear ImGui Test",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        800,
        600,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    // ------------------------------------------------------------
    // Create DirectX 11 device
    // ------------------------------------------------------------

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
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
        &g_pSwapChain,
        &g_pd3dDevice,
        &featureLevel,
        &g_pd3dDeviceContext);

    if (FAILED(hr))
        return 1;

    CreateRenderTarget();

    // ------------------------------------------------------------
    // Initialize Dear ImGui
    // ------------------------------------------------------------

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(
        g_pd3dDevice,
        g_pd3dDeviceContext);

    // Show window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

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

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // --------------------------------------------------------
        // Our UI
        // --------------------------------------------------------

        ImGui::Begin("Test Window");

        ImGui::Text("Hello from Dear ImGui!");

        ImGui::Separator();

        static int number = 42;

        ImGui::InputInt("Number", &number);

        ImGui::SliderInt(
            "Slider",
            &number,
            0,
            100);

        static bool checkbox = false;

        ImGui::Checkbox(
            "Enable something",
            &checkbox);

        static float value = 0.5f;

        ImGui::SliderFloat(
            "Value",
            &value,
            0.0f,
            1.0f);

        if (ImGui::Button("Click me"))
        {
            number++;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
        {
            number = 0;
            value = 0.5f;
            checkbox = false;
        }

        ImGui::Separator();

        ImGui::Text(
            "Number = %d",
            number);

        ImGui::Text(
            "Checkbox = %s",
            checkbox ? "true" : "false");

        ImGui::Text(
            "Value = %.2f",
            value);

        ImGui::End();

        // --------------------------------------------------------
        // Render
        // --------------------------------------------------------

        ImGui::Render();

        const float clearColor[4] = {
            0.1f,
            0.1f,
            0.1f,
            1.0f
        };

        g_pd3dDeviceContext->OMSetRenderTargets(
            1,
            &g_mainRenderTargetView,
            nullptr);

        g_pd3dDeviceContext->ClearRenderTargetView(
            g_mainRenderTargetView,
            clearColor);

        ImGui_ImplDX11_RenderDrawData(
            ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    CleanupRenderTarget();

    if (g_pSwapChain)
        g_pSwapChain->Release();

    if (g_pd3dDeviceContext)
        g_pd3dDeviceContext->Release();

    if (g_pd3dDevice)
        g_pd3dDevice->Release();

    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


// Create render target
static void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;

    g_pSwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&pBackBuffer));

    g_pd3dDevice->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &g_mainRenderTargetView);

    pBackBuffer->Release();
}


// Cleanup render target
static void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}


// Windows message handler
LRESULT WINAPI WndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(
        hWnd,
        msg,
        wParam,
        lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_SIZE:

        if (g_pd3dDevice != nullptr &&
            wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();

            g_pSwapChain->ResizeBuffers(
                0,
                LOWORD(lParam),
                HIWORD(lParam),
                DXGI_FORMAT_UNKNOWN,
                0);

            CreateRenderTarget();
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
