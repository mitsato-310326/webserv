#include "server.hpp"
#include "server_error.hpp"
#include "connection_events.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int createListenSocket(unsigned short port, int &listenFd) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return FAILURE;

  int opt = 1;
  sockaddr_in serv_addr = sockaddr_in();
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
      fcntl(fd, F_SETFL, O_NONBLOCK) < 0 ||
      bind(fd, (const sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ||
      listen(fd, SOMAXCONN) < 0) {
    close(fd);
    return FAILURE;
  }
  listenFd = fd;
  return SUCCESS;
}

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
  fds.push_back(entry);
  clients[fd]; // この fd に対応する client をデフォルト構築する。
}

bool handleListenEvents(pollfd entry, std::vector<pollfd> &fds,
                        std::map<int, client> &clients) {
  if (entry.revents & (POLLERR | POLLHUP | POLLNVAL))
    return false;
  if (entry.revents & POLLIN)
    acceptClient(entry.fd, fds, clients);
  return true;
}
