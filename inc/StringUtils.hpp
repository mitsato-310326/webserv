#ifndef STRING_UTILS_HPP
# define STRING_UTILS_HPP

# include <string>
# include <vector>
# include <sstream>
# include <iostream>

# define NUMBER "0123456789"
# define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz"
# define ALNUM ALPHABET"0123456789"
# define METHODPERMITION ALNUM"!#$%&'*+-.^_`|~"
# define PATHPERMITION ALNUM"-._~!$&'()*+,;=:@/%"
# define QUERYPERMITION PATHPERMITION"?"

std::vector<std::string>	split(const std::string& text);
bool					    StringPermition(const std::string& text, const std::string allowed);
unsigned int	   		 	HexToDec(const std::string& text);
std::string					DecodeHex(const std::string &text);
std::string 				DecodeString(const std::string &text);
std::string 				toString(char decimal);
std::string					toUpper(const std::string &text);
std::vector<std::string> 	csplit(const std::string& text, char c);

class urlDecodeException : public std::exception
{
	public:
		virtual const char* what() const throw();
};

#endif
