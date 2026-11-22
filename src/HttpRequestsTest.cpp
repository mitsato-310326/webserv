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

static void	isLineTooLongTest(std::string str, std::string sample, std::string::size_type n)
{
	HttpRequestsParser parser;
	bool	flag;
	flag = true;
	try
	{
		parser.isLineTooLong(str, n);
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

int main(int argc, char **argv)
{
	bool	isLineTooLongParser = true;	
	bool	ParseFirstLine = true;	

	if (isLineTooLongParser == true)
	{
		title("isLineTooLongParser");
		isLineTooLongTest("a\r\n", "a\\r\\n", 3);
		isLineTooLongTest("abcde", "abcde", 3);
		isLineTooLongTest("abcde", "abcde", 5);
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
}
