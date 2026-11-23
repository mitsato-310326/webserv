#include "../inc/HttpRequestsParser.hpp"

void	HttpRequestsParser::RequestsParser(const std::string& str)
{
	ParseFirstLine(str);
	ParseHeader(str);
	if (_httprequests.contentLength >= 0)
		ParseContentLength(str);
}
