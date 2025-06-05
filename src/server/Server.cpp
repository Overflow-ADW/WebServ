#include "Server.hpp"

Server::Server(const ConfigParser& config) : _config_parser(config), _max_fd(0), _running(false) {
    _configs = config.getServers();
    setupSockets();
}

Server::~Server() {
    stop();
    
    // Clean up listening sockets
    for (size_t i = 0; i < _sockets.size(); ++i) {
        delete _sockets[i];
    }
    _sockets.clear();
}

void Server::setupSockets() {
    std::cout << YELLOW << "🔌 Setting up listening sockets..." << RESET << std::endl;
    
    for (size_t i = 0; i < _configs.size(); ++i) {
        const ServerConfig& config = _configs[i];
        
        try {
            Socket* socket = new Socket(config.port, config.host);
            _sockets.push_back(socket);
            
            int socket_fd = socket->getFd();
            if (socket_fd > _max_fd) {
                _max_fd = socket_fd;
            }
            
            std::cout << GREEN << "✅ Socket listening on " << config.host 
                      << ":" << config.port << RESET << std::endl;
                      
        } catch (const std::exception& e) {
            std::cerr << RED << "❌ Failed to create socket for port " 
                      << config.port << ": " << e.what() << RESET << std::endl;
            throw;
        }
    }
}

void Server::run() {
    _running = true;
    std::cout << GREEN << "🚀 Server is running... Press Ctrl+C to stop" << RESET << std::endl;
    
    while (_running) {
        // Recalculate max_fd each iteration
        _max_fd = 0;
        
        // Initialiser les fd_sets
        FD_ZERO(&_read_fds);
        FD_ZERO(&_write_fds);
        
        // Ajouter les sockets d'écoute
        for (size_t i = 0; i < _sockets.size(); ++i) {
            int fd = _sockets[i]->getFd();
            if (fd >= 0) {  // Check if fd is valid
                FD_SET(fd, &_read_fds);
                if (fd > _max_fd) {
                    _max_fd = fd;
                }
            }
        }
        
        // Ajouter les sockets clients
        for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); 
             it != _client_sockets.end(); ++it) {
            int fd = it->first;
            if (fd >= 0) {  // Check if fd is valid
                FD_SET(fd, &_read_fds);
                if (fd > _max_fd) {
                    _max_fd = fd;
                }
            }
        }
        
        // Si aucun socket valide, attendre un peu et continuer
        if (_max_fd == 0) {
            usleep(100000); // 100ms
            continue;
        }
        
        // Select avec timeout
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(_max_fd + 1, &_read_fds, &_write_fds, NULL, &timeout);
        
        if (activity < 0) {
            if (errno != EINTR) {
                std::cerr << RED << "❌ Select error: " << strerror(errno) << RESET << std::endl;
                break;
            }
            continue;
        }
        
        if (activity == 0) {
            continue; // Timeout, continuer la boucle
        }
        
        // Vérifier les nouvelles connexions
        for (size_t i = 0; i < _sockets.size(); ++i) {
            int fd = _sockets[i]->getFd();
            if (fd >= 0 && FD_ISSET(fd, &_read_fds)) {
                handleNewConnection(*_sockets[i]);
            }
        }
        
        // Vérifier les requêtes des clients (utiliser un vecteur pour éviter les modifications pendant l'itération)
        std::vector<int> client_fds;
        for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); 
             it != _client_sockets.end(); ++it) {
            client_fds.push_back(it->first);
        }
        
        for (size_t i = 0; i < client_fds.size(); ++i) {
            int fd = client_fds[i];
            if (_client_sockets.find(fd) != _client_sockets.end() && FD_ISSET(fd, &_read_fds)) {
                handleClientRequest(fd);
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
            
            std::cout << CYAN << "📥 New connection accepted on fd " << client_fd << RESET << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << RED << "❌ Error accepting connection: " << e.what() << RESET << std::endl;
    }
}

