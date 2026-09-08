#pragma once

#include "Grid.h"
#include "ISimulation.h"

#include <chrono>

class Simulation : public ISimulation
{
public:
	Simulation(double updateInterval);

private:
	// ISimulation
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

	int CountNeighbours(const Coord& centre) const;
	void Update();
	static constexpr std::size_t MaxX = 30;
	static constexpr std::size_t MaxY = 20;
	Grid<bool, MaxX, MaxY> m_cells{};
	bool m_running = false;
	double m_updateInterval = 0.0;
	std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
};
