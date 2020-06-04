#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include <vector>
#include <stack>
#include <functional>
#include <assert.h>

constexpr int INVALID_OBJECT_ID = -1;

//Internal Use - Object Pool
template <class Object>
struct ObjectInPool : private NonCopyable
{
	ObjectInPool(int ID)
		: object(),
		ID(ID)
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig.object)),
		ID(orig.ID)
	{
		orig.ID = INVALID_OBJECT_ID;
	}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);
		ID = orig.ID;

		orig.ID = INVALID_OBJECT_ID;
		return *this;
	}

	Object object;
	int ID;
};

//External Use - Object Pool
template <class Object>
class ObjectPool;
template <class Object>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(ObjectInPool<Object>* objectInPool = nullptr, std::function<void(const ObjectInPool<Object>&)> onDestroyFunction = nullptr)
		: objectInPool(objectInPool),
		onDestroyFunction(onDestroyFunction)
	{
		assert((objectInPool && onDestroyFunction) || (!objectInPool && !onDestroyFunction));
	}
	~ObjectFromPool()
	{
		if (objectInPool)
		{
			objectInPool->object.reset();
			onDestroyFunction(*objectInPool);
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
		return (objectInPool ? &objectInPool->object : nullptr);
	}

private:
	ObjectInPool<Object>* objectInPool;
	std::function<void(const ObjectInPool<Object>&)> onDestroyFunction;
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
		m_objectPool.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			m_objectPool.emplace_back(i);
			m_availableObjects.push(i);
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
			
			int ID = m_availableObjects.top();
			m_availableObjects.pop();
			assert(ID < m_objectPool.size());
			return ObjectFromPool<Object>(&m_objectPool[ID], std::bind(&ObjectPool<Object>::releaseObject, this, _1));
		}
		else
		{
			return ObjectFromPool<Object>();
		}
	}

private:
	const size_t m_maxSize;
	std::stack<int> m_availableObjects;
	std::vector<ObjectInPool<Object>> m_objectPool;

	void releaseObject(const ObjectInPool<Object>& objectInPool)
	{
		assert(objectInPool.ID != INVALID_OBJECT_ID);
		m_availableObjects.push(objectInPool.ID);
	}
};