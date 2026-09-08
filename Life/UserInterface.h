#pragma once

#include <memory>

#include "ID3D11FrameworkExtension.h"
#include "ISimulation.h"

class UserInterface : public ID3D11FrameworkExtension
{
public:
	UserInterface(const D3D11Internals& internals);
	virtual ~UserInterface() override;

	void SetSimulation(std::unique_ptr<ISimulation> simulation);

private:
	std::unique_ptr<ISimulation> m_simulation;

	// ID3D11FrameworkExtension
	virtual void Render() const override;
	virtual void Present() const override;
	virtual bool ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const override;

	void RenderFileControls(bool isRunning) const;
	void RenderPlaybackControls(bool isRunning) const;
	void RenderControls() const;
	void RenderGrid() const;

	std::optional<std::filesystem::path> OpenFileDialog() const;
	std::optional<std::filesystem::path> SaveFileDialog() const;
};
