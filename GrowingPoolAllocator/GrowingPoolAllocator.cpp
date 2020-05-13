#include "GrowingPoolAllocator.h"

#include <cstdlib>


GrowingPoolAllocator::GrowingPoolAllocator(size_t poolSize, char elemSize) : poolSize(poolSize), elemSize(elemSize)
{
	pools = new void* [MAX_SIZE / poolSize];

	pools[0] = malloc(poolSize);
}

template<typename T>
inline T* GrowingPoolAllocator::alloc(T& data)
{
	if(sizeof(T) > elemSize)
	{
		return nullptr;
	}

	T* ptr = first_free_element;

	if (ptr != nullptr)
	{
		first_free_element = first_free_element->next;

		*ptr = data;
	}
	
	return ptr;
}
