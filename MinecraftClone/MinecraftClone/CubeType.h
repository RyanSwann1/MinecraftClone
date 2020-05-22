#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <bitset>
#include <vector>

enum class eCubeType
{
	Air = 0,
	Grass,
	Dirt,
	Sand,
	Stone,
	Water, //Transparent
	Log,
	LogTop,
	Leaves, //Transparent
	Cactus,
	CactusTop,
	Shrub, //Transparent
	TallGrass, //Transparent
	Max = TallGrass
};

class CubeTypeComparison : private NonCopyable, private NonMovable
{
public:
	CubeTypeComparison(const std::vector<eCubeType>& cubeTypes)
	{
		for (auto cubeType : cubeTypes)
		{
			bitset.set(static_cast<int>(cubeType));
		}
	}
	CubeTypeComparison(eCubeType cubeType)
	{
		bitset.set(static_cast<int>(cubeType));
	}

	const std::bitset<static_cast<int>(eCubeType::Max) + 1>& getBitSet() const
	{
		return bitset;
	}

	bool isMatch(const CubeTypeComparison& other) const
	{
		return (bitset & other.getBitSet()).any();
	}

private:
	std::bitset<static_cast<int>(eCubeType::Max) + 1> bitset{ 0 };
};