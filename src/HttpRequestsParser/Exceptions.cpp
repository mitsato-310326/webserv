#include "HttpRequestsParser.hpp"

const char* HttpRequestsParser::LineTooLongException::what() const throw()
{
    return ("HttpRequestsParser: HTTP request line is too long");
}

const char* HttpRequestsParser::RequestLineException::what() const throw()
{
    return ("HttpRequestsParser: Invalid HTTP request line");
}

const char* HttpRequestsParser::RequestBodyException::what() const throw()
{
    return ("HttpRequestsParser: Invalid HTTP request Body");
}

const char* HttpRequestsParser::RequestHeaderException::what() const throw()
{
    return ("HttpRequestsParser: Invalid HTTP request header");
}

const char* HttpRequestsParser::InternalSystemException::what() const throw()
{
    return ("HttpRequestsParser: Invalid internal parser state");
}
