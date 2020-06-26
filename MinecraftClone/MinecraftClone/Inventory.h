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

class Inventory : private NonCopyable, private NonMovable
{
public:
	Inventory();

	eHotBarSelection getSelectedHotbarItem() const;
	eCubeType getFirstItem() const;
	bool isEmpty() const;
	bool isSelectedItemEmpty() const;

	void add(eCubeType cubeTypeToAdd);
	void handleInputEvents(const sf::Event& currentSFMLEvent);
	void removeFirstItem();

private:
	std::vector<Item> m_items;
	eHotBarSelection m_currentSelectedItem;
};