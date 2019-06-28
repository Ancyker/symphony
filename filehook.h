#ifndef SYMPHONY4_FILEHOOK_H
#define SYMPHONY4_FILEHOOK_H

#include <windows.h>
#include <boost/signal.hpp>
#include "callchain.h"

class CFile
{
public:
	HANDLE handle;
	char *buffer;
	int pointer;
	DWORD size;
	char file_name[128];

	CFile() { ctor_(this); }
	~CFile() { dtor_(this); }
	bool Open(const char *file_name, int flags) { return Open_(this, file_name, flags); }
	bool Close() { return Close_(this); }
	bool Read(void *buffer, unsigned long length) { return Read_(this, buffer, length); }
	bool Write(void *buffer, unsigned long length) { return Write_(this, buffer, length); }
	bool Seek(long distance, unsigned long method) { return Seek_(this, distance, method); }

	friend class FileHook;

protected:
	static bool (__thiscall *ctor_)(CFile* file);
	static bool (__thiscall *dtor_)(CFile* file);
	static bool (__thiscall *Open_)(CFile* file, const char *file_name, int flags);
	static bool (__thiscall *Close_)(CFile* file);
	static bool (__thiscall *Read_)(CFile* file, void *buffer, unsigned long length);
	static bool (__thiscall *Write_)(CFile* file, void *buffer, unsigned long length);
	static bool (__thiscall *Seek_)(CFile* file, long distance, unsigned long method);

	static bool (__cdecl *IsFileExist)(const char *file_name);
	static char* (__cdecl *MakeFileName)(char *buffer, char *file_name);
};

class FileHook
{
public:
	typedef boost::signal<bool (CFile*, const char*, int), CallChain> open_signal_t;
	typedef boost::signal<void (CFile*)> opened_signal_t;
	typedef boost::signal<bool (const char*), CallChain> is_file_exist_signal_t;
	typedef boost::signal<bool (void*, const char*), CallChain> add_pak_signal_t;
	typedef boost::signal<void (void*, const char*)> pak_added_signal_t;

	void Init();
	void Dispose();

	bool AddPak(void *file_mgr, const char *file_name) { AddPakHook(file_mgr, file_name); }

	boost::signals::connection ConnectOpenSlot(open_signal_t::slot_function_type slot, int priority = 0) { return open_signal_.connect(priority, slot); }
	boost::signals::connection ConnectOpenedSlot(opened_signal_t::slot_function_type slot, int priority = 0) { return opened_signal_.connect(priority, slot); }
	boost::signals::connection ConnectIsFileExistSlot(is_file_exist_signal_t::slot_function_type slot, int priority = 0) { return is_file_exist_signal_.connect(priority, slot); }
	boost::signals::connection ConnectAddPakSlot(add_pak_signal_t::slot_function_type slot, int priority = 0) { return add_pak_signal_.connect(priority, slot); }
	boost::signals::connection ConnectPakAddedSlot(pak_added_signal_t::slot_function_type slot, int priority = 0) { return pak_added_signal_.connect(priority, slot); }

protected:
	static bool __cdecl IsFileExistHook(const char *file_name);
	bool (__cdecl *is_file_exist_pointer_)(const char *file_name);

	static HANDLE WINAPI CreateFileMappingAHook(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
	HANDLE (WINAPI *create_file_mapping_a_pointer_)(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);

	static bool __thiscall OpenHook(CFile* file, const char *file_name, int flags);
	bool (__thiscall *open_pointer_)(CFile* file, const char *file_name, int flags);
	bool OpenSlot(CFile* file, const char *file_name, int flags) { return open_pointer_(file, file_name, flags); }

	static bool __thiscall AddPakHook(void* file_mgr, const char *file_name);
	bool (__thiscall *add_pak_pointer_)(void* file_mgr, const char *file_name);
	bool AddPakSlot(void* file_mgr, const char *file_name) { return add_pak_pointer_(file_mgr, file_name); }

	open_signal_t open_signal_;
	opened_signal_t opened_signal_;
	is_file_exist_signal_t is_file_exist_signal_;
	add_pak_signal_t add_pak_signal_;
	pak_added_signal_t pak_added_signal_;
};

extern FileHook file_hook;

#endif