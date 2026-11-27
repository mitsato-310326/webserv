#include "HttpRequestsParser.hpp"
#include <iostream>

void HttpRequestsParser::Parse(const std::string& buffer)
{
	std::clog << "[parser mock] buffer: " << buffer.size() << " bytes\n";
}
