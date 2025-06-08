#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Server {
private:
    const ConfigParser& _config_parser;  // Store reference to config parser
    std::vector<ServerConfig> _configs;
    std::vector<Socket*> _sockets;  // Changed to pointers
    std::map<int, Socket*> _client_sockets;
    fd_set _read_fds;
    fd_set _write_fds;
    int _max_fd;
    bool _running;
    
    // For handling partial requests (multi-read scenarios)
    std::map<int, std::string> _partial_requests;
    std::map<int, size_t> _expected_lengths;
    
    // Write buffer system for select() compliance
    std::map<int, std::string> _write_buffers;  // Pending data to write per client
    std::map<int, size_t> _write_positions;     // Current write position per client
    
    void setupSockets();
    void handleNewConnection(Socket& listening_socket);
    void handleClientRequest(int client_fd);
    void handleClientResponse(int client_fd);
    void closeConnection(int client_fd);
    
    // Select()-compliant write system
    void queueResponse(int client_fd, const std::string& response);
    void handlePendingWrites(int client_fd);
    bool hasDataToWrite(int client_fd) const;
    
    // HTTP Response helpers
    void sendSimpleResponse(int client_fd, const HttpRequest& request);
    void sendErrorResponse(int client_fd, int status_code, const std::string& message);
    void processHttpRequest(int client_fd, const HttpRequest& request);
    
    // CGI support
    bool isCgiRequest(const std::string& path, const LocationConfig& location);
    void executeCgiRequest(int client_fd, const HttpRequest& request, 
                          const ServerConfig& server_config, const LocationConfig& location_config);
    std::map<std::string, std::string> buildCgiEnvironment(const HttpRequest& request, 
                                                           const ServerConfig& server_config, 
                                                           const LocationConfig& location_config,
                                                           const std::string& script_path);
    
    // File upload support
    void handleFileUpload(int client_fd, const HttpRequest& request, 
                         const ServerConfig& server_config, const LocationConfig& location_config);
    bool parseMultipartData(const std::string& body, const std::string& boundary, 
                           const std::string& upload_path, std::vector<std::string>& uploaded_files);
    bool saveUploadedFile(const std::string& filename, const std::string& content, 
                         const std::string& upload_path);
    
    // Configuration helpers
    const ServerConfig* findServerConfig(const std::string& host, int port) const;
    const LocationConfig* findLocationConfig(const ServerConfig& server, const std::string& path) const;
    void serveStaticFile(int client_fd, const HttpRequest& request, 
                        const ServerConfig& server_config, const LocationConfig& location_config);
    
public:
    Server(const ConfigParser& config);
    ~Server();
    
    void run();
    void stop();
    
    // Getters
    bool isRunning() const;
};

#endif
