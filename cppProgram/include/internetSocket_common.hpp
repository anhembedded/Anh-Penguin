#ifndef INTERNETSOCKET_COMMON_HPP_
#define INTERNETSOCKET_COMMON_HPP_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Cho inet_addr
#include <unistd.h>

const int PORT = 9999;
const char* SERVER_IP = "127.0.0.1";

#endif // INTERNETSOCKET_COMMON_HPP_