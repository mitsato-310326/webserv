#include "server.hpp"
#include "server_error.hpp"
#include "connection_events.hpp"
#include <cerrno>

#define POLL_TIMEOUT_MS 1000

int runEventLoop(int listen_fd) {
  std::map<int, client> clients;
  pollfd listen_poll = {listen_fd, POLLIN, 0};
  std::vector<pollfd> fds(1, listen_poll);

  while (1) {
    int status = poll(&fds[0], fds.size(), POLL_TIMEOUT_MS);
    if (status < 0) {
      if (errno == EINTR)
        continue;
      return FAILURE;
    }
    if (status == 0)
      continue;
    for (size_t i = 0; i < fds.size();) {
      pollfd &entry = fds[i];
      if (entry.fd == listen_fd) {
        if (!handleListenEvents(entry, fds, clients))
          return FAILURE;
      } else if (entry.revents != 0 && !handleClientEvents(entry, clients[entry.fd])) {
        disconnectClient(i, fds, clients);
        continue; // erase 後は次の fd が同じ添字に移る。
      }
      ++i;
    }
  }
}
