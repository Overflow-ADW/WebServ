#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "webserv.hpp"

struct ServerConfig {
    int port;
    std::string host;
    std::string server_name;
    std::string root;
    std::string index;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages;
    
    ServerConfig() : port(8080), host("127.0.0.1"), server_name("localhost"), 
                   root("./www"), index("index.html"), client_max_body_size(1000000) {}
};

struct LocationConfig {
    std::string path;
    std::vector<std::string> allowed_methods;
    std::string upload_path;
    std::vector<std::string> cgi_extensions;
    std::string cgi_path;
    std::string root;
    std::string index;
    size_t client_max_body_size;
    bool autoindex;
    std::string redirect_url;
    int redirect_code;
    
    LocationConfig() : client_max_body_size(0), autoindex(false), redirect_code(0) {}
};

class ConfigParser {
private:
    std::string _config_file;
    std::vector<ServerConfig> _servers;
    std::map<std::string, std::vector<LocationConfig> > _locations; // key = server_name
    
    void parseFile();
    void parseServerBlock(std::ifstream& file, std::string& line);
    void parseLocationBlock(std::ifstream& file, std::string& line, const std::string& server_name);
    std::vector<std::string> split(const std::string& str, char delimiter);
    void trim(std::string& str);
    
public:
    ConfigParser(const std::string& config_file);
    ~ConfigParser();
    
    const std::vector<ServerConfig>& getServers() const;
    const std::vector<LocationConfig>& getLocations(const std::string& server_name) const;
    void printConfig() const; // Pour debug
};

#endif
