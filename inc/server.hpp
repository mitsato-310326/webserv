#ifndef SERVER_HPP
#define SERVER_HPP

class ListenSocket {
public:
  explicit ListenSocket(unsigned short port);
  ~ListenSocket();
  int fd() const { return fd_; }

private:
  int fd_;
  ListenSocket(const ListenSocket &);
  ListenSocket &operator=(const ListenSocket &);
};

void runEventLoop(int listen_fd);

#endif
