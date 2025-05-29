#pragma once

#include <string>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>

class Client {
public:
	Client(int fd = -1);

	void recvFrom(int epoll_fd);
	void sendTo(int epoll_fd);

private:
	int			fd;
	std::string recv_buf;
	std::string send_buf;
};
