# life
[Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) written in C++ for programming practice

# Overview

A grid of cells is simulated from an initial state such that the cells live and die according to the number of neighbouring cells.

The user draws or loads an initial state and begins the simulation. The users views the cells graphically on a grid and watches as old cells die and new cells are born.

The user interface contains a number of buttons:

- New: All the cells are cleared so that a new arrangement of cells can be drawn.
- Load: The user loads a JSON file which contains an arrangement of cells.
- Save: The user saves a JSON file which contains an arrangement of cells.
- Play/Stop: The user starts or stops the simulation and watches the cells update three times a second.
- Next: The user moves the simulation one step manually to see the next state.

# Code Structure

There are four main components to the code:

- A D3D11 framework which allows us to display a user interface.
- A user interface created using the imgui package.
- A simulation of Conway's Game of Life which implements the rules and updates the grid of cells.
- A grid which is represented as a 2-D array and includes a coordinate type.

Simulation states are saved to JSON files using the nlohmann package.