void Server::handleClientRequest(int client_fd) {
    std::cout << BLUE << "📨 Handling request from client " << client_fd << RESET << std::endl;
    
    // Read the request with proper handling for Content-Length
    std::string raw_request;
    char buffer[4096];
    ssize_t total_bytes_read = 0;
    ssize_t content_length = -1;
    size_t headers_end_pos = std::string::npos;
    
    // First, read until we have the headers
    while (headers_end_pos == std::string::npos) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << RED << "❌ Error reading from client " << client_fd 
                          << ": " << strerror(errno) << RESET << std::endl;
                closeConnection(client_fd);
            }
            return;
        }
        
        if (bytes_read == 0) {
            std::cout << YELLOW << "🔌 Client " << client_fd << " disconnected" << RESET << std::endl;
            closeConnection(client_fd);
            return;
        }
        
        raw_request.append(buffer, bytes_read);
        total_bytes_read += bytes_read;
        
        // Check if we have the end of headers
        headers_end_pos = raw_request.find("\r\n\r\n");
        if (headers_end_pos == std::string::npos) {
            headers_end_pos = raw_request.find("\n\n");
        }
    }
    
    // Extract Content-Length from headers if present
    size_t cl_pos = raw_request.find("Content-Length:");
    if (cl_pos == std::string::npos) {
        cl_pos = raw_request.find("content-length:");
    }
    if (cl_pos != std::string::npos) {
        size_t cl_start = raw_request.find(":", cl_pos) + 1;
        size_t cl_end = raw_request.find("\r\n", cl_start);
        if (cl_end == std::string::npos) {
            cl_end = raw_request.find("\n", cl_start);
        }
        std::string cl_str = raw_request.substr(cl_start, cl_end - cl_start);
        
        // Trim whitespace
        size_t first = cl_str.find_first_not_of(" \t");
        size_t last = cl_str.find_last_not_of(" \t");
        if (first != std::string::npos && last != std::string::npos) {
            cl_str = cl_str.substr(first, last - first + 1);
            content_length = atoi(cl_str.c_str());
        }
    }
    
    // If we have Content-Length, make sure we read the full body
    if (content_length > 0) {
        size_t headers_end_marker_len = (raw_request.find("\r\n\r\n") != std::string::npos) ? 4 : 2;
        size_t expected_total_length = headers_end_pos + headers_end_marker_len + content_length;
        
        // Continue reading until we have the full request
        while (raw_request.length() < expected_total_length) {
            memset(buffer, 0, sizeof(buffer));
            
            // Set a short timeout for non-blocking read
            struct timeval timeout;
            timeout.tv_sec = 1;  // 1 second timeout
            timeout.tv_usec = 0;
            
            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(client_fd, &read_fds);
            
            int select_result = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
            if (select_result <= 0) {
                break;
            }
            
            ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            
            if (bytes_read <= 0) {
                if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << RED << "❌ Error reading request body from client " << client_fd 
                              << ": " << strerror(errno) << RESET << std::endl;
                } else if (bytes_read == 0) {
                    std::cout << YELLOW << "⚠️  Client closed connection while reading body" << RESET << std::endl;
                }
                break;
            }
            
            raw_request.append(buffer, bytes_read);
            total_bytes_read += bytes_read;
        }
    }
    
    std::cout << CYAN << "📋 Raw request (" << total_bytes_read << " bytes):" << RESET << std::endl;
    // Only print headers for readability, not the full body which may contain binary data
    size_t body_start = raw_request.find("\r\n\r\n");
    if (body_start == std::string::npos) {
        body_start = raw_request.find("\n\n");
        if (body_start != std::string::npos) body_start += 2;
    } else {
        body_start += 4;
    }
    
    if (body_start != std::string::npos && body_start < raw_request.length()) {
        std::cout << raw_request.substr(0, body_start) << std::endl;
        std::cout << "[Body: " << (raw_request.length() - body_start) << " bytes]" << std::endl;
    } else {
        std::cout << raw_request << std::endl;
    }
    
    // Parser la requête HTTP
    HttpRequest request;
    if (request.parseRequest(raw_request)) {
        std::cout << GREEN << "✅ HTTP request parsed successfully" << RESET << std::endl;
        request.print();
        
        // Process request based on configuration
        processHttpRequest(client_fd, request);
    } else {
        std::cerr << RED << "❌ Failed to parse HTTP request" << RESET << std::endl;
        sendErrorResponse(client_fd, 400, "Bad Request");
    }
    
    // Pour le moment, fermer la connexion après la réponse
    closeConnection(client_fd);
}

