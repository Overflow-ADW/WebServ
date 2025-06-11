#include "Server.hpp"

Server::Server(const ConfigParser& config) : _config_parser(config), _max_fd(0), _running(false) {
    _configs = config.getServers();
    setupSockets();
}

Server::~Server() {
    stop();
    
    for (size_t i = 0; i < _sockets.size(); ++i) {
        delete _sockets[i];
    }
    _sockets.clear();
}

void Server::setupSockets() {
    std::cout << YELLOW << "Setting up listening sockets..." << RESET << std::endl;
    
    for (size_t i = 0; i < _configs.size(); ++i) {
        const ServerConfig& config = _configs[i];
        
        try {
            Socket* socket = new Socket(config.port, config.host);
            _sockets.push_back(socket);
            
            int socket_fd = socket->getFd();
            if (socket_fd > _max_fd)
                _max_fd = socket_fd;
            
            std::cout << GREEN << "Socket listening on " << config.host << ":" << config.port << RESET << std::endl;
                      
        } catch (const std::exception& e) {
            std::cerr << RED << "Failed to create socket for port " << config.port << ": " << e.what() << RESET << std::endl;
            throw;
        }
    }
}

void Server::run() {
    _running = true;
    std::cout << GREEN << "Server is running... Press Ctrl+C to stop" << RESET << std::endl;
    
    while (_running) {
        _max_fd = 0;
        
        FD_ZERO(&_read_fds);
        FD_ZERO(&_write_fds);
        
        for (size_t i = 0; i < _sockets.size(); ++i) {
            int fd = _sockets[i]->getFd();
            if (fd >= 0) {
                FD_SET(fd, &_read_fds);
                if (fd > _max_fd)
                    _max_fd = fd;
            }
        }
        
        for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); 
             it != _client_sockets.end(); ++it) {
            int fd = it->first;
            if (fd >= 0) { 
                FD_SET(fd, &_read_fds);
                
                if (_response_handler.hasDataToWrite(fd))
                    FD_SET(fd, &_write_fds);
                
                if (fd > _max_fd)
                    _max_fd = fd;
            }
        }
        
        if (_max_fd == 0) {
            usleep(100000);
            continue;
        }
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(_max_fd + 1, &_read_fds, &_write_fds, NULL, &timeout);
        
        if (activity < 0) {
            if (errno != EINTR) {
                std::cerr << RED << "Select error: " << strerror(errno) << RESET << std::endl;
                break;
            }
            continue;
        }
        
        if (activity == 0) {
            time_t current_time = time(NULL);
            std::vector<int> timed_out_clients;
            
            for (std::map<int, time_t>::iterator it = _client_timestamps.begin(); it != _client_timestamps.end(); ++it) {
                if (current_time - it->second > 30)
                    timed_out_clients.push_back(it->first);
            }
            
            for (size_t i = 0; i < timed_out_clients.size(); ++i) {
                int client_fd = timed_out_clients[i];
                std::cout << YELLOW << "Client " << client_fd << " timed out - closing connection" << RESET << std::endl;
                _response_handler.sendErrorResponse(client_fd, 408, "Request Timeout");
                closeConnection(client_fd);
            }
            
            continue; 
        }
        
        for (size_t i = 0; i < _sockets.size(); ++i) {
            int fd = _sockets[i]->getFd();
            if (fd >= 0 && FD_ISSET(fd, &_read_fds))
                handleNewConnection(*_sockets[i]);
        }
        
        std::vector<int> client_fds;
        for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); it != _client_sockets.end(); ++it)
            client_fds.push_back(it->first);
        
        for (size_t i = 0; i < client_fds.size(); ++i) {
            int fd = client_fds[i];
            if (_client_sockets.find(fd) != _client_sockets.end()) {
                if (FD_ISSET(fd, &_read_fds))
                    handleClientRequest(fd);
                if (FD_ISSET(fd, &_write_fds)) {
                    int write_result = _response_handler.handlePendingWrites(fd);
                    if (write_result != 0)
                        closeConnection(fd);
                }
            }
        }
    }
}

