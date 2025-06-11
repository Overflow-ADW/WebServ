#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"

class ResponseHandler {
private:
    std::map<int, std::string> _write_buffers;
    std::map<int, size_t> _write_positions;

public:
    ResponseHandler();
    ~ResponseHandler();
    
    void queueResponse(int client_fd, const std::string& response);
    
    int handlePendingWrites(int client_fd);
    
    bool hasDataToWrite(int client_fd) const;
    
    void sendSimpleResponse(int client_fd, const HttpRequest& request);
    
    void sendErrorResponse(int client_fd, int status_code, const std::string& message);
    
    void cleanupClient(int client_fd);
    
    const std::map<int, std::string>& getWriteBuffers() const;
};

#endif
