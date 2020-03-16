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
struct ObjectFromPool : private NonCopyable, private NonMovable
{
	ObjectFromPool(Object& object, ObjectPool& objectPool) 
		: objectPool(objectPool),
		object(object)
	{}
	virtual ~ObjectFromPool() {}

	ObjectPool& objectPool;
	Object& object;
};

//Object Pool
template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
public:
	ObjectPool()
		: m_objectPool(),
		m_nextAvailableObject(nullptr)
	{}
	virtual ~ObjectPool() {}

protected:
	std::vector<ObjectInPool<Object>> m_objectPool;
	ObjectInPool<Object>* m_nextAvailableObject;
};