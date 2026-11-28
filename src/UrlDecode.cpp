#include "UrlDecode.hpp"
#include "StringUtils.hpp"

static unsigned int HexToDec(const std::string& text)
{
    unsigned int decodedByte;
    std::stringstream hexStream;
    hexStream << text;
    hexStream >> std::hex >> decodedByte;

    if (hexStream.fail() || !hexStream.eof())
        throw UrlDecodeException();
    return (decodedByte);
}

static std::string DecodeHex(const std::string &text)
{
    if (text.length() != 2)
        throw UrlDecodeException ();
    std::string allowed = ALNUM;
    if(text.find_first_not_of(allowed) != std::string::npos)
    {
        throw UrlDecodeException ();
    }
	char decimal = HexToDec(text);
    return (std::string(1, decimal));
}

std::string PercentDecode(const std::string &text)
{
    std::string decoded = text;
    for (std::string::size_type pos = decoded.find("%"); decoded.find("%", pos) != std::string::npos;)
    {
        std::string decodedByte = DecodeHex(decoded.substr(pos + 1, 2));
        decoded.replace(pos, 3, decodedByte);
        pos = decoded.find("%", pos + 1);
    }
    return (decoded);
}

const char *UrlDecodeException::what() const throw()
{
    return ("String: URL decode string must be written with hex characters");
}
