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
    
    // Disable copy constructor and assignment operator (C++98 style)
    Socket(const Socket& other);
    Socket& operator=(const Socket& other);
    
public:
    Socket(int port, const std::string& host);
    Socket(int existing_fd); // Pour les connexions acceptées
    ~Socket();
    
    void bind();
    void listen();
    int acceptConnection();
    
    // Getters
    int getFd() const;
    int getPort() const;
    const std::string& getHost() const;
    bool isListening() const;
    
    // Utils
    void setNonBlocking();
    void setReuseAddr();
};

#endif
