#pragma once

#include "StandardLibraries.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "Signal.hpp"

typedef enum e_cgi_state {
	CGI_INACTIVE,
	CGI_READY,
	CGI_WAITING,
	CGI_FAILED,
	CGI_TIMEOUT
} t_cgi_state;

class CgiHandler {
	private:
		int _pid;
		time_t	_start_time;
		static const int	_CGI_TIMEOUT_S = 1;
		t_cgi_state	_state = CGI_INACTIVE;
	public:
		std::string output_filename;
		void	launchCgi(Request &request); //fork(), execve()
		t_cgi_state	checkCgi(); // waitpid()
};
