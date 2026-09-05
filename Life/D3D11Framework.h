#pragma once

#include <memory>

#include "ID3D11FrameworkExtension.h"

class D3D11Framework
{
public:
	D3D11Framework();
	~D3D11Framework();

	D3D11Internals GetD3D11Internals() const;
	void SetExtension(std::unique_ptr<ID3D11FrameworkExtension> extension);
	void EventLoop() const;

private:
	void CreateRenderTarget();
	void CleanupRenderTarget();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WNDCLASSEXW m_wc = { 0 };
	HWND m_hwnd = NULL;
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pd3dDeviceContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;

	std::unique_ptr<ID3D11FrameworkExtension> m_extension;
};
