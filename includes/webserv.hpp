#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <exception>
#include <ctime>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/stat.h>
#include <cctype>
#include <cstdio>

#define MAX_CONNECTIONS 1024

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

class Server;
class ConfigParser;
class HttpRequest;
class HttpResponse;
class Socket;
class Cgi;
class FileUploadHandler;
class ResponseHandler;
class Utils;


#endif
