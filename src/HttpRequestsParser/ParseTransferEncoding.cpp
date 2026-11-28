#include "HttpRequestsParser.hpp"

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

void HttpRequestsParser::ParseTransferEncodingValue(const std::string& value)
{
	const std::vector<std::string> codings = csplit(value, ',');
	for (std::size_t i = 0; i < codings.size(); ++i)
	{
		const std::vector<std::string> tokens = split(codings[i]);
		if (tokens.size() != 1)
			throw RequestHeaderException();
		const TransferEncoding coding = ParseTransferCoding(tokens[0]);
		if (coding == CHUNKED && _request.chunked)
			throw RequestHeaderException();
		_request.transferEncoding.push_back(coding);
		if (coding == CHUNKED)
			_request.chunked = true;
	}
}
