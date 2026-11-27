# include "TestScript.hpp"

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

int main(int argc, char **argv)
{
	bool HttpRequest = true;
	bool String = false;

	if (HttpRequest)
		HttpRequestTest(argc, argv);
	if (String)
		StringTest();
	return (g_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
