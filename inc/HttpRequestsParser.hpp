#ifndef HTTPREQUESTSPARSER_HPP
# define HTTPREQUESTSPARSER_HPP

# include "HttpRequests.hpp"

# ifndef MAXLINEBYTES
#  define MAXLINEBYTES 8192
# endif

class HttpRequestsParser
{
	public:
		HttpRequestsParser();
		HttpRequestsParser(const HttpRequestsParser&);
		HttpRequestsParser&	operator=(const HttpRequestsParser&);
		~HttpRequestsParser();
		bool	ParseFirstLine(const std::string& str);
		void	isLineTooLong(const std::string& str, std::string::size_type max);

		class LineTooLongException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

	private:
		HttpRequests			_httprequests;
		std::string::size_type	_cursor;
};

#endif
