#include "Socket.hpp"

Socket::Socket(int port, const std::string& host) : _port(port), _host(host), _is_listening(false) {
    // Créer le socket
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    
    // Configuration de l'adresse
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_port);
    
    if (_host == "localhost" || _host == "127.0.0.1") {
        _address.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, _host.c_str(), &_address.sin_addr) <= 0) {
            close(_fd);
            throw std::runtime_error("Invalid host address: " + _host);
        }
    }
    
    // Options du socket
    setReuseAddr();
    setNonBlocking();
    
    // Bind et listen
    bind();
    listen();
}

Socket::Socket(int existing_fd) : _fd(existing_fd), _port(0), _host(""), _is_listening(false) {
    setNonBlocking();
}

Socket::~Socket() {
    if (_fd >= 0) {
        close(_fd);
    }
}

void Socket::bind() {
    if (::bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
        close(_fd);
        std::ostringstream oss;
        oss << "Failed to bind socket to port " << _port << ": " << strerror(errno);
        throw std::runtime_error(oss.str());
    }
}

void Socket::listen() {
    if (::listen(_fd, MAX_CONNECTIONS) < 0) {
        close(_fd);
        throw std::runtime_error("Failed to listen on socket");
    }
    _is_listening = true;
}

int Socket::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -1; // Pas de connexion en attente
        }
        throw std::runtime_error("Failed to accept connection");
    }
    
    return client_fd;
}

int Socket::getFd() const {
    return _fd;
}

int Socket::getPort() const {
    return _port;
}

const std::string& Socket::getHost() const {
    return _host;
}

bool Socket::isListening() const {
    return _is_listening;
}

void Socket::setNonBlocking() {
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags < 0) {
        throw std::runtime_error("Failed to get socket flags");
    }
    
    if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw std::runtime_error("Failed to set socket non-blocking");
    }
}

void Socket::setReuseAddr() {
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_fd);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }
}
