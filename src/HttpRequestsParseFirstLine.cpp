#include "../inc/HttpRequestsParser.hpp"

HttpRequestsParser::HttpRequestsParser()
{
	_httprequests.versionMajor = 0;
	_httprequests.versionMajor = 0;
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

void	HttpRequestsParser::isLineTooLong(const std::string& str, std::string::size_type max)
{
	std::string::size_type	end = str.find("\r\n", _cursor);
	if (end == std::string::npos)
	{
		end = str.length();
	}
	if (end - _cursor > max)
	{
		throw LineTooLongException();
	}
	else
		return ;
}

void	HttpRequestsParser::ParseMethod(const std::string& method)
{
	if (!StringPermition(method, METHODPERMITION))
		throw RequestFirstLineException();
	_httprequests.method = method;
}

void	HttpRequestsParser::ParseUri(const std::string& target)
{
	_httprequests.target = target;
	std::string::size_type	qpos = target.find("?");
	std::string	path = target.substr(0, qpos);
	_httprequests.path = ParsePath(path);
	if (qpos == std::string::npos)
		return ;
	std::string query = target.substr(qpos + 1, std::string::npos);
	_httprequests.query = ParseQuery(query);	
}

std::string HttpRequestsParser::ParsePath(const std::string& path)
{
	if (path[0] != '/')
		return (ParseAbsoluteForm(path));
	return (ParseOriginForm(path));
}

std::string	HttpRequestsParser::ParseAbsoluteForm(const std::string& path)
{
	std::string::size_type cursor = path.find_first_not_of(ALPHABET);
	
	std::string http = path.substr(0, cursor);
	http = toUpper(http);
	if (http != "HTTP" && http!= "HTTPS")
		throw RequestFirstLineException ();
	_httprequests.absProtocol = http;
	std::string cols = path.substr(cursor, 3);
	if (cols != "://")
		throw RequestFirstLineException ();
	cursor += 3;
	std::string::size_type end = path.find_first_of("/:");
	if (end == std::string::npos)
		throw RequestFirstLineException ();
	_httprequests.absHost = path.substr(cursor, end);
	cursor = end;
	if (path[cursor] == ':')
	{
		end = path.find('/', cursor + 1);
		if (end == std::string::npos)
			throw RequestFirstLineException ();
		_httprequests.absPort = path.substr(cursor, end);
		cursor = end;
	}
	return (ParseOriginForm(path.substr(cursor, std::string::npos)));
}

std::string	HttpRequestsParser::ParseOriginForm(const std::string& path)
{
	if (path[0] != '/')
		throw RequestFirstLineException ();
	if (!StringPermition(path, PATHPERMITION))
		throw RequestFirstLineException ();
	return (DecodeString(path));
}

std::string	HttpRequestsParser::ParseQuery(const std::string& query)
{
	if (query.empty())
		return ("");
	if (!StringPermition(query, QUERYPERMITION))
		throw RequestFirstLineException ();
	return (DecodeString(query));
}

void HttpRequestsParser::ParseHttp(const std::string& Http)
{
	if (Http.length() != 8)
		throw RequestFirstLineException ();
	if (Http.substr(0, 5) != "HTTP/")
		throw RequestFirstLineException ();
	std::string	Major = Http.substr(5, 1);
	if (Major.find_first_of(NUMBER))
		throw RequestFirstLineException ();
	if (Http.substr(6, 1) != ".")
		throw RequestFirstLineException ();
	std::string	Minor = Http.substr(7, 1);
	if (Minor.find_first_of(NUMBER))
		throw RequestFirstLineException ();
	char *end;
	_httprequests.versionMajor = strtoul(Major.c_str(), &end, 10);
	_httprequests.versionMinor = strtoul(Minor.c_str(), &end, 10);
}

void	HttpRequestsParser::ParseFirstLine(const std::string& str)
{
	if (_cursor != 0)
		throw InternalSystemException();
	std::string::size_type pos = str.find("\r\n");
	if (pos == std::string::npos)
		throw RequestFirstLineException();
	std::string FirstLine = str.substr(_cursor, pos);
	isLineTooLong(FirstLine, MAXLINEBYTES);
	std::vector<std::string> words = split(FirstLine);
	if (words.size() != 3)
		throw RequestFirstLineException();
	ParseMethod(words[0]);
	ParseUri(words[1]);
	ParseHttp(words[2]);
}
