#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    try {
        if (argc != 2) {
            std::cerr << RED << "Usage: " << argv[0] << " <config_file>" << RESET << std::endl;
            return 1;
        }

        std::cout << BLUE << "Starting Webserv..." << RESET << std::endl;

        // Parsing Config
        std::cout << YELLOW << "Reading config file... " << argv[1] << RESET << std::endl;
        ConfigParser config(argv[1]);
        
        // Server Init
        std::cout << GREEN << "Initializing server..." << RESET << std::endl;
        Server server(config);
        
        // Starting Server
        std::cout << GREEN << "Server started" << RESET << std::endl;
        server.run();
        
    } catch (const std::exception& e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
        return 1;
    }
    
    return 0;
}
