#include "../inc/TestScript.hpp"
#include "../inc/ConfigParser.hpp"

static bool ReadTestFile(std::string path, std::string &text)
{
	std::ifstream	inFile(path.c_str());
	if (!inFile.is_open())
		return (false);
	text.assign(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>());
	inFile.close();
	return (true);
}

static void ParseConfigTest(const std::string &filepath, const std::string &expectedResult)
{
	ConfigParser parser;
	bool	flag = true;
	try
	{
		parser.parse(filepath);
	}
	catch (const std::exception &e)
	{
		flag = false;
	}
	check(filepath, flag);
}

int ConfigParserTest(int argc, char **argv)
{
	bool	runConfigTests = true;

	if (runConfigTests == true)
	{
		title("ConfigParser");
		if (argc < 2)
			return (EXIT_FAILURE);
		bool flag = true;
		std::string text;
		for (int i = 1; i < argc && flag == true; i++)
		{
			flag = ReadTestFile(argv[i], text);
			// Determine expected result based on filepath
			bool shouldPass = (std::string(argv[i]).find("/valid/") != std::string::npos);
			bool actualPass = flag && text.length() > 0;
			check(argv[i], actualPass == shouldPass);
		}
	}
	return (0);
}
