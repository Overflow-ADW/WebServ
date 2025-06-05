#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "webserv.hpp"

class HttpRequest {
private:
    std::string _method;
    std::string _path;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _is_complete;
    
public:
    HttpRequest();
    ~HttpRequest();
    
    // Parsing
    bool parseRequest(const std::string& raw_request);
    void parseRequestLine(const std::string& line);
    void parseHeader(const std::string& line);
    
    // Getters
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::string& getHeader(const std::string& name) const;
    const std::string& getBody() const;
    const std::map<std::string, std::string>& getHeaders() const;
    bool isComplete() const;
    
    // HTTP specific getters
    size_t getContentLength() const;
    std::string getHost() const;
    std::string getConnection() const;
    
    // Utils
    void clear();
    void print() const; // Pour debug
    
private:
    std::string trim(const std::string& str);
};

#endif
