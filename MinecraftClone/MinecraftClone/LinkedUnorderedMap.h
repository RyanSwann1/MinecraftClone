#pragma once

#include "glm/glm.hpp"
#include <assert.h>
#include <unordered_map>
#include "glm/gtx/hash.hpp"
#include <iostream>

class PositionQueue
{
	struct Node
	{
		Node(const glm::ivec3& position, Node* previous)
			: position(position),
			previous(previous),
			next(nullptr)
		{}

		glm::ivec3 position;
		Node* previous;
		Node* next;
	};

public:
	PositionQueue()
		: m_initialNodeAdded(nullptr),
		m_recentNodeAdded(nullptr),
		m_container()
	{}

	void add(const glm::ivec3& position)
	{
		if (m_container.empty())
		{
			assert(!m_initialNodeAdded && !m_recentNodeAdded);
			Node& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(position),
				std::forward_as_tuple(position, nullptr)).first->second;

			m_initialNodeAdded = &addedNode;
			m_recentNodeAdded = &addedNode;
		}
		else if (m_container.size() == 1)
		{
			assert(m_initialNodeAdded && m_recentNodeAdded);
			Node& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(position),
				std::forward_as_tuple(position, m_initialNodeAdded)).first->second;
			 
			m_initialNodeAdded->next = &addedNode;
			m_recentNodeAdded = &addedNode;
		}
		else if (m_container.size() > 1)
		{
			assert(m_initialNodeAdded && m_recentNodeAdded);
			Node& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(position),
				std::forward_as_tuple(position, m_recentNodeAdded)).first->second;

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

	const glm::ivec3& front() const
	{
		assert(m_initialNodeAdded && m_recentNodeAdded && !m_container.empty());
		
		auto iter = m_container.find(m_initialNodeAdded->position);
		assert(iter != m_container.end());
		return iter->second.position;
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
			Node* previousNode = iter->second.previous;
			Node* nextNode = iter->second.next;

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
	Node* m_initialNodeAdded;
	Node* m_recentNodeAdded;
	std::unordered_map<glm::ivec3, Node> m_container;
};