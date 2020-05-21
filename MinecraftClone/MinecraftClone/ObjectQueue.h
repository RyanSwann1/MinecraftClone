#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <assert.h>
#include <unordered_map>
#include "glm/gtx/hash.hpp"

template <class Object>
struct ObjectQueueNode : private NonCopyable
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

struct PositionNode : public ObjectQueueNode<PositionNode>
{
	PositionNode(const glm::ivec3& position)
		: ObjectQueueNode(position)
	{}
};

template <class Object>
class ObjectQueue : private NonCopyable, private NonMovable
{
public:
	ObjectQueue()
		: m_initialObjectAdded(nullptr),
		m_recentObjectAdded(nullptr),
		m_container()
	{}

	void add(Object&& newObject)
	{
		if (m_container.empty())
		{
			assert(!m_initialObjectAdded && !m_recentObjectAdded);
			Object& addedObject = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(newObject.position),
				std::forward_as_tuple(std::move(newObject))).first->second;

			addedObject.previous = nullptr;
			m_initialObjectAdded = &addedObject;
			m_recentObjectAdded = &addedObject;
		}
		else if (m_container.size() == 1)
		{
			assert(m_initialObjectAdded && m_recentObjectAdded);
			Object& addedObject = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(newObject.position),
				std::forward_as_tuple(std::move(newObject))).first->second;

			addedObject.previous = m_initialObjectAdded;
			m_initialObjectAdded->next = &addedObject;
			m_recentObjectAdded = &addedObject;
		}
		else if (m_container.size() > 1)
		{
			assert(m_initialObjectAdded && m_recentObjectAdded);
			Object& addedObject = m_container.emplace(std::piecewise_construct,
				std::forward_as_tuple(newObject.position),
				std::forward_as_tuple(std::move(newObject))).first->second;

			addedObject.previous = m_recentObjectAdded;
			m_recentObjectAdded->next = &addedObject;
			m_recentObjectAdded = &addedObject;
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
		assert(m_initialObjectAdded && m_recentObjectAdded && !m_container.empty());

		auto iter = m_container.find(m_initialObjectAdded->position);

		assert(iter != m_container.end());
		return iter->second;
	}

	void pop()
	{
		assert(m_initialObjectAdded && m_recentObjectAdded && !m_container.empty());

		auto iter = m_container.find(m_initialObjectAdded->position);
		assert(iter != m_container.end());

		if (m_container.size() == 1)
		{
			assert(m_initialObjectAdded == m_recentObjectAdded);
			m_initialObjectAdded = nullptr;
			m_recentObjectAdded = nullptr;

		}
		else if (m_container.size() > 1)
		{
			assert(m_initialObjectAdded->next);
			m_initialObjectAdded = m_initialObjectAdded->next;
			m_initialObjectAdded->previous = nullptr;
		}

		m_container.erase(iter);
	}

	void remove(const glm::ivec3& position)
	{
		auto iter = m_container.find(position);
		if (iter != m_container.end())
		{
			assert(m_initialObjectAdded && m_recentObjectAdded);
			Object* previousObject = iter->second.previous;
			Object* nextObject = iter->second.next;

			//Top
			if (!nextObject && previousObject)
			{
				m_recentObjectAdded = previousObject;
				m_recentObjectAdded->next = nullptr;
				previousObject->next = nullptr;
			}
			//Bottom
			else if (!previousObject && nextObject)
			{
				m_initialObjectAdded = nextObject;
				m_initialObjectAdded->previous = nullptr;
				nextObject->previous = nullptr;
			}
			//Inbetween
			else if (previousObject && nextObject)
			{
				previousObject->next = nextObject;
				nextObject->previous = previousObject;
			}
			else
			{
				assert(m_container.size() == 1);
				m_initialObjectAdded = nullptr;
				m_recentObjectAdded = nullptr;
			}

			m_container.erase(iter);
		}
	}

private:
	Object* m_initialObjectAdded;
	Object* m_recentObjectAdded;

	std::unordered_map<glm::ivec3, Object> m_container;
};