#include <iostream>

#include "D3D11Framework.h"
#include "UserInterface.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    try
    {
        D3D11Framework framework;
        D3D11Internals internals = framework.GetD3D11Internals();

        framework.SetExtension(std::make_unique<UserInterface>(internals));
        framework.EventLoop();
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
