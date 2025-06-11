#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _is_complete(false) {
}

HttpRequest::~HttpRequest() {
}

bool HttpRequest::parseRequest(const std::string& raw_request) {
    if (raw_request.empty()) {
        return false;
    }
    
    size_t headers_end = raw_request.find("\r\n\r\n");
    bool crlf_format = true;
    if (headers_end == std::string::npos) {
        headers_end = raw_request.find("\n\n");
        crlf_format = false;
        if (headers_end == std::string::npos)
            headers_end = raw_request.length();
    }
    
    std::string headers_part = raw_request.substr(0, headers_end);
    
    std::vector<std::string> lines;
    std::stringstream ss(headers_part);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        lines.push_back(line);
    }
    
    if (lines.empty()) {
        return false;
    }
    
    parseRequestLine(lines[0]);
    
    for (size_t i = 1; i < lines.size() && !lines[i].empty(); ++i) {
        parseHeader(lines[i]);
    }
    
    if (headers_end < raw_request.length()) {
        size_t body_start = headers_end + (crlf_format ? 4 : 2);
        if (body_start < raw_request.length()) {
            _body = raw_request.substr(body_start);
        }
    }
    
    if (_method.empty() || _path.empty() || _version.empty()) {
        return false;
    }

    _is_complete = true;
    return true;
}

void HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream iss(line);
    
    if (!(iss >> _method >> _path >> _version)) {
        std::cerr << "Invalid request line: " << line << std::endl;
        return;
    }
    
    if (_method.empty() || _path.empty() || _version.empty()) {
        std::cerr << "Incomplete request line: " << line << std::endl;
        return;
    }
    
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0") {
        std::cerr << "Unsupported HTTP version: " << _version << std::endl;
    }
    
    for (size_t i = 0; i < _method.length(); ++i) {
        _method[i] = std::toupper(_method[i]);
    }
}

void HttpRequest::parseHeader(const std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        std::cerr << "Invalid header format: " << line << std::endl;
        return;
    }
    
    std::string name = line.substr(0, colon_pos);
    std::string value = line.substr(colon_pos + 1);
    
    name = trim(name);
    value = trim(value);
    
    if (!name.empty()) {
        for (size_t i = 0; i < name.length(); ++i) {
            name[i] = std::tolower(name[i]);
        }
        _headers[name] = value;
    }
}

const std::string& HttpRequest::getMethod() const {
    return _method;
}

const std::string& HttpRequest::getPath() const {
    return _path;
}

const std::string& HttpRequest::getVersion() const {
    return _version;
}

const std::string& HttpRequest::getHeader(const std::string& name) const {
    static std::string empty;
    std::map<std::string, std::string>::const_iterator it = _headers.find(name);
    if (it != _headers.end()) {
        return it->second;
    }
    return empty;
}

const std::string& HttpRequest::getBody() const {
    return _body;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
    return _headers;
}

size_t HttpRequest::getContentLength() const {
    std::string content_length = getHeader("content-length");
    if (content_length.empty()) {
        return 0;
    }
    
    std::istringstream iss(content_length);
    size_t length;
    iss >> length;
    return length;
}

std::string HttpRequest::getHost() const {
    return getHeader("host");
}

std::string HttpRequest::getConnection() const {
    return getHeader("connection");
}

bool HttpRequest::isComplete() const {
    return _is_complete;
}

void HttpRequest::clear() {
    _method.clear();
    _path.clear();
    _version.clear();
    _headers.clear();
    _body.clear();
    _is_complete = false;
}

void HttpRequest::print() const {
    std::cout << CYAN << "=== 📨 HTTP Request Debug ===" << RESET << std::endl;
    std::cout << GREEN << "Method: " << _method << RESET << std::endl;
    std::cout << GREEN << "Path: " << _path << RESET << std::endl;
    std::cout << GREEN << "Version: " << _version << RESET << std::endl;
    
    if (!_headers.empty()) {
        std::cout << YELLOW << "Headers:" << RESET << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); 
             it != _headers.end(); ++it) {
            std::cout << "  " << it->first << ": " << it->second << std::endl;
        }
    }
    
    if (!_body.empty()) {
        std::cout << BLUE << "Body (" << _body.length() << " bytes):" << RESET << std::endl;
        std::cout << _body.substr(0, 200);
        if (_body.length() > 200) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
    
    std::cout << CYAN << "=== End Request ===" << RESET << std::endl;
}

std::string HttpRequest::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
