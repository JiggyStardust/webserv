
#include "../inc/utils.hpp"
#include "../inc/Client.hpp"

Client::Client(std::vector<ServerConfig> configs, int epoll_fd, int fd) :
	request(configs),
	configs(configs),
	epoll_fd(epoll_fd),
	fd(fd)
{
	state = IDLE;
}

t_client_state Client::getState() {
	return state;
}

int	Client::getClientFd(){
	return fd;
}

void Client::setState(t_client_state state) {
	this->state = state;
}

void Client::changeEpollMode(uint32_t mode) {
	struct epoll_event e_event{};
	e_event.events = mode;
	e_event.data.fd = fd;
	// note: epoll_ct_MOD, not epoll_ctl_ADD
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &e_event) < 0) {
		std::cerr << "changeEpollMode: epoll_ctl() failed" << std::endl;
	}
}

time_t Client::getLastEvent(){
	return _last_event;
}

// Client object is erased in event_loop() after the state is checked
void Client::closeConnection(int epoll_fd, int client_fd) {
	std::cout << "CLIENT_FD: " << client_fd << std::endl;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) < 0) {
		if (errno == EBADF)
		{
			std::cout << "SHITTY FD\n";
		}
		else if (errno == EINVAL)
		{
			std::cout << "SHITTY EPOLLFD\n";
		}
		std::cerr << "closeConnection: epoll_ctl() failed" << std::endl;
	}

	if (close(client_fd) < 0) {
		std::cerr << "failed to close() client_fd" << std::endl;
	}
	std::cout << "Clientfd " << client_fd << " has been closed\n" ;
	state = DISCONNECT;
}

void	Client::updateLastEvent()
{
	_last_event = std::time(nullptr);
}

void Client::recvFrom() {
	// std::cout << "entered recvFrom" << std::endl;
	if (state == IDLE)
	{
		state = RECV;
	}
	char buf[2000] = {0};

	int bytes_read = recv(fd, buf, sizeof(buf) -1, MSG_DONTWAIT);

	if (bytes_read < 0) {
		std::cerr << "recvFrom() returned -1" << std::endl;
		return ;
	}

	// this is possibly correct, i.e. EPOLLHUP is not needed because there is
	// an event anyway when the client closes the connection
	if (bytes_read == 0) {
		state = DISCONNECT;
		return ;
	}

	request.addToRequest(std::string(buf, bytes_read));
	if (request.getState() == READY) {
		if (request.getIsCgi()) {
			state = WAIT_CGI;
			try 
			{
				cgi.launchCgi(request);
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << std::endl;
				state = SEND;
				request.getResponse(500);
				send_queue.push_back(request.getRes());
			}
			changeEpollMode(EPOLLOUT);
			return ;
		}
		 
		send_queue.push_back(request.getRes());

		// reset the request member to an empty state
		request = Request(configs);
		
		// this will cause the event loop to trigger sendTo()
		changeEpollMode(EPOLLOUT);
		state = SEND;
	} else {
		// go back to event loop and wait to receive more
		return ;
	}
}

void Client::sendTo() {
	if (_to_send.length() == 0 && state == WAIT_CGI) {
		t_cgi_state cgi_result = cgi.checkCgi(); // this has waitpid
		if (cgi_result == CGI_READY) {
			if (request.getMethod() == POST){
				std::remove(request.getPostBodyFilename().c_str());
			}
			try {
				Response tmp;
				tmp.full_response = fileToString(cgi.output_filename);
				std::cout << "deleting file: " << cgi.output_filename << std::endl;
				std::remove(cgi.output_filename.c_str());
				send_queue.push_back(tmp);
			} catch (const std::ios_base::failure& e){
				request.getResponse(500);
				send_queue.push_back(request.getRes());
			}
				state = SEND;
			return ;
		}
		else if (cgi_result == CGI_TIMEOUT || cgi_result == CGI_FAILED)
		{
			std::cerr << "Client::sendTo(): cgi timeout or fail" << std::endl;
			std::remove(cgi.output_filename.c_str());
			try {
				if (cgi_result == CGI_TIMEOUT)
					request.getResponse(504);
				else
					request.getResponse(500);
				send_queue.push_back(request.getRes());
			} catch (const std::ios_base::failure& e){
				// TODO how to handle error from client?
			}
			state = SEND;
			return ;
		}
		else
		{
			// keep waiting for CGI
			return;
		}
	} else {
		if (send_queue.size() < 1) {
			std::cerr << "sendTo() called with empty send_queue" << std::endl;
			changeEpollMode(EPOLLIN);
			state = IDLE;
			return ;
		}

		if (_to_send.empty()) {
			_to_send = send_queue.front().full_response;
		}

		int bytes_sent = send(fd, _to_send.c_str(), _to_send.length(), MSG_NOSIGNAL);
		if (bytes_sent < 0) {
			std::cerr << "send() returned -1" << std::endl;
		}

		_to_send.erase(0, bytes_sent);

		if (_to_send.empty()) {
			// request.setIsCgi(false);
			send_queue.erase(send_queue.begin());
			if (send_queue.size() == 0) {
				// we have sent all responses in the queue
				if (request.getConnectionTypeIsClose() == true ) {
					state = DISCONNECT;
				} else { // nothing left to send
					changeEpollMode(EPOLLIN);
					request = Request(configs);
					state = IDLE;
				}
			}
		}
	}
}
