#include "../../includes/Utils.hpp"

// Définition statique
std::map<std::string, std::string> Utils::_mime_types;

// Improved utility implementations
std::string Utils::trim(const std::string& str) {
    if (str.empty()) return str;
    
    size_t start = 0;
    size_t end = str.length() - 1;
    
    // Find first non-whitespace character
    while (start <= end && std::isspace(str[start])) {
        start++;
    }
    
    // Find last non-whitespace character
    while (end > start && std::isspace(str[end])) {
        end--;
    }
    
    return str.substr(start, end - start + 1);
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += str[i];
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

std::string Utils::toUpperCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

bool Utils::fileExists(const std::string& path) {
    std::ifstream file(path.c_str());
    return file.good();
}

bool Utils::isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

std::string Utils::readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read file content
    std::string content(size, '\0');
    file.read(&content[0], size);
    
    return content;
}

bool Utils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(content.c_str(), content.length());
    return file.good();
}

std::string Utils::getFileExtension(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos || dot_pos == path.length() - 1) {
        return "";
    }
    return path.substr(dot_pos);
}

std::string Utils::getMimeType(const std::string& extension) {
    if (_mime_types.empty()) {
        initMimeTypes();
    }
    
    std::string ext = toLowerCase(extension);
    std::map<std::string, std::string>::const_iterator it = _mime_types.find(ext);
    if (it != _mime_types.end()) {
        return it->second;
    }
    return "application/octet-stream"; // Default for unknown types
}

std::string Utils::getContentType(const std::string& file_path) {
    std::string extension = getFileExtension(file_path);
    return getMimeType(extension);
}

std::string Utils::getCurrentDateTime() {
    time_t now = time(0);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buffer);
}

std::string Utils::formatHttpDate(time_t timestamp) {
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&timestamp));
    return std::string(buffer);
}

std::string Utils::urlDecode(const std::string& url) {
    std::string result;
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%' && i + 2 < url.length()) {
            // Convert hex to char
            char hex[3];
            hex[0] = url[i + 1];
            hex[1] = url[i + 2];
            hex[2] = '\0';
            
            char* endptr;
            long val = strtol(hex, &endptr, 16);
            if (endptr == hex + 2) { // Successful conversion
                result += static_cast<char>(val);
                i += 2; // Skip the hex digits
            } else {
                result += url[i]; // Invalid hex, keep original
            }
        } else if (url[i] == '+') {
            result += ' '; // Plus sign becomes space
        } else {
            result += url[i];
        }
    }
    return result;
}

std::string Utils::urlEncode(const std::string& url) {
    std::string result;
    for (size_t i = 0; i < url.length(); ++i) {
        char c = url[i];
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            // Convert to hex
            char hex[4];
            sprintf(hex, "%%%02X", static_cast<unsigned char>(c));
            result += hex;
        }
    }
    return result;
}

std::string Utils::getClientIP(int socket_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    if (getpeername(socket_fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        return std::string(inet_ntoa(addr.sin_addr));
    }
    return "127.0.0.1"; // Fallback for localhost
}

void Utils::initMimeTypes() {
    // Text types
    _mime_types[".html"] = "text/html";
    _mime_types[".htm"] = "text/html";
    _mime_types[".css"] = "text/css";
    _mime_types[".js"] = "application/javascript";
    _mime_types[".txt"] = "text/plain";
    _mime_types[".xml"] = "text/xml";
    _mime_types[".json"] = "application/json";
    
    // Image types
    _mime_types[".jpg"] = "image/jpeg";
    _mime_types[".jpeg"] = "image/jpeg";
    _mime_types[".png"] = "image/png";
    _mime_types[".gif"] = "image/gif";
    _mime_types[".bmp"] = "image/bmp";
    _mime_types[".ico"] = "image/x-icon";
    _mime_types[".svg"] = "image/svg+xml";
    
    // Audio/Video types
    _mime_types[".mp3"] = "audio/mpeg";
    _mime_types[".wav"] = "audio/wav";
    _mime_types[".mp4"] = "video/mp4";
    _mime_types[".avi"] = "video/x-msvideo";
    
    // Document types
    _mime_types[".pdf"] = "application/pdf";
    _mime_types[".doc"] = "application/msword";
    _mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    
    // Archive types
    _mime_types[".zip"] = "application/zip";
    _mime_types[".tar"] = "application/x-tar";
    _mime_types[".gz"] = "application/gzip";
    
    // Binary types
    _mime_types[".bin"] = "application/octet-stream";
    _mime_types[".exe"] = "application/octet-stream";
}
