#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ctime>
#include <iostream>
#include <string>

class client {
private:
  std::string buffer_;
  std::string response_;
  size_t sent_;
  time_t last_active_;

public:
  client();
  ~client();
  void appendData(const char *data, size_t len);
  bool isComplete() const;
  bool isSendFinshed() const;
};

#endif
