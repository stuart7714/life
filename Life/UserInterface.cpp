#include "UserInterface.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UserInterface::UserInterface(const D3D11Internals& internals)
{
    // ------------------------------------------------------------
    // Initialize Dear ImGui
    // ------------------------------------------------------------

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(internals.hwnd);
    ImGui_ImplDX11_Init(
        internals.pd3dDevice,
        internals.pd3dDeviceContext);
}

UserInterface::~UserInterface()
{
    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void UserInterface::Render() const
{
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
}

void UserInterface::Present() const
{
    ImGui_ImplDX11_RenderDrawData(
        ImGui::GetDrawData());
}

bool UserInterface::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const
{
    return ImGui_ImplWin32_WndProcHandler(
        hWnd,
        msg,
        wParam,
        lParam);
}
