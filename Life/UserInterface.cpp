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

void UserInterface::SetSimulation(std::unique_ptr<ISimulation> simulation)
{
    m_simulation = std::move(simulation);
}

void UserInterface::Render() const
{
    if (m_simulation)
    {
        m_simulation->Tick();
    }

    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // --------------------------------------------------------
    // Our UI
    // --------------------------------------------------------

    ImGui::Begin("Conway's Game of Life");

    if (m_simulation)
    {
        Coord size = m_simulation->Size();

        constexpr float cellSize = 20.0f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 origin = ImGui::GetCursorScreenPos();

        for (int y = 0; y < size.y; ++y)
        {
            for (int x = 0; x < size.x; ++x)
            {
                ImVec2 min(
                    origin.x + x * cellSize,
                    origin.y + y * cellSize);

                ImVec2 max(
                    min.x + cellSize,
                    min.y + cellSize);

                ImU32 colour = m_simulation->Get({ x, y })
                    ? IM_COL32(0, 0, 0, 255)       // true = black
                    : IM_COL32(255, 255, 255, 255); // false = white

                drawList->AddRectFilled(min, max, colour);

                // Optional border around each cell
                drawList->AddRect(min, max, IM_COL32(128, 128, 128, 255));
            }
        }

        // Reserve space so subsequent ImGui widgets don't overlap the grid
        ImGui::Dummy(ImVec2(10 * cellSize, 10 * cellSize));
    }

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
