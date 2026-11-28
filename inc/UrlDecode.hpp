#ifndef URL_DECODE_HPP
# define URL_DECODE_HPP

# include <exception>
# include <string>

std::string PercentDecode(const std::string& text);

class UrlDecodeException : public std::exception
{
	public:
		virtual const char* what() const throw();
};

#endif
