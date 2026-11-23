#include "../inc/HttpRequestsParser.hpp"

void	HttpRequestsParser::RequestsParser(const std::string& str)
{
	ParseFirstLine(str);
	ParseHeader(str);
}
