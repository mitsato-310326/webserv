#include "../inc/HttpRequestsParser.hpp"

void	HttpRequestsParser::ParseContentLength(const std::string& text)
{
	std::string::size_type len = _httprequests.contentLength;
	std::string::size_type dist = len - _httprequests.body.size();
	_httprequests.body = text.substr(_cursor, dist);
	std::string::size_type size = _httprequests.body.size();
	if (size < len)
		throw RequestBodyException ();
	else if(size > len)
		throw RequestBodyException ();
	return ;
}
