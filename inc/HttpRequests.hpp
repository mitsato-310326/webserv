#ifndef HTTP_REQUESTS_HPP
# define HTTP_REQUESTS_HPP

# include <string>
# include <vector>
# define MAXLINEBYTES 8192
# define MAXCONTENTLENGTH 1024 * 1024
# define HOST "Host"
# define CONTENTLENGTH "Content-Length"
# define TRANSFERENCODING "Transfer-Encoding"

struct HttpHeader
{
	std::string name;
	std::string value;
};

enum TransferEncoding
{	
	CHUNKED,
	GZIP,
	COMPRESS,
	DEFLATE,
	UNKNOWN
};

struct HttpRequests
{
	std::string             		method;
	unsigned int            		versionMajor;
	unsigned int            		versionMinor;
	std::string             		target;   // 分割・デコード前の要求先
	std::string             		path;     // クエリを分離した、デコード前のパス
	std::string            			query;    // '?' を除いた、デコード前のクエリ
	std::vector<HttpHeader> 		headers;  // Host・Content-Length・Transfer-Encodingなど。重複も保持
	std::string             		body;     // chunkedの場合はデコード後。NULも含めて保持

	std::string						absProtocol;
	std::string						absHost;
	std::string						absPort;

	std::string						host;
	ssize_t							contentLength;
	std::vector<TransferEncoding>	transferEncoding;

	HttpRequests() : versionMajor(0), versionMinor(0) {}
};

#endif
