#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cerrno>
#include "HttpRequest.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <map>

int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
        return EXIT_FAILURE;
    }
    int port = 8080;
    int server_fd;
    struct sockaddr_in address;
    // socklen_t addrlen = sizeof(address);
    std::vector<pollfd> poll_fds;
    pollfd server_poll_fd;

    // creation socket server :
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // preparer l adresse du serveur :
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind l adresse au socket :
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen le socket
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen failed");
        close(server_fd);
        return EXIT_FAILURE;
    }


    // Make socket non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    // boucle principale avec poll()
    server_poll_fd.fd = server_fd;
    server_poll_fd.events = POLLIN;
    poll_fds.push_back(server_poll_fd);

    std::cout << "Server listening on port " << port << std::endl;

    close(server_fd);
    return EXIT_SUCCESS;
}