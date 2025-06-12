
// IN PARSING:
// open the config_file -> trim comments and unnecessary spaces -> necessary config_file information is put into a string vector 
// FROM THAT STRING VECTOR:
	// search for server block(s)
	// inside the server block search matching "values" for keywords

#include "../inc/Structs.hpp"
#include "../inc/StandardLibraries.hpp"

void printServerConfigs(const std::vector<ServerConfig> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const ServerConfig &server = servers[i];
		std::cout << "======== SERVER " << i + 1 << " ========" << std::endl;

	//	Server names
		std::cout << "Server Names: ";
		for (size_t j = 0; j < server.server_names.size(); ++j)
		{
			std::cout << server.server_names[j];
			if (j + 1 < server.server_names.size())
				std::cout << ", ";
		}
		std::cout << std::endl;

//		Listen IP/Port
		std::cout << "Listen IP: " << server.listen_ip << std::endl;
		std::cout << "Listen Port: " << server.listen_port << std::endl;

//		Error pages
		std::cout << "Error Pages:" << std::endl;
		for (std::map<int, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
		{
			std::cout << "  " << it->first << " => " << it->second << std::endl;
		}

//		Client max body size
		std::cout << "Client Max Body Size: " << server.client_max_body_size << std::endl;

	//	Locations
		std::cout << "Locations: " << std::endl;
		for (size_t k = 0; k < server.locations.size(); ++k)
		{
			const LocationConfig &loc = server.locations[k];
			std::cout << "  --- Location " << k + 1 << " ---" << std::endl;
			std::cout << "  Path: " << loc.path << std::endl;
			std::cout << "  Root: " << loc.root << std::endl;
			std::cout << "  Index: " << loc.index << std::endl;
			std::cout << "  Methods: ";
			for (size_t m = 0; m < loc.methods.size(); ++m)
			{
				std::cout << loc.methods[m];
				if (m + 1 < loc.methods.size())
					std::cout << ", ";
			}
			std::cout << std::endl;
			std::cout << "  Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
			std::cout << "  Upload Store: " << loc.upload_store << std::endl;
			if (loc.return_code != 0 || !loc.return_url.empty())
			{
				std::cout << "  Return: " << loc.return_code << " " << loc.return_url << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

std::string	trimComments(const std::string &line)
{
	size_t hash_pos;

	hash_pos = line.find("#");
	if (hash_pos != std::string::npos)
		return (line.substr(0, hash_pos));
	return (line);
}

std::string	trimBeginningAndEnd(const std::string &line)
{
	size_t	start;
	size_t	end;

	start = line.find_first_not_of("\t\n\r "); // skip these whitespaces before the line
	if (start == std::string::npos)
		return ("");
	end = line.find_last_not_of("\t\n\r ");
	return (line.substr(start, end - start + 1));
}

LocationConfig	parseLocationConfig(std::vector<std::string> &config, std::vector<std::string>::iterator &it, std::string &path)
{
	LocationConfig	location;
	std::string	key;

	location.path = path;
	location.autoindex = false;
	location.return_code = 0;
	location.return_url = "";

	while (it != config.end())
	{
	//	//std::cout << "it: " << *it << std::endl;
		std::istringstream iss;
		iss.str(*it);
		iss >> key;
		if (key == "root")
		{
			iss >> key;
			key.pop_back();
			location.root = key;
		}
		else if (key == "index")
		{
			iss >> key;
			key.pop_back();
			location.index = key;
		}
		else if (key == "methods")
		{
			while (iss >> key)
			{
				if (key.back() == ';')
					key.pop_back();
				location.methods.push_back(key);
			}
		}
		else if (key == "autoindex")
		{
			iss >> key;
			if (key == "on")
			{
				location.autoindex = true;
			}
		}
		else if (key == "upload_store")
		{
			iss >> key;
			if (key.back() == ';')
			{
				key.pop_back();
			}
			location.upload_store = key;
		}
		else if (key == "return")
		{
			int code;
			iss >> code;
			location.return_code = code;
			iss >> key;
			if (key.back() == ';')
			{
				key.pop_back();
			}
			location.return_url = key;
		}
		else if (key == "}")
		{
			break ;
		}
		it++;
	}
	return (location);

}

size_t	parseClientBodySize(std::string &client_max_size) // need to adjust this to work with bytes, not SI
{
	int	coefficient = 1;

	if (client_max_size.find("K") != std::string::npos)
	{
		coefficient = 1024;
	}
	else if (client_max_size.find("M") != std::string::npos)
	{
		coefficient = 1024 * 1024;
	}
	else if (client_max_size.find("G") != std::string::npos)
	{
		coefficient = 1024 * 1024 * 1024;
	}
	return (std::stol(client_max_size) * coefficient); // add overflow handling later?
}

void	validateListen(const std::string &ip_port)
{
	std::regex valid_ip(R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\;)"); // 1-3digits.1-3digits.1-3.digits.1-3digits:1-5digits;
	if (!std::regex_match(ip_port, valid_ip))
	{
		throw std::runtime_error ("config file: listen IP and port in wrong format.");
	}
}

// I don't know if there's any rules regarding the servers naming other than directives have to end with semicolon
void validateServerName(const std::string &name)
{
	if (name.back() != ';')
	{
		throw std::runtime_error("config file: server_name directive missing semicolon");
	}
	if (name.find(';') != name.size() - 1)
	{
		throw std::runtime_error("config file: server_name directive contains excessive semicolon");
	}
}

void	validateBodySize(std::string &client_body, std::istringstream &iss)
{
	if (client_body.back() != ';')
	{
		throw std::runtime_error("config file: client_max_body_size directive missing semicolon");
	}
	if (client_body.find(';') != client_body.size() - 1)
	{
		throw std::runtime_error("config file: client_max_body_size directive contains excessive semicolon");
	}
	client_body.pop_back();
	std::regex valid_body_size(R"(^\d{1,10}$|^\d{1,7}[kK]$|^\d{1,4}[mM]$)");

	if (!std::regex_match(client_body, valid_body_size))
	{
		throw std::runtime_error("config file: client_max_body_size doesn't match requirements");
	}
	if (iss >> client_body)
	{
		throw std::runtime_error("config file: client_max_body_size contains excessive info");
	}
}

void	validateAndParseErrorPage(std::istringstream &iss, ServerConfig &server)
{
	std::string error_code;
	std::string error_html;

	(void)server;
	iss >> error_code;
	if (!std::regex_match(error_code, std::regex(R"(^\d{3}$)")))
	{
		throw std::runtime_error ("config file: invalid error_page code: " + error_code);
	}
	int code = std::stoi(error_code);
	iss >> error_html;
	if (error_html.empty() || error_html.back() != ';')
	{
		throw std::runtime_error("config file: error_page " + error_code + " file path missing");
	}
	if (error_html.find(';') != error_html.size() - 1)
	{
		throw std::runtime_error("config file: error_page " + error_code + " not ending with (one) semicolon");
	}
	error_html.pop_back();
	std::ifstream test("./" + error_html);
	if (!test)
	{
		throw std::runtime_error("config file: error_page " + error_code + " not a valid path");
	}
	test.close();
	server.error_pages[code] = error_html;
}

ServerConfig parseIndividualServer(std::vector<std::string> &config, std::vector<std::string>::iterator &it)
{
	ServerConfig	server;
	std::string		key;
	
	while (it != config.end())
	{
	//	//std::cout << "it: " << *it << std::endl;
		if (*it == "{")
		{
			throw std::runtime_error("nested blocks in configuration file");
		}
		if (*it == "}")
			break ;
		std::istringstream iss;
		iss.str(*it);
		iss >> key;
		if (key == "listen")
		{
			std::string ip_port;
			iss >> ip_port;
			validateListen(ip_port);
			ip_port.pop_back();
			size_t colon = ip_port.find(":");
			if (colon == std::string::npos)
			{
				throw std::runtime_error("config file: IP address missing colon");
			}
			server.listen_ip = ip_port.substr(0, colon);
			server.listen_port = std::stoi(ip_port.substr(colon + 1));
		}
		else if (key == "server_name")
		{
			validateServerName(*it);
			std::string name;
			while (iss >> name)
			{
				if (!name.empty() && name.back() == ';')
				{
					name.pop_back();
					server.server_names.push_back(name);
					break;
				}
				server.server_names.push_back(name);
			}
		}
		else if (key == "client_max_body_size")
		{
			std::string max_size;
			iss >> max_size;
			validateBodySize(max_size, iss);
			server.client_max_body_size = parseClientBodySize(max_size);
		}
		else if (key == "error_page")
		{
			validateAndParseErrorPage(iss, server);
		}
		else if (key == "location")
		{
		//	//std::cout << "LOCATION FOUND!" << std::endl;
			std::string	location;
			iss >> location;
			server.locations.push_back(parseLocationConfig(config, it, location));
		}
		else
		{
			throw std::runtime_error("config file:\n\tserver block:\n\t\tunrecognised: " + *it);
		}
		it++;
	}
	return (server);
}

void	validateServerStatement( std::vector<std::string>::iterator &it)
{
	if (it->find_first_not_of("server") < 6)
	{
		throw std::runtime_error("config file:\nunexpected characters before server block statement");
	}
	std::string rest = it->substr(6,6);
	size_t	brace_pos = rest.find_first_not_of(" \t");
	if (rest[brace_pos] != '{')
	{	
		throw std::runtime_error("config file:\nserver block statement should be followed by '{'");
	}
	if (rest.find_first_not_of(" \t", brace_pos + 1) != std::string::npos)
	{
		throw std::runtime_error("config file:\nunexpected characters after '{' in server block");
	}
}

std::vector<ServerConfig> parseServers(std::vector<std::string> &config)
{
	std::vector<ServerConfig> servers;
	std::string	line;
	
	for (std::vector<std::string>::iterator it = config.begin() ; it != config.end(); ++it)
	{
	//	std::cout << "it: " << *it << std::endl;
		if (it->find("server") != std::string::npos)
		{
			validateServerStatement(it);
			it++;
			servers.push_back(parseIndividualServer(config, it));
		}
		else
		{
			throw std::runtime_error("config file: malformed");
		}
	}
	return (servers);
}

std::vector<std::string> trimConfigToVector(std::ifstream &config_file)
{
	std::vector<std::string> config_vector;
	std::string	line;

	while (std::getline(config_file, line))
	{
		line = trimComments(line);
		if (line.empty())
		{
			continue;
		}
		line = trimBeginningAndEnd(line);
		if (line.empty())
		{
			continue;
		}
		config_vector.push_back(line);
	}
	return (config_vector);
}

void validateBraces(std::vector<std::string> &config)
{
	int opening_brace = 0;
	int closing_brace = 0;

	for (std::vector<std::string>::iterator it = config.begin() ; it != config.end(); ++it)
	{
		for (char ch: (*it))
		{
			if (ch == '{')
			{
				opening_brace++;
			}
			else if (ch == '}')
			{
				closing_brace++;
				if (opening_brace < closing_brace)
				{
					throw std::runtime_error("config file:\nunmatching closing brace '}'");
				}
			}
		}
	}
	if (opening_brace < closing_brace)
	{
		throw std::runtime_error("config file:\nunmatching closing brace '}'");
	}
	if (opening_brace > closing_brace)
	{
		throw std::runtime_error("config file:\nunmatching opening brace '{'");
	}
}

std::vector<ServerConfig> configParser(char *path_to_config)
{
	struct ConfigFile;
	std::vector<std::string> trimmed_config;

	if (std::filesystem::path(path_to_config).extension() != ".conf")
	{
		throw std::runtime_error("config file: extension not matching .conf");
	}
	std::ifstream config_file(path_to_config);
	if (!config_file)
	{
		throw std::runtime_error("Opening config file");
	}
	trimmed_config = trimConfigToVector(config_file);
	// std::cout << "Trimmed vector:\n";
	// for (std::vector<std::string>::iterator it = trimmed_config.begin() ; it != trimmed_config.end(); ++it)
	// 	std::cout << *it << "|\n";
	config_file.close();
	validateBraces(trimmed_config);
	std::vector<ServerConfig> servers = parseServers(trimmed_config);
	if (servers.empty())
	{
		throw std::runtime_error("config file: no valid server information configured");
	}
	// std::cout << "PRINTING SERVERS\n";
	// printServerConfigs(servers);
	return servers;
}

