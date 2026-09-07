#include "Simulation.h"

Simulation::Simulation(const std::vector<Coord>& coords, double updateRate)
	: m_updateInterval(updateRate)
{
	for (const Coord& coord : coords)
	{
		m_cells.Set(coord, true);
	}
}

void Simulation::Tick()
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = currentTime - m_startTime;
	if (elapsed.count() > m_updateInterval)
	{
		m_startTime = currentTime;
	}
	else
	{
		return;
	}

	Grid<int, MaxX, MaxY> counts;
	for (int y = 0; y < MaxY; y++)
	{
		for (int x = 0; x < MaxX; x++)
		{
			Coord coord = { x, y };
			counts.Set(coord, CountNeighbours(coord));
		}
	}

	for (int y = 0; y < MaxY; y++)
	{
		for (int x = 0; x < MaxX; x++)
		{
			Coord coord = { x, y };
			if (m_cells.Get(coord))
			{
				// Check to see if a cell dies.
				if (counts.Get(coord) < 2 || counts.Get(coord) > 3)
				{
					m_cells.Set(coord, false);
				}
			}
			else
			{
				// Check to see if a cell is born.
				if (counts.Get(coord) == 3)
				{
					m_cells.Set(coord, true);
				}
			}
		}
	}
}

Coord Simulation::Size() const
{
	return { MaxX, MaxY };
}

bool Simulation::Get(const Coord& coord) const
{
	return m_cells.Get(coord);
}

int Simulation::CountNeighbours(const Coord& centre) const
{
	Coord min = (centre - 1).max(Coord(0, 0));
	Coord max = (centre + 1).min(Coord(MaxX - 1, MaxY - 1));

	int count = 0;
	for (int y = min.y; y <= max.y; y++)
	{
		for (int x = min.x; x <= max.x; x++)
		{
			Coord coord = { x, y };
			if (coord != centre && m_cells.Get(coord))
			{
				count++;
			}
		}
	}

	return count;
}