void Server::handleClientResponse(int client_fd) {
    // TODO: Implémenter l'envoi des réponses HTTP
    (void)client_fd; // Éviter le warning unused parameter
}

void Server::closeConnection(int client_fd) {
    std::map<int, Socket*>::iterator it = _client_sockets.find(client_fd);
    if (it != _client_sockets.end()) {
        delete it->second;
        _client_sockets.erase(it);
        close(client_fd);
        std::cout << YELLOW << "🔌 Connection closed for fd " << client_fd << RESET << std::endl;
    }
}

void Server::stop() {
    _running = false;
    
    // Fermer toutes les connexions clients
    for (std::map<int, Socket*>::iterator it = _client_sockets.begin(); 
         it != _client_sockets.end(); ++it) {
        delete it->second;
        close(it->first);
    }
    _client_sockets.clear();
    
    std::cout << RED << "🛑 Server stopped" << RESET << std::endl;
}

bool Server::isRunning() const {
    return _running;
}

void Server::sendSimpleResponse(int client_fd, const HttpRequest& request) {
    std::string response;
    
    if (request.getMethod() == "GET" && request.getPath() == "/") {
        // Serve index.html
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += "<html><head><title>Webserv</title></head>";
        response += "<body><h1>🚀 Welcome to Webserv!</h1>";
        response += "<p>This is a simple HTTP/1.1 server implemented in C++.</p>";
        response += "<p>Method: " + request.getMethod() + "</p>";
        response += "<p>Path: " + request.getPath() + "</p>";
        response += "<p>Version: " + request.getVersion() + "</p>";
        response += "</body></html>";
    } else {
        // Simple echo response
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += "Echo: " + request.getMethod() + " " + request.getPath() + " " + request.getVersion() + "\n";
        response += "Headers:\n";
        
        const std::map<std::string, std::string>& headers = request.getHeaders();
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
             it != headers.end(); ++it) {
            response += "  " + it->first + ": " + it->second + "\n";
        }
    }
    
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << RED << "❌ Error sending response to client " << client_fd 
                  << ": " << strerror(errno) << RESET << std::endl;
    } else {
        std::cout << GREEN << "✅ Response sent to client " << client_fd 
                  << " (" << bytes_sent << " bytes)" << RESET << std::endl;
    }
}

void Server::sendErrorResponse(int client_fd, int status_code, const std::string& message) {
    std::string status_text;
    switch (status_code) {
        case 400: status_text = "Bad Request"; break;
        case 404: status_text = "Not Found"; break;
        case 500: status_text = "Internal Server Error"; break;
        default: status_text = "Error"; break;
    }
    
    std::string response;
    response = "HTTP/1.1 " + std::string(1, '0' + status_code / 100) + 
               std::string(1, '0' + (status_code / 10) % 10) + 
               std::string(1, '0' + status_code % 10) + " " + status_text + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += "<html><head><title>" + status_text + "</title></head>";
    response += "<body><h1>" + std::string(1, '0' + status_code / 100) + 
                std::string(1, '0' + (status_code / 10) % 10) + 
                std::string(1, '0' + status_code % 10) + " " + status_text + "</h1>";
    response += "<p>" + message + "</p>";
    response += "</body></html>";
    
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << RED << "❌ Error sending error response to client " << client_fd 
                  << ": " << strerror(errno) << RESET << std::endl;
    } else {
        std::cout << YELLOW << "⚠️  Error response sent to client " << client_fd 
                  << " (" << status_code << " " << status_text << ")" << RESET << std::endl;
    }
}

