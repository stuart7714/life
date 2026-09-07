#pragma once

#define NOMINMAX
#include <d3d11.h>

struct D3D11Internals
{
	HWND hwnd = NULL;
	ID3D11Device* pd3dDevice = nullptr;
	ID3D11DeviceContext* pd3dDeviceContext = nullptr;
};

class ID3D11FrameworkExtension
{
public:
	virtual ~ID3D11FrameworkExtension() = default;
	virtual void Render() const = 0;
	virtual void Present() const = 0;
	virtual bool ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const = 0;
};
