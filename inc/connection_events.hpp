#ifndef CONNECTION_EVENTS_HPP
#define CONNECTION_EVENTS_HPP

#include "client.hpp"
#include <cstddef>
#include <map>
#include <poll.h>
#include <vector>

void handleListenEvents(pollfd entry, std::vector<pollfd> &fds,
                        std::map<int, client> &clients);
bool handleClientEvents(pollfd &entry, client &connection);
void disconnectClient(std::size_t i, std::vector<pollfd> &fds,
                      std::map<int, client> &clients);

#endif
