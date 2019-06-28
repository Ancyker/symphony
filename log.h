#ifndef SYMPHONY4_LOG_H
#define SYMPHONY4_LOG_H

#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>

class LogSink
{
public:
	typedef char char_type;
	typedef boost::iostreams::sink_tag category;

	std::streamsize write(const char_type* s, std::streamsize n);

protected:
	std::string buffer_;
};

extern boost::iostreams::stream<LogSink> logout;

#endif