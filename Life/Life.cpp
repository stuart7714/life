#include <iostream>

#include "D3D11Framework.h"
#include "Simulation.h"
#include "UserInterface.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    try
    {
        // Create a graphics framework so we can render the user interface using D3D11.
        D3D11Framework framework;
        D3D11Internals internals = framework.GetD3D11Internals();

        // Create a user interface.
        std::unique_ptr<UserInterface> userInterface = std::make_unique<UserInterface>(internals);

        // Create a simulation of Conway's Game of Life and give the user interface access.
        userInterface->SetSimulation(std::make_unique<Simulation>(0.33));

        // Run the user interface inside the D3D11 framework.
        framework.SetExtension(std::move(userInterface));
        framework.EventLoop();
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
