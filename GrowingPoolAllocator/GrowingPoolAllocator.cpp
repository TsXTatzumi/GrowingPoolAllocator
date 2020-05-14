#include "GrowingPoolAllocator.h"

GrowingPoolAllocator::GrowingPoolAllocator(size_t elementSize, size_t maxElements, size_t elementsPerBlock) 
:mElementSize(elementSize), mMaxElements(maxElements), mElementsPerBlock(elementsPerBlock)
{
	if (mElementSize < sizeof(element)) {
		//throwException();
	}

	if (mElementsPerBlock == 0) {
		//throwException();
	}

	mMaxBlocks = mMaxElements / mElementsPerBlock;
	if ((mMaxElements % mElementsPerBlock) != 0) {
		mMaxBlocks++;
	}
}

GrowingPoolAllocator::~GrowingPoolAllocator() {
	for (auto block : mBlocks) {
		delete[] (char*)block;
	}
}