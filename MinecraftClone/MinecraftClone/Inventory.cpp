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
	m_currentSelectedItem(eHotbarIndex::One)
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

void Inventory::reduceSelectedItem()
{
	assert(!isSelectedItemEmpty());

	getSelectedItem().reduce();
}

void Inventory::add(eCubeType cubeTypeToAdd, Gui& gui)
{
	//Add to existing item in Inventory
	bool itemAdded = false;
	for (int i = 0; i < static_cast<int>(eHotbarIndex::Max); ++i)
	{
		if (!m_items[i].isEmpty() && m_items[i].getCubeType() == cubeTypeToAdd)
		{
			m_items[i].incrementQuantity();
			itemAdded = true;
			break;
		}
	}

	//Add to next available free space in Inventory
	for (int i = 0; !itemAdded && i < static_cast<int>(eHotbarIndex::Max); ++i)
	{
		if (m_items[i].isEmpty())
		{
			m_items[i].reset(cubeTypeToAdd);
			m_items[i].incrementQuantity();
			break;
			//gui.addItem(static_cast<eHotbarIndex>(m_items.size() - 1), cubeTypeToAdd);
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
			m_currentSelectedItem = eHotbarIndex::One;
			break;
		case sf::Keyboard::Num2:
			m_currentSelectedItem = eHotbarIndex::Two;
			break;
		case sf::Keyboard::Num3:
			m_currentSelectedItem = eHotbarIndex::Three;
			break;
		case sf::Keyboard::Num4:
			m_currentSelectedItem = eHotbarIndex::Four;
			break;
		case sf::Keyboard::Num5:
			m_currentSelectedItem = eHotbarIndex::Five;
			break;
		case sf::Keyboard::Num6:
			m_currentSelectedItem = eHotbarIndex::Six;
			break;
		case sf::Keyboard::Num7:
			m_currentSelectedItem = eHotbarIndex::Seven;
			break;
		case sf::Keyboard::Num8:
			m_currentSelectedItem = eHotbarIndex::Eight;
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

		m_currentSelectedItem = static_cast<eHotbarIndex>(currentSelectedItem);

		if (static_cast<int>(m_currentSelectedItem) < static_cast<int>(eHotbarIndex::One))
		{
			m_currentSelectedItem = eHotbarIndex::Eight;
		}
		else if (static_cast<int>(m_currentSelectedItem) > static_cast<int>(eHotbarIndex::Max))
		{
			m_currentSelectedItem = eHotbarIndex::One;
		}
	}
}

const Item& Inventory::getSelectedItem() const
{
	return m_items[static_cast<int>(m_currentSelectedItem) - 1];
}

Item& Inventory::getSelectedItem()
{
	return m_items[static_cast<int>(m_currentSelectedItem) - 1];
}