#pragma once

#include <algorithm>
#include <array>
#include <nlohmann/json.hpp>

/*
 * A coordinate in 2-D space.
 */
struct Coord
{
	// The x component of the 2-D coordinate.
	int x;

	// The y component of the 2-D coordinate.
	int y;

	// Add a single value to both components of the 2-D coordinate.
	Coord operator+(int offset) const
	{
		return { x + offset, y + offset };
	}

	// Subtract a single value from both components of the 2-D coordinate.
	Coord operator-(int offset) const
	{
		return Coord({ x - offset, y - offset });
	}
	
	// Check to see if two 2-D coordinates are different.
	bool operator!=(const Coord& other)
	{
		return x != other.x || y != other.y;
	}
	
	// Get the minimum of each component of the 2-D coordinate.
	Coord min(const Coord& other) const
	{
		return Coord({ std::min(x, other.x), std::min(y, other.y) });
	}
	
	// Get the maximum of each component of the 2-D coordinate.
	Coord max(const Coord& other) const
	{
		return Coord({ std::max(x, other.x), std::max(y, other.y) });
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Coord, x, y)

/*
 * A list of 2-D coordinates which can be serialized.
 */
struct Coords
{
	// A variable list of 2-D coordinates.
	std::vector<Coord> m_coords;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Coords, m_coords)

/*
 * A 2-D grid of values of any type.
 */
template<typename T, size_t MaxX, size_t MaxY>
class Grid
{
public:
	// Set the value at a particular 2-D coordinate.
	void Set(const Coord& coord, T value)
	{
		m_elements[coord.y][coord.x] = value;
	}
	
	// Get the value at a particular 2-D coordinate.
	T Get(const Coord& coord) const
	{
		return m_elements[coord.y][coord.x];
	}
	
	// Reset all the values of the grid to their default values.
	void Reset()
	{
		m_elements = {};
	}
private:
	// A fixed 2-D array of values which comprise the grid.
	std::array<std::array<T, MaxX>, MaxY> m_elements{};
};
