#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

class ConfigParser
{
	private:
		std::map<std::string, std::string> serverConfig;
		std::vector<std::map<std::string, std::string> > locations;

		void validateDirective(const std::string &directive, const std::string &value);
		void parseBlock(std::istringstream &stream, std::string &line);

	public:
		ConfigParser();
		~ConfigParser();

		void parse(const std::string &filepath);
		const std::map<std::string, std::string> &getServerConfig() const;
		const std::vector<std::map<std::string, std::string> > &getLocations() const;
};

class ConfigParseException : public std::exception
{
	private:
		std::string message;
	public:
		ConfigParseException(const std::string &msg) : message(msg) {}
		virtual const char *what() const throw() { return message.c_str(); }
		virtual ~ConfigParseException() throw() {}
};

#endif
