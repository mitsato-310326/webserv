#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::parse(const std::string &filepath)
{
	std::ifstream	file(filepath.c_str());
	if (!file.is_open())
		throw ConfigParseException("Cannot open config file: " + filepath);

	std::string line;
	int brace_count = 0;

	while (std::getline(file, line))
	{
		// Remove comments
		size_t comment_pos = line.find('#');
		if (comment_pos != std::string::npos)
			line = line.substr(0, comment_pos);

		// Count braces
		for (size_t i = 0; i < line.length(); i++)
		{
			if (line[i] == '{')
				brace_count++;
			else if (line[i] == '}')
				brace_count--;
		}
	}

	file.close();

	if (brace_count != 0)
		throw ConfigParseException("Mismatched braces in config file");
}

const std::map<std::string, std::string> &ConfigParser::getServerConfig() const
{
	return serverConfig;
}

const std::vector<std::map<std::string, std::string> > &ConfigParser::getLocations() const
{
	return locations;
}

void ConfigParser::validateDirective(const std::string &directive, const std::string &value)
{
	if (directive.empty() || value.empty())
		throw ConfigParseException("Empty directive or value");
}

void ConfigParser::parseBlock(std::istringstream &stream, std::string &line)
{
	(void)stream;
	(void)line;
}
