#pragma once

#include "Grid.h"

class ISimulation
{
public:
	virtual ~ISimulation() = default;

	virtual void Tick() = 0;

	virtual void New() = 0;
	virtual void Load(const std::filesystem::path& pathname) = 0;
	virtual void Save(const std::filesystem::path& pathname) = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void Next() = 0;

	virtual bool IsRunning() const = 0;

	virtual Coord Size() const = 0;
	virtual bool Get(const Coord& coord) const = 0;
	virtual void Set(const Coord& coord, bool value) = 0;
};
