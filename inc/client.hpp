#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ctime>
#include <string>
#include <sys/types.h>

#include "HttpRequestsParser.hpp"

class client {
private:
  std::string buffer_;
  std::string response_;
  HttpRequestsParser parser_;
  size_t sent_;
  time_t last_active_;

public:
  client();
  ~client();
  void appendData(const char *data, size_t len);
  ssize_t sendPending(int fd);
  bool isSendFinished() const;
};

#endif
