#ifndef SYMPHONY4_XRAY_H
#define SYMPHONY4_XRAY_H

#include "sessionhook.h"
#include "filehook.h"

class XRay
{
public:
	void Init();
	void Dispose();

protected:
	void SessionCreatedSlot(CSession *session);

	int LoadXrayTable(const char *table_file_name, char ***table);
	char** AllocTable();
	int ParseTable(CFile *file, char **table);
};

extern XRay xray;

#endif