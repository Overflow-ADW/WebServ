#include "Cgi.hpp"

Cgi::Cgi() {
}

Cgi::~Cgi() {
}

bool Cgi::isCgiRequest(const std::string& path, const LocationConfig& location) {
    if (location.cgi_extensions.empty() || location.cgi_path.empty()) {
        return false;
    }
    
    std::string clean_path = path;
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
        clean_path = path.substr(0, query_pos);
    }
    
    for (size_t i = 0; i < location.cgi_extensions.size(); ++i) {
        const std::string& ext = location.cgi_extensions[i];
        if (clean_path.length() >= ext.length()) {
            if (clean_path.substr(clean_path.length() - ext.length()) == ext) {
                return true;
            }
        }
    }
    
    return false;
}

bool Cgi::executeCgiRequest(const HttpRequest& request, 
                           const ServerConfig& server_config, 
                           const LocationConfig& location_config,
                           HttpResponse& response) {
    std::string script_path = buildScriptPath(request.getPath(), server_config, location_config);
    
    std::cout << BLUE << "Executing CGI script: " << script_path << RESET << std::endl;
    std::cout << BLUE << "CGI interpreter: " << location_config.cgi_path << RESET << std::endl;
    
    std::map<std::string, std::string> env_vars = buildCgiEnvironment(request, server_config, 
                                                                      location_config, script_path);
    
    bool success = response.executeCgi(script_path, location_config.cgi_path, env_vars, request.getBody());
    
    if (success) {
        std::cout << GREEN << "CGI execution successful" << RESET << std::endl;
    } else {
        std::cout << RED << "CGI execution failed: " << script_path << RESET << std::endl;
    }
    
    return success;
}

std::string Cgi::buildScriptPath(const std::string& requested_path, 
                                const ServerConfig& server_config, 
                                const LocationConfig& location_config) {
    std::string clean_path = requested_path;
    size_t query_pos = requested_path.find('?');
    if (query_pos != std::string::npos) {
        clean_path = requested_path.substr(0, query_pos);
    }
    
    std::string relative_path = clean_path;
    if (clean_path.find(location_config.path) == 0 && location_config.path != "/") {
        relative_path = clean_path.substr(location_config.path.length());
    }
    
    std::string script_path = server_config.root + "/" + location_config.path.substr(1) + relative_path;
    return script_path;
}

std::map<std::string, std::string> Cgi::buildCgiEnvironment(const HttpRequest& request, 
                                                           const ServerConfig& server_config, 
                                                           const LocationConfig& location_config,
                                                           const std::string& script_path) {
    std::map<std::string, std::string> env_vars;
    
    (void)location_config;
    
    env_vars["REQUEST_METHOD"] = request.getMethod();
    env_vars["REQUEST_URI"] = request.getPath();
    env_vars["SCRIPT_NAME"] = request.getPath();
    env_vars["SCRIPT_FILENAME"] = script_path;
    env_vars["SERVER_NAME"] = server_config.server_name;
    env_vars["SERVER_SOFTWARE"] = "Webserv/1.0";
    env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
    
    std::ostringstream port_str;
    port_str << server_config.port;
    env_vars["SERVER_PORT"] = port_str.str();
    
    size_t query_pos = request.getPath().find('?');
    if (query_pos != std::string::npos) {
        env_vars["QUERY_STRING"] = request.getPath().substr(query_pos + 1);
        env_vars["REQUEST_URI"] = request.getPath().substr(0, query_pos);
        env_vars["SCRIPT_NAME"] = request.getPath().substr(0, query_pos);
    } else {
        env_vars["QUERY_STRING"] = "";
    }
    
    const std::string& body = request.getBody();
    std::ostringstream content_length;
    content_length << body.length();
    env_vars["CONTENT_LENGTH"] = content_length.str();
    
    std::string content_type = request.getHeader("content-type");
    if (!content_type.empty()) {
        env_vars["CONTENT_TYPE"] = content_type;
    }
    
    const std::map<std::string, std::string>& headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        std::string env_name = "HTTP_" + it->first;
        
        for (size_t i = 0; i < env_name.length(); ++i) {
            if (env_name[i] == '-') {
                env_name[i] = '_';
            } else {
                env_name[i] = std::toupper(env_name[i]);
            }
        }
        
        env_vars[env_name] = it->second;
    }
    
    std::string host = request.getHeader("Host");
    if (!host.empty()) {
        env_vars["HTTP_HOST"] = host;
        env_vars["SERVER_NAME"] = host;
    }
    
    std::string user_agent = request.getHeader("User-Agent");
    if (!user_agent.empty()) {
        env_vars["HTTP_USER_AGENT"] = user_agent;
    }
    
    return env_vars;
}