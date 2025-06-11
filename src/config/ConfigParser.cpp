#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& config_file) : _config_file(config_file) {
    std::cout << YELLOW << "Parsing config file: " << config_file << RESET << std::endl;
    parseFile();
    
    printConfig();
}

ConfigParser::~ConfigParser() {
}

void ConfigParser::parseFile() {
    std::ifstream file(_config_file.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + _config_file);
    
    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
            parseServerBlock(file);
    }
    
    if (_servers.empty())
        throw std::runtime_error("No server config found");
    
    std::cout << GREEN << "Configuration parsed successfully. Found " << _servers.size() << " server(s)" << RESET << std::endl;
}

void ConfigParser::parseServerBlock(std::ifstream& file) {
    ServerConfig server_config;
    std::string current_line;
    
    std::cout << YELLOW << " Parsing server block..." << RESET << std::endl;
    
    while (std::getline(file, current_line)) {
        trim(current_line);
        
        if (current_line.empty() || current_line[0] == '#')
            continue;
        
        if (current_line == "}")
            break;
        
        if (current_line.find("location") != std::string::npos && current_line.find("{") != std::string::npos) {
            parseLocationBlock(file, current_line, server_config.server_name);
            continue;
        }
        
        std::vector<std::string> tokens = split(current_line, ' ');
        if (tokens.empty()) 
            continue;
        
        if (tokens[0] == "listen" && tokens.size() >= 2) {
            server_config.port = std::atoi(tokens[1].c_str());
            std::cout << GREEN << "    Port: " << server_config.port << RESET << std::endl;
            
        } 
        else if (tokens[0] == "server_name" && tokens.size() >= 2) {
            server_config.server_name = tokens[1];
            std::cout << GREEN << "    Server name: " << server_config.server_name << RESET << std::endl; 
        }
        else if (tokens[0] == "root" && tokens.size() >= 2) {
            server_config.root = tokens[1];
            std::cout << GREEN << "    Root: " << server_config.root << RESET << std::endl;
        } 
        else if (tokens[0] == "index" && tokens.size() >= 2) {
            server_config.index = tokens[1];
            std::cout << GREEN << "    Index: " << server_config.index << RESET << std::endl;
        } 
        else if (tokens[0] == "client_max_body_size" && tokens.size() >= 2) {
            server_config.client_max_body_size = std::atoi(tokens[1].c_str());
            std::cout << GREEN << "    Max body size: " << server_config.client_max_body_size << " bytes" << RESET << std::endl;
        } 
        else if (tokens[0] == "error_page" && tokens.size() >= 3) {
            int error_code = std::atoi(tokens[1].c_str());
            std::string error_page = tokens[2];
            server_config.error_pages[error_code] = error_page;
            std::cout << GREEN << "    Error page " << error_code << ": " << error_page << RESET << std::endl;
        }
        else if (tokens[0] == "host" && tokens.size() >= 2){
            server_config.host = tokens[1];
            std::cout << GREEN << "    Host: " << server_config.host << RESET << std::endl;
        }
        else
            std::cout << YELLOW << "    Unknown directive: " << tokens[0] << RESET << std::endl;
    }
    
    _servers.push_back(server_config);
    std::cout << GREEN << "  Server configuration added" << RESET << std::endl;
}

void ConfigParser::parseLocationBlock(std::ifstream& file, std::string& line, const std::string& server_name) {
    LocationConfig location_config;
    std::string current_line = line;
    
    size_t start = current_line.find("location") + 8;
    size_t end = current_line.find("{");
    if (start != std::string::npos && end != std::string::npos) {
        std::string path_part = current_line.substr(start, end - start);
        trim(path_part);
        location_config.path = path_part;
    }
    
    std::cout << CYAN << "Parsing location block: " << location_config.path << RESET << std::endl;
    
    while (std::getline(file, current_line)) {
        trim(current_line);
        
        if (current_line.empty() || current_line[0] == '#')
            continue;
        
        if (current_line == "}")
            break;
        
        std::vector<std::string> tokens = split(current_line, ' ');
        if (tokens.empty()) 
            continue;
        
        if (tokens[0] == "allowed_methods" && tokens.size() >= 2) {
            for (size_t i = 1; i < tokens.size(); ++i)
                location_config.allowed_methods.push_back(tokens[i]);

            std::cout << CYAN << "      Allowed methods: ";
            for (size_t i = 0; i < location_config.allowed_methods.size(); ++i) {
                std::cout << location_config.allowed_methods[i];
                if (i < location_config.allowed_methods.size() - 1)
                    std::cout << ", ";
            }
            std::cout << RESET << std::endl;
            
        } 
        else if (tokens[0] == "upload_path" && tokens.size() >= 2) {
            location_config.upload_path = tokens[1];
            std::cout << CYAN << "      Upload path: " << location_config.upload_path << RESET << std::endl;
        } 

        else if (tokens[0] == "cgi_extension" && tokens.size() >= 2){
            for (size_t i = 1; i < tokens.size(); ++i)
                location_config.cgi_extensions.push_back(tokens[i]);
                
            std::cout << CYAN << "      CGI extensions: ";
            for (size_t i = 0; i < location_config.cgi_extensions.size(); ++i) {
                std::cout << location_config.cgi_extensions[i];
                if (i < location_config.cgi_extensions.size() - 1) 
                    std::cout << ", ";
            }
            std::cout << RESET << std::endl;    
        } 

        else if (tokens[0] == "cgi_path" && tokens.size() >= 2) {
            location_config.cgi_path = tokens[1];
            std::cout << CYAN << "      CGI path: " << location_config.cgi_path << RESET << std::endl;
        } 

        else if (tokens[0] == "autoindex" && tokens.size() >= 2) {
            location_config.autoindex = (tokens[1] == "on" || tokens[1] == "true");
            std::cout << CYAN << "      Autoindex: " << (location_config.autoindex ? "ON" : "OFF") << RESET << std::endl;
        } 

        else if (tokens[0] == "return" && tokens.size() >= 3) {
            location_config.redirect_code = std::atoi(tokens[1].c_str());
            location_config.redirect_url = tokens[2];
            std::cout << CYAN << "      Redirect: " << location_config.redirect_code 
                      << " -> " << location_config.redirect_url << RESET << std::endl;
        } 

        else if (tokens[0] == "root" && tokens.size() >= 2) {
            location_config.root = tokens[1];
            std::cout << CYAN << "      Root: " << location_config.root << RESET << std::endl;
        } 

        else if (tokens[0] == "index" && tokens.size() >= 2) {
            location_config.index = tokens[1];
            std::cout << CYAN << "      Index: " << location_config.index << RESET << std::endl;
        }
        else
            std::cout << YELLOW << "      Unknown location directive: " << tokens[0] << RESET << std::endl;
    }
    
    _locations[server_name].push_back(location_config);
    std::cout << CYAN << "    Location " << location_config.path << " added to " << server_name << RESET << std::endl;
}

