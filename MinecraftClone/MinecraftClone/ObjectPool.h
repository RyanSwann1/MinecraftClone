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
		inUse(false),
		ID(ID)
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig.object)),
		inUse(orig.inUse),
		ID(orig.ID)
	{
		orig.ID = INVALID_OBJECT_ID;
		orig.inUse = false;
	}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);
		inUse = orig.inUse;
		ID = orig.ID;

		orig.ID = INVALID_OBJECT_ID;
		orig.inUse = false;
		return *this;
	}
	~ObjectInPool() {}

	Object object;
	bool inUse;
	int ID;
};

//External Use - Object Pool
template <class Object>
class ObjectPool;
template <class Object>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(ObjectInPool<Object>* objectInPool, std::function<void(int)> func)
		: objectInPool(objectInPool),
		m_func(func)
	{
		if (objectInPool)
		{
			objectInPool->inUse = true;
		}
	}
	~ObjectFromPool()
	{
		if (objectInPool)
		{
			objectInPool->object.reset();
			objectInPool->inUse = false;

			m_func(objectInPool->ID);
		}
	}
	ObjectFromPool(ObjectFromPool&& orig) noexcept
		: objectInPool(orig.objectInPool),
		m_func(orig.m_func)
	{
		orig.objectInPool = nullptr;
	}
	ObjectFromPool& operator=(ObjectFromPool&& orig) noexcept
	{
		objectInPool = orig.objectInPool;
		m_func = orig.m_func;

		orig.objectInPool = nullptr;

		return *this;
	}

	Object* getObject() const 
	{
		return (objectInPool ? &objectInPool->object : nullptr);
	}

private:
	ObjectInPool<Object>* objectInPool;
	std::function<void(int)> m_func;
};

using std::placeholders::_1;

//Object Pool
template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
public:
	ObjectPool(int visibilityDistance, int chunkWidth, int chunkDepth)
		: m_objectPool()
	{
		//Added a little bit more than neccessary due to how the inifinite map generates
		int x = visibilityDistance / chunkWidth;
		x += x += 1;
		int z = visibilityDistance / chunkDepth;
		z += z += 1;

		m_objectPool.reserve(x * z);
		for (int i = 0; i < x * z; ++i)
		{
			m_objectPool.emplace_back(i);
			m_availableObjects.push(i);
		}
	}

	ObjectFromPool<Object> getNextAvailableObject()
	{	
		if (!m_availableObjects.empty())
		{
			int ID = m_availableObjects.top();
			m_availableObjects.pop();
			assert(ID < m_objectPool.size());
			return ObjectFromPool<Object>(&m_objectPool[ID], std::bind(&ObjectPool<Object>::releaseID, this, _1));
		}
		else
		{
			return ObjectFromPool<Object>(nullptr, std::bind(&ObjectPool<Object>::releaseID, this, _1));
		}
	}

private:
	std::stack<int> m_availableObjects;
	std::vector<ObjectInPool<Object>> m_objectPool;

	void releaseID(int ID)
	{
		assert(ID != INVALID_OBJECT_ID);
		m_availableObjects.push(ID);
	}
};