#include "client.hpp"

#include <sys/socket.h>

client::client() : sent_(0), last_active_(time(NULL)) {}

client::~client() {}

void client::appendData(const char *data, size_t len) {
  buffer_.append(data, len);
  last_active_ = time(NULL);
  parser_.Parse(buffer_);
}

bool client::isSendFinished() const { return sent_ >= response_.size(); }

ssize_t client::sendPending(int fd) {
  const char *nextByte = response_.c_str() + sent_;
  const size_t remaining = response_.size() - sent_;
  const ssize_t written = send(fd, nextByte, remaining, MSG_NOSIGNAL);
  if (written > 0)
    sent_ += written;
  return written;
}
