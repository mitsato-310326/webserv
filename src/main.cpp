#include "client.hpp"
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

int main(void) {
  int listen_fd;
  std::map<int, client> clients;
  char buf[4096];

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  fcntl(listen_fd, F_SETFL, O_NONBLOCK);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(8080);

  bind(listen_fd, (const sockaddr *)&serv_addr, sizeof(serv_addr));
  listen(listen_fd, 5);

  // 受付用のfd
  std::vector<struct pollfd> fds;
  struct pollfd listen_poll;
  listen_poll.fd = listen_fd;
  listen_poll.events = POLLIN;
  listen_poll.revents = 0;
  fds.push_back(listen_poll);

  while (1) {
    bool remove = false;
    int status = poll(&fds[0], fds.size(), 1000);
    if (status < 0)
      return (1);
    if (status == 0)
      continue;
    for (size_t i = 0; i < fds.size();) {
      if (fds[i].revents == 0)
        continue;
      if (fds[i].revents & POLL_IN) {
        if (fds[i].fd == listen_fd) {
          // 新規クライアントの追加処理
          struct sockaddr_in client_addr;
          socklen_t len = sizeof(client_addr);

          int client_fd = accept(listen_fd, (sockaddr *)&client_addr, &len);
          setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
          fcntl(client_fd, F_SETFL, O_NONBLOCK);
          struct pollfd client_poll;
          client_poll.fd = client_fd;
          client_poll.events = POLLIN;
          client_poll.revents = 0;
          fds.push_back(client_poll);
          clients[client_fd] = client();
        } else {
          ssize_t n = recv(fds[i].fd, buf, sizeof(buf), 0);
          if (n <= 0)
            remove = true;
          else
            clients[fds[i].fd].appendData(buf, n);
        }
      }
      if (fds[i].revents & POLLOUT) {
       
        ssize_t n = send(fds[i].fd, clients[fds[i].fd]., sizeof(response));
      }

      if (remove) {
      }

      i++;
    }
  }
}