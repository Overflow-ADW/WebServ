#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status_code(200), _status_message("OK") {
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::setStatus(int code, const std::string& message) {
    _status_code = code;
    _status_message = message;
}

void HttpResponse::setHeader(const std::string& name, const std::string& value) {
    _headers[name] = value;
}

void HttpResponse::setBody(const std::string& body) {
    _body = body;
    std::ostringstream oss;
    oss << body.length();
    setHeader("Content-Length", oss.str());
}

int HttpResponse::getStatusCode() const {
    return _status_code;
}

const std::string& HttpResponse::getStatusMessage() const {
    return _status_message;
}

const std::string& HttpResponse::getHeader(const std::string& name) const {
    static std::string empty;
    std::map<std::string, std::string>::const_iterator it = _headers.find(name);
    if (it != _headers.end()) {
        return it->second;
    }
    return empty;
}

const std::string& HttpResponse::getBody() const {
    return _body;
}

std::string HttpResponse::toString() const {
    std::ostringstream response;
    
    response << "HTTP/1.1 " << _status_code << " " << _status_message << "\r\n";
    
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    
    response << "\r\n";
    
    response << _body;
    
    return response.str();
}

void HttpResponse::clear() {
    _status_code = 200;
    _status_message = "OK";
    _headers.clear();
    _body.clear();
}

std::string HttpResponse::getStatusText(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

bool HttpResponse::serveFile(const std::string& file_path) {
    if (!fileExists(file_path)) {
        return false;
    }
    
    std::string content = readFile(file_path);
    if (content.empty()) {
        return false;
    }
    
    setStatus(200, "OK");
    setHeader("Content-Type", getMimeType(file_path));
    setHeader("Connection", "close");
    setBody(content);
    
    return true;
}

bool HttpResponse::serveErrorPage(int error_code, const std::string& error_path) {
    std::string status_text = getStatusText(error_code);
    setStatus(error_code, status_text);
    setHeader("Content-Type", "text/html");
    setHeader("Connection", "close");
    
    std::string body;
    
    if (!error_path.empty() && fileExists(error_path)) {
        body = readFile(error_path);
    }
    
    if (body.empty()) {
        body = "<html><head><title>" + status_text + "</title></head>";
        body += "<body style='font-family: Arial, sans-serif; text-align: center; padding: 50px;'>";
        body += "<h1 style='color: #e74c3c;'>" + std::string(1, '0' + error_code / 100) + 
                std::string(1, '0' + (error_code / 10) % 10) + 
                std::string(1, '0' + error_code % 10) + " " + status_text + "</h1>";
        body += "<p>The requested resource could not be found on this server.</p>";
        body += "<hr><p><em>Webserv/1.0</em></p>";
        body += "</body></html>";
    }
    
    setBody(body);
    return true;
}

std::string HttpResponse::getMimeType(const std::string& file_path) {
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    
    std::string ext = file_path.substr(dot_pos);
    
    for (size_t i = 0; i < ext.length(); ++i) {
        ext[i] = std::tolower(ext[i]);
    }
    
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".xml") return "application/xml";
    if (ext == ".txt") return "text/plain";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".ico") return "image/x-icon";
    if (ext == ".pdf") return "application/pdf";
    if (ext == ".zip") return "application/zip";
    
    return "application/octet-stream";
}

bool HttpResponse::fileExists(const std::string& path) {
    std::ifstream file(path.c_str());
    return file.good();
}

std::string HttpResponse::readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string content(size, '\0');
    file.read(&content[0], size);
    
    return content;
}

bool HttpResponse::executeCgi(const std::string& script_path, const std::string& cgi_path, 
                             const std::map<std::string, std::string>& env_vars, 
                             const std::string& request_body) {
    if (!fileExists(script_path)) {
        setStatus(404, "Not Found");
        setBody("<html><body><h1>404 Not Found</h1><p>CGI script not found</p></body></html>");
        return false;
    }
    
    int pipe_in[2];
    int pipe_out[2];
    
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        setStatus(500, "Internal Server Error");
        setBody("<html><body><h1>500 Internal Server Error</h1><p>Pipe creation failed</p></body></html>");
        return false;
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        setStatus(500, "Internal Server Error");
        setBody("<html><body><h1>500 Internal Server Error</h1><p>Fork failed</p></body></html>");
        return false;
    }
    
    if (pid == 0) {
        close(pipe_in[1]);
        close(pipe_out[0]);
        
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        for (std::map<std::string, std::string>::const_iterator it = env_vars.begin(); 
             it != env_vars.end(); ++it) {
            setenv(it->first.c_str(), it->second.c_str(), 1);
        }
        
        execl(cgi_path.c_str(), cgi_path.c_str(), script_path.c_str(), (char*)NULL);
        
        exit(1);
    } else {
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        if (!request_body.empty()) {
            ssize_t bytes_written = write(pipe_in[1], request_body.c_str(), request_body.length());
            if (bytes_written < 0) {
                close(pipe_in[1]);
                close(pipe_out[0]);
                setStatus(500, "Internal Server Error");
                setBody("<html><body><h1>500 Internal Server Error</h1><p>Failed to send data to CGI script</p></body></html>");
                return false;
            }
            if ((size_t)bytes_written != request_body.length()) {
                std::cerr << "Warning: CGI write was partial (" << bytes_written 
                          << "/" << request_body.length() << " bytes)" << std::endl;
            }
        }
        close(pipe_in[1]);
        
        std::string cgi_output;
        char buffer[1024];
        ssize_t bytes_read;
        
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            cgi_output += buffer;
        }
        
        if (bytes_read < 0) {
            std::cerr << "Warning: CGI read error encountered (may be normal if CGI closed pipe)" << std::endl;
        }
        
        close(pipe_out[0]);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            size_t header_end = cgi_output.find("\n\n");
            if (header_end == std::string::npos) {
                header_end = cgi_output.find("\r\n\r\n");
                if (header_end != std::string::npos) {
                    header_end += 2;
                }
            }
            
            if (header_end != std::string::npos) {
                std::string headers_part = cgi_output.substr(0, header_end);
                std::string body_part = cgi_output.substr(header_end + 2);
                
                std::istringstream header_stream(headers_part);
                std::string line;
                bool content_type_set = false;
                
                while (std::getline(header_stream, line)) {
                    if (line.empty() || line == "\r") break;
                    
                    size_t colon_pos = line.find(':');
                    if (colon_pos != std::string::npos) {
                        std::string name = line.substr(0, colon_pos);
                        std::string value = line.substr(colon_pos + 1);
                        
                        while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
                            value = value.substr(1);
                        }
                        while (!value.empty() && (value[value.length()-1] == '\r' || value[value.length()-1] == '\n')) {
                            value = value.substr(0, value.length()-1);
                        }
                        
                        setHeader(name, value);
                        if (name == "Content-Type") {
                            content_type_set = true;
                        }
                    }
                }
                
                if (!content_type_set) {
                    setHeader("Content-Type", "text/html");
                }
                
                setStatus(200, "OK");
                setBody(body_part);
            } else {
                setStatus(200, "OK");
                setHeader("Content-Type", "text/html");
                setBody(cgi_output);
            }
            
            setHeader("Connection", "close");
            return true;
        } else {
            setStatus(500, "Internal Server Error");
            setHeader("Content-Type", "text/html");
            setBody("<html><body><h1>500 Internal Server Error</h1><p>CGI execution failed</p></body></html>");
            return false;
        }
    }
}
