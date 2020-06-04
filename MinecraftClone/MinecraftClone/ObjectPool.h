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
	ObjectFromPool(Object* objectInPool = nullptr, std::function<void(Object*)> onDestroyFunction = nullptr)
		: objectInPool(objectInPool),
		onDestroyFunction(onDestroyFunction)
	{
		assert((objectInPool && onDestroyFunction) || (!objectInPool && !onDestroyFunction));
	}
	~ObjectFromPool()
	{
		if (objectInPool)
		{
			objectInPool->reset();
			onDestroyFunction(objectInPool);
		}
	}
	ObjectFromPool(ObjectFromPool&& orig) noexcept
		: objectInPool(orig.objectInPool),
		onDestroyFunction(orig.onDestroyFunction)
	{
		orig.objectInPool = nullptr;
	}
	ObjectFromPool& operator=(ObjectFromPool&& orig) noexcept
	{
		objectInPool = orig.objectInPool;
		onDestroyFunction = orig.onDestroyFunction;

		orig.objectInPool = nullptr;

		return *this;
	}

	Object* getObject() const 
	{
		return (objectInPool ? objectInPool : nullptr);
	}

private:
	Object* objectInPool;
	std::function<void(Object*)> onDestroyFunction;
};

using std::placeholders::_1;

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
			m_availableObjects.push(&object);
		}
	}
	~ObjectPool()
	{
		assert(m_availableObjects.size() == m_maxSize);
	}

	ObjectFromPool<Object> getNextAvailableObject()
	{	
		if (!m_availableObjects.empty())
		{
			assert(!m_objectPool.empty());
			
			Object* objectInPool = m_availableObjects.top();
			assert(objectInPool);
			m_availableObjects.pop();
			return ObjectFromPool<Object>(objectInPool, std::bind(&ObjectPool<Object>::releaseObject, this, _1));
		}
		else
		{
			return ObjectFromPool<Object>();
		}
	}

private:
	const size_t m_maxSize;
	std::stack<Object*> m_availableObjects;
	std::vector<Object> m_objectPool;

	void releaseObject(Object* objectInPool)
	{
		assert(objectInPool);
		m_availableObjects.push(objectInPool);
	}
};