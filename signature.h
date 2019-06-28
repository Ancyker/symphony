#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <string>

class Signature
{
public:
	class NotFoundException {};

	Signature(const std::string signature, unsigned char *start_address = NULL);
	Signature(const unsigned char *bytes, const bool *wildcards, unsigned int length, unsigned char *start_address = 0);

	~Signature();

	unsigned char *findAddress();
	unsigned char *getAddress(int offset = 0);
	unsigned char *getRelativeAddress(int offset = 0);
private:
	unsigned int _length;
	unsigned char *_address, *_bytes, *_start_address;
	bool *_wildcards;
};

#endif