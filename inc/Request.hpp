#pragma once

#include "parse_header.hpp"
#include "StandardLibraries.hpp"
#include "Structs.hpp"
#include "Webserv.hpp"
#include "PostFile.hpp"

const std::map<std::string, std::string> extensions {
	{".aac", "audio/aac"},
	{".bmp", "image/bmp"},
	{".css", "text/css"},
	{".csv", "text/csv"},
	{".epub", "application/epub+zip"},
	{".gif", "image/gif"},
	{".htm", "text/html"},
	{".html", "text/html"},
	{".jpeg", "image/jpeg"},
	{".jpg", "image/jpeg"},
	{".js", "text/javascript"},
	{".json", "application/json"},
	{".mid", "audio/midi"},
	{".midi", "audio.midi"},
	{".mp3", "audio/mpeg"},
	{".mp4", "video/mp4"},
	{".mpeg", "audio/mpeg"},
	{".pdf", "application.pdf"},
	{".php", "application/x-httpd-php"},
	{".png", "image/png"},
	{".py", "text/x-python"},
	{".sh", "application/x-sh"},
	{".txt", "text/html"},
	{".wav", "audio/wave"},
	{".zip", "application/zip"}
};

const std::map<int, std::string> errorCodes {
	{301, "Moved Permanently"},			//Redirect error
	{400, "Bad Request"},				//Malformed request syntax
	{403, "Forbidden"}, 				//Understood the request but refused to fulfill it
	{404, "Not Found"}, 				//Request was not found either temporarily or permantly
	{405, "Method Not Allowed"}, 		//Method is known but not supported
	{408, "Request Timeout"}, 			//Did not receive the request in the time allowed to wait
	{411, "Length Required"}, 			//Missing content length for a body in a request
	{413, "Content Too Large"}, 		//Request content is too large. Terminate request or close connection
	{414, "URI Too Long"}, 				//It's just tooooooo long
	{500, "Internal Server Error"}, 	//Generic response when the server itself has an error
	{501, "Not Implemented"}, 			//Server does not support functionality. Includes method?
	{503, "Service Unavailable"}, 		//Server down for too much traffic or maintenance
	{505, "HTTP Version Not Supported"}	//Exactly what it sounds like
};

