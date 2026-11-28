#include "HttpRequestsParser.hpp"
#include "UrlDecode.hpp"

struct HttpRequestsParser::ParsedTarget
{
	std::string path;
	std::string query;
	std::string scheme;
	std::string host;
	std::string port;
};

struct HttpRequestsParser::HttpVersion
{
	unsigned int major;
	unsigned int minor;
};

void	HttpRequestsParser::ParseRequestLine(const std::string& buffer)
{
	const std::string requestLine = ExtractLine(buffer);
	if (_result.status == NeedMoreData)
		return;

	const std::vector<std::string> fields = SplitRequestLine(requestLine);
	const std::string method = ParseMethod(fields[0]);
	const ParsedTarget target = ParseRequestTarget(fields[1]);
	const HttpVersion version = ParseHttpVersion(fields[2]);

	_request.method = method;
	_request.target = fields[1];
	_request.path = target.path;
	_request.query = target.query;
	_request.absProtocol = target.scheme;
	_request.absHost = target.host;
	_request.absPort = target.port;
	_request.versionMajor = version.major;
	_request.versionMinor = version.minor;
}

std::vector<std::string> HttpRequestsParser::SplitRequestLine(const std::string& line)
{
	const std::vector<std::string> fields = csplit(StripTrailingCRLF(line), ' ');
	if (fields.size() != 3 || fields[0].empty() || fields[1].empty() || fields[2].empty())
		throw RequestLineException();
	return (fields);
}

std::string HttpRequestsParser::ParseMethod(const std::string& method)
{
	if (!ContainsOnly(method, TOKEN_CHARS))
		throw RequestLineException();
	return (method);
}

HttpRequestsParser::ParsedTarget HttpRequestsParser::ParseRequestTarget(const std::string& target) const
{
	const std::string::size_type queryStart = target.find('?');
	const std::string path = target.substr(0, queryStart);
	ParsedTarget parsed;
	if (!path.empty() && path[0] == '/')
		parsed.path = ParseOriginForm(path);
	else
		parsed = ParseAbsoluteForm(path);
	if (queryStart != std::string::npos)
		parsed.query = ParseQuery(target.substr(queryStart + 1));
	return (parsed);
}

HttpRequestsParser::ParsedTarget HttpRequestsParser::ParseAbsoluteForm(const std::string& path) const
{
	const std::string::size_type schemeEnd = path.find("://");
	if (schemeEnd == std::string::npos)
		throw RequestLineException();
	const std::string scheme = toUpper(path.substr(0, schemeEnd));
	if (scheme != "HTTP" && scheme != "HTTPS")
		throw RequestLineException();

	const std::string::size_type authorityStart = schemeEnd + 3;
	const std::string::size_type pathStart = path.find('/', authorityStart);
	const std::string authority = path.substr(authorityStart, pathStart - authorityStart);
	if (authority.find('\0') != std::string::npos)
		throw RequestLineException();
	const HostType hostType = ParseHost(authority);
	const std::string::size_type portSeparator = hostType == HOST_TYPE_IPV6
		? authority.find(':', authority.find(']') + 1) : authority.find(':');

	ParsedTarget parsed;
	parsed.scheme = scheme;
	parsed.host = authority.substr(0, portSeparator);
	if (portSeparator != std::string::npos)
		parsed.port = authority.substr(portSeparator + 1);
	parsed.path = ParseOriginForm(pathStart == std::string::npos ? "/" : path.substr(pathStart));
	return (parsed);
}

std::string	HttpRequestsParser::ParseOriginForm(const std::string& path)
{
	if (path[0] != '/')
		throw RequestLineException ();
	if (!ContainsOnly(path, PATH_CHARS))
		throw RequestLineException ();
	return (DecodeRequestTargetPart(path));
}

std::string	HttpRequestsParser::ParseQuery(const std::string& query)
{
	if (query.empty())
		return ("");
	if (!ContainsOnly(query, QUERY_CHARS))
		throw RequestLineException ();
	return (DecodeRequestTargetPart(query));
}

HttpRequestsParser::HttpVersion HttpRequestsParser::ParseHttpVersion(const std::string& version)
{
	if (version.size() != 8 ||
		version.compare(0, 5, "HTTP/") != 0 ||
		version[5] < '0' || version[5] > '9' ||
		version[6] != '.' ||
		version[7] < '0' || version[7] > '9')
		throw RequestLineException();
	const HttpVersion parsed = {
		static_cast<unsigned int>(version[5] - '0'),
		static_cast<unsigned int>(version[7] - '0')
	};
	return (parsed);
}

std::string HttpRequestsParser::DecodeRequestTargetPart(const std::string& text)
{
	try
	{
		return (PercentDecode(text));
	}
	catch (const UrlDecodeException&)
	{
		throw RequestLineException();
	}
}
