#ifndef SYMPHONY4_ACTHASHER_H
#define SYMPHONY4_ACTHASHER_H

#include "filehook.h"

class ActHasher
{
public:
	void Init();
	void Dispose();

protected:
	void FileOpenedSlot(CFile *file);
};

extern ActHasher act_hasher;

#endif