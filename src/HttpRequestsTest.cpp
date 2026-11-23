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

static void ParseFirstLineTest(const std::string &text, const std::string &sample)
{
	HttpRequestsParser parser;
	bool	flag;
	flag = true;
	try
	{
		parser.ParseFirstLine(text);
	}
		catch (const std::exception & e)
	{
		flag = false;
	}
	check(sample, flag);
}

int HttpRequestTest(int argc, char **argv)
{
	bool	Extract = false;	
	bool	isLineTooLongParser = false;	
	bool	ParseFirstLine = true;	

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

	if (ParseFirstLine == true)
	{
		title ("ParseFirstLine");
		if (argc < 2)
		return (EXIT_FAILURE);
		bool flag = true;
		std::string text;
		for (int i = 1; i < argc && flag == true; i++)
		{
			flag = ReadTestFile(argv[i], text);
			ParseFirstLineTest(text, argv[i]);
		}
	}
	return (0);
}
