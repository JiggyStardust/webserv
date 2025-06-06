#pragma once

#include "StandardLibraries.hpp"

struct LocationConfig
{
	std::string	path;
	std::string	root;
	std::string	index;
	std::vector<std::string> methods;
	bool		autoindex;
	std::string	upload_store; // not sure if a good name
	int			return_code;
	std::string return_url;
	// stuff for the cgi later cause no idea yet
};

struct ServerConfig
{
	std::vector<std::string> server_names;
	std::string listen_ip;
	int			listen_port;
	std::map<int, std::string> error_pages;
	size_t		client_max_body_size;
	std::vector<LocationConfig> locations;
};

struct ConfigFile
{
	std::vector<ServerConfig> servers;
};