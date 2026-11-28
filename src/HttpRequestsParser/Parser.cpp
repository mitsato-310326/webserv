#include "HttpRequestsParser.hpp"

ParseResult HttpRequestsParser::RequestsParser(const std::string& str)
{
	if (_result.status == Complete || _result.status == ParseError)
		return (_result);
	_result.status = ParseLoad;
	try
	{
		switch (_phase)
		{
			case ReadRequestLine:
			{
				ParseRequestLine(str);
				if (_result.status == NeedMoreData)
					return (_result);
				_phase = ReadHeaders;
			}
			case ReadHeaders:
			{
				ParseHeader(str);
				if (_result.status == NeedMoreData)
					return (_result);
				_phase = ReadBody;
			}
			case ReadBody:
			{
				ParseRequestBody(str);
				if (_result.status != ParseLoad)
					return (_result);
				_result.status = Complete;
				_result.consumed = _cursor;
				break;
			}
		}
		return (_result);
	}
	catch (const RequestParseException& error)
	{
		_result.status = ParseError;
		_result.errorReason = error.Reason();
		return (_result);
	}
}
