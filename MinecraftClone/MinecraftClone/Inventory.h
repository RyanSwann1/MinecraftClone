#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include <SFML/Graphics.hpp>

class Item
{
public:
	Item(eCubeType cubeType);

	bool isEmpty() const;
	eCubeType getCubeType() const;

	void remove();
	void add();

private:
	eCubeType m_cubeType;
	int m_currentAmount;
};

class Gui;
class Inventory : private NonCopyable, private NonMovable
{
public:
	Inventory();

	const std::vector<Item>& getItems() const;
	eHotbarIndex getSelectedHotbarItem() const;
	eCubeType getFirstItem() const;
	bool isEmpty() const;
	bool isSelectedItemEmpty() const;

	void add(eCubeType cubeTypeToAdd, Gui& gui);
	void handleInputEvents(const sf::Event& currentSFMLEvent);
	void removeFirstItem();

private:
	std::vector<Item> m_items;
	eHotbarIndex m_currentSelectedItem;
};