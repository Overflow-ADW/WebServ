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
    static std::string readFile(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string getMimeType(const std::string& extension);
                
private:
    Utils();
    static std::map<std::string, std::string> _mime_types;
    static void initMimeTypes();
};

#endif