void Server::processHttpRequest(int client_fd, const HttpRequest& request) {
    // Find the appropriate server configuration based on the socket that received the request
    std::string host = request.getHost();
    if (host.empty()) {
        host = "localhost"; // Default host
    }
    
    // Remove port from host if present
    size_t colon_pos = host.find(':');
    int request_port = 8080; // Default
    if (colon_pos != std::string::npos) {
        std::string port_str = host.substr(colon_pos + 1);
        request_port = atoi(port_str.c_str());
        host = host.substr(0, colon_pos);
    }
    
    const ServerConfig* server_config = findServerConfig(host, request_port);
    if (!server_config) {
        std::cerr << RED << "❌ No server configuration found for host: " << host 
                  << ":" << request_port << RESET << std::endl;
        sendErrorResponse(client_fd, 500, "Internal Server Error");
        return;
    }
    
    std::cout << CYAN << "🔍 Using server config: " << server_config->server_name 
              << " (port " << server_config->port << ")" << RESET << std::endl;
    
    // Find the appropriate location configuration
    const LocationConfig* location_config = findLocationConfig(*server_config, request.getPath());
    if (!location_config) {
        std::cerr << RED << "❌ No location configuration found for path: " << request.getPath() << RESET << std::endl;
        sendErrorResponse(client_fd, 404, "Not Found");
        return;
    }
    
    std::cout << CYAN << "📍 Using location: " << location_config->path << RESET << std::endl;
    
    // Check if method is allowed
    bool method_allowed = false;
    for (size_t i = 0; i < location_config->allowed_methods.size(); ++i) {
        if (location_config->allowed_methods[i] == request.getMethod()) {
            method_allowed = true;
            break;
        }
    }
    
    if (!method_allowed) {
        std::cerr << RED << "❌ Method " << request.getMethod() << " not allowed for " << request.getPath() << RESET << std::endl;
        sendErrorResponse(client_fd, 405, "Method Not Allowed");
        return;
    }
    
    // Handle redirections
    if (!location_config->redirect_url.empty()) {
        HttpResponse response;
        response.setStatus(location_config->redirect_code, HttpResponse::getStatusText(location_config->redirect_code));
        response.setHeader("Location", location_config->redirect_url);
        response.setHeader("Connection", "close");
        response.setBody("");
        
        std::string response_str = response.toString();
        send(client_fd, response_str.c_str(), response_str.length(), 0);
        std::cout << GREEN << "↗️  Redirect sent to " << location_config->redirect_url << RESET << std::endl;
        return;
    }
    
    // Handle CGI requests
    if (isCgiRequest(request.getPath(), *location_config)) {
        executeCgiRequest(client_fd, request, *server_config, *location_config);
        return;
    }
    
    // Handle file uploads for POST requests to upload locations
    if (request.getMethod() == "POST" && !location_config->upload_path.empty()) {
        handleFileUpload(client_fd, request, *server_config, *location_config);
        return;
    }
    
    // Serve static files
    if (request.getMethod() == "GET") {
        serveStaticFile(client_fd, request, *server_config, *location_config);
    } else {
        // For now, just echo other methods
        sendSimpleResponse(client_fd, request);
    }
}

const ServerConfig* Server::findServerConfig(const std::string& host, int port) const {
    // First try to find by server_name and port
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].server_name == host && _configs[i].port == port) {
            return &_configs[i];
        }
    }
    
    // Then try to find by port only (default server for that port)
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].port == port) {
            return &_configs[i];
        }
    }
    
    // Fallback to first server
    if (!_configs.empty()) {
        return &_configs[0];
    }
    
    return NULL;
}

