#ifndef TESTSCRIPT_HPP
# define TESTSCRIPT_HPP
# include <iostream>
# include <iterator>
# include <string>
# include <vector>
# include <deque>
# include <list>
# include "HttpRequestsParser.hpp"

# define RESET   "\033[0m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define CYAN    "\033[36m"
# define MAGENTA "\033[35m"

void	title(const std::string& text);
void	check(const std::string& label, bool ok);

#endif
