#include "RedImage.hpp"


namespace RedImageTool
{

	void* RedDefaultMemoryAllocationFunction(void* pointer, size_t size)
	{
		if (!size)
		{
			if (pointer != nullptr)
			free(pointer);
			return nullptr;
		}
		if (pointer)
		{
			return realloc(pointer, size);
		}
		return malloc(size);
	}
	red_memory_allocation_function MemoryAllocationFunction = RedDefaultMemoryAllocationFunction;
}