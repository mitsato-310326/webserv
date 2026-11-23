#include "../inc/HttpRequestsParser.hpp"

HttpHeader	HttpRequestsParser::ParseHeaderOneLine(const std::string& str)
{
	std::vector<std::string> vector = csplit(str, ':');
	if (vector.size() != 2)
		throw RequestHeaderException ();
	HttpHeader header;
	header.name = vector[0];
	header.value = vector[1];
	return (header);
}

void	HttpRequestsParser::ParseHeader(const std::string& str)
{

}