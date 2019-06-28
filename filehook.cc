#include "filehook.h"
#include "signature.h"
#include <madCHook.h>
#include <boost/bind.hpp>

FileHook file_hook;

bool (__thiscall *CFile::ctor_)(CFile* file);
bool (__thiscall *CFile::dtor_)(CFile* file);
bool (__thiscall *CFile::Open_)(CFile* file, const char *file_name, int flags);
bool (__thiscall *CFile::Close_)(CFile* file);
bool (__thiscall *CFile::Read_)(CFile* file, void *buffer, unsigned long length);
bool (__thiscall *CFile::Write_)(CFile* file, void *buffer, unsigned long length);
bool (__thiscall *CFile::Seek_)(CFile* file, long distance, unsigned long method);
bool (__cdecl *CFile::IsFileExist)(const char *file_name);
char* (__cdecl *CFile::MakeFileName)(char *buffer, char *file_name);

void FileHook::Init()
{
	CFile::ctor_ = reinterpret_cast<bool (__thiscall *)(CFile *)>(Signature("8BC1C70000000000C7400400000000C3................................").getAddress());
	CFile::dtor_ = reinterpret_cast<bool (__thiscall *)(CFile *)>(Signature("568BF18B0685C0740F50FF15........C706000000005EC38B460485C0741568").getAddress());
	CFile::Open_ = reinterpret_cast<bool (__thiscall *)(CFile *,const char *,int)>(Signature("558BEC518B450C5356578BF985C0897DFC0F85830100008B45088D5F105053E8").getAddress());
	CFile::Close_ = reinterpret_cast<bool (__thiscall *)(CFile *)>(Signature("568BF18B0685C0740F50FF15........C706000000005EC38B460485C0741568").getAddress());
	CFile::Read_ = reinterpret_cast<bool (__thiscall *)(CFile *,void *,unsigned long)>(Signature("558BEC8BC15356578B0885C974218B550C8D450C6A00508B4508525051FF15..").getAddress());
	CFile::Write_ = reinterpret_cast<bool (__thiscall *)(CFile *,void *,unsigned long)>(Signature("558BEC8B0185C0741E8B550C8D4D0C6A00518B4D08525150FF15........85C0").getAddress());
	CFile::Seek_ = reinterpret_cast<bool (__thiscall *)(CFile *,long,unsigned long)>(Signature("558BEC8B0185C074178B4D0C8B5508516A005250FF15........B0015DC20800").getAddress());
	CFile::IsFileExist = reinterpret_cast<bool (__cdecl *)(const char *)>(Signature("558BEC81EC400300008B4508568D8D00FFFFFF5051E8........A1........83").getAddress());
	CFile::MakeFileName = reinterpret_cast<char *(__cdecl *)(char *,char *)>(Signature("558BEC81EC00040000535657BF........83C9FF33C0F2AE8B5D0868........").getAddress());

	//add_pak_pointer_ = reinterpret_cast<bool (__thiscall *)(void *,const char *)>(Signature("558BEC6AFF68........64A100000000506489250000000083EC105356578BF9").getAddress());
	add_pak_pointer_ = reinterpret_cast<bool (__thiscall *)(void *,const char *)>(0x5431F0);

	HookCode(CFile::IsFileExist, IsFileExistHook, (PVOID*)&is_file_exist_pointer_);
	HookCode(CFile::Open_, OpenHook, (PVOID*)&open_pointer_);
	HookCode(add_pak_pointer_, AddPakHook, (PVOID*)&add_pak_pointer_);
	HookAPI("kernel32", "CreateFileMappingA", CreateFileMappingAHook, (PVOID*)&create_file_mapping_a_pointer_);

	open_signal_.connect(50, boost::bind(&FileHook::OpenSlot, this, _1, _2, _3));
	is_file_exist_signal_.connect(50, boost::function<bool (const char*)>(is_file_exist_pointer_));
	add_pak_signal_.connect(50, boost::bind(&FileHook::AddPakSlot, this, _1, _2));
}

void FileHook::Dispose()
{
	UnhookCode((PVOID*)&is_file_exist_pointer_);
	UnhookCode((PVOID*)&open_pointer_);
	UnhookCode((PVOID*)&add_pak_pointer_);
	UnhookAPI((PVOID*)&create_file_mapping_a_pointer_);
}

bool __cdecl FileHook::IsFileExistHook(const char *file_name)
{
	return file_hook.is_file_exist_signal_(file_name);
}

HANDLE WINAPI FileHook::CreateFileMappingAHook(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
	return file_hook.create_file_mapping_a_pointer_(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, NULL);
}

bool __thiscall FileHook::OpenHook(CFile* file, const char *file_name, int flags)
{
	bool result = file_hook.open_signal_(file, file_name, flags);

	if (result && file_hook.opened_signal_.num_slots())
	{
		file_hook.opened_signal_(file);
	}

	return result;
}

bool __thiscall FileHook::AddPakHook(void* file_mgr, const char *file_name)
{
	bool result = file_hook.add_pak_signal_(file_mgr, file_name);

	if (result && file_hook.pak_added_signal_.num_slots())
	{
		file_hook.pak_added_signal_(file_mgr, file_name);
	}

	return result;
}