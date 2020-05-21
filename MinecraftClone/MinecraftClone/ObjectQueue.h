#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <assert.h>
#include <unordered_map>
#include "glm/gtx/hash.hpp"
#include <iostream>

template <class Object>
class ObjectQueueNode : private NonCopyable
{
public:
	ObjectQueueNode(const glm::ivec3& position)
		: position(position),
		previous(nullptr),
		next(nullptr)
	{}
	ObjectQueueNode(ObjectQueueNode&& orig) noexcept
		: position(orig.position),
		previous(orig.previous),
		next(orig.next)
	{
		orig.previous = nullptr;
		orig.next = nullptr;
	}
	ObjectQueueNode& operator=(ObjectQueueNode&& orig) noexcept
	{
		position = orig.position;
		previous = orig.previous;
		next = orig.next;

		orig.previous = nullptr;
		orig.next = nullptr;

		return *this;
	}

	glm::ivec3 position;
	Object* previous;
	Object* next;
};

class PositionNode : public ObjectQueueNode<PositionNode>
{
public:
	PositionNode(const glm::ivec3& position)
		: ObjectQueueNode(position)
	{}
};

//Queue data structure with O(1) best case for quiries
template <class Object>
class ObjectQueue
{
public:
	ObjectQueue()
		: m_initialNodeAdded(nullptr),
		m_recentNodeAdded(nullptr),
		m_container()
	{}

	void add(Object&& node)
	{
		if (m_container.empty())
		{
			assert(!m_initialNodeAdded && !m_recentNodeAdded);
			Object& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(node.position),
				std::forward_as_tuple(std::move(node))).first->second;

			addedNode.previous = nullptr;
			m_initialNodeAdded = &addedNode;
			m_recentNodeAdded = &addedNode;
		}
		else if (m_container.size() == 1)
		{
			assert(m_initialNodeAdded && m_recentNodeAdded);
			Object& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(node.position),
				std::forward_as_tuple(std::move(node))).first->second;
			 
			addedNode.previous = m_initialNodeAdded;
			m_initialNodeAdded->next = &addedNode;
			m_recentNodeAdded = &addedNode;
		}
		else if (m_container.size() > 1)
		{
			assert(m_initialNodeAdded && m_recentNodeAdded);
			Object& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(node.position),
				std::forward_as_tuple(std::move(node))).first->second;

			addedNode.previous = m_recentNodeAdded;
			m_recentNodeAdded->next = &addedNode;
			m_recentNodeAdded = &addedNode;
		}
	}

	bool contains(const glm::ivec3& position) const
	{
		return m_container.find(position) != m_container.cend();
	}

	bool isEmpty() const
	{
		return m_container.empty();
	}

	Object& front() 
	{
		assert(m_initialNodeAdded && m_recentNodeAdded && !m_container.empty());
		
		auto iter = m_container.find(m_initialNodeAdded->position);

		assert(iter != m_container.end());
		return iter->second;
	}

	void pop()
	{
		assert(m_initialNodeAdded && m_recentNodeAdded && !m_container.empty());

		auto iter = m_container.find(m_initialNodeAdded->position);
		assert(iter != m_container.end());
		
		if (m_container.size() == 1)
		{
			assert(m_initialNodeAdded == m_recentNodeAdded);
			m_initialNodeAdded = nullptr;
			m_recentNodeAdded = nullptr;

		}
		else if(m_container.size() > 1)
		{
			assert(m_initialNodeAdded->next);
			m_initialNodeAdded = m_initialNodeAdded->next;
			m_initialNodeAdded->previous = nullptr;
		}

		m_container.erase(iter);
	}

	void remove(const glm::ivec3& position)
	{
		auto iter = m_container.find(position);
		if (iter != m_container.end())
		{
			assert(m_initialNodeAdded && m_recentNodeAdded);
			Object* previousNode = iter->second.previous;
			Object* nextNode = iter->second.next;

			//Top
			if (!nextNode && previousNode)
			{
				m_recentNodeAdded = previousNode;
				m_recentNodeAdded->next = nullptr;
				previousNode->next = nullptr;
			}
			//Bottom
			else if (!previousNode && nextNode)
			{
				m_initialNodeAdded = nextNode;
				m_initialNodeAdded->previous = nullptr;
				nextNode->previous = nullptr;
			}
			//Inbetween
			else if (previousNode && nextNode)
			{
				previousNode->next = nextNode;
				nextNode->previous = previousNode;
			}
			else
			{
				assert(m_container.size() == 1);
				m_initialNodeAdded = nullptr;
				m_recentNodeAdded = nullptr;
			}

			m_container.erase(iter);
		}
	}

private:
	Object* m_initialNodeAdded;
	Object* m_recentNodeAdded;

	std::unordered_map<glm::ivec3, Object> m_container;
};