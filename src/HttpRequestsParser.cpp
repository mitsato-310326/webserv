#include "../inc/HttpRequestsParser.hpp"

HttpRequestsParser::HttpRequestsParser()
{
	_httprequests.versionMajor = 0;
	_httprequests.versionMajor = 0;
	_cursor = 0;
}
HttpRequestsParser::~HttpRequestsParser() {}

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

void	HttpRequestsParser::isLineTooLong(const std::string& str, std::string::size_type max)
{
	std::string::size_type	end = str.find("\r\n", _cursor);
	if (end == std::string::npos)
	{
		end = str.length();
	}
	if (end - _cursor + 2 > max)
	{
		throw LineTooLongException();
	}
	else
		return ;
}

bool	HttpRequestsParser::ParseFirstLine(const std::string& str)
{
	(void)str;
	return (true);
}

const char* HttpRequestsParser::LineTooLongException::what() const throw()
{
	return ("HttpRequestsParser: One line is too long to be used in HTPP Request");
}


