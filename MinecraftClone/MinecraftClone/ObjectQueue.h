#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include "Rectangle.h"
#include <assert.h>
#include <unordered_map>
#include "glm/gtx/hash.hpp"

template <class Object>
class ObjectQueue;
class ObjectQueuePositionNode: private NonCopyable
{
	friend class ObjectQueue<ObjectQueuePositionNode>;
public:
	ObjectQueuePositionNode(const glm::ivec3& position)
		: position(position),
		previous(nullptr),
		next(nullptr)
	{}
	ObjectQueuePositionNode(ObjectQueuePositionNode&& orig) noexcept
		: position(orig.position),
		previous(orig.previous),
		next(orig.next)
	{
		orig.previous = nullptr;
		orig.next = nullptr;
	}
	ObjectQueuePositionNode& operator=(ObjectQueuePositionNode&& orig) noexcept
	{
		position = orig.position;
		previous = orig.previous;
		next = orig.next;

		orig.previous = nullptr;
		orig.next = nullptr;

		return *this;
	}

	const glm::ivec3& getPosition() const
	{
		return position;
	}

private:
	glm::ivec3 position;
	ObjectQueuePositionNode* previous;
	ObjectQueuePositionNode* next;
};

template<class Object>
class ObjectQueue;
template <class Object>
class ObjectQueueObjectNode : private NonCopyable
{
	friend class ObjectQueue<ObjectQueueObjectNode<Object>>;
public:
	ObjectQueueObjectNode(const glm::ivec3& position, Object&& object)
		: object(std::move(object)),
		position(position),
		previous(nullptr),
		next(nullptr)
	{}
	ObjectQueueObjectNode(const glm::ivec3& position, Object& object)
		: object(object),
		position(position),
		previous(nullptr),
		next(nullptr)
	{}
	ObjectQueueObjectNode(ObjectQueueObjectNode&& orig) noexcept
		: object(std::move(orig.object)),
		position(orig.position),
		previous(orig.previous),
		next(orig.next)
	{
		orig.previous = nullptr;
		orig.next = nullptr;
	}
	ObjectQueueObjectNode& operator=(ObjectQueueObjectNode&& orig) noexcept
	{
		object = std::move(orig.object);
		position = orig.position;
		previous = orig.previous;
		next = orig.next;

		orig.previous = nullptr;
		orig.next = nullptr;

		return *this;
	}

	const glm::ivec3& getPosition() const
	{
		return position;
	}

	Object object;

private:
	glm::ivec3 position;
	ObjectQueueObjectNode* previous;
	ObjectQueueObjectNode* next;
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
		glm::ivec3 position = newObject.position;

		auto iter = m_container.find(position);
		assert(iter == m_container.cend());
		if (iter == m_container.cend())
		{
			if (m_container.empty())
			{
				assert(!m_initialObjectAdded && !m_recentObjectAdded);
				Object& addedObject = m_container.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(std::move(newObject))).first->second;

				addedObject.previous = nullptr;
				m_initialObjectAdded = &addedObject;
				m_recentObjectAdded = &addedObject;
			}
			else if (m_container.size() == 1)
			{
				assert(m_initialObjectAdded && m_recentObjectAdded);
				Object& addedObject = m_container.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(std::move(newObject))).first->second;

				addedObject.previous = m_initialObjectAdded;
				m_initialObjectAdded->next = &addedObject;
				m_recentObjectAdded = &addedObject;
			}
			else if (m_container.size() > 1)
			{
				assert(m_initialObjectAdded && m_recentObjectAdded);
				Object& addedObject = m_container.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(std::move(newObject))).first->second;

				addedObject.previous = m_recentObjectAdded;
				m_recentObjectAdded->next = &addedObject;
				m_recentObjectAdded = &addedObject;
			}
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

	Object* next(Object* object)
	{
		assert(m_initialObjectAdded && m_recentObjectAdded && !m_container.empty());
		if (object)
		{
			return object->next;
		}
		else
		{
			return nullptr;
		}
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

	void removeOutOfBoundsElements(const Rectangle& visibilityAABB)
	{
		if (!isEmpty())
		{
			Object* object = &front();
			while (object)
			{
				glm::ivec2 centrePosition(object->getPosition().x + 16, object->getPosition().z + 16);
				Rectangle objectAABB(centrePosition, 16);
				if (!visibilityAABB.contains(objectAABB))
				{
					object = remove(object);
				}
				else
				{
					object = next(object);
				}
			}
		}
	}

	Object* remove(const Object* object)
	{
		assert(object);
		auto iter = m_container.find(object->getPosition());
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
			return nextObject;
		}

		return nullptr;
	}

	Object* remove(const glm::ivec3& position)
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
			return nextObject;
		}
		
		return nullptr;
	}

private:
	Object* m_initialObjectAdded;
	Object* m_recentObjectAdded;

	std::unordered_map<glm::ivec3, Object> m_container;
};