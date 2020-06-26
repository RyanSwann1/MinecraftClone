#include "Inventory.h"
#include "Gui.h"
#include <iostream>

//Item
Item::Item(eCubeType cubeType)
	: m_cubeType(cubeType),
	m_currentAmount(1)
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

void Item::remove()
{
	assert(m_currentAmount > 0);
	--m_currentAmount;
}

void Item::add()
{
	++m_currentAmount;
}

//Inventory
Inventory::Inventory()
	: m_items(),
	m_currentSelectedItem(eHotbarIndex::One)
{}

const std::vector<Item>& Inventory::getItems() const
{
	return m_items;
}

eHotbarIndex Inventory::getSelectedHotbarItem() const
{
	return m_currentSelectedItem;
}

eCubeType Inventory::getFirstItem() const
{
	assert(!isEmpty());
	return m_items.front().getCubeType();
}

bool Inventory::isEmpty() const
{
	return m_items.empty();
}

bool Inventory::isSelectedItemEmpty() const
{
	return false;
}

void Inventory::add(eCubeType cubeTypeToAdd, Gui& gui)
{
	auto iter = std::find_if(m_items.begin(), m_items.end(), [cubeTypeToAdd](const auto& item)
	{
		return cubeTypeToAdd == item.getCubeType();
	});

	if (iter != m_items.end())
	{
		iter->add();
	}
	else
	{
		m_items.emplace_back(cubeTypeToAdd);
		gui.addItem(static_cast<eHotbarIndex>(m_items.size() - 1), cubeTypeToAdd);
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

		//std::cout << static_cast<int>(m_currentSelectedItem) << "\n";
		//currentSFMLEvent.mouseWheel.delta
		//std::cout << currentSFMLEvent.mouseWheel.delta << "\n";
	}
}

void Inventory::removeFirstItem()
{
	assert(!isEmpty());

	m_items.front().remove();
	if (m_items.front().isEmpty())
	{
		m_items.erase(m_items.begin());
	}
}