#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Cgi.hpp"
#include "FileUploadHandler.hpp"
#include "ResponseHandler.hpp"
#include "FileUploadHandler.hpp"

class Server {
private:
    const ConfigParser& _config_parser;
    std::vector<ServerConfig> _configs;
    std::vector<Socket*> _sockets;
    std::map<int, Socket*> _client_sockets;
    fd_set _read_fds;
    fd_set _write_fds;
    int _max_fd;
    bool _running;
    
    std::map<int, std::string> _partial_requests;
    std::map<int, size_t> _expected_lengths;
    std::map<int, time_t> _client_timestamps;
    
    Cgi _cgi;
    FileUploadHandler _upload_handler;
    ResponseHandler _response_handler;
    
    void setupSockets();
    void handleNewConnection(Socket& listening_socket);
    void handleClientRequest(int client_fd);
    void closeConnection(int client_fd);
    
    void processHttpRequest(int client_fd, const HttpRequest& request);
    
    const ServerConfig* findServerConfig(const std::string& host, int port) const;
    const LocationConfig* findLocationConfig(const ServerConfig& server, const std::string& path) const;
    void serveStaticFile(int client_fd, const HttpRequest& request, 
                        const ServerConfig& server_config, const LocationConfig& location_config);
    
public:
    Server(const ConfigParser& config);
    ~Server();
    
    void run();
    void stop();
    
    bool isRunning() const;
};

#endif
