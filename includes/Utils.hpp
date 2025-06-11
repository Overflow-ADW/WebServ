#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    
    static bool fileExists(const std::string& path);
    static bool isDirectory(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static std::string getFileExtension(const std::string& path);
    static std::string getMimeType(const std::string& extension);
    
    static std::string urlDecode(const std::string& url);
    static std::string urlEncode(const std::string& url);
    static std::string getContentType(const std::string& file_path);
    
    static std::string getCurrentDateTime();
    static std::string formatHttpDate(time_t timestamp);
    
    static std::string getClientIP(int socket_fd);
    
private:
    Utils();
    static std::map<std::string, std::string> _mime_types;
    static void initMimeTypes();
};

#endif
