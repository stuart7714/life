#pragma once

#include "Grid.h"
#include "ISimulation.h"

#include <chrono>

/*
 * A simulation of Conway's Game of Life.
 */
class Simulation : public ISimulation
{
public:
	// Create the simulation which will update at the given time interval.
	Simulation(double updateInterval);

private:
	// ISimulation interface implementation.
	virtual void Tick() override;
	virtual void New() override;
	virtual void Load(const std::filesystem::path& pathname) override;
	virtual void Save(const std::filesystem::path& pathname) override;
	virtual void Play() override;
	virtual void Stop() override;
	virtual void Next() override;
	virtual bool IsRunning() const override;
	virtual Coord Size() const override;
	virtual bool Get(const Coord& coord) const override;
	virtual void Set(const Coord& coord, bool value) override;

	// Count the number of living neighbours around the given centre location.
	int CountNeighbours(const Coord& centre) const;
	
	// Move the simulation to the next state.
	void Update();

	// The width of the grid of cells.
	static constexpr std::size_t MaxX = 30;
	
	// The height of the grid of cells.
	static constexpr std::size_t MaxY = 20;
	
	// The grid of cells which tells us if each cell is alive or dead.
	Grid<bool, MaxX, MaxY> m_cells{};
	
	// Whether the simulation is running.
	bool m_running = false;
	
	// The time interval controlling how frequently we update the simulation.
	double m_updateInterval = 0.0;
	
	// The time when we started the update so we know when to next perform an update.
	std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
};
