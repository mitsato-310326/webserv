#include "HttpRequestsParser.hpp"

HttpHeader	HttpRequestsParser::ParseHeaderOneLine(const std::string& str)
{
	std::pair<std::string, std::string> pair = cdvide(str, ':');
	HttpHeader header;
	if (!StringPermition(pair.first, METHODPERMITION))
		throw RequestHeaderException ();
	header.name = toUpper(pair.first);
	header.value = strtrim(pair.second, " \t\r\n");
	if (header.value.find_first_of(OPERATORCHAR, 0, sizeof(OPERATORCHAR)) != std::string::npos)
		throw RequestHeaderException ();
	return (header);
}

void	HttpRequestsParser::ParseTrailer(const std::string& text)
{
	while (true)
	{
		std::string line = ExtractLine(text);
		if (line.empty())
			return ;
		if (line == "\r\n")
			return ;
		HttpHeader header = ParseHeaderOneLine(line);
		if (header.name.empty())
			throw RequestHeaderException ();
	}
}

void	HttpRequestsParser::ParseOneHeader(const std::string& line)
{
	if (line.empty())
		throw RequestHeaderException ();
	HttpHeader header = ParseHeaderOneLine(line);
	if (header.name.empty())
		throw RequestHeaderException ();
	if (header.name == HOST)
		ParseHostValue(header.value);
	else if (header.name == CONTENTLENGTH)
		ParseContentLengthValue(header.value);
	else if (header.name == TRANSFERENCODING)
		ParseTransferEncodingValue(header.value);
	_httprequests.headers.push_back(header);
}

void	HttpRequestsParser::ParseHeader(const std::string& str)
{
	while (true)
	{
		std::string line = ExtractLine(str);
		if (line.empty())
			return ;
		if (line == "\r\n")
		{
			if (_httprequests.host.empty())
				throw RequestHeaderException ();
			return ;
		}
		ParseOneHeader(line);
	}
}

const HttpRequests& HttpRequestsParser::GetRequest() const
{
	return (_httprequests);
}
