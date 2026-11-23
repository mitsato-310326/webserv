#include "../inc/HttpRequestsParser.hpp"

HttpHeader	HttpRequestsParser::ParseHeaderOneLine(const std::string& str)
{
	std::vector<std::string> vector = csplit(str, ':');
	HttpHeader header;
	header.name = vector[0];
	header.value = vector[1];
	return (header);
}

static TransferEncoding ParseTransferCoding(const std::string& token)
{
    if (token == "chunked")
        return CHUNKED;
    if (token == "gzip")
        return GZIP;
    if (token == "compress")
        return COMPRESS;
    if (token == "deflate")
        return DEFLATE;
    return UNKNOWN;
}

void	HttpRequestsParser::ParseOneHeader(const std::string& line)
{
	if (line.empty())
		throw RequestHeaderException ();
	HttpHeader header = ParseHeaderOneLine(line);
	if (header.name == HOST)
	{
		if (!_httprequests.host.empty())
			throw RequestHeaderException ();
		_httprequests.host = header.value;
	}
	else if (header.name == CONTENTLENGTH)
	{
		if (_httprequests.contentLength != -1)
			throw RequestHeaderException ();
		char *end;
		_httprequests.contentLength = strtoul(header.value.c_str(), &end, 10);
	}
	else if (header.name == TRANSFERENCODING)
	{
		std::vector<std::string> vector = csplit(header.value, ',');
		size_t size = vector.size();
		for (size_t i = 0; i < size; i++)
		{
			std::vector<std::string> value = split(vector[i]);
			if (value.size() != 1)
				throw RequestHeaderException ();
			TransferEncoding type = ParseTransferCoding(value[0]);
			_httprequests.transferEncoding.push_back(type);
		}
	}
	else
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
			return ;
		ParseOneHeader(line);
	}
}
