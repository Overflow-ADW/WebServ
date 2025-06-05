#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _is_complete(false) {
}

HttpRequest::~HttpRequest() {
}

bool HttpRequest::parseRequest(const std::string& raw_request) {
    if (raw_request.empty()) {
        return false;
    }
    
    // Find the end of headers (double CRLF or double LF)
    size_t headers_end = raw_request.find("\r\n\r\n");
    bool crlf_format = true;
    if (headers_end == std::string::npos) {
        headers_end = raw_request.find("\n\n");
        crlf_format = false;
        if (headers_end == std::string::npos) {
            // No body, just headers
            headers_end = raw_request.length();
        }
    }
    
    // Extract headers part
    std::string headers_part = raw_request.substr(0, headers_end);
    
    // Split headers into lines
    std::vector<std::string> lines;
    std::stringstream ss(headers_part);
    std::string line;
    
    while (std::getline(ss, line)) {
        // Remove \r if present (Windows line endings)
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        lines.push_back(line);
    }
    
    if (lines.empty()) {
        return false;
    }
    
    // Parse request line (first line)
    parseRequestLine(lines[0]);
    
    // Parse headers
    for (size_t i = 1; i < lines.size() && !lines[i].empty(); ++i) {
        parseHeader(lines[i]);
    }
    
    // Extract body if present (after headers)
    if (headers_end < raw_request.length()) {
        size_t body_start = headers_end + (crlf_format ? 4 : 2); // Skip \r\n\r\n or \n\n
        if (body_start < raw_request.length()) {
            _body = raw_request.substr(body_start);
        }
    }
    
    // Check if request is complete
    if (!_method.empty() && !_path.empty() && !_version.empty()) {
        _is_complete = true;
        return true;
    }
    
    return false;
}

void HttpRequest::parseRequestLine(const std::string& line) {
    // Parse: METHOD /path HTTP/version
    std::istringstream iss(line);
    
    if (!(iss >> _method >> _path >> _version)) {
        std::cerr << "❌ Invalid request line: " << line << std::endl;
        return;
    }
    
    // Basic validation
    if (_method.empty() || _path.empty() || _version.empty()) {
        std::cerr << "❌ Incomplete request line: " << line << std::endl;
        return;
    }
    
    // Validate HTTP version
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0") {
        std::cerr << "❌ Unsupported HTTP version: " << _version << std::endl;
    }
    
    // Normalize method to uppercase
    for (size_t i = 0; i < _method.length(); ++i) {
        _method[i] = std::toupper(_method[i]);
    }
}

void HttpRequest::parseHeader(const std::string& line) {
    // Parse: Header-Name: Header-Value
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        std::cerr << "❌ Invalid header format: " << line << std::endl;
        return;
    }
    
    std::string name = line.substr(0, colon_pos);
    std::string value = line.substr(colon_pos + 1);
    
    // Trim whitespace
    name = trim(name);
    value = trim(value);
    
    if (!name.empty()) {
        // Convert header name to lowercase for case-insensitive lookup
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
        std::cout << _body.substr(0, 200); // Show first 200 chars
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
