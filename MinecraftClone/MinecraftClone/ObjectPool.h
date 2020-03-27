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
		: object(),
		nextAvailableObject()
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig.object)),
		nextAvailableObject(orig.nextAvailableObject)
	{
		orig.nextAvailableObject = nullptr;
	}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);
		nextAvailableObject = orig.nextAvailableObject;
		orig.nextAvailableObject = nullptr;

		return *this;
	}
	virtual ~ObjectInPool() {}

	Object object;
	ObjectInPool* nextAvailableObject;
};

//External Use - Object Pool
template <class Object, class ObjectPool>
struct ObjectFromPool : private NonCopyable
{
	ObjectFromPool(Object& object) 
		: object(&object)
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
		: m_objectPool(),
		m_nextAvailableObject(nullptr)
	{
		//Added a little bit more than neccessary due to how the inifinite map generates
		int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
		x += x += 2;
		int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
		y += y += 2;

		m_objectPool.resize(size_t((x * y)));

		for (int i = 0; i < static_cast<int>(m_objectPool.size()) - 1; ++i)
		{
			m_objectPool[i].nextAvailableObject = &m_objectPool[i + 1];
		}

		m_nextAvailableObject = &m_objectPool.front();
		m_objectPool.back().nextAvailableObject = m_nextAvailableObject;
	}
	virtual ~ObjectPool() {}

protected:
	ObjectInPool<Object>& getNextAvailableObject()
	{
		int iterationCount = 0;
		bool validChunkFound = false;

		assert(m_nextAvailableObject);
		while (m_nextAvailableObject->object.isInUse())
		{
			assert(m_nextAvailableObject->nextAvailableObject);
			m_nextAvailableObject = m_nextAvailableObject->nextAvailableObject;

			assert(++iterationCount && iterationCount <= m_objectPool.size());
		}

		return *m_nextAvailableObject;
	}
private:
	std::vector<ObjectInPool<Object>> m_objectPool;
	ObjectInPool<Object>* m_nextAvailableObject;
};