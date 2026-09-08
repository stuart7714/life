#pragma once

#define NOMINMAX
#include <d3d11.h>

/*
 * Certain D3D11 internal values which allow us to initialize other graphics systems.
 */
struct D3D11Internals
{
	HWND hwnd = NULL;
	ID3D11Device* pd3dDevice = nullptr;
	ID3D11DeviceContext* pd3dDeviceContext = nullptr;
};

/*
 * An interface which allows us to add extra functionality to the D3D11 Windows event loop.
 */
class ID3D11FrameworkExtension
{
public:
	// Virtual destructor.
	virtual ~ID3D11FrameworkExtension() = default;
	
	// Extra functionality during the render phase of the Windows event loop.
	virtual void Render() const = 0;
	
	// Extra functionality during the present phase of the Windows event loop.
	virtual void Present() const = 0;
	
	// Extra functionality during the event processing phase of the Windows event loop.
	virtual bool ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const = 0;
};
