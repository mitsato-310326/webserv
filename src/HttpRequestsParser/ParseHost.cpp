#include "HttpRequestsParser.hpp"

#include <arpa/inet.h>

static bool IsPortValid(const std::string& port)
{
	return (StringPermition(port, NUMBER));
}

static bool IsIPv4Address(const std::string& address)
{
	std::vector<std::string> octets = csplit(address, '.');
	if (octets.size() != 4)
		return (false);
	for (std::vector<std::string>::const_iterator it = octets.begin();
		it != octets.end(); ++it)
	{
		if (!StringPermition(*it, NUMBER) || it->size() > 3 ||
			(it->size() > 1 && (*it)[0] == '0'))
			return (false);
		unsigned int value = 0;
		for (std::string::size_type i = 0; i < it->size(); ++i)
			value = value * 10 + static_cast<unsigned int>((*it)[i] - '0');
		if (value > 255)
			return (false);
	}
	return (true);
}

static bool IsNumericDotted(const std::string& host)
{
	if (host.find('.') == std::string::npos)
		return (false);
	for (std::string::size_type i = 0; i < host.size(); ++i)
	{
		if ((host[i] < '0' || host[i] > '9') && host[i] != '.')
			return (false);
	}
	return (true);
}

static bool IsDNSName(const std::string& host)
{
	if (host.empty())
		return (false);
	std::string name = host;
	if (name[name.size() - 1] == '.')
		name.erase(name.size() - 1);
	if (name.empty() || name.size() > 253)
		return (false);
	std::vector<std::string> labels = csplit(name, '.');
	for (std::vector<std::string>::const_iterator it = labels.begin();
		it != labels.end(); ++it)
	{
		if (it->empty() || it->size() > 63 ||
			!StringPermition(*it, ALNUM "-") ||
			it->at(0) == '-' || it->at(it->size() - 1) == '-')
			return (false);
	}
	return (true);
}

HostType HttpRequestsParser::ParseHost(const std::string& authority) const
{
	if (authority.empty())
		throw RequestHeaderException();

	std::string host;
	if (authority[0] == '[')
	{
		const std::string::size_type close = authority.find(']');
		if (close == std::string::npos || close == 1)
			throw RequestHeaderException();
		const std::string address = authority.substr(1, close - 1);
		struct in6_addr ipv6;
		if (inet_pton(AF_INET6, address.c_str(), &ipv6) != 1)
			throw RequestHeaderException();
		const std::string suffix = authority.substr(close + 1);
		if (!suffix.empty() &&
			(suffix[0] != ':' || !IsPortValid(suffix.substr(1))))
			throw RequestHeaderException();
		return (HOST_TYPE_IPV6);
	}

	const std::string::size_type colon = authority.find(':');
	if (colon != std::string::npos)
	{
		if (authority.find(':', colon + 1) != std::string::npos ||
			!IsPortValid(authority.substr(colon + 1)))
			throw RequestHeaderException();
		host = authority.substr(0, colon);
	}
	else
		host = authority;

	if (IsNumericDotted(host))
	{
		if (!IsIPv4Address(host))
			throw RequestHeaderException();
		return (HOST_TYPE_IPV4);
	}
	if (!IsDNSName(host))
		throw RequestHeaderException();
	return (HOST_TYPE_DNS);
}

void HttpRequestsParser::ParseHostValue(const std::string& value)
{
	if (!_httprequests.host.empty())
		throw RequestHeaderException();
	_httprequests.hostType = ParseHost(value);
	_httprequests.host = value;
}
