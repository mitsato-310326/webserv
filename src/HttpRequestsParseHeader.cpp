#include "../inc/HttpRequestsParser.hpp"

HttpHeader	HttpRequestsParser::ParseHeaderOneLine(const std::string& str)
{
	std::pair<std::string, std::string> pair = cdvide(str, ':');
	HttpHeader header;
	header.name = pair.first;
	header.value = strtrim(pair.second, " \t\r\n");
	return (header);
}

static TransferEncoding ParseTransferCoding(const std::string& token)
{
    const std::string coding = toUpper(token);
    if (coding == "CHUNKED")
        return CHUNKED;
    if (coding == "GZIP")
        return GZIP;
    if (coding == "COMPRESS")
        return COMPRESS;
    if (coding == "DEFLATE")
        return DEFLATE;
    return UNKNOWN;
}

void	HttpRequestsParser::ParseOneHeader(const std::string& line)
{
	if (line.empty())
		throw RequestHeaderException ();
	HttpHeader header = ParseHeaderOneLine(line);
	if (header.name.empty())
		throw RequestHeaderException ();
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
		if (header.value.find_first_not_of(NUMBER) != std::string::npos)
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

const HttpRequests& HttpRequestsParser::GetRequest() const 
{
	return (_httprequests);
}