const std::map<int, std::string> errorHttps {
	{301, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>301 Moved Permanently</title>\r\n</head>\r\n<body>\r\n<h1>301 Moved Permanently</h1>\r\n<p>The requested document has moved permanently.</p>\r\n</body>\r\n</html>"},
    {400, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>400 Bad Request</title>\r\n</head>\r\n<body>\r\n<h1>400 Bad Request</h1>\r\n<p>Request body could not be read properly.</p>\r\n</body>\r\n</html>\r\n"},
    {403, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>403 Forbidden</title>\r\n</head>\r\n<body>\r\n<h1>403 Forbidden</h1>\r\n<p>You don't have permission to access the resource.</p>\r\n</body>\r\n</html>\r\n"},
    {404, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>404 Not Found</title>\r\n</head>\r\n<body>\r\n<h1>404 Page Not Found</h1>\r\n<p>Sorry, the resource you requested could not be found.</p>\r\n</body>\r\n</html>\r\n"},
    {405, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>405 Method Not Allowed</title>\r\n</head>\r\n<body>\r\n<h1>405 Method Not Allowed</h1>\r\n<p>The requested method is not suported.</p>\r\n</body>\r\n</html>\r\n"},
    {408, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>408 Request Timeout</title>\r\n</head>\r\n<body>\r\n<h1>408 Request Timeout</h1>\r\n<p>Failed to process request in time. Please try again.</p>\r\n</body>\r\n</html>\r\n"},
    {411, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>411 Length Required</title>\r\n</head>\r\n<body>\r\n<h1>411 Length Required</h1>\r\n<p>Requests must have a content length header.</p>\r\n</body>\r\n</html>\r\n"},
    {413, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>413 Payload Too Large</title>\r\n</head>\r\n<body>\r\n<h1>413 Payload Too Large</h1>\r\n<p>The request is larger than the server is willing or able to process.</p>\r\n</body>\r\n</html>\r\n"},
    {414, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>414 URI Too Long</title>\r\n</head>\r\n<body>\r\n<h1>414 URI Too Long</h1>\r\n<p>The URI provided was too long for the server to process.</p>\r\n</body>\r\n</html>\r\n"},
    {500, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>500 Internal Server Error</title>\r\n</head>\r\n<body>\r\n<h1>500 Internal Server Error</h1>\r\n<p>The server was unable to complete your request. Please try again later.</p>\r\n</body>\r\n</html>\r\n"},
    {501, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>501 Not Implemented</title>\r\n</head>\r\n<body>\r\n<h1>501 Not Implemented</h1>\r\n<p>The server either does not recognize the request method, or it lacks the ability to fulfil the request.</p>\r\n</body>\r\n</html>\r\n"},
    {503, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>503 Service Unavailable</title>\r\n</head>\r\n<body>\r\n<h1>503 Service Unavailable</h1>\r\n<p>The server cannot handle the request because it is overloaded or down for maintenance.</p>\r\n</body>\r\n</html>\r\n"},
    {505, "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<title>505 HTTP Version Not Supported</title>\r\n</head>\r\n<body>\r\n<h1>505 HTTP Version Not Supported</h1>\r\n<p>The server does not support the HTTP version used in the request.</p>\r\n</body>\r\n</html>\r\n"}
};

typedef enum {
	INITIAL_RECV,
	RECV_MORE,
	READY
} e_req_state;

//class Request;
//Response getResponse(std::string request, ServerConfig config, int status_code, Request *req_ref);
class Request {
public:
	Request(std::vector<ServerConfig> configs);

	// This gets called from Client when we receive new data. It then returns:
	//
	// READY: we have received a complete request and the response struct is 
	// complete and accessible by getRes(). The Client will then send the
	// response and destroy the current instance of Request.
	//
	// RECV_MORE: we still need to receive more data before forming a response.
	// The Client will attempt to receive more data end the call this function 
	// again.
	void							addToRequest(std::string part);

	// sets config to the one from all_configs that matches
	// the request host field
	void								setConfig();

	// GET calls this directly
	void 								handleCompleteRequest(int status);

	void								initialPost();
	void								handlePost();

	bool								headerIsComplete();
	void								getResponse(int status_code);
	int									getPostContentLength (std::string request);
	//bool								isPostAllowed(std::string path, ServerConfig config);
	void								validateRequest();
	void								checkLocation();
	void								handleCgi();
	void 								handleGet();
	void								respondPost();
	void 								handleDelete();
	void								createBody(std::string filename);
	void 								createHeader(std::string content_type);
	void								handleError(int status_code);
	void								getAutoIndex();
	void 								setStatusCode(int status_code);
	void								createBodyForError(std::string filename);
	void								initResponseStruct(int status_code);
	bool								methodIsNotAllowed();
	void 								printRequest(); //Remove
	void		 						handleChunked();
	void								separateMultipart();

	//Getters
	Response							getRes();
	ServerConfig						getConfig();
	std::map<std::string, std::string>	getHeaders();
	std::string							getPath();
	LocationConfig 						getLocation();

	bool								getIsCgi();

	e_req_state							getState();

	bool								getConnectionTypeIsClose();

private:
	std::vector<ServerConfig>			_all_configs;
	ServerConfig						_config;
	std::string							_raw_request;
	Response							_response;
	std::map<std::string, std::string>	_headers;
	bool								_is_cgi = false;
	bool 								_is_directory = false;
	
	std::string							_method;
	std::string							_path;
	std::string							_version;

	LocationConfig 						_location;

	int									_status_code = 200;
	std::string							_status_code_str = "OK";

	bool								_receiving_chunked = false;

	std::string							_post_body_filename;
	std::vector<PostFile>				_post_file_uploads;
	std::string							_multipart_tmp_filename;

	e_req_state							_state = INITIAL_RECV;
	size_t								_body_bytes_read = 0;

	size_t								_content_length;
	bool								_connection_type_is_close = false;
};
