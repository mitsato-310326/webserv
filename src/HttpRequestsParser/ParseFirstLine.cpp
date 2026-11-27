#include "HttpRequestsParser.hpp"

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
	std::string FirstLine = ExtractLine(str);
	const std::string::size_type newline = str.find('\n');
	if (newline != std::string::npos && (newline == 0 || str[newline - 1] != '\r'))
		throw RequestFirstLineException();
	if (FirstLine.empty())
		throw InternalSystemException();
	FirstLine.erase(FirstLine.size() - 2);
	std::vector<std::string> words = csplit(FirstLine, ' ');
	if (words.size() != 3 || words[0].empty() || words[1].empty() || words[2].empty())
		throw RequestFirstLineException();
	ParseMethod(words[0]);
	ParseUri(words[1]);
	ParseHttp(words[2]);
}