const LocationConfig* Server::findLocationConfig(const ServerConfig& server, const std::string& path) const {
    // Get locations for this server from the config parser
    const std::vector<LocationConfig>& locations = _config_parser.getLocations(server.server_name);
    
    const LocationConfig* best_match = NULL;
    size_t best_match_length = 0;
    
    // Find the longest matching location
    for (size_t i = 0; i < locations.size(); ++i) {
        const LocationConfig& location = locations[i];
        const std::string& location_path = location.path;
        
        // Check if the request path starts with the location path
        if (path.find(location_path) == 0) {
            // For non-root locations, ensure proper path boundary
            if (location_path != "/") {
                // The location path must be followed by '/' or end of string
                // This prevents /upload from matching /upload.html
                if (path.length() > location_path.length() && 
                    path[location_path.length()] != '/') {
                    continue; // Skip this location, not a proper match
                }
            }
            
            // Exact match or path-based match
            if (location_path.length() > best_match_length) {
                best_match = &location;
                best_match_length = location_path.length();
            }
        }
    }
    
    // If no specific location found, try to find a default "/" location
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

void Server::serveStaticFile(int client_fd, const HttpRequest& request, 
                            const ServerConfig& server_config, const LocationConfig& location_config) {
    std::string requested_path = request.getPath();
    
    // Strip query parameters from path
    size_t query_pos = requested_path.find('?');
    if (query_pos != std::string::npos) {
        requested_path = requested_path.substr(0, query_pos);
    }
    
    std::string file_path;
    
    // Build the file path
    if (requested_path == "/") {
        // Serve root index file
        file_path = server_config.root + "/" + server_config.index;
    } else if (requested_path == location_config.path || 
               (requested_path == location_config.path + "/" && location_config.path != "/")) {
        // Serve location-specific index file (handle both /cgi-bin and /cgi-bin/)
        file_path = server_config.root + location_config.path + "/" + server_config.index;
    } else {
        // Remove location prefix from path
        std::string relative_path = requested_path;
        if (requested_path.find(location_config.path) == 0 && location_config.path != "/") {
            relative_path = requested_path.substr(location_config.path.length());
            // Remove leading slash if present after removing location prefix
            if (!relative_path.empty() && relative_path[0] == '/') {
                relative_path = relative_path.substr(1);
            }
        }
        // Ensure we don't have double slashes
        if (relative_path.empty() || relative_path[0] != '/') {
            file_path = server_config.root + "/" + relative_path;
        } else {
            file_path = server_config.root + relative_path;
        }
    }
    
    std::cout << BLUE << "📁 Serving file: " << file_path << RESET << std::endl;
    
    HttpResponse response;
    if (response.serveFile(file_path)) {
        std::string response_str = response.toString();
        ssize_t bytes_sent = send(client_fd, response_str.c_str(), response_str.length(), 0);
        if (bytes_sent > 0) {
            std::cout << GREEN << "✅ File served successfully (" << bytes_sent << " bytes)" << RESET << std::endl;
        }
    } else {
        // Try to serve custom 404 page
        std::string error_page_path;
        if (server_config.error_pages.find(404) != server_config.error_pages.end()) {
            error_page_path = server_config.root + "/" + server_config.error_pages.at(404);
        }
        
        response.serveErrorPage(404, error_page_path);
        std::string response_str = response.toString();
        send(client_fd, response_str.c_str(), response_str.length(), 0);
        std::cout << YELLOW << "⚠️  404 Not Found: " << file_path << RESET << std::endl;
    }
}

bool Server::isCgiRequest(const std::string& path, const LocationConfig& location) {
    // Check if the location has CGI extensions configured
    if (location.cgi_extensions.empty() || location.cgi_path.empty()) {
        return false;
    }
    
    // Strip query string from path before checking extension
    std::string clean_path = path;
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
        clean_path = path.substr(0, query_pos);
    }
    
    // Check if the path matches the location and has a CGI extension
    for (size_t i = 0; i < location.cgi_extensions.size(); ++i) {
        const std::string& ext = location.cgi_extensions[i];
        if (clean_path.length() >= ext.length()) {
            if (clean_path.substr(clean_path.length() - ext.length()) == ext) {
                return true;
            }
        }
    }
    
    return false;
}

void Server::executeCgiRequest(int client_fd, const HttpRequest& request, 
                              const ServerConfig& server_config, const LocationConfig& location_config) {
    std::string requested_path = request.getPath();
    std::string script_path;
    
    // Strip query string from path for file system operations
    std::string clean_path = requested_path;
    size_t query_pos = requested_path.find('?');
    if (query_pos != std::string::npos) {
        clean_path = requested_path.substr(0, query_pos);
    }
    
    // Build the script path using clean path (without query string)
    std::string relative_path = clean_path;
    if (clean_path.find(location_config.path) == 0 && location_config.path != "/") {
        relative_path = clean_path.substr(location_config.path.length());
    }
    script_path = server_config.root + "/" + location_config.path.substr(1) + relative_path;
    
    std::cout << BLUE << "🐍 Executing CGI script: " << script_path << RESET << std::endl;
    std::cout << BLUE << "📜 CGI interpreter: " << location_config.cgi_path << RESET << std::endl;
    
    // Build environment variables (using original path with query string for CGI env)
    std::map<std::string, std::string> env_vars = buildCgiEnvironment(request, server_config, 
                                                                      location_config, script_path);
    
    // Execute CGI and get response
    HttpResponse response;
    if (response.executeCgi(script_path, location_config.cgi_path, env_vars, request.getBody())) {
        std::string response_str = response.toString();
        ssize_t bytes_sent = send(client_fd, response_str.c_str(), response_str.length(), 0);
        if (bytes_sent > 0) {
            std::cout << GREEN << "✅ CGI response sent successfully (" << bytes_sent << " bytes)" << RESET << std::endl;
        }
    } else {
        std::string response_str = response.toString();
        send(client_fd, response_str.c_str(), response_str.length(), 0);
        std::cout << RED << "❌ CGI execution failed: " << script_path << RESET << std::endl;
    }
}

