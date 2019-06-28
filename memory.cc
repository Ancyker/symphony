#include "memory.h"
#include "signature.h"
#include <windows.h>

Memory memory;

void Memory::Init()
{
	heap_alloc_pointer_ = reinterpret_cast<void *(__cdecl *)(unsigned int)>(0x70BA6B);
	free_pointer_ = reinterpret_cast<void (__cdecl *)(void *)>(0x6FD6A6);
}

void Memory::Dispose()
{
}