#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include <SFML/Graphics.hpp>

class Item
{
public:
	Item();

	bool isEmpty() const;
	eCubeType getCubeType() const;

	void reduce();
	void incrementQuantity();
	void reset(eCubeType cubeType);

private:
	eCubeType m_cubeType;
	int m_currentAmount;
};

class Gui;
class Inventory : private NonCopyable, private NonMovable
{
public:
	Inventory();

	eCubeType getSelectedItemType() const;
	bool isSelectedItemEmpty() const;

	void reduceSelectedItem();
	void add(eCubeType cubeTypeToAdd, Gui& gui);
	void handleInputEvents(const sf::Event& currentSFMLEvent);

private:
	std::array<Item, 8> m_items;
	eHotbarIndex m_currentSelectedItem;

	const Item& getSelectedItem() const;
	Item& getSelectedItem();
};