void Server::handleNewConnection(Socket& listening_socket) {
    try {
        int client_fd = listening_socket.acceptConnection();
        if (client_fd > 0) {
            Socket* client_socket = new Socket(client_fd);
            _client_sockets[client_fd] = client_socket;
            _client_timestamps[client_fd] = time(NULL);
            
            _client_listening_ports[client_fd] = listening_socket.getPort();
            
            std::cout << CYAN << "New connection accepted on fd " << client_fd << " (listening port: " << listening_socket.getPort() << ")" << RESET << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << RED << "Error accepting connection: " << e.what() << RESET << std::endl;
    }
}

void Server::handleClientRequest(int client_fd) {
    std::cout << BLUE << "Handling request from client " << client_fd << RESET << std::endl;
    
    time_t current_time = time(NULL);
    if (_client_timestamps.find(client_fd) != _client_timestamps.end()) {
        time_t client_start_time = _client_timestamps[client_fd];
        if (current_time - client_start_time > 30) {
            std::cerr << RED << "Client " << client_fd << " timeout exceeded (30s)" << RESET << std::endl;
            _response_handler.sendErrorResponse(client_fd, 408, "Request Timeout");
            closeConnection(client_fd);
            return;
        }
    }
    
    std::string raw_request;
    if (_partial_requests.find(client_fd) != _partial_requests.end())
        raw_request = _partial_requests[client_fd];
    
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read < 0) {
        std::cerr << RED << "Unexpected error reading from client " << client_fd << RESET << std::endl;
        closeConnection(client_fd);
        return;
    }
    
    if (bytes_read == 0) {
        std::cout << YELLOW << "Client " << client_fd << " disconnected" << RESET << std::endl;
        closeConnection(client_fd);
        return;
    }
    
    raw_request.append(buffer, bytes_read);
    
    size_t headers_end_pos = raw_request.find("\r\n\r\n");
    if (headers_end_pos == std::string::npos) {
        _partial_requests[client_fd] = raw_request;
        return;
    }
    
    ssize_t content_length = -1;
    std::istringstream header_stream(raw_request);
    std::string line;
    while (std::getline(header_stream, line) && !line.empty()) {
        if (line.find("Content-Length:") == 0 || line.find("content-length:") == 0) {
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string length_str = line.substr(colon_pos + 1);
                size_t start = length_str.find_first_not_of(" \t\r\n");
                size_t end = length_str.find_last_not_of(" \t\r\n");
                if (start != std::string::npos && end != std::string::npos) {
                    length_str = length_str.substr(start, end - start + 1);
                    content_length = std::atoi(length_str.c_str());
                }
            }
            break;
        }
    }
    
    if (content_length > 0) {
        size_t expected_total_length = headers_end_pos + 4 + content_length;
        
        if (raw_request.length() < expected_total_length) {
            _partial_requests[client_fd] = raw_request;
            _expected_lengths[client_fd] = expected_total_length;
            return;
        }
    }
    
    _partial_requests.erase(client_fd);
    _expected_lengths.erase(client_fd);
    
    
    std::cout << CYAN << "Raw request (" << raw_request.length() << " bytes):" << RESET << std::endl;
    size_t body_start = raw_request.find("\r\n\r\n");
    if (body_start != std::string::npos)
        body_start += 4;
    
    if (body_start != std::string::npos && body_start < raw_request.length()) {
        std::cout << raw_request.substr(0, body_start) << std::endl;
        std::cout << "[Body: " << (raw_request.length() - body_start) << " bytes]" << std::endl;
    }
    else
        std::cout << raw_request << std::endl;
    
    HttpRequest request;
    if (request.parseRequest(raw_request)) {
        std::cout << GREEN << "HTTP request parsed successfully" << RESET << std::endl;
        request.print();
        
        if (request.getPath().length() > 2048) {
            std::cerr << RED << "URI too long: " << request.getPath().length() << " characters" << RESET << std::endl;
            _response_handler.sendErrorResponse(client_fd, 414, "URI Too Long");
            return;
        }
        
        const std::string& method = request.getMethod();
        if (method != "GET" && method != "POST" && method != "DELETE") {
            std::cerr << RED << "Method not implemented: " << method << RESET << std::endl;
            _response_handler.sendErrorResponse(client_fd, 501, "Not Implemented");
            return;
        }
        
        processHttpRequest(client_fd, request);
    }
    else {
        std::cerr << RED << "Failed to parse HTTP request" << RESET << std::endl;
        _response_handler.sendErrorResponse(client_fd, 400, "Bad Request");
    }
}

