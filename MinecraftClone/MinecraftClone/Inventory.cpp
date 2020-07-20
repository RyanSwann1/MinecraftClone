#include "Inventory.h"
#include "Gui.h"
#include <iostream>
#include "GameEventMessenger.h"
#include "GameEvents.h"

namespace
{
	constexpr int MAX_ITEM_CAPACITY = 64;

	eCubeType convertCubeType(eCubeType cubeType)
	{
		switch (cubeType)
		{
		case eCubeType::Grass:
			return eCubeType::Grass;
		case eCubeType::Dirt:
			return eCubeType::Dirt;
		case eCubeType::Cactus:
		case eCubeType::CactusTop:
		 	return eCubeType::Cactus;
		case eCubeType::Sand:
			return eCubeType::Sand;
		case eCubeType::Log:
			return eCubeType::Log;
		case eCubeType::LogTop:
			return eCubeType::LogTop;
		case eCubeType::Stone:
			return eCubeType::Stone;
		default:
			assert(false);
		}
	};
};

//Item
Item::Item()	
	: m_cubeType(),
	m_currentAmount(0)
{}

int Item::getSize() const
{
	return m_currentAmount;
}

bool Item::isFull() const
{
	return m_currentAmount == MAX_ITEM_CAPACITY;
}

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
	assert(m_currentAmount < MAX_ITEM_CAPACITY);
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

eInventoryIndex Inventory::getSelectedItemIndex() const
{
	return m_currentSelectedItem;
}

eCubeType Inventory::getSelectedItemType() const
{
	assert(!isSelectedItemEmpty());
	return getSelectedItem().getCubeType();
}

bool Inventory::isSelectedItemEmpty() const
{
	return getSelectedItem().isEmpty();
}

bool Inventory::isItemAddable(eCubeType cubeType) const
{
	for (const auto& item : m_items)
	{
		if (item.isEmpty() || (item.getCubeType() == cubeType && !item.isFull()))
		{
			return true;
		}
	}

	return false;
}

void Inventory::reduceSelectedItem()
{
	assert(!isSelectedItemEmpty());

	getSelectedItem().reduce();
	GameEventMessenger::getInstance().broadcast<GameEvents::UpdateItemQuantityGUI>({ m_currentSelectedItem, getSelectedItem().getSize() });
	
	if (getSelectedItem().isEmpty())
	{
		GameEventMessenger::getInstance().broadcast<GameEvents::RemoveItemGUI>({ m_currentSelectedItem });
	}
}

void Inventory::add(eCubeType cubeTypeToAdd)
{
	eCubeType convertedCubeType = convertCubeType(cubeTypeToAdd);
	//Add to existing item in Inventory
	bool itemAdded = false;
	for (int i = 0; i < static_cast<int>(eInventoryIndex::Max) + 1; ++i)
	{
		if (!m_items[i].isEmpty() && 
			!m_items[i].isFull() && 
			m_items[i].getCubeType() == convertedCubeType)
		{
			m_items[i].incrementQuantity();
			itemAdded = true;
			
			GameEventMessenger::getInstance().broadcast<GameEvents::UpdateItemQuantityGUI>({ static_cast<eInventoryIndex>(i), m_items[i].getSize() });

			break;
		}
	}

	//Add to next available free space in Inventory
	for (int i = 0; !itemAdded && i < static_cast<int>(eInventoryIndex::Max) + 1; ++i)
	{
		if (m_items[i].isEmpty())
		{
			m_items[i].reset(convertedCubeType);
			m_items[i].incrementQuantity();

			GameEventMessenger::getInstance().broadcast<GameEvents::AddItemGUI>({ convertedCubeType, static_cast<eInventoryIndex>(i) });

			GameEventMessenger::getInstance().broadcast<GameEvents::UpdateItemQuantityGUI>({ static_cast<eInventoryIndex>(i), m_items[i].getSize() });
			
			break;
		}
	}
}

void Inventory::handleInputEvents(const sf::Event& currentSFMLEvent)
{
	assert(currentSFMLEvent.type == sf::Event::KeyPressed);

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
		case sf::Keyboard::Num9:
			m_currentSelectedItem = eInventoryIndex::Nine;
			break;
		}
	}

	GameEventMessenger::getInstance().broadcast<GameEvents::UpdateSelectionBoxGUI>({ m_currentSelectedItem });
}

const Item& Inventory::getSelectedItem() const
{
	return m_items[static_cast<int>(m_currentSelectedItem)];
}

Item& Inventory::getSelectedItem()
{
	return m_items[static_cast<int>(m_currentSelectedItem)];
}