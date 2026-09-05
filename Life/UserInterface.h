#pragma once

#include "ID3D11FrameworkExtension.h"

class UserInterface : public ID3D11FrameworkExtension
{
public:
	UserInterface(const D3D11Internals& internals);
	virtual ~UserInterface() override;
	virtual void Render() const override;
	virtual void Present() const override;
	virtual bool ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const override;
};
