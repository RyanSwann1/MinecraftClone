#pragma once

#include "glm/glm.hpp"
#include <assert.h>
#include <unordered_map>
#include "glm/gtx/hash.hpp"
#include <iostream>

class PositionStack
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
	PositionStack()
		: m_previousAddedNode(nullptr),
		m_container()
	{}

	void add(const glm::ivec3& position)
	{
		if (m_previousAddedNode)
		{
			Node& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(position),
				std::forward_as_tuple(position, m_previousAddedNode)).first->second;

			m_previousAddedNode->next = &addedNode;
			m_previousAddedNode = &addedNode;
		}
		else
		{
			Node& addedNode = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(position),
				std::forward_as_tuple(position, nullptr)).first->second;

			m_previousAddedNode = &addedNode;
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
		assert(m_previousAddedNode && !m_container.empty());
		auto iter = m_container.find(m_previousAddedNode->position);
		assert(iter != m_container.end());
		return iter->second.position;
	}

	void pop()
	{
		assert(m_previousAddedNode || m_container.empty());
		auto iter = m_container.find(m_previousAddedNode->position);
		assert(iter != m_container.end());

		if (!iter->second.previous)
		{
			m_previousAddedNode = nullptr;
		}
		else
		{
			m_previousAddedNode = iter->second.previous;
			m_previousAddedNode->next = nullptr;
		}

		m_container.erase(iter);
	}

	void remove(const glm::ivec3& position)
	{
		auto iter = m_container.find(position);
		if (iter != m_container.end())
		{
			Node* previousNode = iter->second.previous;
			Node* nextNode = iter->second.next;

			if (previousNode)
			{
				previousNode->next = nextNode;
			}

			if (nextNode)
			{
				nextNode->previous = previousNode;
			}
			
			if(!nextNode && previousNode)
			{
				m_previousAddedNode = previousNode;
			}
			else if (!nextNode && !previousNode)
			{
				m_previousAddedNode = nullptr;
			}

			m_container.erase(iter);
		}
	}

	void printAll() const
	{
		if (m_previousAddedNode)
		{
			Node* node = m_previousAddedNode;
			while (node)
			{
				std::cout << node->position.x << "\n";
				if (node->previous)
				{
					node = node->previous;
				}
				else
				{
					node = nullptr;
				}
			}
		}
		else
		{
			std::cout << "Nothing\n";
		}
		std::cout << "\n";
	}

private:
	Node* m_previousAddedNode;
	std::unordered_map<glm::ivec3, Node> m_container;
};