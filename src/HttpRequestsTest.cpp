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

static void ParseRequestTest(const std::string &text, const std::string &sample)
{
	HttpRequestsParser parser;
	bool	flag;
	flag = true;
	try
	{
		parser.RequestsParser(text);
	}
		catch (const std::exception & e)
	{
		flag = false;
	}
	check(sample, flag);
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

int HttpRequestTest(int argc, char **argv)
{
	bool	HeaderStorage = true;	
	bool	Extract = false;	
	bool	isLineTooLongParser = false;	
	bool	ParseFirstLine = false;	
	
	if (HeaderStorage)
		TestHeaderStorage();
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

	if (ParseFirstLine && argc > 1)
	{
		title ("ParseFirstLine");
		bool flag = true;
		std::string text;
		for (int i = 1; i < argc && flag == true; i++)
		{
			flag = ReadTestFile(argv[i], text);
			if (flag)
				ParseRequestTest(text, argv[i]);
			else
				check(std::string("Could not read ") + argv[i], false);
		}
	}
	return (0);
}
