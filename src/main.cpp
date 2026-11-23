#include "server.hpp"
#include "server_error.hpp"
#include <unistd.h>

int main(void) {
  int listen_fd;
  if (createListenSocket(8080, listen_fd) == FAILURE)
    return FAILURE;
  int status = runEventLoop(listen_fd);
  close(listen_fd);
  return status;
}
