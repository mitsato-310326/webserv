#ifndef HTTPREQUESTSPARSER_HPP
# define HTTPREQUESTSPARSER_HPP

# include "HttpRequests.hpp"
# include "StringUtils.hpp"

# include <iostream>
# include <string>
# include <cctype>

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

		/* ParseFirstLine.cpp */
		void		ParseFirstLine(const std::string& str);
		void		isLineTooLong(const std::string& str, std::string::size_type max);
		void		ParseMethod(const std::string& method);
		void		ParseUri(const std::string& target);
		std::string	ParsePath(const std::string& path);
		std::string	ParseOriginForm(const std::string& path);
		std::string	ParseAbsoluteForm(const std::string& path);
		std::string	ParseQuery(const std::string& query);
		void 		ParseHttp(const std::string& Http);

		/* ParseHeader.cpp */
		void	ParseHeader(const std::string& str);
		void	ParseHeaderOneLine(const std::string& str);

		/* Exceptions */
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
