#include "connection_events.hpp"
#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>

static bool canRetryIO() {
  return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR;
}

// false を返した接続はイベントループで閉じる。
static bool readClient(int fd, client &connection) {
  char buf[4096];
  ssize_t n = recv(fd, buf, sizeof(buf), 0);
  if (n < 0)
    return canRetryIO();
  if (n == 0)
    return false;
  connection.appendData(buf, n);
  return true;
}

static bool writeClient(pollfd &entry, client &connection) {
  if (!connection.isSendFinshed() && connection.sendPending(entry.fd) < 0)
    return canRetryIO();
  if (connection.isSendFinshed())
    entry.events &= ~POLLOUT;
  return true;
}

bool handleClientEvents(pollfd &entry, client &connection) {
  bool keep = !(entry.revents & (POLLERR | POLLNVAL));
  if (keep && (entry.revents & (POLLIN | POLLHUP)))
    keep = readClient(entry.fd, connection);
  if (keep && (entry.revents & POLLOUT))
    keep = writeClient(entry, connection);
  return keep;
}

void disconnectClient(size_t i, std::vector<pollfd> &fds,
                      std::map<int, client> &clients) {
  int fd = fds[i].fd;
  close(fd);
  clients.erase(fd);
  fds.erase(fds.begin() + i);
}
