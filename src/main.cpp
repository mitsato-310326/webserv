#include "server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

int main(void) {
  try {
    ListenSocket listener(8080);
    runEventLoop(listener.fd());
  } catch (const std::exception &error) {
    std::cerr << "webserv: " << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
