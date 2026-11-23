#include "../inc/TestScript.hpp"
#include "../inc/HttpRequestsParser.hpp"

static bool ReadTestFile(std::string path, std::string &text)
{
	std::ifstream	inFile(path.c_str());
	if (!inFile.is_open())
		return (false);
	text.assign(std::istreambuf_iterator<char>(inFile),std::istreambuf_iterator<char>());
	inFile.close();
	return (true);
}

static void	isLineTooLongTest(std::string str, std::string sample)
{
	HttpRequestsParser parser;
	bool	flag;
	flag = true;
	try
	{
		parser.LineLength(str);
	}
	catch (const std::exception & e)
	{
		flag = false;
	}
	check(sample, flag);
}

static void CheckFixtureHeader(const HttpRequests& request, const std::string& sample)
{
	const std::string file = sample.substr(sample.find_last_of('/') + 1);
	if (file == "header-port.http")
		check(file + " stores Host with its port", request.host == "example.test:8080");
	else if (file == "header-ipv6.http")
		check(file + " stores IPv6 Host with its port", request.host == "[::1]:8080");
	else if (file == "header-colon-value.http")
	{
		std::vector<HttpHeader>::const_iterator header = request.headers.begin();
		while (header != request.headers.end() && header->name != "X-Item")
			++header;
		check(file + " stores X-Item = a:b:c",
			header != request.headers.end() && header->value == "a:b:c");
	}
}

static void ParseRequestTest(const std::string &text, const std::string &sample)
{
	HttpRequestsParser parser;
	const bool shouldReject =
		(sample.find("/invalid/") != std::string::npos ||
		 sample.find("/incomplete/") != std::string::npos);
	const bool isUnsupported =
		 sample.find("/unsupported/") != std::string::npos;
	try
	{
		parser.RequestsParser(text);
		if (shouldReject)
			check(sample + ": expected incomplete/error result", false);
		else if (isUnsupported)
			check(sample + ": unsupported syntax parsed", true);
		else
		{
			check(sample, true);
			CheckFixtureHeader(parser.GetRequest(), sample);
		}
	}
	catch (const std::exception & e)
	{
		if (shouldReject || isUnsupported)
			check(sample + ": rejected/error as expected", true);
		else
			check(sample + ": " + e.what(), false);
	}
}

static void CheckHeader(const HttpRequests& request, std::size_t index,
	const std::string& name, const std::string& value)
{
	check(name + " = [" + value + "]",
		index < request.headers.size() &&
		request.headers[index].name == name &&
		request.headers[index].value == value);
}

static void TestHeaderStorage()
{
	title("Header storage");
	try
	{
		HttpRequestsParser fixedLength;
		fixedLength.RequestsParser(
			"POST / HTTP/1.1\r\n"
			"Host: example.test:8080\r\n"
			"Content-Length: 3\r\n"
			"X-Note:\tfirst value \t\r\n"
			"X-Note: second\r\n"
			"Accept-Encoding:\r\n"
			"\r\nabc");
		const HttpRequests& fixed = fixedLength.GetRequest();
		check("Parsed Host includes port", fixed.host == "example.test:8080");
		check("Parsed Content-Length is 3", fixed.contentLength == 3);
		check("Transfer-Encoding absent", fixed.transferEncoding.empty());
		check("Five headers retained", fixed.headers.size() == 5);
		CheckHeader(fixed, 0, "Host", "example.test:8080");
		CheckHeader(fixed, 1, "Content-Length", "3");
		CheckHeader(fixed, 2, "X-Note", "first value");
		CheckHeader(fixed, 3, "X-Note", "second");
		CheckHeader(fixed, 4, "Accept-Encoding", "");

		HttpRequestsParser transferCoded;
		transferCoded.RequestsParser(
			"POST / HTTP/1.1\r\n"
			"Host: example.test\r\n"
			"Transfer-Encoding: gzip, Chunked\r\n"
			"\r\n0\r\n\r\n");
		const HttpRequests& coded = transferCoded.GetRequest();
		check("Content-Length absent", coded.contentLength == -1);
		check("Transfer-Encoding retains coding order",
			coded.transferEncoding.size() == 2 &&
			coded.transferEncoding[0] == GZIP &&
			coded.transferEncoding[1] == CHUNKED);
		CheckHeader(coded, 1, "Transfer-Encoding", "gzip, Chunked");
	}
	catch (const std::exception& e)
	{
		check(std::string("Header parsing: ") + e.what(), false);
	}
}

