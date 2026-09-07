#pragma once

#include "Grid.h"

class ISimulation
{
public:
	virtual ~ISimulation() = default;
	virtual void Tick() = 0;
	virtual Coord Size() const = 0;
	virtual bool Get(const Coord& coord) const = 0;
};
