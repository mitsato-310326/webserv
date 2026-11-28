#include "HttpRequestsParser.hpp"

void	HttpRequestsParser::RequestsParser(const std::string& str)
{
	_result.status = ParseLoad;
	if (_result.nstatus == First)
	{
		ParseFirstLine(str);
		if (_result.status == ParseLoad)
			_result.nstatus = Header;
	}
	if (_result.nstatus == Header)
	{
		ParseHeader(str);
		if (_result.status == ParseLoad)
		_result.nstatus = Body;	
	}
	if (_result.nstatus == Body)
		ParseRequestBody(str);
}
