#include "server.hpp"
#include "server_error.hpp"
#include "connection_events.hpp"
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

ListenSocket::ListenSocket(unsigned short port)
    : fd_(socket(AF_INET, SOCK_STREAM, 0)) {
  if (fd_ < 0)
    throw ServerError("socket", errno);

  int opt = 1;
  sockaddr_in serv_addr = sockaddr_in();
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  try {
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
      throw ServerError("setsockopt", errno);
    if (fcntl(fd_, F_SETFL, O_NONBLOCK) < 0)
      throw ServerError("fcntl", errno);
    if (bind(fd_, (const sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      throw ServerError("bind", errno);
    if (listen(fd_, SOMAXCONN) < 0)
      throw ServerError("listen", errno);
  } catch (...) {
    close(fd_);
    throw;
  }
}

ListenSocket::~ListenSocket() { close(fd_); }

static void acceptClient(int listenFd, std::vector<pollfd> &fds,
                         std::map<int, client> &clients) {
  int fd = accept(listenFd, NULL, NULL);
  if (fd < 0)
    return;
  int opt = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
    close(fd);
    return;
  }
  pollfd entry = {fd, POLLIN, 0};
  try {
    fds.push_back(entry);
  } catch (...) {
    close(fd);
    throw;
  }
  clients[fd]; // この fd に対応する client をデフォルト構築する。
}

void handleListenEvents(pollfd entry, std::vector<pollfd> &fds,
                        std::map<int, client> &clients) {
  if (entry.revents & (POLLERR | POLLHUP | POLLNVAL))
    throw ServerError("poll: listening socket is unavailable");
  if (entry.revents & POLLIN)
    acceptClient(entry.fd, fds, clients);
}
