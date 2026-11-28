#include "HttpRequestsParser.hpp"

static bool IsChunkExtensionsValid(const std::string& extensions);
static bool IsChunkExtensionQuotedValue(const std::string& value);

void	HttpRequestsParser::ParseRequestBody(const std::string& str)
{
	const bool hasContentLength = _request.contentLength >= 0;
	if (hasContentLength && !_request.transferEncoding.empty())
		throw RequestHeaderException();
	if (hasContentLength)
		ParseContentLength(str);
	else if (TransferEncodingImplementationCheck())
		ParseTransferEncoding(str);
}

bool		HttpRequestsParser::TransferEncodingImplementationCheck() const
{
	const std::vector<TransferEncoding>& transferCodings = _request.transferEncoding;
	if (transferCodings.empty())
		return (false);
	if (transferCodings.back() != CHUNKED)
		throw RequestHeaderException();
	for (std::vector<TransferEncoding>::const_iterator it = transferCodings.begin(); it < transferCodings.end(); it++)
	{
		if (*it == GZIP || *it == COMPRESS || *it == DEFLATE || *it == UNKNOWN)
			throw RequestParseException(UnsupportedTransferEncoding);
	}
	return (_request.chunked);
}

void	HttpRequestsParser::ParseContentLength(const std::string& text)
{
	std::string::size_type contentLength = _request.contentLength;
	std::string::size_type remainingBytes = contentLength - _request.body.size();
	const std::string incomingBodyBytes = text.substr(_cursor, remainingBytes);
	_request.body += incomingBodyBytes;
	const std::string::size_type accumulatedBodySize = _request.body.size();
	_cursor += incomingBodyBytes.size();
	if (accumulatedBodySize < contentLength)
		_result.status = NeedMoreData;
	return ;
}

void HttpRequestsParser::ParseTransferEncoding(const std::string& text)
{
	while (_chunkStatus != ChunkComplete)
	{
		ChunkParseStatus nextStatus = _chunkStatus;
		switch (_chunkStatus)
		{
			case ChunkNotStarted:
			case ChunkBlockRemain:
				ParseChunkSizeLine(text);
				nextStatus = _request.chunkBytes == 0 ? ChunkTrailerRemain : ChunkBytesRemain;
				break;
			case ChunkBytesRemain:
				ParseChunkData(text);
				nextStatus = ChunkEnterRemain;
				break;
			case ChunkEnterRemain:
				ParseChunkDelimiter(text);
				nextStatus = ChunkBlockRemain;
				break;
			case ChunkTrailerRemain:
				ParseTrailer(text);
				nextStatus = ChunkComplete;
				break;
			case ChunkComplete:
				return;
		}
		if (_result.status != ParseLoad)
			return;
		_chunkStatus = nextStatus;
	}
}

void HttpRequestsParser::ParseChunkSizeLine(const std::string& text)
{
	std::string sizeLine = ExtractLine(text);
	if (_result.status == NeedMoreData)
		return;
	sizeLine = StripTrailingCRLF(sizeLine);
	const std::string::size_type extensionStart = sizeLine.find(';');
	if (extensionStart != std::string::npos)
	{
		if (!IsChunkExtensionsValid(sizeLine.substr(extensionStart)))
			throw RequestBodyException();
		sizeLine.erase(extensionStart);
	}
	if (!ContainsOnly(sizeLine, HEX))
		throw RequestBodyException();
	errno = 0;
	const unsigned long chunkSize = std::strtoul(sizeLine.c_str(), NULL, 16);
	if (errno == ERANGE || chunkSize > MAXCONTENTLENGTH - _request.body.size())
		throw RequestParseException(BodyTooLarge);
	_request.chunkBytesRemaining = chunkSize;
	_request.chunkBytes = chunkSize;
}

void HttpRequestsParser::ParseChunkData(const std::string& text)
{
	const std::string chunkData = text.substr(_cursor, _request.chunkBytesRemaining);
	_request.body += chunkData;
	_cursor += chunkData.size();
	_request.chunkBytesRemaining -= chunkData.size();
	if (_request.chunkBytesRemaining != 0)
		_result.status = NeedMoreData;
}

void HttpRequestsParser::ParseChunkDelimiter(const std::string& text)
{
	const std::string::size_type delimiterLength = sizeof(CLEF) - 1;
	if (text.size() - _cursor < delimiterLength)
	{
		_result.status = NeedMoreData;
		return;
	}
	if (text.compare(_cursor, delimiterLength, CLEF) != 0)
		throw RequestBodyException();
	_cursor += delimiterLength;
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
		std::pair<std::string, std::string> pair = SplitAtFirst(item, '=');
		std::string name = strtrim(pair.first, " \t");
		if (!ContainsOnly(name, TOKEN_CHARS))
			return (false);
		if (item.find('=') != std::string::npos)
		{
			std::string value = strtrim(pair.second, " \t");
			if (value.empty() ||
				(!ContainsOnly(value, TOKEN_CHARS) &&
				 !IsChunkExtensionQuotedValue(value)))
				return (false);
		}
	}
	return (true);
}

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
