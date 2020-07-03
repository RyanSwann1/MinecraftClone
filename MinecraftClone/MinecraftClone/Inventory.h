#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include <SFML/Graphics.hpp>

class Item : private NonCopyable, private NonMovable
{
public:
	Item();

	int getSize() const;
	bool isFull() const;
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

	eInventoryIndex getSelectedItemIndex() const;
	eCubeType getSelectedItemType() const;
	bool isSelectedItemEmpty() const;
	bool isItemAddable(eCubeType cubeType) const;
	
	void reduceSelectedItem(Gui& Gui);
	void add(eCubeType cubeTypeToAdd, Gui& gui);
	void handleInputEvents(const sf::Event& currentSFMLEvent, Gui& gui);

private:
	std::array<Item, static_cast<size_t>(eInventoryIndex::Max) + 1> m_items;
	eInventoryIndex m_currentSelectedItem;

	const Item& getSelectedItem() const;
	Item& getSelectedItem();
};