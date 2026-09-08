#include "UserInterface.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <windows.h>
#include <shobjidl.h>

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
    if (!m_simulation)
    {
        return;
    }

    m_simulation->Tick();

    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // --------------------------------------------------------
    // Our UI
    // --------------------------------------------------------

    RenderControls();
    RenderGrid();

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

void UserInterface::RenderFileControls(bool isRunning) const
{
    ImGui::BeginDisabled(isRunning);

    if (ImGui::Button("New"))
    {
        m_simulation->New();
    }
    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        std::optional<std::filesystem::path> pathname = OpenFileDialog();
        if (pathname)
        {
            m_simulation->Load(*pathname);
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Save"))
    {
        std::optional<std::filesystem::path> pathname = SaveFileDialog();
        if (pathname)
        {
            m_simulation->Save(*pathname);
        }
    }
    ImGui::SameLine();

    ImGui::EndDisabled();
}

void UserInterface::RenderPlaybackControls(bool isRunning) const
{
    if (ImGui::Button(isRunning ? "Stop" : "Play"))
    {
        if (isRunning)
        {
            m_simulation->Stop();
        }
        else
        {
            m_simulation->Play();
        }
    }
    ImGui::SameLine();

    ImGui::BeginDisabled(isRunning);
    if (ImGui::Button("Next"))
    {
        m_simulation->Next();
    }
    ImGui::EndDisabled();
}

void UserInterface::RenderControls() const
{
    bool isRunning = m_simulation->IsRunning();

    ImGui::Begin("Controls");

    RenderFileControls(isRunning);
    RenderPlaybackControls(isRunning);

    ImGui::End();
}

void UserInterface::RenderGrid() const
{
    Coord size = m_simulation->Size();
    constexpr float cellSize = 20.0f;

    ImGui::Begin("Conway's Game of Life");

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

            Coord coord = { x, y };
            bool alive = m_simulation->Get(coord);

            ImU32 colour = alive
                ? IM_COL32(0, 0, 0, 255)        // true = black
                : IM_COL32(255, 255, 255, 255); // false = white

            drawList->AddRectFilled(min, max, colour);

            // Optional border around each cell
            drawList->AddRect(min, max, IM_COL32(128, 128, 128, 255));

            ImGui::SetCursorScreenPos(min);
            ImGui::PushID(y * size.x + x);

            if (ImGui::InvisibleButton("cell", ImVec2(cellSize, cellSize)))
            {
                m_simulation->Set(coord, !alive);
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
}

std::optional<std::filesystem::path> UserInterface::OpenFileDialog() const
{
    IFileOpenDialog* dialog = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog));

    if (FAILED(hr))
        return std::nullopt;

    // Optional: restrict the file types
    COMDLG_FILTERSPEC filters[] =
    {
        { L"JSON files", L"*.json" },
        { L"All files",  L"*.*"   }
    };

    dialog->SetFileTypes(
        ARRAYSIZE(filters),
        filters);

    hr = dialog->Show(nullptr);

    if (FAILED(hr))
    {
        dialog->Release();
        return std::nullopt;   // Cancelled, or an error occurred
    }

    IShellItem* item = nullptr;

    hr = dialog->GetResult(&item);

    if (FAILED(hr))
    {
        dialog->Release();
        return std::nullopt;
    }

    PWSTR filename = nullptr;

    hr = item->GetDisplayName(SIGDN_FILESYSPATH, &filename);

    std::filesystem::path result;

    if (SUCCEEDED(hr))
    {
        result = filename;
        CoTaskMemFree(filename);
    }

    item->Release();
    dialog->Release();

    return result;
}

std::optional<std::filesystem::path> UserInterface::SaveFileDialog() const
{
    IFileSaveDialog* dialog = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_FileSaveDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog));

    if (FAILED(hr))
        return std::nullopt;

    COMDLG_FILTERSPEC filters[] =
    {
        { L"JSON files", L"*.json" },
        { L"All files",  L"*.*"   }
    };

    dialog->SetFileTypes(ARRAYSIZE(filters), filters);

    // Default extension when the user doesn't type one.
    dialog->SetDefaultExtension(L"json");

    hr = dialog->Show(nullptr);

    if (FAILED(hr))
    {
        dialog->Release();
        return std::nullopt;
    }

    IShellItem* item = nullptr;

    hr = dialog->GetResult(&item);

    if (FAILED(hr))
    {
        dialog->Release();
        return std::nullopt;
    }

    PWSTR filename = nullptr;

    hr = item->GetDisplayName(SIGDN_FILESYSPATH, &filename);

    std::filesystem::path result;

    if (SUCCEEDED(hr))
    {
        result = filename;
        CoTaskMemFree(filename);
    }

    item->Release();
    dialog->Release();

    return result;
}