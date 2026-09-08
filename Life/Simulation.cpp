#include <fstream>
#include <iostream>

#include "Simulation.h"

Simulation::Simulation(double updateRate)
	: m_updateInterval(updateRate)
{
}

void Simulation::Tick()
{
	if (!m_running)
	{
		return;
	}

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

	Update();
}

void Simulation::New()
{
	m_cells.Reset();
}

void Simulation::Load(const std::filesystem::path& pathname)
{
	std::ifstream file(pathname);
	if (!file)
	{
		std::cerr << std::format("Failed to load simulation {}", pathname.string()) << std::endl;
	}
	std::string jsonString;
	std::getline(file, jsonString);

	Coords coordList = nlohmann::json::parse(jsonString).get<Coords>();

	m_cells.Reset();
	for (const Coord& coord : coordList.m_coords)
	{
		m_cells.Set(coord, true);
	}
}

void Simulation::Save(const std::filesystem::path& pathname)
{
	Coords coordList;

	for (int y = 0; y < MaxY; y++)
	{
		for (int x = 0; x < MaxX; x++)
		{
			Coord coord = { x, y };
			if (m_cells.Get(coord))
			{
				coordList.m_coords.push_back(coord);
			}
		}
	}

	nlohmann::json json = coordList;
	std::string jsonString = json.dump();
	std::ofstream file(pathname);
	if (!file)
	{
		std::cerr << std::format("Failed to save simulation {}", pathname.string()) << std::endl;
	}
	file << jsonString;
}

void Simulation::Play()
{
	m_running = true;
}

void Simulation::Stop()
{
	m_running = false;
}

void Simulation::Next()
{
	Update();
}

bool Simulation::IsRunning() const
{
	return m_running;
}

Coord Simulation::Size() const
{
	return { MaxX, MaxY };
}

bool Simulation::Get(const Coord& coord) const
{
	return m_cells.Get(coord);
}

void Simulation::Set(const Coord& coord, bool value)
{
	m_cells.Set(coord, value);
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

void Simulation::Update()
{
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
