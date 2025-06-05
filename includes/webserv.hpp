#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// Standard C++ includes (C++98 compatible)
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <exception>
#include <ctime>

// System includes for networking
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>  // Pour strerror
#include <sys/wait.h>  // Pour waitpid
#include <cstdlib>     // Pour setenv
#include <sys/stat.h>  // Pour stat et S_ISDIR
#include <cctype>      // Pour isspace, tolower, toupper
#include <cstdio>      // Pour sprintf

// Defines and constants
#define MAX_CONNECTIONS 1024
#define BUFFER_SIZE 8192
#define DEFAULT_PORT 8080
#define DEFAULT_HOST "127.0.0.1"

// Colors for debug output
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

// Forward declarations
class Server;
class ConfigParser;
class HttpRequest;
class HttpResponse;
class Socket;

#endif
