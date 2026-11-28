#include "HttpRequestsParser.hpp"

HttpRequestsParser::HttpRequestsParser()
{
	_request.versionMajor = 0;
	_request.versionMinor = 0;
	_request.chunkBytesRemaining = 0;
	_request.chunkBytes = std::string::npos;
	_request.transferEncoding.clear();
	_result.status = ParseLoad;
	_result.errorReason = NoParseError;
	_phase = ReadRequestLine;
	_result.consumed = 0;
	_chunkStatus = ChunkNotStarted;
	_cursor = 0;
}

HttpRequestsParser::~HttpRequestsParser()
{
}

std::string::size_type	HttpRequestsParser::LineLength(const std::string& str)
{
	const std::string::size_type newline = str.find('\n', _cursor);
	if (newline == std::string::npos)
	{
		const std::string::size_type pending = str.size() - _cursor;
		const bool endsWithCR = pending != 0 && str[str.size() - 1] == '\r';
		const std::string::size_type terminatorBytes = endsWithCR ? 1 : 2;
		if (pending > MAXLINEBYTES - terminatorBytes)
			throw LineTooLongException();
		return (std::string::npos);
	}
	if (newline == _cursor || str[newline - 1] != '\r')
		throw RequestParseException();
	const std::string::size_type length = newline - _cursor + 1;
	if (length > MAXLINEBYTES)
		throw LineTooLongException();
	return (length);
}

/* byte が途中の時、この関数内でループ待ち処理を行う想定。 */
std::string	HttpRequestsParser::ExtractLine(const std::string &str)
{
	std::string::size_type len =  LineLength(str);
	if (len == std::string::npos)
	{
		_result.status = NeedMoreData;
		return ("");
	}
	std::string line = str.substr(_cursor, len);
	_cursor += len;
	return (line);
}

std::string HttpRequestsParser::StripTrailingCRLF(const std::string& line)
{
	const std::string::size_type crlfLength = sizeof(CLEF) - 1;
	if (line.size() < crlfLength ||
		line.compare(line.size() - crlfLength, crlfLength, CLEF) != 0)
		return (line);
	return (line.substr(0, line.size() - crlfLength));
}
