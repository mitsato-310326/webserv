#include "../inc/HttpRequestsParser.hpp"

const char* HttpRequestsParser::LineTooLongException::what() const throw()
{
    return ("HttpRequestsParser: HTTP request line is too long");
}

const char* HttpRequestsParser::RequestFirstLineException::what() const throw()
{
    return ("HttpRequestsParser: Invalid HTTP request line");
}

const char* HttpRequestsParser::RequestHeaderException::what() const throw()
{
    return ("HttpRequestsParser: Invalid HTTP request header");
}

const char* HttpRequestsParser::InternalSystemException::what() const throw()
{
    return ("HttpRequestsParser: Invalid internal parser state");
}
