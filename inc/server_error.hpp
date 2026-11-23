#ifndef SERVER_ERROR_HPP
#define SERVER_ERROR_HPP

#include <cstring>
#include <stdexcept>
#include <string>

class ServerError : public std::runtime_error {
public:
  explicit ServerError(const char *message) : std::runtime_error(message) {}

  ServerError(const char *operation, int errorNumber)
      : std::runtime_error(std::string(operation) + ": " +
                           std::strerror(errorNumber)) {}
};

#endif
