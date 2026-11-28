#ifndef TESTSCRIPT_HPP
# define TESTSCRIPT_HPP
# include <iterator>
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <deque>
# include <list>
# include <dirent.h>

# define RESET   "\033[0m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define CYAN    "\033[36m"
# define MAGENTA "\033[35m"

void	title(const std::string& text);
void	check(const std::string& label, bool ok);

/* HttpRequestTest */
int HttpRequestTest(int argc, char **argv);

#endif
