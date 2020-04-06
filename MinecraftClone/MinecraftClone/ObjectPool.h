#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include "Utilities.h"
#include <vector>

//Internal Use - Object Pool
template <class Object>
struct ObjectInPool : private NonCopyable
{
	ObjectInPool()
		: object()
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig.object))
	{}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);

		return *this;
	}
	virtual ~ObjectInPool() {}

	Object object;
};

//External Use - Object Pool
template <class Object, class ObjectPool>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(Object* object) 
		: object(object)
	{}
	virtual ~ObjectFromPool() {}
	ObjectFromPool(ObjectFromPool&& orig) noexcept
		: object(orig.object)
	{
		orig.object = nullptr;
	}
	ObjectFromPool& operator=(ObjectFromPool&& orig) noexcept
	{
		object = orig.object;
		orig.object = nullptr;

		return *this;
	}

	Object* object;
};

//Object Pool
template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
public:
	ObjectPool()
		: m_objectPool()
	{
		//Added a little bit more than neccessary due to how the inifinite map generates
		int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
		x += x += 2;
		int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
		y += y += 2;

		m_objectPool.resize(size_t(x * y));
	}
	virtual ~ObjectPool() {}

protected:
	Object* getNextAvailableObject()
	{		
		for (auto& objectInPool : m_objectPool)
		{
			if (!objectInPool.object.isInUse())
			{
				return &objectInPool.object;
			}
		}

		return nullptr;
	}
private:
	std::vector<ObjectInPool<Object>> m_objectPool;
};