std::map<std::string, std::string> Server::buildCgiEnvironment(const HttpRequest& request, 
                                                               const ServerConfig& server_config, 
                                                               const LocationConfig& location_config,
                                                               const std::string& script_path) {
    std::map<std::string, std::string> env_vars;
    
    // Avoid unused parameter warning
    (void)location_config;
    
    // Standard CGI environment variables
    env_vars["REQUEST_METHOD"] = request.getMethod();
    env_vars["REQUEST_URI"] = request.getPath();
    env_vars["SCRIPT_NAME"] = request.getPath();
    env_vars["SCRIPT_FILENAME"] = script_path;
    env_vars["SERVER_NAME"] = server_config.server_name;
    env_vars["SERVER_SOFTWARE"] = "Webserv/1.0";
    env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
    
    // Convert port to string
    std::ostringstream port_str;
    port_str << server_config.port;
    env_vars["SERVER_PORT"] = port_str.str();
    
    // Query string (if present)
    size_t query_pos = request.getPath().find('?');
    if (query_pos != std::string::npos) {
        env_vars["QUERY_STRING"] = request.getPath().substr(query_pos + 1);
        env_vars["REQUEST_URI"] = request.getPath().substr(0, query_pos);
        env_vars["SCRIPT_NAME"] = request.getPath().substr(0, query_pos);
    } else {
        env_vars["QUERY_STRING"] = "";
    }
    
    // Content length and type
    const std::string& body = request.getBody();
    std::ostringstream content_length;
    content_length << body.length();
    env_vars["CONTENT_LENGTH"] = content_length.str();
    
    // Always check for Content-Type header, regardless of body content
    std::string content_type = request.getHeader("content-type");
    if (!content_type.empty()) {
        env_vars["CONTENT_TYPE"] = content_type;
    }
    
    // HTTP headers (convert to HTTP_* format)
    const std::map<std::string, std::string>& headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        std::string env_name = "HTTP_" + it->first;
        
        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < env_name.length(); ++i) {
            if (env_name[i] == '-') {
                env_name[i] = '_';
            } else {
                env_name[i] = std::toupper(env_name[i]);
            }
        }
        
        env_vars[env_name] = it->second;
    }
    
    // Special handling for some headers
    std::string host = request.getHeader("Host");
    if (!host.empty()) {
        env_vars["HTTP_HOST"] = host;
        env_vars["SERVER_NAME"] = host;
    }
    
    std::string user_agent = request.getHeader("User-Agent");
    if (!user_agent.empty()) {
        env_vars["HTTP_USER_AGENT"] = user_agent;
    }
    
    return env_vars;
}

void Server::handleFileUpload(int client_fd, const HttpRequest& request, 
                             const ServerConfig& /* server_config */, const LocationConfig& location_config) {
    try {
        // Check if content type is multipart/form-data
        std::string content_type = request.getHeader("content-type");
        if (content_type.find("multipart/form-data") == std::string::npos) {
            sendErrorResponse(client_fd, 400, "Bad Request: Expected multipart/form-data");
            return;
        }
        
        // Extract boundary from content type
        std::string boundary;
        size_t boundary_pos = content_type.find("boundary=");
        if (boundary_pos == std::string::npos) {
            sendErrorResponse(client_fd, 400, "Bad Request: Missing boundary in multipart data");
            return;
        }
        boundary = content_type.substr(boundary_pos + 9); // Skip "boundary="
        
        // Remove quotes if present
        if (!boundary.empty() && boundary[0] == '"' && boundary[boundary.length() - 1] == '"') {
            boundary = boundary.substr(1, boundary.length() - 2);
        }
        
        // Get the request body
        const std::string& body = request.getBody();
        if (body.empty()) {
            sendErrorResponse(client_fd, 400, "Bad Request: Empty request body");
            return;
        }
        
        // Parse multipart data
        std::vector<std::string> uploaded_files;
        if (!parseMultipartData(body, boundary, location_config.upload_path, uploaded_files)) {
            sendErrorResponse(client_fd, 500, "Internal Server Error: Failed to process upload");
            return;
        }
        
        if (uploaded_files.empty()) {
            sendErrorResponse(client_fd, 400, "Bad Request: No files uploaded");
            return;
        }
        
        // Send success response
        std::string response_body = "<html><body><h1>Upload Successful</h1><p>Files uploaded:</p><ul>";
        for (size_t i = 0; i < uploaded_files.size(); ++i) {
            response_body += "<li>" + uploaded_files[i] + "</li>";
        }
        response_body += "</ul></body></html>";
        
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << response_body.length() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << response_body;
        
        std::string response_str = response.str();
        send(client_fd, response_str.c_str(), response_str.length(), 0);
        
    } catch (const std::exception& e) {
        sendErrorResponse(client_fd, 500, "Internal Server Error: " + std::string(e.what()));
    }
}

