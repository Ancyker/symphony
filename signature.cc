#include "signature.h"
#include <windows.h>
#include <iostream>

Signature::Signature(const std::string signature, unsigned char *start_address)
{
	_length = signature.length()/2;
	_bytes = new unsigned char[_length];
	_wildcards = new bool[_length];
	_address = 0;
	_start_address = start_address;

	memset(_wildcards, 0, _length);

	//Translate each byte in the representation of the pattern into an actual byte
	for (unsigned i = 0; i < _length; ++i)
	{
		std::string byte = signature.substr(i*2,2); 
		if (byte == "..")
		{
			_wildcards[i] = true;
		}
		else
		{
			_bytes[i] = (unsigned char)strtol(byte.c_str(), NULL, 16);
		}
	}
}

Signature::Signature(const unsigned char *bytes, const bool *wildcards, unsigned int length, unsigned char *start_address)
{
	_length = length;
	_bytes = new unsigned char[_length];
	_wildcards = new bool[_length];
	_address = 0;
	_start_address = start_address;

	memcpy(_bytes, bytes, _length);
	memcpy(_wildcards, wildcards, _length);
}

Signature::~Signature()
{
	delete [] _bytes;
	delete [] _wildcards;
}

unsigned char *Signature::findAddress()
{
	unsigned char *baseAddress = (_start_address ? _start_address : (unsigned char*)GetModuleHandle(NULL));

	//Iterate through the module until a match is found or the pointer becomes unreadable
	for (unsigned char *i = baseAddress; !IsBadReadPtr(i, _length); ++i)
	{
		bool isMatch = true;

		for (unsigned int j = 0; j < _length; ++j)
		{
			if (_wildcards[j]) //Skip wildcard bytes
				continue;

			if (i[j] != _bytes[j])
			{
				isMatch = false;
				break;
			}
		}

		if (isMatch)
		{
			_address = i;
			return _address;
		}
	}

	throw NotFoundException();
}

unsigned char *Signature::getAddress(int offset)
{
	if (_address == 0)
	{
		return findAddress()+offset;
	}
	else
	{
		return _address+offset;
	}
}

unsigned char *Signature::getRelativeAddress(int offset)
{
	unsigned char *address = getAddress(offset);
	DWORD relative_address = *reinterpret_cast<DWORD*>(address);

	return address+relative_address+sizeof(DWORD);
}