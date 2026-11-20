#include "../inc/TestScript.hpp"

static int	g_total = 0;
static int	g_failed = 0;

void	title(const std::string& text)
{
	std::cout << "\n" << CYAN << "--- " << text << " ---" << RESET << "\n";
}

void	check(const std::string& label, bool ok)
{
	g_total++;
	if (ok)
		std::cout << GREEN << "  [ OK ] " << RESET << label << "\n";
	else
	{
		std::cout << RED << "  [FAIL] " << RESET << label << "\n";
		g_failed++;
	}
}

void	HttpRequestParsertest(std::string str, std::string::size_type n)
{
	
}


int main(void)
{
	HttpRequestsParser	parse = HttpRequestsParser();
	bool				flag = true;
	title("isLineToolong");
	try
	{
		parse.isLineTooLong("a\r\n", 3);
	}
	catch (const std::exception & e)
	{
		flag = false;
	}
	check("a\\r\\n, 3", flag);
}