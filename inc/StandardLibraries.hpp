#pragma once


#include <algorithm>
#include <asm-generic/socket.h>
#include <cstddef> // for size_t?
#include <cstdio> // remove
#include <exception>
#include <fcntl.h> // fcntl() // only for MacOS?
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <regex>
#include <sstream> // for istrinstream
#include <stdexcept>
#include <string>
#include <string.h> //memset
//#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <netdb.h> // struct addrinfo
#include <unistd.h> // 
#include <vector>
#include <sys/wait.h>

