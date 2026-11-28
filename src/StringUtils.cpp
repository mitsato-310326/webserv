#include "../inc/StringUtils.hpp"

std::string strtrim(const std::string& text, const std::string& charset)
{
    std::string::size_type start = text.find_first_not_of(charset);
    if (start == std::string::npos)
        return ("");
    std::string::size_type last = text.find_last_not_of(charset);
    return (text.substr(start, last - start + 1));
}

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

std::pair<std::string, std::string> SplitAtFirst(const std::string& text, char c)
{
    std::string::size_type end = text.find(c, 0);
    if (end == std::string::npos)
        return (std::make_pair("", ""));
    std::string lhs = text.substr(0, end);
    std::string rhs = text.substr(end + 1, std::string::npos);
    return (std::make_pair(lhs, rhs));
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

bool    ContainsOnly(const std::string& text, const std::string allowed)
{
    if (text.empty())
        return (false);
    if (text.find_first_not_of(allowed) != std::string::npos)
        return (false);
    return (true);
}

std::string toUpper(const std::string &text)
{
    std::string uppercase = "";
    for (std::string::size_type j = 0; j < text.size(); ++j)
	{
		uppercase += static_cast<char>(std::toupper(static_cast<unsigned char>(text[j])));
	}
    return (uppercase);
}