static void CheckContentLengthBody(const std::string& label,
	const std::string& headers, const std::string& bytes,
	const std::string& expected)
{
	HttpRequestsParser parser;
	try
	{
		parser.RequestsParser("POST / HTTP/1.1\r\nHost: example.test\r\n"
			+ headers + "\r\n" + bytes);
		check(label, parser.GetRequest().body == expected);
	}
	catch (const std::exception& e)
	{
		check(label + ": " + e.what(), false);
	}
}

static void TestContentLengthBody()
{
	title("Content-Length body");
	CheckContentLengthBody("Exact body", "Content-Length: 5\r\n", "hello", "hello");
	CheckContentLengthBody("Zero length", "Content-Length: 0\r\n", "", "");
	const std::string binary("A\0\r\nB", 5);
	CheckContentLengthBody("Body preserves NUL and CRLF",
		"Content-Length: 5\r\n", binary, binary);
	CheckContentLengthBody("Body stops at declared length",
		"Content-Length: 5\r\n", "helloextra", "hello");
	CheckContentLengthBody("No Content-Length and no body", "", "", "");

	HttpRequestsParser parser;
	try
	{
		parser.RequestsParser("POST / HTTP/1.1\r\nHost: example.test\r\n"
			"Content-Length: 5\r\n\r\nhe");
		check("Incomplete body does not parse as complete", false);
	}
	catch (const HttpRequestsParser::RequestBodyException&)
	{
		check("Incomplete body does not parse as complete", true);
	}
	catch (const std::exception& e)
	{
		check(std::string("Incomplete body: ") + e.what(), false);
	}
}

static void ChunkSizeTest(const std::string& text)
{
	HttpRequestsParser	http;
	bool				flag = true;

	try
	{
		http.ParseTransferEncoding(text);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		flag = false;
	}
	check("chunk size test", flag);
}

int HttpRequestTest(int argc, char **argv)
{
	const bool	chunksize = false;
	const bool	HeaderStorage = false;
	const bool	ContentLengthBody = false;
	const bool	Extract = false;
	const bool	isLineTooLongParser = false;
	const bool	RequestFixtures = true;


	if (chunksize)
	{
		title ("ChunkSize TransferEncoding");
		ChunkSizeTest("0\r\n\r\n");
		ChunkSizeTest("4\r\nabcd\r\n0\r\n\r\n");
	}
	if (HeaderStorage)
		TestHeaderStorage();
	if (ContentLengthBody)
		TestContentLengthBody();
	if (Extract)
	{
		title("ExtractLine");
		std::string	text;
		for (int i = 1; i < argc; i++)
		{
			HttpRequestsParser http;
			ReadTestFile(argv[i], text);
			std::cout << http.ExtractLine(text) << http.ExtractLine(text) << std::endl;
		}
	}

	if (isLineTooLongParser == true)
	{
		title("isLineTooLongParser");
		isLineTooLongTest("a\r\n", "a\\r\\n");
		isLineTooLongTest("abcde", "abcde");
		isLineTooLongTest("abcde", "abcde");
	}

	if (RequestFixtures && argc > 1)
	{
		title ("Request fixtures");
		std::string text;
		for (int i = 1; i < argc; i++)
		{
			if (ReadTestFile(argv[i], text))
				ParseRequestTest(text, argv[i]);
			else
				check(std::string("Could not read ") + argv[i], false);
		}
	}
	return (0);
}
