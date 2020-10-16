#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include <vector>
#include <stack>
#include <functional>
#include <assert.h>

template <class Object>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(Object& object, std::function<void(Object&)> onDestroyFunction)
		: object(object),
		onDestroyFunction(onDestroyFunction)
	{}
	~ObjectFromPool()
	{
		if (onDestroyFunction)
		{
			object.get().reset();
			onDestroyFunction(object);
		}
	}
	ObjectFromPool(ObjectFromPool&& orig) noexcept
		: object(orig.object),
		onDestroyFunction(orig.onDestroyFunction)
	{
		orig.onDestroyFunction = nullptr;
	}
	ObjectFromPool& operator=(ObjectFromPool&& orig) noexcept
	{
		object = orig.object;
		onDestroyFunction = orig.onDestroyFunction;

		orig.onDestroyFunction = nullptr;

		return *this;
	}

	std::reference_wrapper<Object> object;

private:
	std::function<void(Object&)> onDestroyFunction;
};

//Object Pool
template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
public:
	ObjectPool(size_t size = 0)
		: m_maxSize(size),
		m_availableObjects(),
		m_objectPool()
	{
		m_objectPool.resize(size);
		for (auto& object : m_objectPool)
		{
			m_availableObjects.push(object);
		}
	}
	~ObjectPool()
	{
		assert(m_availableObjects.size() == m_maxSize);
	}

	bool isObjectAvailable() const
	{
		return !m_availableObjects.empty();
	}

	ObjectFromPool<Object> getAvailableObject()
	{
		assert(isObjectAvailable());

		Object& objectInPool = m_availableObjects.top();
		m_availableObjects.pop();
		return ObjectFromPool<Object>(objectInPool, [this](Object& objectInPool)
		{
			return releaseObject(objectInPool);
		});
	}

private:
	const size_t m_maxSize;
	std::stack<std::reference_wrapper<Object>> m_availableObjects;
	std::vector<Object> m_objectPool;

	void releaseObject(Object& objectInPool)
	{
		m_availableObjects.push(objectInPool);
	}
};