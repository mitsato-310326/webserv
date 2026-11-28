#include "HttpRequestsParser.hpp"

static bool IsPortValid(const std::string& port)
{
	return (ContainsOnly(port, NUMBER));
}

static bool IsIPv4Address(const std::string& address)
{
	std::vector<std::string> octets = csplit(address, '.');
	if (octets.size() != 4)
		return (false);
	for (std::vector<std::string>::const_iterator it = octets.begin();
		it != octets.end(); ++it)
	{
		if (!ContainsOnly(*it, NUMBER) || it->size() > 3 ||
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

// A dotted IPv4 tail occupies two IPv6 groups.
static bool CountIPv6Groups(const std::string& part, bool allowIPv4, std::size_t& count)
{
	if (part.empty())
		return true;
	const std::vector<std::string> groups = csplit(part, ':');
	for (std::size_t i = 0; i < groups.size(); ++i)
	{
		const std::string& group = groups[i];
		if (allowIPv4 && i + 1 == groups.size() && group.find('.') != std::string::npos)
		{
			if (!IsIPv4Address(group))
				return false;
			count += 2;
		}
		else
		{
			if (group.size() > 4 || !ContainsOnly(group, HEX))
				return false;
			++count;
		}
	}
	return true;
}

static bool IsIPv6Address(const std::string& address)
{
	const std::string::size_type compression = address.find("::");
	std::size_t groups = 0;
	if (compression == std::string::npos)
		return CountIPv6Groups(address, true, groups) && groups == 8;
	return CountIPv6Groups(address.substr(0, compression), false, groups) &&
		CountIPv6Groups(address.substr(compression + 2), true, groups) && groups < 8;
}

static bool IsNumericDotted(const std::string& host)
{
	return (host.find('.') != std::string::npos && ContainsOnly(host, NUMBER "."));
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
			!ContainsOnly(*it, ALNUM "-") ||
			it->at(0) == '-' || it->at(it->size() - 1) == '-')
			return (false);
	}
	return (true);
}

HostType HttpRequestsParser::ParseHost(const std::string& authority) const
{
	if (authority.empty())
		throw RequestHeaderException();

	if (authority[0] == '[')
	{
		const std::string::size_type close = authority.find(']');
		if (close == std::string::npos || close == 1)
			throw RequestHeaderException();
		const std::string address = authority.substr(1, close - 1);
		if (!IsIPv6Address(address))
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
	}
	const std::string host = authority.substr(0, colon);

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
	if (!_request.host.empty())
		throw RequestHeaderException();
	_request.hostType = ParseHost(value);
	_request.host = value;
}
