#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Cgi {
private:
    std::map<std::string, std::string> buildCgiEnvironment(const HttpRequest& request, 
                                                           const ServerConfig& server_config, 
                                                           const LocationConfig& location_config,
                                                           const std::string& script_path);
    
    std::string buildScriptPath(const std::string& requested_path, 
                               const ServerConfig& server_config, 
                               const LocationConfig& location_config);

public:
    Cgi();
    ~Cgi();
    
    static bool isCgiRequest(const std::string& path, const LocationConfig& location);
    
    bool executeCgiRequest(const HttpRequest& request, 
                          const ServerConfig& server_config, 
                          const LocationConfig& location_config,
                          HttpResponse& response);
};

#endif