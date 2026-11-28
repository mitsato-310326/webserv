#include "HttpRequestsParser.hpp"
#include <limits>

void HttpRequestsParser::ParseContentLengthValue(const std::string& value)
{
	if (_request.contentLength != UNDEFINED)
		throw RequestHeaderException();
	if (!ContainsOnly(value, NUMBER))
		throw RequestHeaderException();
	errno = 0;
	const unsigned long length = strtoul(value.c_str(), NULL, 10);
	if (errno == ERANGE ||
		length > static_cast<unsigned long>(std::numeric_limits<ssize_t>::max()))
		throw RequestHeaderException();
	if (length > MAXCONTENTLENGTH)
		throw RequestParseException(BodyTooLarge);
	_request.contentLength = static_cast<ssize_t>(length);
}
