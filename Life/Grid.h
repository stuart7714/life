#pragma once

#include <algorithm>
#include <array>

struct Coord
{
	int x;
	int y;
	Coord operator+(int offset) const
	{
		return { x + offset, y + offset };
	}
	Coord operator-(int offset) const
	{
		return Coord({ x - offset, y - offset });
	}
	bool operator!=(const Coord& other)
	{
		return x != other.x || y != other.y;
	}
	Coord min(const Coord& other) const
	{
		return Coord({ std::min(x, other.x), std::min(y, other.y) });
	}
	Coord max(const Coord& other) const
	{
		return Coord({ std::max(x, other.x), std::max(y, other.y) });
	}
};

template<typename T, size_t MaxX, size_t MaxY>
class Grid
{
public:
	void Set(const Coord& coord, T value)
	{
		m_elements[coord.y][coord.x] = value;
	}
	T Get(const Coord& coord) const
	{
		return m_elements[coord.y][coord.x];
	}
private:
	std::array<std::array<T, MaxX>, MaxY> m_elements{};
};