void Server::closeConnection(int client_fd) {
    std::map<int, Socket*>::iterator it = _client_sockets.find(client_fd);
    if (it != _client_sockets.end()) {
        delete it->second;
        _client_sockets.erase(it);
        close(client_fd);
        std::cout << YELLOW << "Connection closed for fd " << client_fd << RESET << std::endl;
    }
    
    _response_handler.cleanupClient(client_fd);
    
    _partial_requests.erase(client_fd);
    _expected_lengths.erase(client_fd);
    _client_timestamps.erase(client_fd);
    _client_listening_ports.erase(client_fd);
}

void Server::stop() {
    _running = false;
    
    for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); 
         it != _client_sockets.end(); ++it) {
        delete it->second;
        close(it->first);
    }
    _client_sockets.clear();
    
    _partial_requests.clear();
    _expected_lengths.clear();
    _client_timestamps.clear();
    _client_listening_ports.clear();
    
    std::cout << RED << "Server stopped" << RESET << std::endl;
}

bool Server::isRunning() const {
    return _running;
}

void Server::processHttpRequest(int client_fd, const HttpRequest& request) {
    std::string host = request.getHost();
    
    size_t colon_pos = host.find(':');
    int request_port = -1;
    if (colon_pos != std::string::npos) {
        std::string port_str = host.substr(colon_pos + 1);
        request_port = atoi(port_str.c_str());
        host = host.substr(0, colon_pos);
    }
    
    if (request_port == -1) {
        std::map<int, int>::iterator port_it = _client_listening_ports.find(client_fd);
        if (port_it != _client_listening_ports.end()) {
            request_port = port_it->second;
        } else
            request_port = 8080;
    }
    
    const ServerConfig* server_config = findServerConfig(host, request_port);
    if (!server_config) {
        std::cerr << RED << "No server configuration found for host: " << host << ":" << request_port << RESET << std::endl;
        _response_handler.sendErrorResponse(client_fd, 500, "Internal Server Error");
        return;
    }
    
    std::cout << CYAN << "Using server config: " << server_config->server_name << " (port " << server_config->port << ")" << RESET << std::endl;
    
    const LocationConfig* location_config = findLocationConfig(*server_config, request.getPath());
    if (!location_config) {
        std::cerr << RED << "No location configuration found for path: " << request.getPath() << RESET << std::endl;
        _response_handler.sendErrorResponse(client_fd, 404, "Not Found");
        return;
    }
    
    std::cout << CYAN << "Using location: " << location_config->path << RESET << std::endl;
    
    const std::string& body = request.getBody();
    
    if (body.length() > server_config->client_max_body_size) {
        std::cerr << RED << "Request body too large: " << body.length() 
                  << " bytes (limit: " << server_config->client_max_body_size << " bytes)" << RESET << std::endl;
        _response_handler.sendErrorResponse(client_fd, 413, "Payload Too Large");
        
        std::cout << YELLOW << "Closing connection due to payload too large" << RESET << std::endl;
        closeConnection(client_fd);
        return;
    }
    
    if (!location_config->redirect_url.empty() && location_config->redirect_code >= 300 && location_config->redirect_code < 400) {
        
        HttpResponse response;
        response.setStatus(location_config->redirect_code, HttpResponse::getStatusText(location_config->redirect_code));
        response.setHeader("Location", location_config->redirect_url);
        response.setHeader("Connection", "close");
        response.setBody("");
        
        std::string response_str = response.toString();
        _response_handler.queueResponse(client_fd, response_str);
        std::cout << GREEN << "Redirect queued to " << location_config->redirect_url << RESET << std::endl;
        return;
    }
    
    if (Cgi::isCgiRequest(request.getPath(), *location_config)) {
        HttpResponse response;
        if (_cgi.executeCgiRequest(request, *server_config, *location_config, response)) {
            std::string response_str = response.toString();
            _response_handler.queueResponse(client_fd, response_str);
            std::cout << GREEN << "CGI response queued (" << response_str.length() << " bytes)" << RESET << std::endl;
        }
        else {
            std::string response_str = response.toString();
            _response_handler.queueResponse(client_fd, response_str);
            std::cout << RED << "CGI execution failed" << RESET << std::endl;
        }
        return;
    }
    
    bool method_allowed = false;
    for (size_t i = 0; i < location_config->allowed_methods.size(); ++i) {
        if (location_config->allowed_methods[i] == request.getMethod()) {
            method_allowed = true;
            break;
        }
    }
    
    if (!method_allowed) {
        std::cerr << RED << "Method " << request.getMethod() << " not allowed for " << request.getPath() << RESET << std::endl;
        _response_handler.sendErrorResponse(client_fd, 405, "Method Not Allowed");
        return;
    }
    
    if (FileUploadHandler::isFileUpload(request, *location_config)) {
        std::string response_html;
        if (_upload_handler.handleFileUpload(request, *location_config, response_html)) {
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << response_html.length() << "\r\n";
            response << "Connection: close\r\n";
            response << "\r\n";
            response << response_html;
            
            std::string response_str = response.str();
            _response_handler.queueResponse(client_fd, response_str);
            std::cout << GREEN << "Upload success response queued (" << response_str.length() << " bytes)" << RESET << std::endl;
        }
        else {
            std::ostringstream response;
            response << "HTTP/1.1 400 Bad Request\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << response_html.length() << "\r\n";
            response << "Connection: close\r\n";
            response << "\r\n";
            response << response_html;
            
            std::string response_str = response.str();
            _response_handler.queueResponse(client_fd, response_str);
            std::cout << RED << "Upload error response queued" << RESET << std::endl;
        }
        return;
    }
    
    if (request.getMethod() == "GET")
        serveStaticFile(client_fd, request, *server_config, *location_config);
    else
        _response_handler.sendSimpleResponse(client_fd, request);
}

