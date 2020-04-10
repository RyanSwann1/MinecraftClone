#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include <vector>

//Internal Use - Object Pool
template <class Object>
struct ObjectInPool : private NonCopyable
{
	ObjectInPool()
		: object(),
		inUse(false)
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig.object)),
		inUse(orig.inUse)
	{
		orig.inUse = false;
	}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);
		inUse = orig.inUse;

		orig.inUse = false;
		return *this;
	}
	virtual ~ObjectInPool() {}

	Object object;
	bool inUse;
};

//External Use - Object Pool
template <class Object>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(ObjectInPool<Object>* objectInPool) 
		: objectInPool(objectInPool)
	{
		if (objectInPool)
		{
			objectInPool->inUse = true;
		}
	}
	virtual ~ObjectFromPool() 
	{
		if (objectInPool)
		{
			objectInPool->object.reset();
			objectInPool->inUse = false;
		}
	}
	ObjectFromPool(ObjectFromPool&& orig) noexcept
		: objectInPool(orig.objectInPool)
	{
		orig.objectInPool = nullptr;
	}
	ObjectFromPool& operator=(ObjectFromPool&& orig) noexcept
	{
		objectInPool = orig.objectInPool;
		orig.objectInPool = nullptr;

		return *this;
	}

	bool isInUsssse() const
	{
		return (objectInPool ? objectInPool->inUse : false);
	}

	Object* getObject() const 
	{
		return (objectInPool ? &objectInPool->object : nullptr);
	}

private:
	ObjectInPool<Object>* objectInPool;
};

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
		x += x += 2;
		int z = visibilityDistance / chunkDepth;
		z += z += 2;

		m_objectPool.resize(size_t(x * z));
	}

	ObjectFromPool<Object> getNextAvailableObject()
	{	
		ObjectInPool<Object>* availableObject = nullptr;
		for (auto& objectInPool : m_objectPool)
		{
			if (!objectInPool.inUse)
			{
				availableObject = &objectInPool;
				break;
			}
		}

		return ObjectFromPool<Object>(availableObject);
	}

private:
	std::vector<ObjectInPool<Object>> m_objectPool;
};