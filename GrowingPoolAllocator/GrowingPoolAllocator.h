#ifndef GROWINGPOOLALLOCATOR_H
#define GROWINGPOOLALLOCATOR_H

#include <vector>
#include <cstdint>

class GrowingPoolAllocator {
public:
	GrowingPoolAllocator(size_t elementSize, size_t maxElements, size_t elementsPerBlock);
	~GrowingPoolAllocator();

	template<typename T, typename... args>
	T* alloc(args... arguments);
	
	template<typename T>
	void free(const T* ptr);

private:
	struct element {
		element* next;
	};

	const size_t mElementSize;
	const size_t mMaxElements;
	const size_t mElementsPerBlock;
	size_t mMaxBlocks;
	std::vector<void*> mBlocks;
	element* mFreeList = nullptr;
};

template<typename T, typename... args>
T* GrowingPoolAllocator::alloc(args... arguments) {
	if (sizeof(T) > mElementSize) {
		return nullptr;
	}

	if (mFreeList == nullptr) {
		if (mBlocks.size() >= mMaxBlocks) {
			return nullptr;
		}

		void* block = new char[mElementSize * mElementsPerBlock];
		if (block == nullptr) {
			return nullptr;
		}

		mBlocks.push_back(block);
		mFreeList = (element*)block;
		for (size_t i = 0; i < mElementsPerBlock; ++i) {
			element* current = ((element*)((uintptr_t)block + (mElementSize * i)));
			element* next = nullptr;

			if (i < (mElementsPerBlock - 1)) {
				next = ((element*)((uintptr_t)block + (mElementSize * (i + 1))));
			}

			current->next = next;
		}
	}

	void* elem = mFreeList;
	mFreeList = mFreeList->next;

	return new (elem) T(arguments...);
}

template<typename T>
void GrowingPoolAllocator::free(const T* ptr) {
	if (ptr == nullptr) {
		return;
	}

	//check if pointer is inside one of our blocks
	//if not throw exception

	ptr->~T();
	element* tmp = (element*)ptr;
	tmp->next = mFreeList;
	mFreeList = tmp;
}

#endif