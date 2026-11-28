#ifndef HTTPREQUESTSPARSER_HPP
# define HTTPREQUESTSPARSER_HPP

# include "HttpRequests.hpp"
# include "StringUtils.hpp"

# include <iostream>
# include <string>
# include <cctype>



class HttpRequestsParser
{
	public:
		HttpRequestsParser();
		HttpRequestsParser(const HttpRequestsParser&);
		HttpRequestsParser&	operator=(const HttpRequestsParser&);
		~HttpRequestsParser();

		/* ParseUtils.cpp */
		std::string::size_type	LineLength(const std::string& str);
		std::string				ExtractLine(const std::string& str);
		void		            Parse(const std::string& buffer);

		/* ParseFirstLine.cpp */
		void		ParseFirstLine(const std::string& str);
		void		ParseMethod(const std::string& method);
		void		ParseUri(const std::string& target);
		std::string	ParsePath(const std::string& path);
		std::string	ParseOriginForm(const std::string& path);
		std::string	ParseAbsoluteForm(const std::string& path);
		std::string	ParseQuery(const std::string& query);
		void 		ParseHttp(const std::string& Http);

		/* ParseHeader.cpp */
		void		ParseOneHeader(const std::string& str);
		void		ParseHeader(const std::string& str);
		HttpHeader	ParseHeaderOneLine(const std::string& str);
		HostType	ParseHost(const std::string& authority) const;
		void		RequestsParser(const std::string& str);
		const HttpRequests& GetRequest() const;
		void		ParseTrailer(const std::string& text);

		/* Parsebody.cpp */
		void		ParseContentLength(const std::string& text);
		void		ParseTransferEncodingOneChunk(const std::string& text);
		void		ParseTransferEncoding(const std::string& text);
		bool		TransferEncodingImplementationCheck() const;
		void		ParseRequestBody(const std::string& str);

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
		
		class RequestBodyException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
	
		class RequestHeaderException : public std::exception
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
		void ParseHostValue(const std::string& value);
		void ParseContentLengthValue(const std::string& value);
		void ParseTransferEncodingValue(const std::string& value);

		HttpRequests			_httprequests;
		ParseResult				_result;
		ChunkParseStatus		_chunkStatus;
		std::string::size_type	_cursor;
};

#endif
