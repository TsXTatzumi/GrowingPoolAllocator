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
	std::vector<size_t> mElementsUsedPerBlock;
	element* mFreeList = nullptr; // root for alloc order == next position

	template<typename T>
	int getBlockNr(const T* ptr) {

		int blockNr = -1;
		for (int b = 0; b < mBlocks.size(); b++) {
			if ((int)ptr < (int)mBlocks[b] || (int)ptr >= (int)mBlocks[b] + mElementsPerBlock * mElementSize) {
				continue;
			}
			for (int e = 0; e < mElementsPerBlock; e++) {
				if ((int)mBlocks[b] + e * mElementSize == (int)ptr) {
					blockNr = b;
					break;
				}
			}
		}
		return blockNr;
	}
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
		mElementsUsedPerBlock.push_back(0);

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
	mElementsUsedPerBlock[getBlockNr(elem)]++;
	mFreeList = mFreeList->next;

	return new (elem) T(arguments...);
}

template<typename T>
void GrowingPoolAllocator::free(const T* ptr) {
	if (ptr == nullptr) {
		return;
	}


	//check if pointer is inside one of our blocks
	int blockNr = getBlockNr(ptr);
	if (blockNr < 0) {
		//if not throw exception
		return;
	}
	
	ptr->~T();
	element* current = (element*)ptr;
	current->next = mFreeList;
	mFreeList = current; // root

	
	mElementsUsedPerBlock[blockNr]--;
	if (mElementsUsedPerBlock[blockNr] == 0) {
		// remove all elements of the block (except the root)

		element* next = current->next; // current is root right now
		while (true) {
			if (next == nullptr) {
				break;
			}

			if (getBlockNr(next) == blockNr) {
				// keep current & remove next
				current->next = next->next;
				next = current->next;
			}
			else {
				// move both over by one
				current = current->next;
				next = current->next;
			}
		}

		// remove the root (its the last element in the block)
		mFreeList = mFreeList->next;

		// finally remove the block
		auto block = mBlocks[blockNr];
		mBlocks.erase(mBlocks.begin() + blockNr);
		mElementsUsedPerBlock.erase(mElementsUsedPerBlock.begin() + blockNr);
		delete[](char*)block;
	}
}

#endif