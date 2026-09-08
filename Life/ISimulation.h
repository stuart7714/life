#pragma once

#include "Grid.h"

/*
 * An interface which allows us to control a simulation of Conway's Game of Life.
 * It also allows us to access information about the state of the simulation.
 */
class ISimulation
{
public:
	// Virtual destructor.
	virtual ~ISimulation() = default;

	// Tick the simulation to move it to the next state.
	virtual void Tick() = 0;

	// Create a new simulation.
	virtual void New() = 0;

	// Load a simulation state from the given file.
	virtual void Load(const std::filesystem::path& pathname) = 0;
	
	// Save the current simulation state to the given file.
	virtual void Save(const std::filesystem::path& pathname) = 0;
	
	// Play the simulation so it will continually move to the next state.
	virtual void Play() = 0;
	
	// Stop the simulation.
	virtual void Stop() = 0;
	
	// Incrementally move the simulation to the next state.
	virtual void Next() = 0;

	// Is the simulation currently in the play state?
	virtual bool IsRunning() const = 0;

	// Get the size of the grid the simulation is using.
	virtual Coord Size() const = 0;
	
	// Get a cell of the simulation which tells us if the cell is alive.
	virtual bool Get(const Coord& coord) const = 0;
	
	// Set a cell of the simulation so the cell can be alive or dead.
	virtual void Set(const Coord& coord, bool value) = 0;
};