bool Server::parseMultipartData(const std::string& body, const std::string& boundary, 
                                const std::string& upload_path, std::vector<std::string>& uploaded_files) {
    std::string delimiter = "--" + boundary;
    
    size_t pos = 0;
    while ((pos = body.find(delimiter, pos)) != std::string::npos) {
        // Skip the delimiter
        pos += delimiter.length();
        
        // Check if this is the end delimiter
        if (body.substr(pos, 2) == "--") {
            break;
        }
        
        // Skip CRLF after delimiter
        if (pos < body.length() && body[pos] == '\r') pos++;
        if (pos < body.length() && body[pos] == '\n') pos++;
        
        // Find the end of headers (double CRLF or LF)
        size_t headers_end = body.find("\r\n\r\n", pos);
        if (headers_end == std::string::npos) {
            // Try with just LF
            headers_end = body.find("\n\n", pos);
            if (headers_end == std::string::npos) {
                continue;
            }
        }
        
        // Extract headers
        std::string headers = body.substr(pos, headers_end - pos);
        
        // Parse Content-Disposition header to get filename
        std::string filename;
        size_t disp_pos = headers.find("Content-Disposition:");
        if (disp_pos != std::string::npos) {
            size_t filename_pos = headers.find("filename=\"", disp_pos);
            if (filename_pos != std::string::npos) {
                filename_pos += 10; // Skip 'filename="'
                size_t filename_end = headers.find("\"", filename_pos);
                if (filename_end != std::string::npos) {
                    filename = headers.substr(filename_pos, filename_end - filename_pos);
                }
            }
        }
        
        // Skip headers and appropriate line endings
        if (body.find("\r\n\r\n", pos) == headers_end) {
            pos = headers_end + 4; // CRLF
        } else {
            pos = headers_end + 2; // LF only
        }
        
        // Find the next boundary to get the file content
        size_t content_end = body.find("\r\n--" + boundary, pos);
        if (content_end == std::string::npos) {
            content_end = body.find("\n--" + boundary, pos);
            if (content_end == std::string::npos) {
                break;
            }
        }
        
        // Extract file content
        std::string file_content = body.substr(pos, content_end - pos);
        
        // Save file if we have a filename
        if (!filename.empty()) {
            if (file_content.empty()) {
                std::cout << YELLOW << "⚠️  Warning: Empty file content for " << filename << std::endl;
                // Still save empty files - some users might want to upload empty files
            }
            if (saveUploadedFile(filename, file_content, upload_path)) {
                uploaded_files.push_back(filename);
            }
        }
        
        pos = content_end;
    }
    
    return !uploaded_files.empty();
}

bool Server::saveUploadedFile(const std::string& filename, const std::string& content, 
                             const std::string& upload_path) {
    // Create full file path
    std::string file_path = upload_path + "/" + filename;
    
    // Open file for writing
    std::ofstream file(file_path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << file_path << std::endl;
        return false;
    }
    
    // Write content to file
    file.write(content.c_str(), content.length());
    file.close();
    
    if (file.fail()) {
        std::cerr << "Error: Failed to write file: " << file_path << std::endl;
        return false;
    }
    
    std::cout << "File uploaded successfully: " << file_path << std::endl;
    return true;
}