const ServerConfig* Server::findServerConfig(const std::string& host, int port) const {
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].server_name == host && _configs[i].port == port)
            return &_configs[i];
    }
    
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].port == port)
            return &_configs[i];
    }
    
    if (!_configs.empty())
        return &_configs[0];
    
    return NULL;
}

const LocationConfig* Server::findLocationConfig(const ServerConfig& server, const std::string& path) const {
    const std::vector<LocationConfig>& locations = _config_parser.getLocations(server.server_name);
    
    const LocationConfig* best_match = NULL;
    size_t best_match_length = 0;
    
    for (size_t i = 0; i < locations.size(); ++i) {
        const LocationConfig& location = locations[i];
        const std::string& location_path = location.path;
        
        if (path.find(location_path) == 0) {
            if (location_path != "/") {
                if (path.length() > location_path.length() && 
                    path[location_path.length()] != '/') {
                    continue;
                }
            }
            
            if (location_path.length() > best_match_length) {
                best_match = &location;
                best_match_length = location_path.length();
            }
        }
    }
    
    if (!best_match) {
        for (size_t i = 0; i < locations.size(); ++i) {
            if (locations[i].path == "/") {
                best_match = &locations[i];
                break;
            }
        }
    }
    
    return best_match;
}

void Server::serveStaticFile(int client_fd, const HttpRequest& request, const ServerConfig& server_config, const LocationConfig& location_config) {
    std::string requested_path = request.getPath();
    
    size_t query_pos = requested_path.find('?');
    if (query_pos != std::string::npos)
        requested_path = requested_path.substr(0, query_pos);
    
    std::string file_path;
    
    if (requested_path == "/")
        file_path = server_config.root + "/" + server_config.index;
    else if (requested_path == location_config.path || (requested_path == location_config.path + "/" && location_config.path != "/"))
        file_path = server_config.root + location_config.path + "/" + server_config.index;
    else {
        std::string relative_path = requested_path;
        if (requested_path.find(location_config.path) == 0 && location_config.path != "/") {
            relative_path = requested_path.substr(location_config.path.length());
            if (!relative_path.empty() && relative_path[0] == '/')
                relative_path = relative_path.substr(1);
        }
        if (relative_path.empty() || relative_path[0] != '/')
            file_path = server_config.root + "/" + relative_path;
        else
            file_path = server_config.root + relative_path;
    }
    
    std::cout << BLUE << "Serving file: " << file_path << RESET << std::endl;
    
    HttpResponse response;
    if (response.serveFile(file_path)) {
        std::string response_str = response.toString();
        _response_handler.queueResponse(client_fd, response_str);
        std::cout << GREEN << "File response queued (" << response_str.length() << " bytes)" << RESET << std::endl;
    }
    else {
        std::string error_page_path;
        if (server_config.error_pages.find(404) != server_config.error_pages.end())
            error_page_path = server_config.root + "/" + server_config.error_pages.at(404);
        
        response.serveErrorPage(404, error_page_path);
        std::string response_str = response.toString();
        _response_handler.queueResponse(client_fd, response_str);
        std::cout << YELLOW << "404 Not Found queued: " << file_path << RESET << std::endl;
    }
}
