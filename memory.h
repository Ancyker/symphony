#ifndef SYMPHONY4_MEMORY_H
#define SYMPHONY4_MEMORY_H

class Memory
{
public:
	void Init();
	void Dispose();

	void* Alloc(unsigned int length) { return heap_alloc_pointer_(length); }
	void Free(void *buffer) { free_pointer_(buffer); }

protected:
	void* (__cdecl *heap_alloc_pointer_)(unsigned int length);
	void (__cdecl *free_pointer_)(void *buffer);
};

extern Memory memory;

#endif