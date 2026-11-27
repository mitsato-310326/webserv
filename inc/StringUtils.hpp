#ifndef STRING_UTILS_HPP
# define STRING_UTILS_HPP

# include <string>
# include <vector>
# include <sstream>
# include <iostream>

# define NUMBER "0123456789"
# define HEX NUMBER"ABCDEF""abcdef"
# define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz"
# define ALNUM ALPHABET"0123456789"
# define METHODPERMITION ALNUM"!#$%&'*+-.^_`|~"
# define PATHPERMITION ALNUM"-._~!$&'()*+,;=:@/%"
# define QUERYPERMITION PATHPERMITION"?"
# define OPERATORCHAR "\x01\x02\x03\x04\x05\x06\x07\x08\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F"

std::vector<std::string>	split(const std::string& text);
bool					    StringPermition(const std::string& text, const std::string allowed);
unsigned int	   		 	HexToDec(const std::string& text);
std::string					DecodeHex(const std::string &text);
std::string 				DecodeString(const std::string &text);
std::string 				toString(char decimal);
std::string					toUpper(const std::string &text);
std::vector<std::string> 	csplit(const std::string& text, char c);
std::pair<std::string, std::string> cdvide(const std::string& text, char c);
std::string					 strtrim(const std::string& text, const std::string& charset);

class urlDecodeException : public std::exception
{
	public:
		virtual const char* what() const throw();
};

#endif
