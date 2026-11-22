#ifndef HTTPREQUESTSPARSER_HPP
# define HTTPREQUESTSPARSER_HPP

# include "HttpRequests.hpp"
# include "StringUtils.hpp"

# include <iostream>
# include <string>

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
		void		ParseFirstLine(const std::string& str);
		void		isLineTooLong(const std::string& str, std::string::size_type max);
		void		ParseMethod(const std::string& method);
		void		ParseUri(const std::string& target);
		std::string	ParsePath(const std::string& path);
		std::string	ParseQuery(const std::string& query);
		void 		ParseHttp(const std::string& Http);

		class LineTooLongException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
	
		class RequestFirstLineException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		
		class InternalSystemException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

	private:
		HttpRequests			_httprequests;
		std::string::size_type	_cursor;
};

#endif
