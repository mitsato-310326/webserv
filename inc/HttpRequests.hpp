#ifndef HTTP_REQUESTS_HPP
# define HTTP_REQUESTS_HPP

# include <string>
# include <vector>


struct HttpHeader
{
	std::string name;
	std::string value;
};

struct HttpRequests
{
	std::string             method;
	unsigned int            versionMajor;
	unsigned int            versionMinor;
	std::string             target;   // 分割・デコード前の要求先
	std::string             path;     // クエリを分離した、デコード前のパス
	std::string             query;    // '?' を除いた、デコード前のクエリ
	std::vector<HttpHeader> headers;  // Host・Content-Length・Transfer-Encodingなど。重複も保持
	std::string             body;     // chunkedの場合はデコード後。NULも含めて保持

	HttpRequests() : versionMajor(0), versionMinor(0) {}
};

#endif
