#include "../inc/Client.hpp"
// #include "../inc/request_handler.hpp"
#include <sys/epoll.h>
#include <unistd.h> //sleep()

static std::string getResponse() {
	std::string body = "<!DOCTYPE html><html><head><title>First Web Page</title></head><body>Hello Maria!</body></html>";
	//#include "marquee.cpp"
	std::string header = "HTTP/1.1 200 OK\nDate: Thu, 26 May 2025 10:00:00 GMT\nServer: Apache/2.4.41 (Unix)\nContent-Type: text/html; charset=UTF-8\nContent-Length:" + std::to_string(body.length()) + "\nSet-Cookie: session=some-session-id; Path=/; HttpOnly\nCache-Control: no-cache, private \n\n ";
	return header + body;
}

Client::Client(int fd) : fd(fd)  {}

bool isCompleteRequest(std::string request) {
	if (request.find("\r\n\r\n") != std::string::npos) {
		std::cout << "found end of request" << std::endl;
		return true;
	}
	return false;
	// return request.length() > 0;
}

static void changeEpollMode(int epoll_fd, int client_fd, uint32_t mode) {
	struct epoll_event e_event{};
	e_event.events = mode;
	e_event.data.fd = client_fd;
	// note: epoll_ct_MOD, not epoll_ctl_ADD
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &e_event);
}

void Client::recvFrom(int epoll_fd) {
	sleep(1);
	std::cout << "entered recvFrom" << std::endl;
	char buf[50] = {0};

	int bytes_read = recv(fd, buf, sizeof(buf) -1, MSG_DONTWAIT);
	std::cout << "partial receive: " << std::endl;
	std::cout << recv_buf << std::endl;
	recv_buf += std::string(buf, bytes_read);
	if (bytes_read > 0) {
		std::cout << "recvFrom: read " << bytes_read << " bytes" << std::endl;
		if (isCompleteRequest(recv_buf)) {
			std::cout << "##### RECEIVED REQUEST #####" << std::endl;
			std::cout << recv_buf << std::endl;
			std::cout << "############################" << std::endl;
			// send_buf = getResponse(recv_buf);
			//
			// TODO: do we decide, based on the request, whether we want to
			// send anthing back at all? So instead of calling getResponse(),
			// we would call handleRequest() which would then decide whether 
			// to toggle to EPOLLOUT...
			// 
			send_buf = getResponse();
			// this will cause the event loop to trigger sendTo()
			changeEpollMode(epoll_fd, fd, EPOLLOUT);
		}
	} else if (bytes_read == 0) {
		std::cout << "recvFrom(): read 0 bytes" << std::endl;
	} else {
		std::cout << "recvFrom() returned -1" << std::endl;
	}
}

void Client::sendTo(int epoll_fd) {
	std::cout << "entered sendTo()" << std::endl;
	int bytes_sent = send(fd, send_buf.c_str(), send_buf.length(), MSG_NOSIGNAL);
	std::cout << "sent " << bytes_sent << " bytes" << std::endl;
	send_buf.erase(0, bytes_sent);

	// we have sent the whole response
	if (send_buf.empty()) {
		changeEpollMode(epoll_fd, fd, EPOLLIN);
	}
}
