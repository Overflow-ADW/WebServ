#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"

class Socket {
private:
    int _fd;
    int _port;
    std::string _host;
    struct sockaddr_in _address;
    bool _is_listening;
    
    Socket(const Socket& other);
    Socket& operator=(const Socket& other);
    
public:
    Socket(int port, const std::string& host);
    Socket(int existing_fd);
    ~Socket();
    
    void bind();
    void listen();
    int acceptConnection();
    
    int getFd() const;
    int getPort() const;
    const std::string& getHost() const;
    bool isListening() const;
    
    void setNonBlocking();
    void setReuseAddr();
};

#endif
