#include <iostream>

#include "D3D11Framework.h"
#include "Simulation.h"
#include "UserInterface.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    try
    {
        D3D11Framework framework;
        D3D11Internals internals = framework.GetD3D11Internals();

        std::unique_ptr<UserInterface> userInterface = std::make_unique<UserInterface>(internals);

        std::vector<Coord> coords = {
            {10,10},
            {11,10},
            {11,9},
            {11,8},
            {12,8},
            {12,7},
            {13,7} };
        userInterface->SetSimulation(std::make_unique<Simulation>(coords, 1.0));

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
