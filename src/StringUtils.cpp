#include "../inc/StringUtils.hpp"

std::vector<std::string> csplit(const std::string& text, char c)
{
    std::vector<std::string> result;
    std::string::size_type cursor = 0;

    while (true)
    {
        std::string::size_type end = text.find(c, cursor);

        if (end == std::string::npos)
        {
            result.push_back(text.substr(cursor));
            break;
        }

        result.push_back(text.substr(cursor, end - cursor));
        cursor = end + 1;
    }
    return result;
}

std::vector<std::string> split(const std::string& text)
{
    std::istringstream			stream(text);
    std::vector<std::string>	words;
    std::string 				word;

    while (stream >> word)
        words.push_back(word);
    return words;
}

bool    StringPermition(const std::string& text, const std::string allowed)
{
    if (text.empty())
        return (false);
    if (text.find_first_not_of(allowed) != std::string::npos)
        return (false);
    return (true);
}

unsigned int    HexToDec(const std::string& text)
{
    unsigned int x;
    std::stringstream ss;
    ss << text;
    ss >> std::hex >> x;
    
    if (ss.fail() || !ss.eof())
        throw urlDecodeException();
    return (x);
}

std::string toString(char decimal)
{
    return (std::string(1, decimal));
}

std::string DecodeHex(const std::string &text)
{
    if (text.length() != 2)
        throw urlDecodeException ();
    std::string allowed = ALNUM;
    if(text.find_first_not_of(allowed) != std::string::npos)
    {
        throw urlDecodeException ();
    }
	char decimal = HexToDec(text);
    return (toString(decimal));
}

std::string DecodeString(const std::string &text)
{
    std::string decoded = text;
    for (std::string::size_type pos = decoded.find("%"); decoded.find("%", pos) != std::string::npos;)
    {
        std::string tmp = DecodeHex(decoded.substr(pos + 1, 2));
        decoded.replace(pos, 3, tmp);
        pos = decoded.find("%", pos + 1);
    }
    return (decoded);
}

std::string toUpper(const std::string &text)
{
	const char *ptr = text.c_str();
    std::string uppercase = "";
    for (int j = 0; ptr[j] != '\0'; j++)
	{
		char upper_char = static_cast<char>(std::toupper(ptr[j]));
		uppercase += toString(upper_char);
	}
    return (uppercase);
}

const char *urlDecodeException::what() const throw()
{
    return ("String: URL decode string must be written with hex characters");
}
