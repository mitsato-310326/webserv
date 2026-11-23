#ifndef SERVER_HPP
#define SERVER_HPP

int createListenSocket(unsigned short port, int &listenFd);
int runEventLoop(int listen_fd);

#endif
