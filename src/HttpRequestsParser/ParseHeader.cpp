#include "HttpRequestsParser.hpp"

void HttpRequestsParser::ParseHeader(const std::string& buffer)
{
	while (true)
	{
		const std::string line = ExtractLine(buffer);
		if (_result.status == NeedMoreData)
			return;
		if (line == HEADERSEND)
		{
			if (_request.host.empty())
				throw RequestHeaderException();
			return;
		}
		const HttpHeader header = ParseHeaderLine(line);
		ApplyHeaderValue(header);
		_request.headers.push_back(header);
	}
}

HttpHeader HttpRequestsParser::ParseHeaderLine(const std::string& line)
{
	const std::pair<std::string, std::string> field =
		SplitAtFirst(StripTrailingCRLF(line), ':');
	if (!ContainsOnly(field.first, TOKEN_CHARS))
		throw RequestHeaderException();
	if (field.second.find_first_of(FORBIDDEN_HEADER_VALUE_CHARS, 0,
			sizeof(FORBIDDEN_HEADER_VALUE_CHARS) - 1) != std::string::npos)
		throw RequestHeaderException();

	HttpHeader header;
	header.name = toUpper(field.first);
	header.value = strtrim(field.second, " \t");
	return (header);
}

void HttpRequestsParser::ApplyHeaderValue(const HttpHeader& header)
{
	if (header.name == HOST)
		ParseHostValue(header.value);
	else if (header.name == CONTENTLENGTH)
		ParseContentLengthValue(header.value);
	else if (header.name == TRANSFERENCODING)
		ParseTransferEncodingValue(header.value);
}

void HttpRequestsParser::ParseTrailer(const std::string& buffer)
{
	while (true)
	{
		const std::string line = ExtractLine(buffer);
		if (_result.status == NeedMoreData || line == CLEF)
			return;
		ParseHeaderLine(line);
	}
}

const HttpRequests& HttpRequestsParser::GetRequest() const
{
	return (_request);
}
