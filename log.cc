#include "log.h"
#include "loghook.h"

boost::iostreams::stream<LogSink> logout;

std::streamsize LogSink::write(const LogSink::char_type* s, std::streamsize n)
{
	buffer_.append(s, s+n);

	while (buffer_.find('\n') != buffer_.npos)
	{
		log_hook.Write(buffer_.substr(0, buffer_.find('\n')).c_str(), 0x00FFFFFF, 0);

		buffer_.erase(buffer_.begin(), buffer_.begin()+buffer_.find('\n')+1);
	}

	return n;
}