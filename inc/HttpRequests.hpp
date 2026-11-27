#ifndef HTTP_REQUESTS_HPP
# define HTTP_REQUESTS_HPP

# include <string>
# include <vector>
# define MAXLINEBYTES 8192
# define MAXCONTENTLENGTH 1024 * 1024
# define UNDIFINED (-1)
# define HOST "HOST"
# define CONTENTLENGTH "CONTENT-LENGTH"
# define TRANSFERENCODING "TRANSFER-ENCODING"

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

enum HostType
{
	HOST_TYPE_UNKNOWN,
	HOST_TYPE_DNS,
	HOST_TYPE_IPV4,
	HOST_TYPE_IPV6
};

enum ParseStatus {
	NeedMoreData,
	Complete,
	ParseError
};

struct ParseResult {
    ParseStatus status;
    std::size_t consumed;  // Complete のとき、先頭の1リクエストが占めたバイト数
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
	HostType						hostType;
	ssize_t							contentLength;
	std::vector<TransferEncoding>	transferEncoding;
	bool							chunked;
	std::string::size_type			chunkBytesRemaining;
	std::string::size_type			chunkBytes;

	HttpRequests() : versionMajor(0), versionMinor(0),
		hostType(HOST_TYPE_UNKNOWN), contentLength(UNDIFINED), chunked(false) {}
};

#endif
