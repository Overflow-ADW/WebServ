#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "webserv.hpp"

class HttpResponse {
private:
    int _status_code;
    std::string _status_message;
    std::map<std::string, std::string> _headers;
    std::string _body;
    
public:
    HttpResponse();
    ~HttpResponse();
    
    void setStatus(int code, const std::string& message);
    void setHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& body);
    
    int getStatusCode() const;
    const std::string& getStatusMessage() const;
    const std::string& getHeader(const std::string& name) const;
    const std::string& getBody() const;
    
    std::string toString() const;
    void clear();
    
    bool serveFile(const std::string& file_path);
    bool serveErrorPage(int error_code, const std::string& error_path = "");
    
    bool executeCgi(const std::string& script_path, const std::string& cgi_path, 
                   const std::map<std::string, std::string>& env_vars, 
                   const std::string& request_body = "");
    
    static std::string getStatusText(int code);
    static std::string getMimeType(const std::string& file_path);
    static bool fileExists(const std::string& path);
    static std::string readFile(const std::string& path);
};

#endif
