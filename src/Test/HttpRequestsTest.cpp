#include "TestScript.hpp"
#include "HttpRequestsParser.hpp"

static bool ReadTestFile(std::string path, std::string &text)
{
	std::ifstream	inFile(path.c_str());
	if (!inFile.is_open())
		return (false);
	text.assign(std::istreambuf_iterator<char>(inFile),std::istreambuf_iterator<char>());
	inFile.close();
	return (true);
}

static void CheckFixtureHeader(const HttpRequests& request, const std::string& sample)
{
	const std::string file = sample.substr(sample.find_last_of('/') + 1);
	if (file == "header-port.http")
	{
		check(file + " stores Host with its port", request.host == "example.test:8080");
		check(file + " classifies Host as DNS", request.hostType == HOST_TYPE_DNS);
	}
	else if (file == "header-ipv4.http")
		check(file + " classifies Host as IPv4", request.hostType == HOST_TYPE_IPV4);
	else if (file == "header-ipv6.http")
	{
		check(file + " stores IPv6 Host with its port", request.host == "[::1]:8080");
		check(file + " classifies Host as IPv6", request.hostType == HOST_TYPE_IPV6);
	}
	else if (file == "header-colon-value.http")
	{
		std::vector<HttpHeader>::const_iterator header = request.headers.begin();
		while (header != request.headers.end() && header->name != "X-ITEM")
			++header;
		check(file + " stores X-ITEM = a:b:c",
			header != request.headers.end() && header->value == "a:b:c");
	}
}

static bool HasHeader(const HttpRequests& request, const std::string& name,
	const std::string& value)
{
	for (std::vector<HttpHeader>::const_iterator it = request.headers.begin();
		it != request.headers.end(); ++it)
	{
		if (it->name == name && it->value == value)
			return true;
	}
	return false;
}

static void ParseRequestTest(const std::string &text, const std::string &sample)
{
	HttpRequestsParser parser;
	const bool isPolicy = sample.find("/policy/") != std::string::npos;
	const std::string file = sample.substr(sample.find_last_of('/') + 1);
	const bool expectsBody = isPolicy && file == "expect-continue.http";
	const bool policyReject = isPolicy && (
		file == "lf-only.http" || file == "multiple-spaces.http" ||
		file == "tab-separator.http" || file == "obs-fold.http" ||
		file == "cl-duplicate-equal.http" || file == "cl-list-equal.http" ||
		file == "te-and-cl.http" || file == "huge-cl.http" ||
		file == "huge-chunk.http" || file == "chunk-parameter.http" ||
		file == "empty-host.http" || file == "long-target.http");
	const bool isInvalid = sample.find("/invalid/") != std::string::npos;
	const bool isIncomplete = sample.find("/incomplete/") != std::string::npos;
	const bool isUnsupported = sample.find("/unsupported/") != std::string::npos;
	const ParseResult result = parser.RequestsParser(text);
	const HttpRequests& request = parser.GetRequest();
	if (file == "content-length-over-body-limit.http" ||
		file == "chunk-size-over-body-limit.http")
		check(sample + ": body too large",
			result.status == ParseError && result.errorReason == BodyTooLarge);
	else if (file == "header-line-over-limit.http")
		check(sample + ": line too long",
			result.status == ParseError && result.errorReason == LineTooLong);
	else if (file == "gzip-chunked.http")
		check(sample + ": unsupported transfer encoding",
			result.status == ParseError && result.errorReason == UnsupportedTransferEncoding);
	else if (file == "cl-empty.http")
		check(sample + ": invalid syntax",
			result.status == ParseError && result.errorReason == InvalidSyntax);
	else if (policyReject || isInvalid)
		check(sample + ": rejected as expected", result.status == ParseError);
	else if (expectsBody)
		check(sample + ": headers parsed, body pending",
			result.status == NeedMoreData && result.consumed == 0 &&
			request.contentLength == 3 && request.body.empty() &&
			HasHeader(request, "EXPECT", "100-continue"));
	else if (isIncomplete)
		check(sample + ": needs more data",
			result.status == NeedMoreData && result.consumed == 0);
	else if (isUnsupported)
		check(sample + ": complete or unsupported",
			result.status == Complete || result.status == ParseError);
	else if (isPolicy && (file == "trailer-unknown.http" || file == "trailer-content-length.http"))
		check(sample + ": trailer discarded", result.status == Complete &&
			result.consumed == text.size() && request.body.empty() &&
			request.contentLength == UNDEFINED && request.headers.size() == 2);
	else
	{
		check(sample, result.status == Complete);
		CheckFixtureHeader(request, sample);
	}
}

