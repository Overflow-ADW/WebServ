#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler() {
}

ResponseHandler::~ResponseHandler() {
}

void ResponseHandler::queueResponse(int client_fd, const std::string& response) {
    _write_buffers[client_fd] = response;
    _write_positions[client_fd] = 0;
    
    std::cout << BLUE << "Response queued for client " << client_fd 
              << " (" << response.length() << " bytes)" << RESET << std::endl;
}

int ResponseHandler::handlePendingWrites(int client_fd) {
    std::map<int, std::string>::iterator buffer_it = _write_buffers.find(client_fd);
    std::map<int, size_t>::iterator pos_it = _write_positions.find(client_fd);
    
    if (buffer_it == _write_buffers.end() || pos_it == _write_positions.end()) {
        return 0;
    }
    
    const std::string& buffer = buffer_it->second;
    size_t& position = pos_it->second;
    
    if (position >= buffer.length()) {
        _write_buffers.erase(buffer_it);
        _write_positions.erase(pos_it);
        std::cout << GREEN << "All data already sent to client " << client_fd 
                  << " - connection ready for next request" << RESET << std::endl;
        return 0;
    }
    
    size_t remaining = buffer.length() - position;
    ssize_t bytes_sent = send(client_fd, buffer.c_str() + position, remaining, 0);
    
    if (bytes_sent < 0) {
        std::cerr << RED << "Error sending data to client " << client_fd << RESET << std::endl;
        _write_buffers.erase(buffer_it);
        _write_positions.erase(pos_it);
        return -1;
    }
    
    if (bytes_sent == 0) {
        std::cout << YELLOW << "Client " << client_fd << " closed connection during write" << RESET << std::endl;
        _write_buffers.erase(buffer_it);
        _write_positions.erase(pos_it);
        return -1;
    }
    
    position += bytes_sent;
    
    if (position >= buffer.length()) {
        _write_buffers.erase(buffer_it);
        _write_positions.erase(pos_it);
        std::cout << GREEN << "All data sent to client " << client_fd << RESET << std::endl;
    }
    
    return 0;
}

bool ResponseHandler::hasDataToWrite(int client_fd) const {
    std::map<int, std::string>::const_iterator it = _write_buffers.find(client_fd);
    if (it == _write_buffers.end()) {
        return false;
    }
    
    std::map<int, size_t>::const_iterator pos_it = _write_positions.find(client_fd);
    if (pos_it == _write_positions.end()) {
        return false;
    }
    
    return pos_it->second < it->second.length();
}

void ResponseHandler::sendSimpleResponse(int client_fd, const HttpRequest& request) {
    std::string response;
    
    if (request.getMethod() == "GET" && request.getPath() == "/") {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += "<html><head><title>Webserv</title></head>";
        response += "<body><h1>Welcome to Webserv!</h1>";
        response += "<p>This is a simple HTTP/1.1 server implemented in C++.</p>";
        response += "<p>Method: " + request.getMethod() + "</p>";
        response += "<p>Path: " + request.getPath() + "</p>";
        response += "<p>Version: " + request.getVersion() + "</p>";
        response += "</body></html>";
    } else {
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
    
    queueResponse(client_fd, response);
}

void ResponseHandler::sendErrorResponse(int client_fd, int status_code, const std::string& message) {
    std::string status_text;
    switch (status_code) {
        case 400: status_text = "Bad Request"; break;
        case 404: status_text = "Not Found"; break;
        case 405: status_text = "Method Not Allowed"; break;
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
    
    queueResponse(client_fd, response);
}

void ResponseHandler::cleanupClient(int client_fd) {
    _write_buffers.erase(client_fd);
    _write_positions.erase(client_fd);
}

const std::map<int, std::string>& ResponseHandler::getWriteBuffers() const {
    return _write_buffers;
}
