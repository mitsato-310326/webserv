#include "HttpRequestsParser.hpp"

HttpRequestsParser::HttpRequestsParser()
{
	_httprequests.versionMajor = 0;
	_httprequests.versionMinor = 0;
	_httprequests.chunkBytesRemaining = 0;
	_httprequests.chunkBytes = std::string::npos;
	_httprequests.transferEncoding.clear();
	_result.status = ParseLoad;
	_result.nstatus = First;
	_chunkStatus = ChunkNotStarted;
	_cursor = 0;
}

HttpRequestsParser::~HttpRequestsParser()
{
}

HttpRequestsParser&	HttpRequestsParser::operator=(const HttpRequestsParser& rhs)
{
	_httprequests.body = rhs._httprequests.body;
	_httprequests.headers = rhs._httprequests.headers;
	_httprequests.method = rhs._httprequests.method;
	_httprequests.path = rhs._httprequests.path;
	_httprequests.query = rhs._httprequests.query;
	_httprequests.target = rhs._httprequests.target;
	_httprequests.versionMajor = rhs._httprequests.versionMajor;
	_httprequests.versionMinor = rhs._httprequests.versionMinor;
	return (*this);
}


std::string::size_type	HttpRequestsParser::LineLength(const std::string& str)
{
	std::string::size_type	end = str.find("\r\n", _cursor);
	std::string::size_type len;
	if (end == std::string::npos)
	{
		len = str.length() - _cursor + 2;
		if (len > MAXLINEBYTES)
			throw LineTooLongException ();
		return (std::string::npos);
	}
	len = end - _cursor + 2;
	if (len > MAXLINEBYTES)
		throw LineTooLongException ();
	return (len);
}

/* byte が途中の時、この関数内でループ待ち処理を行う想定。 */
std::string	HttpRequestsParser::ExtractLine(const std::string &str)
{
	std::string::size_type len =  LineLength(str);
	if (len == std::string::npos)
		return ("");
	std::string line = str.substr(_cursor, len);
	_cursor += len;
	return (line);
}
