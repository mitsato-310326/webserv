#include "../inc/HttpRequestsParser.hpp"

void	HttpRequestsParser::ParseContentLength(const std::string& text)
{
	std::string::size_type len = _httprequests.contentLength;
	std::string::size_type dist = len - _httprequests.body.size();
	_httprequests.body += text.substr(_cursor, dist);
	std::string::size_type size = _httprequests.body.size();
	if (size < len)
	throw RequestBodyException ();
	else if(size > len)
	throw RequestBodyException ();
	return ;
}

void		HttpRequestsParser::ParseTransferEncodingOneChunk(const std::string& text)
{
	std::string hex = ExtractLine(text);
	hex = hex.substr(0, hex.length() -2);
	hex = hex.substr(0, hex.find(';'));
	errno = 0;
	char* end = NULL;
	if (hex.find_first_not_of(HEX) != std::string::npos)
	throw RequestBodyException ();
	const char* begin = hex.c_str();
	const unsigned long chunkSize = std::strtoul(begin, &end, 16);
	if (errno == ERANGE)
	throw RequestBodyException ();
	_httprequests.chunkBytesRemaining = chunkSize;
	_httprequests.chunkBytes = chunkSize;
	std::string append = text.substr(_cursor, chunkSize);
	_httprequests.body += append;
	_cursor += append.size();
	if (append.size() > _httprequests.chunkBytesRemaining)
	throw RequestBodyException ();
	else if (append.size() < _httprequests.chunkBytesRemaining)
	{
		_httprequests.chunkBytesRemaining -= append.size();
		throw RequestBodyException ();
	}
	if (text.substr(_cursor, 2) != "\r\n")
	throw RequestBodyException ();
	_cursor += 2;
	return ;
}

void		HttpRequestsParser::ParseTransferEncoding(const std::string& text)
{
	while (_httprequests.chunkBytes > 0)
	ParseTransferEncodingOneChunk(text);
}

bool		HttpRequestsParser::TransferEncodingImplementationCheck() const
{
	std::vector<TransferEncoding> te = _httprequests.transferEncoding;
	if (te.empty())
		return (false);
	for (std::vector<TransferEncoding>::iterator it = te.begin(); it < te.end(); it++) 
	{
		if (*it == GZIP)
		throw RequestBodyException ();
		if (*it == COMPRESS)
		throw RequestBodyException ();
		if (*it == DEFLATE)
		throw RequestBodyException ();
		if (*it == UNKNOWN)
		throw RequestBodyException ();
	} 
	std::vector<TransferEncoding>::iterator it = std::find(te.begin(),
	te.end(), CHUNKED);
	if (it != _httprequests.transferEncoding.end())
	return (true);
	return (false);
}
