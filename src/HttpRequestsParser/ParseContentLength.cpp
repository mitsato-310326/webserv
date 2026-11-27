#include "HttpRequestsParser.hpp"

void HttpRequestsParser::ParseContentLengthValue(const std::string& value)
{
	if (_httprequests.contentLength != -1)
		throw RequestHeaderException();
	if (!StringPermition(value, NUMBER))
		throw RequestHeaderException();
	errno = 0;
	const unsigned long length = strtoul(value.c_str(), NULL, 10);
	if (errno == ERANGE)
		throw RequestHeaderException();
	_httprequests.contentLength = length;
}
