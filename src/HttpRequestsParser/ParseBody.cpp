#include "HttpRequestsParser.hpp"

static bool IsChunkExtensionQuotedValue(const std::string& value)
{
	if (value.size() < 2 || value[0] != '"' || value[value.size() - 1] != '"')
		return (false);
	for (std::string::size_type i = 1; i < value.size() - 1; ++i)
	{
		const unsigned char ch = static_cast<unsigned char>(value[i]);
		if (ch == '\\')
		{
			if (++i >= value.size() - 1)
				return (false);
			const unsigned char escaped = static_cast<unsigned char>(value[i]);
			if (!(escaped == '\t' || escaped == ' ' ||
				(escaped >= 0x21 && escaped <= 0x7e) || escaped >= 0x80))
				return (false);
		}
		else if (!(ch == '\t' || ch == ' ' || ch == 0x21 ||
			(ch >= 0x23 && ch <= 0x5b) ||
			(ch >= 0x5d && ch <= 0x7e) || ch >= 0x80))
			return (false);
	}
	return (true);
}

static bool IsChunkExtensionsValid(const std::string& extensions)
{
	std::string::size_type cursor = 0;
	while (cursor < extensions.size())
	{
		if (extensions[cursor++] != ';')
			return (false);
		while (cursor < extensions.size() &&
			(extensions[cursor] == ' ' || extensions[cursor] == '\t'))
			++cursor;

		std::string::size_type itemStart = cursor;
		bool quoted = false;
		bool escaped = false;
		while (cursor < extensions.size())
		{
			const char ch = extensions[cursor];
			if (escaped)
				escaped = false;
			else if (quoted && ch == '\\')
				escaped = true;
			else if (ch == '"')
				quoted = !quoted;
			else if (!quoted && ch == ';')
				break;
			++cursor;
		}
		if (quoted || escaped)
			return (false);

		std::string item = strtrim(extensions.substr(itemStart, cursor - itemStart), " \t");
		if (item.empty())
			return (false);
		std::pair<std::string, std::string> pair = cdvide(item, '=');
		std::string name = strtrim(pair.first, " \t");
		if (!StringPermition(name, METHODPERMITION))
			return (false);
		if (item.find('=') != std::string::npos)
		{
			std::string value = strtrim(pair.second, " \t");
			if (value.empty() ||
				(!StringPermition(value, METHODPERMITION) &&
				 !IsChunkExtensionQuotedValue(value)))
				return (false);
		}
	}
	return (true);
}

void	HttpRequestsParser::ParseRequestBody(const std::string& str)
{
	if (_httprequests.contentLength >= 0 && TransferEncodingImplementationCheck())
		throw RequestHeaderException ();
	else if (_httprequests.contentLength >= 0)
		ParseContentLength(str);
	else if (TransferEncodingImplementationCheck())
			ParseTransferEncoding(str);
	else
		return ;
}

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
	std::string::size_type extensionStart = hex.find(';');
	std::string extensions;
	if (extensionStart != std::string::npos)
	{
		extensions = hex.substr(extensionStart);
		hex = hex.substr(0, extensionStart);
		if (!IsChunkExtensionsValid(extensions))
			throw RequestBodyException ();
	}
	errno = 0;
	char* end = NULL;
	if (!StringPermition(hex, HEX))
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
	if (chunkSize == 0)
	{
		ParseTrailer(text);
		return ;
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
	return (_httprequests.chunked);
}