std::vector<std::string> ConfigParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        trim(token);
        if (!token.empty())
            tokens.push_back(token);
    }
    
    return tokens;
}

void ConfigParser::trim(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    
    if (!str.empty() && str[str.length() - 1] == ';')
        str.erase(str.length() - 1);
}

const std::vector<ServerConfig>& ConfigParser::getServers() const {
    return _servers;
}

const std::vector<LocationConfig>& ConfigParser::getLocations(const std::string& server_name) const {
    static std::vector<LocationConfig> empty;
    std::map<std::string, std::vector<LocationConfig> >::const_iterator it = _locations.find(server_name);
    if (it != _locations.end())
        return it->second;
    return empty;
}

void ConfigParser::printConfig() const {
    std::ofstream logFile("logConfig.log");
    if (!logFile.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier logConfig" << std::endl;
        return;
    }

    logFile << "=== Configuration Debug ===" << std::endl;
    
    for (size_t i = 0; i < _servers.size(); ++i) {
        const ServerConfig& server = _servers[i];
        logFile << "\n  Server " << i + 1 << ":" << std::endl;
        logFile << "   Host: " << server.host << std::endl;
        logFile << "   Port: " << server.port << std::endl;
        logFile << "   Server name: " << server.server_name << std::endl;
        logFile << "   Root: " << server.root << std::endl;
        logFile << "   Index: " << server.index << std::endl;
        logFile << "   Max body size: " << server.client_max_body_size << " bytes" << std::endl;
        
        if (!server.error_pages.empty()) {
            logFile << "  Error pages:" << std::endl;
            for (std::map<int, std::string>::const_iterator it = server.error_pages.begin();
                 it != server.error_pages.end(); ++it) {
                logFile << "    " << it->first << " -> " << it->second << std::endl;
            }
        }
        
        std::map<std::string, std::vector<LocationConfig> >::const_iterator loc_it = 
            _locations.find(server.server_name);
        if (loc_it != _locations.end() && !loc_it->second.empty()) {
            logFile << "  Locations:" << std::endl;
            
            for (size_t j = 0; j < loc_it->second.size(); ++j) {
                const LocationConfig& location = loc_it->second[j];
                logFile << "      " << location.path << ":" << std::endl;
                
                if (!location.allowed_methods.empty()) {
                    logFile << "      Methods: ";
                    for (size_t k = 0; k < location.allowed_methods.size(); ++k) {
                        logFile << location.allowed_methods[k];
                        if (k < location.allowed_methods.size() - 1)
                            logFile << ", ";
                    }
                    logFile << std::endl;
                }
                
                if (!location.upload_path.empty())
                    logFile << "      Upload: " << location.upload_path << std::endl;
                
                if (!location.cgi_extensions.empty()) {
                    logFile << "      CGI extensions: ";
                    for (size_t k = 0; k < location.cgi_extensions.size(); ++k) {
                        logFile << location.cgi_extensions[k];
                        if (k < location.cgi_extensions.size() - 1) logFile << ", ";
                    }
                    logFile << std::endl;
                }
                
                if (!location.cgi_path.empty())
                    logFile << "      CGI path: " << location.cgi_path << std::endl;
                
                if (!location.root.empty())
                    logFile << "      Root: " << location.root << std::endl;
                
                if (!location.index.empty())
                    logFile << "      Index: " << location.index << std::endl;
                logFile << "      Autoindex: " << (location.autoindex ? "ON" : "OFF") << std::endl;
                
                if (location.redirect_code > 0) 
                    logFile << "      Redirect: " << location.redirect_code << " -> " << location.redirect_url << std::endl;
            }
        }
    }
    
    logFile << "\n=== End Configuration ===" << std::endl;
    logFile.close();
    
    std::cout << GREEN << "Configuration written to logConfig file" << RESET << std::endl;
}
