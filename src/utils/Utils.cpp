#include "../../includes/Utils.hpp"

std::map<std::string, std::string> Utils::_mime_types;

std::string Utils::trim(const std::string& str) {
    if (str.empty()) return str;
    
    size_t start = 0;
    size_t end = str.length() - 1;
    
    while (start <= end && std::isspace(str[start]))
        start++;
    
    while (end > start && std::isspace(str[end]))
        end--;
    
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
        }
        else
            current += str[i];
    }
    
    if (!current.empty())
        result.push_back(current);
    
    return result;
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i)
        result[i] = std::tolower(result[i]);
    return result;
}

std::string Utils::toUpperCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i)
        result[i] = std::toupper(result[i]);
    return result;
}

bool Utils::fileExists(const std::string& path) {
    std::ifstream file(path.c_str());
    return file.good();
}

std::string Utils::readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return "";
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string content(size, '\0');
    file.read(&content[0], size);
    
    return content;
}

std::string Utils::getFileExtension(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos || dot_pos == path.length() - 1)
        return "";
    return path.substr(dot_pos);
}

std::string Utils::getMimeType(const std::string& extension) {
    if (_mime_types.empty())
        initMimeTypes();
    
    std::string ext = toLowerCase(extension);
    std::map<std::string, std::string>::const_iterator it = _mime_types.find(ext);
    if (it != _mime_types.end())
        return it->second;
    return "application/octet-stream";
}

void Utils::initMimeTypes() {
    _mime_types[".html"] = "text/html";
    _mime_types[".htm"] = "text/html";
    _mime_types[".css"] = "text/css";
    _mime_types[".js"] = "application/javascript";
    _mime_types[".txt"] = "text/plain";
    _mime_types[".xml"] = "text/xml";
    _mime_types[".json"] = "application/json";
    
    _mime_types[".jpg"] = "image/jpeg";
    _mime_types[".jpeg"] = "image/jpeg";
    _mime_types[".png"] = "image/png";
    _mime_types[".gif"] = "image/gif";
    _mime_types[".bmp"] = "image/bmp";
    _mime_types[".ico"] = "image/x-icon";
    _mime_types[".svg"] = "image/svg+xml";
    
    _mime_types[".mp3"] = "audio/mpeg";
    _mime_types[".wav"] = "audio/wav";
    _mime_types[".mp4"] = "video/mp4";
    _mime_types[".avi"] = "video/x-msvideo";
    
    _mime_types[".pdf"] = "application/pdf";
    _mime_types[".doc"] = "application/msword";
    _mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    
    _mime_types[".zip"] = "application/zip";
    _mime_types[".tar"] = "application/x-tar";
    _mime_types[".gz"] = "application/gzip";
    
    _mime_types[".bin"] = "application/octet-stream";
    _mime_types[".exe"] = "application/octet-stream";
}
