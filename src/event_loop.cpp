#include "server.hpp"
#include "server_error.hpp"
#include "connection_events.hpp"
#include <cerrno>
#include <unistd.h>

#define POLL_TIMEOUT_MS 1000

static void pollConnections(int listen_fd, std::vector<pollfd> &fds) {
  std::map<int, client> clients;

  while (1) {
    int status = poll(&fds[0], fds.size(), POLL_TIMEOUT_MS);
    if (status < 0) {
      if (errno == EINTR)
        continue;
      throw ServerError("poll", errno);
    }
    if (status == 0)
      continue;
    for (size_t i = 0; i < fds.size();) {
      pollfd &entry = fds[i];
      if (entry.fd == listen_fd) {
        handleListenEvents(entry, fds, clients);
      } else if (entry.revents != 0 && !handleClientEvents(entry, clients[entry.fd])) {
        disconnectClient(i, fds, clients);
        continue; // erase 後は次の fd が同じ添字に移る。
      }
      ++i;
    }
  }
}

void runEventLoop(int listen_fd) {
  pollfd listener = {listen_fd, POLLIN, 0};
  std::vector<pollfd> fds(1, listener);
  try {
    pollConnections(listen_fd, fds);
  } catch (...) {
    for (size_t i = 1; i < fds.size(); ++i)
      close(fds[i].fd);
    throw;
  }
}
