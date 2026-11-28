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
		~HttpRequestsParser();

		void Parse(const std::string& buffer);
		// Resume a parser or its copy with the same cumulative buffer until completion.
		// Copies include the request, current phase, cursor and result.
		ParseResult RequestsParser(const std::string& str);
		const HttpRequests& GetRequest() const;

	private:
		/* ParseUtils.cpp */
		std::string::size_type	LineLength(const std::string& str);
		std::string				ExtractLine(const std::string& str);
		static std::string StripTrailingCRLF(const std::string& line);

		struct ParsedTarget;
		struct HttpVersion;

		/* ParseRequestLine.cpp */
		void ParseRequestLine(const std::string& buffer);
		static std::vector<std::string> SplitRequestLine(const std::string& line);
		static std::string ParseMethod(const std::string& method);
		ParsedTarget ParseRequestTarget(const std::string& target) const;
		ParsedTarget ParseAbsoluteForm(const std::string& path) const;
		static std::string ParseOriginForm(const std::string& path);
		static std::string ParseQuery(const std::string& query);
		static HttpVersion ParseHttpVersion(const std::string& version);
		static std::string DecodeRequestTargetPart(const std::string& text);

		/* ParseHeader.cpp */
		void ParseHeader(const std::string& buffer);
		static HttpHeader ParseHeaderLine(const std::string& line);
		void ApplyHeaderValue(const HttpHeader& header);
		void		ParseTrailer(const std::string& text);
		HostType	ParseHost(const std::string& authority) const;

		/* ParseBody.cpp */
		void		ParseRequestBody(const std::string& str);
		bool		TransferEncodingImplementationCheck() const;
		void		ParseContentLength(const std::string& text);
		void		ParseTransferEncoding(const std::string& text);
		void ParseChunkSizeLine(const std::string& text);
		void ParseChunkData(const std::string& text);
		void ParseChunkDelimiter(const std::string& text);

		/* Exceptions */
		class RequestParseException : public std::exception
		{
			public:
				explicit RequestParseException(ParseErrorReason reason = InvalidSyntax)
					: _reason(reason) {}
				ParseErrorReason Reason() const { return _reason; }

			private:
				ParseErrorReason _reason;
		};

		class LineTooLongException : public RequestParseException
		{
			public:
				LineTooLongException() : RequestParseException(LineTooLong) {}
				virtual const char* what() const throw();
		};
	
		class RequestLineException : public RequestParseException
		{
			public:
				virtual const char* what() const throw();
		};
		
		class RequestBodyException : public RequestParseException
		{
			public:
				virtual const char* what() const throw();
		};
	
		class RequestHeaderException : public RequestParseException
		{
			public:
				virtual const char* what() const throw();
		};
		
		class InternalSystemException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		enum ParsePhase { ReadRequestLine, ReadHeaders, ReadBody };

		ParsePhase _phase;
		void ParseHostValue(const std::string& value);
		void ParseContentLengthValue(const std::string& value);
		void ParseTransferEncodingValue(const std::string& value);

		HttpRequests			_request;
		ParseResult				_result;
		ChunkParseStatus		_chunkStatus;
		std::string::size_type	_cursor;
};

#endif
