#pragma once

#include "Grid.h"
#include "ISimulation.h"

#include <chrono>
#include <vector>

class Simulation : public ISimulation
{
public:
	Simulation(const std::vector<Coord>& coords, double updateInterval);

	// ISimulation
	virtual void Tick() override;
	virtual Coord Size() const override;
	virtual bool Get(const Coord& coord) const override;

private:
	int CountNeighbours(const Coord& centre) const;
	static constexpr std::size_t MaxX = 30;
	static constexpr std::size_t MaxY = 20;
	Grid<bool, MaxX, MaxY> m_cells{};
	double m_updateInterval = 0.0;
	std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
};
