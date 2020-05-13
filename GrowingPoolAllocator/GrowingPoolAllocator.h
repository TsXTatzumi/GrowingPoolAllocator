#pragma once

constexpr auto MAX_SIZE = 8192;

union storageBlock
{
	storageBlock* next;
};

class GrowingPoolAllocator
{
	GrowingPoolAllocator(size_t poolSize, char elemSize);
	
	template<typename T>
	T* alloc(T& data);

	template<typename T>
	void  free(T* ptr);

private:
	storageBlock* first_free_element;
	
	const size_t poolSize;
	const char elemSize;

	void** pools;
};