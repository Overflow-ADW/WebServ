#ifndef FILEUPLOADHANDLER_HPP
#define FILEUPLOADHANDLER_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"
#include "ConfigParser.hpp"

class FileUploadHandler {
private:
    bool parseMultipartData(const std::string& body, const std::string& boundary, 
                           const std::string& upload_path, std::vector<std::string>& uploaded_files);
    bool saveUploadedFile(const std::string& filename, const std::string& content, 
                         const std::string& upload_path);
    std::string extractBoundary(const std::string& content_type);

public:
    FileUploadHandler();
    ~FileUploadHandler();
    
    bool handleFileUpload(const HttpRequest& request, const LocationConfig& location_config,
                         std::string& response_html);
    
    static bool isFileUpload(const HttpRequest& request, const LocationConfig& location_config);
};

#endif