static bool ParsesByteByByte(const std::string& request,
	const std::string& expectedBody)
{
	HttpRequestsParser parser;
	for (std::size_t size = 1; size <= request.size(); ++size)
	{
		const ParseResult result = parser.RequestsParser(request.substr(0, size));
		const bool complete = size == request.size();
		if (result.status != (complete ? Complete : NeedMoreData) ||
			result.consumed != (complete ? size : 0))
		{
			std::cout << "    Unexpected parse result after byte " << size << "\n";
			return (false);
		}
	}
	return (parser.GetRequest().body == expectedBody);
}

static void TestFragmentedRequests()
{
	title("Fragmented requests");
	const std::string prefix = "POST / HTTP/1.1\r\nHost: example.test\r\n";
	const std::string chunked = prefix +
		"Transfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n";
	const std::string trailer = chunked + "X-Note: example\r\n\r\n";
	check("Content-Length request one byte at a time",
		ParsesByteByByte(prefix + "Content-Length: 5\r\n\r\nhello", "hello"));
	check("Chunked request one byte at a time",
		ParsesByteByByte(chunked + "\r\n", "hello"));
	check("Chunked request with trailer one byte at a time",
		ParsesByteByByte(trailer, "hello"));

	HttpRequestsParser parser;
	const ParseResult result = parser.RequestsParser(
		trailer + "GET / HTTP/1.1\r\nHost: example.test\r\n\r\n");
	check("Consumed stops before the next request",
		result.status == Complete && result.consumed == trailer.size());
}

static void TestCopyDuringRequest()
{
	const std::string partial = "POST /a%20b?q=c%20d HTTP/1.1\r\n"
		"Host: [::1]\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhe";
	HttpRequestsParser original;
	const ParseResult pending = original.RequestsParser(partial);
	HttpRequestsParser copied(original);
	HttpRequestsParser assigned;
	assigned = original;

	const std::string complete = partial + "llo\r\n0\r\n\r\n";
	const ParseResult copiedResult = copied.RequestsParser(complete);
	const ParseResult assignedResult = assigned.RequestsParser(complete);
	const HttpRequests& request = copied.GetRequest();
	check("Copied and assigned parsers resume independently from the same buffer",
		pending.status == NeedMoreData && pending.errorReason == NoParseError &&
		copiedResult.status == Complete && assignedResult.status == Complete &&
		copiedResult.consumed == complete.size() && assignedResult.consumed == complete.size() &&
		copiedResult.errorReason == NoParseError && request.body == "hello" &&
		assigned.GetRequest().body == "hello" && original.GetRequest().body == "he" &&
		request.hostType == HOST_TYPE_IPV6 && request.target == "/a%20b?q=c%20d" &&
		request.path == "/a b" && request.query == "q=c d");
}

int HttpRequestTest(int argc, char **argv)
{
	TestFragmentedRequests();
	TestCopyDuringRequest();
	if (argc <= 1)
		return (0);

	title("Request fixtures");
	std::string text;
	for (int i = 1; i < argc; ++i)
	{
		if (ReadTestFile(argv[i], text))
			ParseRequestTest(text, argv[i]);
		else
			check(std::string("Could not read ") + argv[i], false);
	}
	return (0);
}
