#include "xray.h"
#include "signature.h"
#include <boost/bind.hpp>
#include "memory.h"

XRay xray;

void XRay::Init()
{
	session_hook.ConnectCreatedSlot(boost::bind(&XRay::SessionCreatedSlot, this, _1));
}

void XRay::Dispose()
{
}

void XRay::SessionCreatedSlot(CSession *session)
{
	char ***new_pc_head_name_table_m = reinterpret_cast<char***>(0x8461A8+5280);
	char ***new_pc_head_name_table_f = reinterpret_cast<char***>(0x8461A8+5296);

	LoadXrayTable("vanity_m_tab.txt", new_pc_head_name_table_m);
	int head_name_table_size = LoadXrayTable("vanity_f_tab.txt", new_pc_head_name_table_f);

	BYTE *address = 0;

	for (int i = 0; i < 2; ++i)
	{
		address = Signature("558BEC8B550C8B0285C07C0583F8..7E06C7020D0000008B451085C07405B801", address).getAddress(14);
		DWORD old_protect;

		VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &old_protect);
	
		*address = head_name_table_size;
	}
}

int XRay::LoadXrayTable(const char *table_file_name, char ***table)
{
	//CFile is allocated on the heap (and never freed) to prevent calling the destructor.
	CFile *file = new CFile();
	file->Open(table_file_name, 0);

	memory.Free(*table);
	*table = AllocTable();

	return ParseTable(file, *table);
}

char** XRay::AllocTable()
{
	char **table = reinterpret_cast<char**>(memory.Alloc(sizeof(char*)*32767));

	for (int i = 0; i < 32767; ++i)
	{
		table[i] = "";
	}

	return table;
}

int XRay::ParseTable(CFile *file, char **table)
{
	char *token = strtok(file->buffer, "\r\n");
	int i = 0;

	do
	{
		if (token[0] == '!')
		{
			i = atoi(token+1);

			continue;
		}

		if (token[0] == '*')
		{
			for (int k = 0; k < 32767; ++k)
			{
				if (!table[k][0])
				{
					table[k] = token+1;
				}
			}
			
			continue;
		}

		table[i++] = token;
	}
	while ((token = strtok(NULL, "\r\n")));

	return i;
}