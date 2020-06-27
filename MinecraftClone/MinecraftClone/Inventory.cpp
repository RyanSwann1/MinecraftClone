#include "Inventory.h"
#include "Gui.h"
#include <iostream>

//Item
Item::Item()
	: m_cubeType(),
	m_currentAmount(0)
{}

bool Item::isEmpty() const
{
	return m_currentAmount == 0;
}

eCubeType Item::getCubeType() const
{
	assert(m_currentAmount > 0);
	return m_cubeType;
}

void Item::reduce()
{
	assert(m_currentAmount > 0);
	--m_currentAmount;
}

void Item::incrementQuantity()
{
	++m_currentAmount;
}

void Item::reset(eCubeType cubeType)
{
	assert(isEmpty());
	m_cubeType = cubeType;
}
                
//Inventory
Inventory::Inventory()
	: m_items(),
	m_currentSelectedItem(eInventoryIndex::One)
{}

eCubeType Inventory::getSelectedItemType() const
{
	assert(!isSelectedItemEmpty());
	return getSelectedItem().getCubeType();
}

bool Inventory::isSelectedItemEmpty() const
{
	return getSelectedItem().isEmpty();
}

void Inventory::reduceSelectedItem(Gui& gui)
{
	assert(!isSelectedItemEmpty());

	getSelectedItem().reduce();
	if (getSelectedItem().isEmpty())
	{
		gui.removeItem(m_currentSelectedItem);
	}
}

void Inventory::add(eCubeType cubeTypeToAdd, Gui& gui)
{
	//Add to existing item in Inventory
	bool itemAdded = false;
	for (int i = 0; i < static_cast<int>(eInventoryIndex::Max) + 1; ++i)
	{
		if (!m_items[i].isEmpty() && m_items[i].getCubeType() == cubeTypeToAdd)
		{
			m_items[i].incrementQuantity();
			itemAdded = true;
			break;
		}
	}

	//Add to next available free space in Inventory
	for (int i = 0; !itemAdded && i < static_cast<int>(eInventoryIndex::Max) + 1; ++i)
	{
		if (m_items[i].isEmpty())
		{
			m_items[i].reset(cubeTypeToAdd);
			m_items[i].incrementQuantity();

			gui.addItem(static_cast<eInventoryIndex>(i), cubeTypeToAdd);
			break;
		}
	}
}

void Inventory::handleInputEvents(const sf::Event& currentSFMLEvent)
{
	assert(currentSFMLEvent.type == sf::Event::MouseWheelScrolled ||
		currentSFMLEvent.type == sf::Event::KeyPressed);

	if (currentSFMLEvent.type == sf::Event::KeyPressed)
	{
		switch (currentSFMLEvent.key.code)
		{
		case sf::Keyboard::Num1:
			m_currentSelectedItem = eInventoryIndex::One;
			break;
		case sf::Keyboard::Num2:
			m_currentSelectedItem = eInventoryIndex::Two;
			break;
		case sf::Keyboard::Num3:
			m_currentSelectedItem = eInventoryIndex::Three;
			break;
		case sf::Keyboard::Num4:
			m_currentSelectedItem = eInventoryIndex::Four;
			break;
		case sf::Keyboard::Num5:
			m_currentSelectedItem = eInventoryIndex::Five;
			break;
		case sf::Keyboard::Num6:
			m_currentSelectedItem = eInventoryIndex::Six;
			break;
		case sf::Keyboard::Num7:
			m_currentSelectedItem = eInventoryIndex::Seven;
			break;
		case sf::Keyboard::Num8:
			m_currentSelectedItem = eInventoryIndex::Eight;
			break;
		}
	}
	else
	{
		int currentSelectedItem = static_cast<int>(m_currentSelectedItem);

		if (currentSFMLEvent.mouseWheel.delta > 0)
		{
			--currentSelectedItem;
		}
		else if (currentSFMLEvent.mouseWheel.delta < 0)
		{
			++currentSelectedItem;
		}

		m_currentSelectedItem = static_cast<eInventoryIndex>(currentSelectedItem);

		if (static_cast<int>(m_currentSelectedItem) < static_cast<int>(eInventoryIndex::One))
		{
			m_currentSelectedItem = eInventoryIndex::Eight;
		}
		else if (static_cast<int>(m_currentSelectedItem) > static_cast<int>(eInventoryIndex::Max))
		{
			m_currentSelectedItem = eInventoryIndex::One;
		}
	}
}

const Item& Inventory::getSelectedItem() const
{
	return m_items[static_cast<int>(m_currentSelectedItem)];
}

Item& Inventory::getSelectedItem()
{
	return m_items[static_cast<int>(m_currentSelectedItem)];
}